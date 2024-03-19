#include "perspective_camera.h"
#include <stdlib.h>
#include "logger.h"

typedef struct PerspectiveCamera {
	float fov;
	float aspect_ratio;
	float near;
	float far;
	Camera *abstract_camera;
} PerspectiveCamera;

PerspectiveCamera *perspective_camera_new(float fov, float aspect_ratio, float near, float far) {
	
}

void perspective_camera_deleta(PerspectiveCamera *camera) {
}

Camera *get_abstract_camera(PerspectiveCamera *camera) {
}
