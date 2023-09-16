#!/bin/bash

pushd "$(dirname ${BASH_SOURCE[0]})"
python3 python/Setup.py
popd #$(dirname ${BASH_SOURCE[0]})
