#!/bin/bash
PARPE_ROOT="`dirname \"$0\"`"
PARPE_ROOT="`( cd \"$PARPE_ROOT\" && pwd )`"

set -e

# Build amici
AMICI_DIR=$PARPE_ROOT/deps/AMICI/
cd $AMICI_DIR
scripts/buildAll.sh

# build cpputest without memory leak detector
$PARPE_ROOT/ThirdParty/installCpputest.sh

# build parpe
mkdir -p $PARPE_ROOT/build
cd $PARPE_ROOT/build
CC=mpicc CXX=mpiCC cmake \
      -DIPOPT_DIR=${PARPE_ROOT}/ThirdParty/Ipopt-3.12.9/install \
      -DCERES_LIBRARIES="${PARPE_ROOT}/ThirdParty/ceres-solver-1.13.0/build/install/lib/libceres.a;`pkg-config --libs blas`;cxsparse;lapack;cholmod;camd;colamd" \
      -DCERES_INCLUDE_DIRS="${PARPE_ROOT}/ThirdParty/ceres-solver-1.13.0/build/install/include/;${PARPE_ROOT}/ThirdParty/ceres-solver-1.13.0/build/install/include/ceres/internal/miniglog/;${PARPE_ROOT}/ThirdParty/eigen-eigen-67e894c6cd8f/build/install/include/eigen3/" \
      -DMPI_INCLUDE_DIR=/usr/include/openmpi-x86_64/ \
      -DMPI_LIBRARY=/usr/lib64/openmpi/lib/libmpi_cxx.so \
      -DCppUTest_DIR=${PARPE_ROOT}/deps/AMICI/ThirdParty/cpputest-master/build-noleakcheck/install/ \
      -DENABLE_SWIG=FALSE \
      $PARPE_ROOT
make -j12

# run tests
make test