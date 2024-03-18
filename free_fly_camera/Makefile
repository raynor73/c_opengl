SRC = $(shell find . -type f -name "*.c")

main: $(SRC)
	gcc -g -DDEBUG -Wall -o main $(SRC) -I. -I./glad/include -lm `pkg-config --cflags --libs glib-2.0 glfw3 cglm assimp`

clean:
	rm main
