
# OS X
ifeq ("$(shell uname)", "Darwin")
	LFLAGS = -lglfw3 -framework CoreVideo -framework Cocoa -framework OpenGL -framework IOKit -lGLEW
# Linux
else
	LFLAGS = -lGL -lfreeglut -lGLEW
endif

CC = g++
#LFLAGS = -lGL -lfreeglut -lGLEW
CFLAGS = -g $(LFLAGS)

BIN = a2_Deform
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
