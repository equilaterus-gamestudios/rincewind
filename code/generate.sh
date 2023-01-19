#!/bin/bash

export PATH="$HOME/source/GitHub/RE-flex/bin:$PATH" #change this to point to your reflex location

echo "Cleaning generated files..."
rm -r ./generated

echo "Generating files..."
mkdir generated
bison -o parser.cpp parser.y
reflex −−flex --bison --reentrant −−bison-complete −−bison-locations lexer.l
mv ./lex.yy.cpp ./generated/lex.yy.cpp
mv ./location.hh ./generated/location.hh
mv ./parser.cpp ./generated/parser.cpp
mv ./parser.h ./generated/parser.h