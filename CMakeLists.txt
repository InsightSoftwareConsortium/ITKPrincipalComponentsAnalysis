cmake_minimum_required(VERSION 3.10.2)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

set(PCA PrincipalComponentsAnalysis)
project(${PCA})

if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
  include(ITKModuleExternal)
else()
  itk_module_impl()
endif()

itk_module_examples()


CMAKE_DEPENDENT_OPTION(Module_${PCA}_BUILD_DOCUMENTATION "Generate documentation from LaTeX files, source code examples and result screenshots" OFF
  "BUILD_DOCUMENTATION" OFF)
if(Module_${PCA}_BUILD_DOCUMENTATION)
  add_subdirectory( doc )
endif()
