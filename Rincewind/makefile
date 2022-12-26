.RECIPEPREFIX = >
lib = /home/pipecaniza/Source/GitHub/RE-flex/lib/libreflex.a 
include = /home/pipecaniza/Source/GitHub/RE-flex/include

objects = CodeGenerator.o Context.o parser.o lex.yy.o Source.o

rincewind : $(objects)
> c++ -o ./bin/rincewind ./bin/CodeGenerator.o ./bin/Context.o ./bin/lex.yy.o ./bin/parser.o ./bin/Source.o $(lib)

Source.o : Context.h CodeGenerator.h ./generated/lex.yy.h
> c++ -c Source.cpp -o ./bin/Source.o -I$(include)
CodeGenerator.o : CodeGenerator.h Context.h Types.h
> c++ -c CodeGenerator.cpp -o ./bin/CodeGenerator.o
Context.o : Context.h Types.h Resources.h ./generated/parser.h ./generated/location.hh
> c++ -c Context.cpp -o ./bin/Context.o
lex.yy.o : ./generated/lex.yy.h Context.h
> c++ -c ./generated/lex.yy.cpp -o ./bin/lex.yy.o -I$(include)
parser.o : ./generated/parser.h Context.h Types.h ./generated/lex.yy.h
> c++ -c ./generated/parser.cpp -o ./bin/parser.o -I$(include)


.PHONY : clean
clean :
>rm -r bin