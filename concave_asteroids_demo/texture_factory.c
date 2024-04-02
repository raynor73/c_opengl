#include "texture_factory.h"
#include <stdio.h>
#include <libbmp/libbmp.h>
#include <stdlib.h>
#include "opengl_error_detector.h"

#define TEXTURE_BYTES_PER_PIXEL 4
GLuint create_texture_from_file(const char *path) {
	bmp_img bitmap;
	bmp_img_read(&bitmap, path);
	uint8_t *bitmap_data = (uint8_t *) malloc(bitmap.img_header.biWidth * bitmap.img_header.biHeight * TEXTURE_BYTES_PER_PIXEL);
	uint32_t bitmap_data_offset = 0;
	for (int y = 0; y < bitmap.img_header.biHeight; y++) {
		for (int x = 0; x < bitmap.img_header.biWidth; x++) {
			bmp_pixel pixel;
			if (x == 0 && y == 0) {
				pixel.red = 255;
				pixel.green = 0;
				pixel.blue = 0;
			} else {
				pixel = bitmap.img_pixels[y][x];
			}
				
			bitmap_data[bitmap_data_offset++] = pixel.red;
			bitmap_data[bitmap_data_offset++] = pixel.green;
			bitmap_data[bitmap_data_offset++] = pixel.blue;
			bitmap_data[bitmap_data_offset++] = 255;
		}
	}
	
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        bitmap.img_header.biWidth,
        bitmap.img_header.biHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        bitmap_data
    );
    
	glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
	
	check_opengl_errors("load texture");
	
	bmp_img_free(&bitmap);
	free(bitmap_data);
	
	return texture;
}
