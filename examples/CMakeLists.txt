
#######################
#######################
#
#  Define Executables
#
#######################
#######################

find_package(ITK REQUIRED COMPONENTS PrincipalComponentsAnalysis)

include_directories( ${PrincipalComponentsAnalysis_SOURCE_DIR}/Source )

add_executable(VectorKernelPCA VectorKernelPCA.cxx )
target_link_libraries(VectorKernelPCA ${ITK_LIBRARIES})

