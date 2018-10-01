PrincipalComponentsAnalysis
===========================

.. |CircleCI| image:: https://circleci.com/gh/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis.svg?style=shield
    :target: https://circleci.com/gh/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis

.. |TravisCI| image:: https://travis-ci.org/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis.svg?branch=master
    :target: https://travis-ci.org/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis

.. |AppVeyor| image:: https://img.shields.io/appveyor/ci/itkrobot/itkprincipalcomponentsanalysis.svg
    :target: https://ci.appveyor.com/project/itkrobot/itkprincipalcomponentsanalysis

=========== =========== ===========
   Linux      macOS       Windows
=========== =========== ===========
|CircleCI|  |TravisCI|  |AppVeyor|
=========== =========== ===========

Overview
--------

An `ITK <http://itk.org>`_-based implementation of principal components analysis.

For more information, see the `Insight Journal article <http://hdl.handle.net/10380/3386>`_::

  Bowers M., Younes L.
  Principal Components Analysis of Scalar, Vector, and Mesh Vertex Data
  The Insight Journal. August. 2013.
  http://hdl.handle.net/10380/3386
  http://www.insight-journal.org/browse/publication/878

Installation
------------

Since ITK 4.10.0, this module is available in the ITK source tree as a Remote
module. To enable it, set::

  Module_PrincipalComponentsAnalysis:BOOL=ON

in ITK's CMake build configuration.

License
-------

This software is distributed under the Apache 2.0 license. Please see
the *LICENSE* file for details.
