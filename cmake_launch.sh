#!/bin/bash

usage() {
 cat << EOF

 usage: `basename $0` [options]

 This script set the appropriate compiler envinonment for CMake, and launches cmake in it.

 OPTIONS:
   -h      Show this message
   -g      Prepare environment for GCC toolchain.
   -l      Prepare environment for CLANG toolchain
   -p      Path to CMakeLists.txt
EOF
}

CC=
CXX=
LD=
CMAKEPATH=.
SANITY=0

while getopts ":p:hgl" opt; do
    case $opt in
     g)
        CC=
	CXX=
	SANITY==$(($SANITY+1))
        ;;
     l)
        CC=
	CXX=
	SANITY==$(($SANITY+1))
        ;;
     p)
	CMAKEPATH=$OPTARG
        ;;
     h)
	usage
	exit 0
        ;;
    \?)
        usage
        exit 1
      ;;
  esac
done


if [[ $SANITY -ne 1 ]];
then
	usage()
	exit 1
fi

export $CC
export $CXX
export $LD
cd $CMAKEPATH
cmake .
