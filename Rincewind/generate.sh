#!/bin/bash

export PATH="$HOME/Source/GitHub/RE-flex/bin:$PATH"

echo "Cleaning generated"
rm -r ./generated

echo "Generating files..."
mkdir generated
bison -o parser.cpp parser.y
reflex −−header-file −−flex --bison --reentrant −−bison-complete −−bison-locations lexer.l
mv ./lex.yy.cpp ./generated/lex.yy.cpp
mv ./lex.yy.h ./generated/lex.yy.h
mv ./location.hh ./generated/location.hh
mv ./parser.cpp ./generated/parser.cpp
mv ./parser.h ./generated/parser.h