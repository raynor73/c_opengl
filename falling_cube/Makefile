SRC = $(shell find . -type f -name "*.c")
OBJ = $(SRC:%.c=%.o)

all: main

main: $(OBJ) bullet_physics.o
	g++ -g -Wall -o main $(OBJ) bullet_physics.o -lm `pkg-config --libs glib-2.0 glfw3 cglm assimp bullet`

clean:
	rm main &> /dev/null
	find . -type f -name "*.o" | xargs rm

bullet_physics.o: bullet_physics.cpp
	g++ -Wall -g -DDEBUG -c -I. `pkg-config --cflags cglm bullet` bullet_physics.cpp -o bullet_physics.o
	
%.o: %.c
	gcc -g -DDEBUG -Wall -c -I. -I./glad/include `pkg-config --cflags glib-2.0 glfw3 cglm assimp` $< -o $@
