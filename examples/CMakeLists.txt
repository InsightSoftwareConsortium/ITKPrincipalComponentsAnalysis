
#######################
#######################
#
#  Define Executables
#
#######################
#######################

find_package(ITK REQUIRED COMPONENTS PrincipalComponentsAnalysis)

add_executable(VectorKernelPCA VectorKernelPCA.cxx )
target_link_libraries(VectorKernelPCA ITK::PrincipalComponentsAnalysisModule)

