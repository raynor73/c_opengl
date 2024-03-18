main: main.c
	gcc -g -DDEBUG -Wall -o main main.c logger.c opengl_error_detector.c mesh.c glad/src/gl.c libbmp/libbmp.c -I. -I./glad/include -lm `pkg-config --cflags --libs glib-2.0 glfw3 cglm assimp`

clean:
	rm main
