ITKPrincipalComponentsAnalysis
==============================

.. image:: https://github.com/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis/workflows/Build,%20test,%20package/badge.svg

Overview
--------

An `ITK <https://itk.org>`_-based implementation of principal components analysis.

For more information, see the `Insight Journal article <https://hdl.handle.net/10380/3386>`_::

  Bowers M., Younes L.
  Principal Components Analysis of Scalar, Vector, and Mesh Vertex Data
  The Insight Journal. August. 2013.
  https://hdl.handle.net/10380/3386
  https://www.insight-journal.org/browse/publication/878

Installation
------------

Since ITK 4.10.0, this module is available in the ITK source tree as a Remote
module. To enable it, set::

  Module_PrincipalComponentsAnalysis:BOOL=ON

in ITK's CMake build configuration.

For Python packages::

  pip install itk-principalcomponentsanalysis

License
-------

This software is distributed under the Apache 2.0 license. Please see
the *LICENSE* file for details.
