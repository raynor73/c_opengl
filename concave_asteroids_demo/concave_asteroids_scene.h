#ifndef ILAPIN_CONCAVE_ASTEROIDS_SCENE_H
#define ILAPIN_CONCAVE_ASTEROIDS_SCENE_H

#include <GLFW/glfw3.h>

void concave_asterodis_scene_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void concave_asterodis_scene_start(void);
void concave_asteroids_scene_update(GLFWwindow *window, float dt);

#endif
