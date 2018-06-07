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

An `ITK <http://itk.org>`_-based implementation of principal components analysis.
A more detailed description can be found in the Insight Journal article::

  Bowers M., Younes L. "Principal Components Analysis of Scalar, Vector, and Mesh Vertex Data."
    http://hdl.handle.net/10380/3386
    http://www.insight-journal.org/browse/publication/878
    August, 2013.

Since ITK 4.10.0, this module is available in the ITK source tree as a Remote
module.  To enable it, set::

  Module_PrincipalComponentsAnalysis:BOOL=ON

in ITK's CMake build configuration.
