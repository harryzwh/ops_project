#!/bin/bash

mkdir -p build
cd build
cmake ../
make

#scp libOPS_SCHED.so enb@oai-enb.5g.lab:~/openairinterface5g/cmake_targets/lte_build_oai/cmake-build-release/