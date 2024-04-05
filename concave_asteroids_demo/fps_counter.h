#ifndef ILAPIN_FPS_COUNTER_H
#define ILAPIN_FPS_COUNTER_H

#include <stdlib.h>

typedef struct RingBuffer {
	void *data;
	size_t buffer_size;
	uint current_index;
} RingBuffer;

//RingBuffer *ring_buffer_new(size_t buffer_size);
void ring_buffer_init(RingBuffer *ring_buffer, size_t size);
float calculate_fps(RingBuffer *fps_values, float dt);

#endif
