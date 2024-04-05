#include "fps_counter.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

/*RingBuffer *ring_buffer_new(size_t buffer_size) {
	
}*/
void ring_buffer_init(RingBuffer *ring_buffer, size_t size) {
	ring_buffer->data = malloc(size);
	if (!ring_buffer->data) {
		error("Failed to allocate memory for ring buffer\n");
	}
	memset(ring_buffer->data, 0, size);
	ring_buffer->buffer_size = size;
	ring_buffer->current_index = 0;
}

float calculate_fps(RingBuffer *fps_values, float dt) {
	float *fps_values_data = (float *) fps_values->data;
	fps_values_data[fps_values->current_index] = 1 / dt;
	fps_values->current_index++;
	if ((fps_values->current_index + 1) * sizeof(float) >= fps_values->buffer_size) {
		fps_values->current_index = 0;
	}
	
	float sum = 0;
	int number_of_float_items = fps_values->buffer_size / sizeof(float);
	for (int i = 0; i < number_of_float_items; i++) {
		sum += fps_values_data[i];
	}
	return sum / number_of_float_items;
}
