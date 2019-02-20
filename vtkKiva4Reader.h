/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkKiva4Reader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkKiva4Reader
 * @brief   reads a dataset in KIVA4 format (kiva4grid)
 *
 * vtkKiva4Reader creates an unstructured grid dataset. It reads ASCII files
 * stored in KIVA4 format (i.e., kiva4grid), with boundary data at cells of
 * the model. The OutputFace option controls the mesh types for outputting:
 *  =0: only volumes
 *  =1: volumes and faces except for interior faces
 *  =2: volumes and all faces
 *
*/

#ifndef vtkKiva4Reader_h
#define vtkKiva4Reader_h

#include "vtkUnstructuredGridAlgorithm.h"

class vtkFloatArray;

class vtkKiva4Reader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkKiva4Reader *New();
  vtkTypeMacro(vtkKiva4Reader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify file name of KIVA4 datafile to read
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  //@{
  /**
   * Specify whether to output face data. 
   *  =0: only volumes
   *  =1: volumes and faces except for interior faces
   *  =2: volumes and all faces
   */
  vtkSetMacro(OutputFace, int);
  vtkGetMacro(OutputFace, int);
  vtkBooleanMacro(OutputFace, int);
  //@}

protected:
  vtkKiva4Reader();
  ~vtkKiva4Reader() override;
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char *FileName;

  int NumberOfNodes;
  int NumberOfCells;
  int NumberOfAllCells;

  int OutputFace;

  ifstream *FileStream;

private:
  vtkKiva4Reader(const vtkKiva4Reader&) = delete;
  void operator=(const vtkKiva4Reader&) = delete;

  void ReadFile(vtkUnstructuredGrid *output);
  void ReadGeometry(vtkUnstructuredGrid* output);

  void ReadMaterials(vtkUnstructuredGrid *output);

  void ReadXYZCoords(vtkFloatArray *coords);
  void ReadCellTopology(vtkUnstructuredGrid *output);
};

#endif
