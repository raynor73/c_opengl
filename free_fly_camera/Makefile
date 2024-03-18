SRC = $(shell find . -type f -name "*.c")
OBJ = $(SRC:%.c=%.o)

main: $(OBJ)
	gcc -g -Wall -o main $(OBJ) -lm `pkg-config --libs glib-2.0 glfw3 cglm assimp`

clean:
	rm main
	find . -type f -name "*.o" | xargs rm
	
%.o: %.c
	gcc -g -DDEBUG -Wall -c -I. -I./glad/include `pkg-config --cflags glib-2.0 glfw3 cglm assimp` $< -o $@
