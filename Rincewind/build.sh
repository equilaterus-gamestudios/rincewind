#!/bin/bash
set echo off
echo "Building Rincewind..."
echo "Cleaning..."
rm -rf bin

sh ./generate.sh

echo "Compiling..."
mkdir bin

lib="/home/pipecaniza/source/GitHub/RE-flex/lib/libreflex.a" #change this with your reflex lib location
include="/home/pipecaniza/source/GitHub/RE-flex/include" #change this with your reflex include location
flags="-std=c++11 -stdlib=libstdc++"
debugFlags="-D DEBUG -g"
profilingFlags="-D PROFILING=1 -fno-exceptions -fno-rtti -pedantic -Wall -Werror -pthreads -fno-omit-frame-pointer"
#profilingFlags="-D PROFILING=1 -fno-exceptions -pedantic -Wall -pthreads -fno-omit-frame-pointer -g"

#clang++ $flags rincewind.cpp -o ./bin/rincewind  $lib -I$include
clang++ $flags $debugFlags rincewind.cpp -o ./bin/rincewind  $lib -I$include
#clang++ $flags $profilingFlags rincewind.cpp -o ./bin/rincewind  $lib -I$include

echo "Done"

#perf stat -B  -e cache-references,cache-misses,cycles,instructions,branches,branch-misses,alignment-faults ./bin/rincewind /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/test2.md /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/output/ output
#perf stat -B dd -e cache-references,cache-misses,cycles,instructions,branches,branch-misses,alignment-faults ./bin/rincewind /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/test2.md /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/output/ output
#perf stat ./bin/rincewind /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/test2.md /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/output/ output
#perf record -g ./bin/rincewind /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/test2.md /home/pipecaniza/source/GitHub/rincewind/Rincewind/examples/output/ output
#perf report -g 'graph,0.5,caller'
