if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED COMPONENTS PrincipalComponentsAnalysis)
  include(${ITK_USE_FILE})
else()
  itk_module_impl()
endif()

ADD_EXECUTABLE(VectorKernelPCA VectorKernelPCA.cxx)
TARGET_LINK_LIBRARIES(VectorKernelPCA ${ITK_LIBRARIES}) 

