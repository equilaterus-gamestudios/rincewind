.RECIPEPREFIX = >
lib = /home/pipecaniza/source/GitHub/RE-flex/lib/libreflex.a 
include = /home/pipecaniza/source/GitHub/RE-flex/include

objects = parser.o lexer.o
FLAGS = -D DEBUG=1 -std=c++11 -stdlib=libstdc++ -g

rincewind : $(objects)
> clang++ $(FLAGS) rincewind.cpp -o ./bin/rincewind  ./bin/parser.o ./bin/lexer.o $(lib) -I$(include)
parser.o : lexer.o
> clang++ $(FLAGS) -c ./generated/parser.cpp -o ./bin/parser.o -I$(include)
lexer.o : ./generated/lex.yy.h
> clang++ $(FLAGS) -c ./generated/lex.yy.cpp -o ./bin/lexer.o -I$(include)

.PHONY : clean
clean :
>rm -r bin