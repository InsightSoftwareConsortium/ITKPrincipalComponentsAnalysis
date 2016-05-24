/*=========================================================================
 *
 *  Copyright Insight Software Consortium
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

#ifndef itkVectorFieldPCA_hxx
#define itkVectorFieldPCA_hxx

#include "itkVectorFieldPCA.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_c_vector.h"
#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Constructor
 */
template<
    typename TVectorFieldElementType,
    typename TPCType,
    typename TPointSetPixelType,
    typename TPointSetCoordRepType,
    typename KernelFunctionType,
    class    TPointSetType
  >
VectorFieldPCA<TVectorFieldElementType, TPCType, TPointSetPixelType, TPointSetCoordRepType, KernelFunctionType, TPointSetType>
::VectorFieldPCA()
{
  m_PCACalculated = false;
  m_SetSize = 0;
  m_VectorDimCount = 0;
  m_PointDim = 0;
  m_ComponentCount = 0;
  m_KernelFunction = NULL;
  m_BasisVectors = BasisSetType::New();
  m_VectorFieldSet = NULL;
  m_PointSet = NULL;
}

/**
 *
 */
template<
    typename TVectorFieldElementType,
    typename TPCType,
    typename TPointSetPixelType,
    typename TPointSetCoordRepType,
    typename KernelFunctionType,
    class    TPointSetType
  >
void
VectorFieldPCA<TVectorFieldElementType, TPCType, TPointSetPixelType, TPointSetCoordRepType, KernelFunctionType, TPointSetType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  if (this->m_VectorFieldSet.IsNull())
    {
    os << indent << "Vector Field Set Empty" << std::endl;
    }
  else
    {
    os << indent << "Vector Field Set Count:  " << this->m_VectorFieldSet->Size() << std::endl;
    }

  os << indent << "Component Count:  " << this->m_ComponentCount << std::endl;
  os << indent << "Eigenvalues: " << this->m_PCAEigenValues << std::endl;
  if (this->m_BasisVectors.IsNull())
    {
    os << indent << "Basis Vector Empty" << std::endl;
    }
  else
    {
    os << indent << "Basis Vector Count:  " << this->m_BasisVectors->Size() << std::endl;
    if (this->m_BasisVectors->Size())
      {
      MatrixType& thisBasis = this->m_BasisVectors->ElementAt(0);
      os << indent << "Basis Vector Dimensions:  "  << thisBasis.rows() << "x" << thisBasis.cols() << std::endl;
      }
    }

  if (this->m_PointSet.IsNull())
    {
    os << indent << "PointSet Empty" << std::endl;
    }
  else
    {
    os << indent << "PointSet is " << m_PointSet->GetNumberOfPoints() <<
         "x" << this->m_PointSet->PointDimension << std::endl;
    }

  if (this->m_KernelFunction.IsNull())
    {
    os << indent << "Kernel Function not set" << std::endl;
    }
  else
    {
    os << indent << "KernelFunction is set." << std::endl;
    }
}

/**
 * Compute the principal components
 */
template<
    typename TVectorFieldElementType,
    typename TPCType,
    typename TPointSetPixelType,
    typename TPointSetCoordRepType,
    typename KernelFunctionType,
    class    TPointSetType
  >
void
VectorFieldPCA<TVectorFieldElementType, TPCType, TPointSetPixelType, TPointSetCoordRepType, KernelFunctionType, TPointSetType>
::Compute(void)
{
  // check parameters
  if (!m_VectorFieldSet || !m_VectorFieldSet->Size())
    {
    itkExceptionMacro("Vector Field Set not specified.");
    return;
    }

  m_SetSize = m_VectorFieldSet->Size();
  if (m_ComponentCount <= 0 || m_ComponentCount > m_SetSize)
    {
    itkExceptionMacro("Component Count N must be 0 < N <= VectorFieldSetSize ("
      << m_VectorFieldSet->Size() << ").");
    return;
    }

  // get vector/point dim from the first member of the vector set

  VectorFieldType& firstField = m_VectorFieldSet->ElementAt(0);
  m_VectorDimCount = firstField.rows();
  m_PointDim = firstField.cols();

  // check all vector dimensions in the set
  for (unsigned int i = 1; i < m_VectorFieldSet->Size(); i++)
    {
    VectorFieldType& thisField = m_VectorFieldSet->ElementAt(i);
    if (thisField.rows() != m_VectorDimCount ||
        thisField.cols() != m_PointDim)
      {
      itkExceptionMacro("Vector " << i << " dimensions (" <<
        thisField.rows() << "x" <<
        thisField.cols() <<
        ") does not match other vector fields dimensions ("
        << m_VectorDimCount << "x" << m_PointDim << ").");
      return;
      }
    }

  if (m_KernelFunction)
    {
    // will try Kernel PCA, so need a point set...
    if (!m_PointSet)
      {
      itkExceptionMacro(
        "KernelFunction is set but no PointSet is available.");
      return;
      }

    //  PointSet only necessary for Kernel PCA, but check that it matches if set...
    if (m_PointSet)
      {
      if (m_PointSet->GetNumberOfPoints() != m_VectorDimCount)
        {
        itkExceptionMacro("Point Set count (" <<
          m_PointSet->GetNumberOfPoints() <<
          ") does not match vector field count ("
          << m_VectorDimCount << ").");
        return;
        }
      }
    }

  computeMomentumSCP();
  KernelPCA();

  // save only the desired eigenvalues
  m_PCAEigenValues = m_PCAEigenValues.extract(m_ComponentCount);

  // save only the desired eigenvectors
  m_V0 = m_V0.extract(m_V0.rows(), m_ComponentCount);

  m_BasisVectors->Reserve(m_ComponentCount);
  for (unsigned int k = 0; k < m_ComponentCount; k++)
    {
    MatrixType basisVector(m_VectorDimCount, m_PointDim);
    VectorFieldType accum(m_VectorDimCount, m_PointDim);
    accum = 0.0;
    basisVector = 0.0;
    for (unsigned int j = 0; j < m_SetSize; j++)
      {
      vnl_c_vector<TVectorFieldElementType>::saxpy(
        m_V0(j, k),
        (m_VectorFieldSet->ElementAt(j)).data_block(),
        accum.data_block(), accum.size());
      }

    for (unsigned int i = 0; i < accum.size(); ++i)
      basisVector.begin()[i] = TPCType(accum.begin()[i]);
    m_BasisVectors->SetElement(k, basisVector);
    }

  m_PCAEigenValues /= m_SetSize;
  m_PCAEigenValues = m_PCAEigenValues.apply(sqrt);

  m_PCACalculated = true;
}


