#!/bin/bash
echo
echo "Building Rincewind..."
echo

echo "Cleaning..."
make clean

sh ./generate.sh

echo "Compiling..."
mkdir bin
make

echo "Cleaning..."
rm ./bin/lexer.o
rm ./bin/parser.o

echo "Done"