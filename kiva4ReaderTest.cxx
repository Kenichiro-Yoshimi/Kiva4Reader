#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtksys/SystemTools.hxx>

#include "vtkKiva4Reader.h"

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "Required parameters: FileName OutputFace" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fileName = argv[1];
  std::string fileNameWithoutExt =
    vtksys::SystemTools::GetFilenameWithoutExtension(fileName);
  int outputFace = atoi(argv[2]);

  auto reader = vtkSmartPointer<vtkKiva4Reader>::New();
  reader->SetFileName(fileName.c_str());
  reader->SetOutputFace(outputFace);
  reader->Update();

  std::string outputFileName = fileNameWithoutExt + ".vtu";
  auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
  writer->SetFileName(outputFileName.c_str());
  writer->SetInputConnection(reader->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}
