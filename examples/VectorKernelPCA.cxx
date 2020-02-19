/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
=========================================================================*/

#include "itkMesh.h"
#include "itkMeshFileReader.h"
#include "itkMeshFileWriter.h"
#include "itkVectorFieldPCA.h"
#include "itkTriangleCell.h"
#include "vnl/vnl_vector.h"

int
showUsage(const char * programName)
{
  std::cerr << "USAGE:  " << programName << " <pcaCount> <kernelSigma> " << std::endl;
  std::cerr << "<vtk_mesh_file> <outputName> <vectorFieldSetFile>" << std::endl;
  std::cerr << "\t\tpcaCount : number of principal components to calculate" << std::endl;
  std::cerr << "\t\tkernelSigma : KernelSigma" << std::endl;
  return EXIT_FAILURE;
}

int
main(int argc, char * argv[])
{
  // Required arguments

#define MIN_ARG_COUNT 6
  if (argc < MIN_ARG_COUNT)
  {
    return (showUsage(argv[0]));
  }

  int    pcaCount = std::stoi(argv[1]);
  double kernelSigma = std::stod(argv[2]);

  using PointDataType = double;
  using PointDataVectorType = itk::Array<PointDataType>;
  using PixelType = PointDataVectorType;
  using OutPointDataVectorType = itk::Vector<PointDataType>;
  using OutPixelType = OutPointDataVectorType;

  using CoordRep = double;
  const unsigned int Dimension = 3;

  //    using PCAResultsType = float;
  using PCAResultsType = double;

  // Declare the type of the input mesh
  using InMeshType = itk::Mesh<PixelType, Dimension>;
  using OutMeshType = itk::Mesh<OutPixelType, Dimension>;

  // Declare the type of the kernel function class
  using KernelType = itk::GaussianDistanceKernel<CoordRep>;

  // Declare the type of the PCA calculator
  using PCACalculatorType =
    itk::VectorFieldPCA<PointDataType, PCAResultsType, PixelType, CoordRep, KernelType, InMeshType>;

  // Here we recover the file names from the command line arguments
  const char * inMeshFile = argv[3];
  const char * outFileNameBase = argv[4];

  //  We can now instantiate the types of the reader/writer.
  using ReaderType = itk::MeshFileReader<InMeshType>;
  using WriterType = itk::MeshFileWriter<OutMeshType>;

  // create readers/writers
  ReaderType::Pointer meshReader = ReaderType::New();
  WriterType::Pointer meshWriter = WriterType::New();

  //  The name of the file to be read or written is passed with the
  //  SetFileName() method.
  meshReader->SetFileName(inMeshFile);

  try
  {
    meshReader->Update();
  }
  catch (itk::ExceptionObject & excp)
  {
    std::cerr << "Error reading mesh file " << inMeshFile << std::endl;
    std::cerr << excp << std::endl;
  }

  // get the objects
  InMeshType::Pointer mesh = meshReader->GetOutput();

  std::cout << "Vertex Count:  " << mesh->GetNumberOfPoints() << std::endl;
  std::cout << "Cell Count:  " << mesh->GetNumberOfCells() << std::endl;

  const char *                       vectorFieldName;
  PCACalculatorType::VectorFieldType vectorField;

  // how many vector field sets?
  unsigned int fieldSetCount = argc - (MIN_ARG_COUNT - 1);

  PCACalculatorType::VectorFieldSetTypePointer vectorFieldSet = PCACalculatorType::VectorFieldSetType::New();

  vectorFieldSet->Reserve(fieldSetCount);

  unsigned int vectorFieldDimension = 0;
  unsigned int vectorFieldCount = 0;
  unsigned int setIx = 0;
  for (int i = MIN_ARG_COUNT - 1; i < argc; i++)
  {
    vectorFieldName = argv[i];
    //  The name of the file to be read or written is passed with the
    //  SetFileName() method.
    meshReader->SetFileName(vectorFieldName);

    try
    {
      meshReader->Update();
    }
    catch (itk::ExceptionObject & excp)
    {
      std::cerr << "Error reading mesh field file " << vectorFieldName << std::endl;
      std::cerr << excp << std::endl;
    }

    // get the objects
    InMeshType::Pointer                   meshWithField = meshReader->GetOutput();
    InMeshType::PointDataContainerPointer pointData = meshWithField->GetPointData();

    // should know vector field dimensions now
    if (setIx == 0)
    {
      vectorFieldCount = pointData->Size();
      if (vectorFieldCount)
      {
        PixelType oneDataSetVal = pointData->GetElement(0);
        vectorFieldDimension = oneDataSetVal.size();
      }
      if (vectorFieldCount != meshWithField->GetNumberOfPoints())
      {
        std::cerr << "Vector field count (" << vectorFieldCount << ") doesn't match mesh vertext count ("
                  << meshWithField->GetNumberOfPoints() << ")." << std::endl;
        exit(EXIT_FAILURE);
      }
      vectorField.set_size(vectorFieldCount, vectorFieldDimension);
    }
    else
    {
      if (vectorFieldDimension != vectorField.cols() || vectorFieldCount != vectorField.rows())
      {
        std::cerr << "Unexpected dimensions in vector field file " << vectorFieldName << std::endl;
        std::cerr << "\tExpected " << vectorFieldCount << " x " << vectorFieldDimension;
        std::cerr << "\t, got " << vectorField.rows() << " x " << vectorField.cols() << std::endl;
        exit(1);
      }
    }

    for (unsigned int k = 0; k < pointData->Size(); k++)
    {
      PixelType oneDataSetVal = pointData->GetElement(k);
      vectorField.set_row(k, oneDataSetVal);
    }

    vectorFieldSet->SetElement(setIx++, vectorField);
  }

  PCACalculatorType::Pointer pcaCalc = PCACalculatorType::New();

  // set user variables
  KernelType::Pointer distKernel = KernelType::New();
  distKernel->SetKernelSigma(kernelSigma);
  pcaCalc->SetKernelFunction(distKernel);
  pcaCalc->SetComponentCount(pcaCount);

  //
  //  Now connect the input and verify that it runs fine.
  //
  pcaCalc->SetPointSet(mesh);

  // set vector fields
  pcaCalc->SetVectorFieldSet(vectorFieldSet);

  try
  {
    pcaCalc->Compute();
  }
  catch (itk::ExceptionObject & excp)
  {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  // copy the mesh for output so the point data is of type VECTORS
  // instead of COLOR_SCALARS
  OutMeshType::Pointer                     outMesh = OutMeshType::New();
  InMeshType::PointsContainerPointer       points = mesh->GetPoints();
  InMeshType::PointsContainerConstIterator it = points->Begin();
  InMeshType::PointsContainerConstIterator itEnd = points->End();
  unsigned int                             ixPt = 0;
  while (it != itEnd)
  {
    InMeshType::PointType point = it.Value();
    outMesh->SetPoint(ixPt++, point);
    it++;
  }

  ixPt = 0;
  InMeshType::CellsContainerPointer       cells = mesh->GetCells();
  InMeshType::CellsContainerConstIterator itCells = cells->Begin();
  InMeshType::CellsContainerConstIterator itCellsEnd = cells->End();
  outMesh->GetCells()->Reserve(cells->Size());

  using CellAutoPointer = OutMeshType::CellType::CellAutoPointer;
  using TriangleType = itk::TriangleCell<OutMeshType::CellType>;

  while (itCells != itCellsEnd)
  {
    CellAutoPointer outCell;
    outCell.TakeOwnership(new TriangleType);
    outCell->SetPointIds(itCells.Value()->PointIdsBegin(), itCells.Value()->PointIdsEnd());
    outMesh->SetCell(ixPt++, outCell);
    itCells++;
  }

  char          fName[1024];
  std::ofstream debugOut;
  debugOut.precision(15);

  // get the output and write to files
  sprintf(fName, "%s_BasisVectors.txt", outFileNameBase);
  debugOut.open(fName);
  for (unsigned int j = 0; j < pcaCalc->GetComponentCount(); j++)
  {
    (pcaCalc->GetBasisVectors()->GetElement(j)).print(debugOut);
  }

  debugOut.close();

  sprintf(fName, "%s_PCAEigenValues.txt", outFileNameBase);
  debugOut.open(fName);
  debugOut << pcaCalc->GetPCAEigenValues();
  debugOut.close();

  sprintf(fName, "%s_AveVectorField.txt", outFileNameBase);
  debugOut.open(fName);
  pcaCalc->GetAveVectorField().print(debugOut);
  debugOut.close();

  PCACalculatorType::MatrixType averages = pcaCalc->GetAveVectorField();

  PixelType oneAverage;
  for (unsigned int k = 0; k < averages.rows(); k++)
  {
    oneAverage = averages.get_row(k);
    mesh->SetPointData(k, oneAverage);
  }

  //  The name of the file to be read or written is passed with the
  //  SetFileName() method.
  char outFileName[2048];
  std::strcpy(outFileName, outFileNameBase);
  std::strcat(outFileName, "Ave.vtk");

  meshWriter->SetFileName(outFileName);
  meshWriter->SetInput(outMesh);

  try
  {
    meshWriter->Update();
  }
  catch (itk::ExceptionObject & excp)
  {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  // set the basis functions
  for (unsigned int j = 0; j < pcaCalc->GetComponentCount(); j++)
  {
    PCACalculatorType::MatrixType basisFunction = pcaCalc->GetBasisVectors()->GetElement(j);
    PixelType                     oneFunctionVal;
    OutPixelType                  oneVectFunctionVal;
    for (unsigned int k = 0; k < basisFunction.rows(); k++)
    {
      oneFunctionVal = basisFunction.get_row(k);
      oneVectFunctionVal[0] = oneFunctionVal[0];
      oneVectFunctionVal[1] = oneFunctionVal[1];
      oneVectFunctionVal[2] = oneFunctionVal[2];
      outMesh->SetPointData(k, oneVectFunctionVal);
    }
    //  The name of the file to be read or written is passed with the
    //  SetFileName() method.
    std::strcpy(outFileName, outFileNameBase);
    std::strcat(outFileName, "Basis");
    char fileCount[8];
    sprintf(fileCount, "%d", j + 1);
    std::strcat(outFileName, fileCount);
    std::strcat(outFileName, ".vtk");

    meshWriter->SetFileName(outFileName);
    meshWriter->SetInput(outMesh);

    try
    {
      meshWriter->Update();
    }
    catch (itk::ExceptionObject & excp)
    {
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
