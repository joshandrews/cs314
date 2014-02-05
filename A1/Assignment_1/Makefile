
CC = g++
LFLAGS = -lglfw3 -framework IOKit -framework OpenGL -framework GLUT -framework Cocoa -lGLEW -L /Users/joshandrews/Documents/UBC\ Year\ 3/UBC\ Semester\ 2/CPSC\ 314/Libs/glew-1.10.0/lib -I/Users/joshandrews/Documents/UBC\ Year\ 3/UBC\ Semester\ 2/CPSC\ 314/Libs/glm -I/Users/joshandrews/Documents/UBC\ Year\ 3/UBC\ Semester\ 2/CPSC\ 314/Libs/glew-1.10.0/include -I/Users/joshandrews/Documents/UBC\ Year\ 3/UBC\ Semester\ 2/CPSC\ 314/Libs/freeglut-2.8.1/include
CFLAGS = -g $(LFLAGS)

BIN = a1_shaderIntro
CLS = main ControlState IBuffer Mesh ShaderUtils VBuffer WorldState
SRC = $(CLS:%=%.cpp)
OBJ = $(SRC:%.cpp=build/%.o)

default: build
build: $(BIN) 

$(BIN): build/main.o $(OBJ)
	$(CC) $(LFLAGS) -o $@ $^

build/%.o: %.cpp
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *~ $(OBJ) $(BIN) build
