#ifndef ILAPIN_PERSPECTIVE_CAMERA_H
#define ILAPIN_PERSPECTIVE_CAMERA_H

#include "camera.h"

typedef struct PerspectiveCamera PerspectiveCamera;

PerspectiveCamera *perspective_camera_new(float fov, float aspect_ratio, float near, float far);
void perspective_camera_deleta(PerspectiveCamera *camera);
Camera *get_abstract_camera(PerspectiveCamera *camera);

#endif
