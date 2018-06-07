# -*- coding: utf-8 -*-
from __future__ import print_function
from os import sys

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.', file=sys.stderr)
    print('Please run:', file=sys.stderr)
    print('', file=sys.stderr)
    print('  python -m pip install scikit-build')
    sys.exit(1)

setup(
    name='itk-principalcomponentsanalysis',
    version='1.0.0',
    author='Michael Bowers and Laurent Younes',
    author_email='mbowers@cis.jhu.edu',
    packages=['itk'],
    package_dir={'itk': 'itk'},
    download_url=r'https://github.com/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis',
    description=r'ITK classes for the analysis of the principal components of data sets, optionally point data associated with the vertices of a mesh.',
    long_description='ITKPrincipalComponentsAnalysis provides an'
                     'implementation of standard principal components analysis'
                     'algorithms for use on scalar or vector data sets.\n'
                     'Please refer to:\n'
                     'M. Bowers and L. Younes, “Principal Components Analysis of Scalar, Vector, and Mesh Vertex Data.”,'
                     'Insight Journal, January-December 2012, http://hdl.handle.net/10380/3386',
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Healthcare Industry",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Medical Science Apps.",
        "Topic :: Scientific/Engineering :: Information Analysis",
        "Topic :: Software Development :: Libraries",
        "Operating System :: Android",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Operating System :: Unix",
        "Operating System :: MacOS"
        ],
    license='Apache',
    keywords='ITK PCA Mesh',
    url=r'https://github.com/InsightSoftwareConsortium/ITKPrincipalComponentsAnalysis',
    install_requires=[
        r'itk'
    ]
    )