#!/bin/bash

mkdir build
cd build
cmake ..
make
cp -f libws.so ../target
