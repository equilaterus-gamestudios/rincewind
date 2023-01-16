#!/bin/bash
set echo off

echo "Building Rincewind..."
echo "Cleaning..."
make -s clean

sh ./generate.sh

echo "Compiling..."
mkdir bin
make -s

echo "Done"