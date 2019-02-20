cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(kiva4ReaderTest)

find_package(VTK COMPONENTS 
  vtkCommonCore
  vtkCommonDataModel
  vtkFiltersGeneral
  vtkIOXML
  vtkInteractionStyle
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingOpenGL2 QUIET)
if (NOT VTK_FOUND)
  message("Skipping kiva4ReaderTest: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(kiva4ReaderTest MACOSX_BUNDLE
    kiva4ReaderTest.cxx
    vtkKiva4Reader.cxx
  )
  target_link_libraries(kiva4ReaderTest PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(kiva4ReaderTest MACOSX_BUNDLE
    kiva4ReaderTest.cxx
    vtkKiva4Reader.cxx
  )
  target_link_libraries(kiva4ReaderTest PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS kiva4ReaderTest 
    MODULES ${VTK_LIBRARIES}
    )
endif () 
