.RECIPEPREFIX = >
lib = /home/pipecaniza/source/GitHub/RE-flex/lib/libreflex.a #change this with your reflex lib location
include = /home/pipecaniza/source/GitHub/RE-flex/include #change this with your reflex include location

FLAGS = -D DEBUG=1 -std=c++11 -stdlib=libstdc++ -g

rincewind : 
> clang++ $(FLAGS) rincewind.cpp -o ./bin/rincewind  $(lib) -I$(include)

.PHONY : clean
clean :
>rm -r bin