/**
 * Compute Momentum SCP
 */
template<
    typename TVectorFieldElementType,
    typename TPCType,
    typename TPointSetPixelType,
    typename TPointSetCoordRepType,
    typename KernelFunctionType,
    class    TPointSetType
>
void
VectorFieldPCA<TVectorFieldElementType, TPCType, TPointSetPixelType, TPointSetCoordRepType, KernelFunctionType, TPointSetType>
::computeMomentumSCP(void)
{

  VectorFieldType accum;
  accum.set_size(m_VectorDimCount, m_PointDim);
  accum = 0.0;

  // determine the average of the vector field over the set
  for (unsigned k = 0; k < m_SetSize; k++)
    {
    accum += m_VectorFieldSet->ElementAt(k);
    }
  accum /= (double) m_SetSize;

  m_AveVectorField.set_size(m_VectorDimCount, m_PointDim);

  for (unsigned int i = 0; i < accum.size(); ++i)
    m_AveVectorField.begin()[i] = TPCType(accum.begin()[i]);


  MatrixType kernelM(m_VectorDimCount, m_VectorDimCount);

  // check whether we're doing kernel PCA
  if (!m_KernelFunction.IsNull())
    {
    unsigned k1, l1;
    k1 = 0;
    for (PointsContainerIterator kIx = m_PointSet->GetPoints()->Begin();
           kIx != m_PointSet->GetPoints()->End();
           kIx++)
      {
      l1 = 0;
      for (PointsContainerIterator lIx = m_PointSet->GetPoints()->Begin();
           lIx != m_PointSet->GetPoints()->End();
           lIx++)
        {
        kernelM(k1, l1) = m_KernelFunction->Evaluate((kIx.Value()).SquaredEuclideanDistanceTo(lIx.Value()));
        kernelM(l1, k1) = kernelM(k1, l1);
        l1++;
        }
      k1++;
      }
    }

  m_K.set_size(m_SetSize, m_SetSize);
  MatrixType alphaK(m_VectorDimCount, m_PointDim);
  MatrixType alphaL(m_VectorDimCount, m_PointDim);
  MatrixType tmpA;
  for (unsigned k = 0; k < m_SetSize; k++)
    {
    for (unsigned l = k; l < m_SetSize; l++)
      {
      for (unsigned int i = 0; i < alphaK.size(); ++i)
        {
        alphaK.begin()[i] = TPCType(m_VectorFieldSet->ElementAt(k).begin()[i]);
        }
      for (unsigned int i = 0; i < alphaL.size(); ++i)
        {
        alphaL.begin()[i] = TPCType(m_VectorFieldSet->ElementAt(l).begin()[i]);
        }

      if (m_KernelFunction)
        {
        tmpA = kernelM * (alphaL - m_AveVectorField);
        }
      else
        {
        tmpA = alphaL - m_AveVectorField;
        }

      MatrixType tmpB = alphaK - m_AveVectorField;
      m_K(k, l) =
        vnl_c_vector<TPCType>::dot_product(
            tmpA.data_block(), tmpB.data_block(), tmpA.size());
      m_K(l, k) = m_K(k, l);
      }
    }
}

/**
 * Kernel PCA
 */
template<
    typename TVectorFieldElementType,
    typename TPCType,
    typename TPointSetPixelType,
    typename TPointSetCoordRepType,
    typename KernelFunctionType,
    class    TPointSetType
>
void
VectorFieldPCA<TVectorFieldElementType, TPCType, TPointSetPixelType, TPointSetCoordRepType, KernelFunctionType, TPointSetType>
::KernelPCA(void)
{
  VectorType rowMeans(m_SetSize);

  unsigned int k, l;
  for (k = 0; k < m_SetSize; k++)
    {
    rowMeans(k) = m_K.get_row(k).mean();
    }

  TPCType meanOfMeans = rowMeans.mean();
  MatrixType K0(m_K - meanOfMeans);
  for (k = 0; k < m_SetSize; k++)
    {
    for (l = 0; l < m_SetSize; l++)
      {
      K0(k, l) -= rowMeans(k) + rowMeans(l);
      }
    }

  vnl_symmetric_eigensystem<TPCType> eigs(K0);

  m_PCAEigenValues = eigs.D.diagonal();
  // eigs come out in ascending order, reorder them
  m_PCAEigenValues.flip();

  // reorder eigenvectors
  m_V0 = eigs.V;
  m_V0.fliplr();

  const double eigenvalue_epsilon  = 1.0e-10;
  for (k = 0; k < m_SetSize; k++)
    {
    m_V0.scale_column(k, 1.0 / vcl_sqrt(m_PCAEigenValues(k) + eigenvalue_epsilon));
    }
}

} // end namespace itk

#endif