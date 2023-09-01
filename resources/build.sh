#!/bin/bash

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

convert_input_file=blockstateidtocolor.txt
convert_output_file=stateToColor.h
echo COMPILING HEADER GENERATOR
g++ "${script_dir}"/main.cpp -o "${script_dir}"/convert.out
echo GENERATING HEADERS
"${script_dir}"/convert.out "$script_dir"/$convert_input_file "$script_dir"/$convert_output_file
echo MOVING HEADERS
cp "$script_dir"/$convert_output_file "$script_dir"/../src/lookup/stateToColor.h