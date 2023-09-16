#!/bin/bash

# Change working directory to the root of the engine.
pushd "$(dirname ${BASH_SOURCE[0]})/../.."

if [ $# -eq 0 ] ; then
  echo "Please specify debug or release build with -d or -r. Build both with -dr."
fi

# Get system architecture
architecture=""
case $(arch) in
  x86_64) architecture="x64" ;;
  armv7l) architecture="arm32" ;;
  arm)    dpkg --print-architecture | grep -q "arm64" && architecture="arm64" || architecture="arm32" ;;
esac

# Loop over debug or release flags
while getopts 'dr' flag
do
  case "${flag}" in
    d) 
      debugarch="debug_${architecture}"
      echo "Building projects $debugarch."
      dependencies/premake/bin/premake5 gmake
      make config=$debugarch
      ;;
    r) 
      releasearch="release_${architecture}"
      echo "Building projects $releasearch."
      dependencies/premake/bin/premake5 gmake
      make config=$releasearch
      ;;
    *) 
      echo "Please specify Debug or Release mode with -d or -r" ;;
  esac
done

popd #$(dirname ${BASH_SOURCE[0]})/..
