#!/usr/bin/env bash
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' "cmake"|grep "install ok installed")
if [ "" = "$PKG_OK" ]
then
  echo "You need to install cmake first! (sudo apt install cmake)"
else
  mkdir build
  cd build
  cmake ..
  cmake --build .
fi