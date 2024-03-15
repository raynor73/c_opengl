main:
	gcc -DDEBUG -Wall -o main main.c logger.c opengl_error_detector.c glad/src/gl.c -I. -I./glad/include -lm `pkg-config --cflags --libs glib-2.0 glfw3 cglm`

clean:
	rm main
