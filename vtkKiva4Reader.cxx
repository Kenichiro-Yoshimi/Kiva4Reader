/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkKiva4Reader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkKiva4Reader.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkErrorCode.h"
#include "vtkExtractSelection.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkUnstructuredGrid.h"

vtkStandardNewMacro(vtkKiva4Reader);

//----------------------------------------------------------------------------
vtkKiva4Reader::vtkKiva4Reader()
{
  this->FileName = NULL;
  this->OutputFace = 0;

  this->FileStream = NULL;
  this->NumberOfNodes = 0;
  this->NumberOfCells = 0;
  this->NumberOfAllCells = 0;

  this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
vtkKiva4Reader::~vtkKiva4Reader()
{
  delete [] this->FileName;
}

//----------------------------------------------------------------------------
int vtkKiva4Reader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // Get the output
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro(<< "Reading Kiva4 file");

  // If ExecuteInformation() failed FileStream will be NULL and
  // ExecuteInformation() will have split out an error.
  if (this->FileStream)
  {
    this->ReadFile(output);
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkKiva4Reader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: "
     << (this->FileName ? this->FileName : "(none)") << std::endl;

  os << indent << "Output Face: " << this->OutputFace << std::endl;
}

//----------------------------------------------------------------------------
int vtkKiva4Reader::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *vtkNotUsed(outputVector))
{

  if (!this->FileName)
  {
    vtkErrorMacro("No filename specified");
    return 0;
  }

  this->FileStream = new ifstream(this->FileName, ios::in);
  if (this->FileStream->fail())
  {
    this->SetErrorCode(vtkErrorCode::FileNotFoundError);
    delete this->FileStream;
    this->FileStream = NULL;
    vtkErrorMacro("Specified filename not found");
    return 0;
  }

  char temp[1024];
  this->FileStream->getline(temp, 1024);
  *(this->FileStream) >> this->NumberOfCells;
  *(this->FileStream) >> this->NumberOfNodes;

  this->NumberOfAllCells = this->NumberOfCells + 
    (int)(this->OutputFace >= 1) * this->NumberOfCells * 6;

  return 1;
}

//----------------------------------------------------------------------------
void vtkKiva4Reader::ReadFile(vtkUnstructuredGrid *output)
{
  this->ReadGeometry(output);

  this->ReadMaterials(output);

  delete this->FileStream;
  this->FileStream = NULL;

  // Remove interior faces
  if (this->OutputFace == 1)
  {
    vtkIntArray *materials = vtkIntArray::SafeDownCast(
      output->GetCellData()->GetArray("Material_Id"));

    vtkIdTypeArray *ids = vtkIdTypeArray::New();
    ids->SetNumberOfComponents(1);

    for (vtkIdType i = 0; i < this->NumberOfAllCells; ++i)
    {
      int mat = materials->GetTuple1(i);
      if (mat == 40)
      {
        continue;
      }

      ids->InsertNextValue(i);
    }
    this->NumberOfAllCells -= ids->GetNumberOfTuples();

    vtkSelectionNode *selectionNode = vtkSelectionNode::New();
    selectionNode->SetFieldType(vtkSelectionNode::CELL);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids);
    ids->Delete();

    vtkSelection *selection = vtkSelection::New();
    selection->AddNode(selectionNode);
    selectionNode->Delete();

    vtkExtractSelection *extractSelection = vtkExtractSelection::New();
    extractSelection->SetInputData(0, output);
    extractSelection->SetInputData(1, selection);
    extractSelection->Update();

    output->ShallowCopy(extractSelection->GetOutput()); 
    extractSelection->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkKiva4Reader::ReadGeometry(vtkUnstructuredGrid *output)
{
  vtkFloatArray *coords = vtkFloatArray::New();
  coords->SetNumberOfComponents(3);
  coords->SetNumberOfTuples(this->NumberOfNodes);

  this->ReadXYZCoords(coords);
  this->ReadCellTopology(output);

  vtkPoints *points = vtkPoints::New();
  points->SetData(coords);
  coords->Delete();

  output->SetPoints(points);
  points->Delete();
}

//----------------------------------------------------------------------------
void vtkKiva4Reader::ReadXYZCoords(vtkFloatArray *coords)
{
  float *ptr = coords->GetPointer(0);

  for (int i = 0; i < this->NumberOfNodes; i++)
  {
    *(this->FileStream) >> ptr[3*i] >> ptr[3*i+1] >> ptr[3*i+2];
  }
}

//----------------------------------------------------------------------------
void vtkKiva4Reader::ReadCellTopology(vtkUnstructuredGrid *output)
{
  vtkIdType list[8], fList[4];

  output->Allocate();
  for (int i = 0; i < this->NumberOfCells; ++i)
  {
    for (int k = 0; k < 8; ++k)
    {
      *(this->FileStream) >> list[k];
      --list[k];
    }
    output->InsertNextCell(VTK_HEXAHEDRON, 8, list);

    if (!this->OutputFace)
    {
      continue;
    }

    // left face
    fList[0] = list[2];
    fList[1] = list[3];
    fList[2] = list[7];
    fList[3] = list[6];
    output->InsertNextCell(VTK_QUAD, 4, fList);
    // front face
    fList[0] = list[0];
    fList[1] = list[4];
    fList[2] = list[7];
    fList[3] = list[3];
    output->InsertNextCell(VTK_QUAD, 4, fList);
    // bottom face
    fList[0] = list[0];
    fList[1] = list[1];
    fList[2] = list[2];
    fList[3] = list[3];
    output->InsertNextCell(VTK_QUAD, 4, fList);
    // right face
    fList[0] = list[0];
    fList[1] = list[1];
    fList[2] = list[5];
    fList[3] = list[4];
    output->InsertNextCell(VTK_QUAD, 4, fList);
    // derriere face
    fList[0] = list[1];
    fList[1] = list[2];
    fList[2] = list[6];
    fList[3] = list[5];
    output->InsertNextCell(VTK_QUAD, 4, fList);
    // top face
    fList[0] = list[4];
    fList[1] = list[5];
    fList[2] = list[6];
    fList[3] = list[7];
    output->InsertNextCell(VTK_QUAD, 4, fList);
  }
}

//----------------------------------------------------------------------------
void vtkKiva4Reader::ReadMaterials(vtkUnstructuredGrid *output)
{
  // add a material array
  vtkIntArray *materials = vtkIntArray::New();
  materials->SetNumberOfTuples(this->NumberOfAllCells);
  materials->SetName("Matrial_Id");

  int *mat = materials->GetPointer(0);
  int matId[7];

  for (int n = 0; n < this->NumberOfCells; ++n)
  {
    vtkIdType id = n;
    if (this->OutputFace)
    {
      id *= 7;
    }

    for (int j = 0; j < 7; ++j)
    {
      *(this->FileStream) >> matId[j];
    }

    mat[id] = matId[0] + 100;
    if (!this->OutputFace)
    {
      continue;
    }

    for (int j = 1; j < 7; ++j)
    {
      mat[id+j] = matId[j];
    }
  }

  // now add the material array
  output->GetCellData()->AddArray(materials);
  if (!output->GetCellData()->GetScalars())
  {
    output->GetCellData()->SetScalars(materials);
  }
  materials->Delete();
}
