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
LD=/usr/bin/ld
CMAKEPATH=.
GCC=
CLANG=
CLEAN=

while getopts ":p:hglc" opt; do
    case $opt in
     g)
        CC=/usr/bin/llvm-gcc
	CXX=/usr/bin/llvm-g++
	GCC=1
        ;;
     l)
        CC=
	CXX=
	CLANG=1
        ;;
     p)
	CMAKEPATH=$OPTARG
        ;;
     c)
	CLEAN=1
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

if [[ $(uname) -ne "Darwin" ]]; then
	echo "This script is meant for OS X. You do not need to run it on linux."
	exit 0
fi

if [[ $GCC -eq 1 && $CLANG -eq 1 ]];
then
	usage
	exit 1
fi

echo "Exporting compiler environment variables..."
export CC=$CC
export CXX=$CXX
export LD=$LD
cd $CMAKEPATH

if [[ ! -z $CLEAN  ]];
then
	echo "Cleaning existing cmake residue..."
	for dir in $(find . -type d -name "CMakeFiles");
	do
		rm -rf $dir 2>/dev/null
	done
	for cache in $(find . -type f -name "CMakeCache.txt");
	do
		rm $cache 2>/dev/null
	done
fi

cmake .
