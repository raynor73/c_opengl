#ifndef ILAPIN_SPACE_BOX_CONTROLLER_H
#define ILAPIN_SPACE_BOX_CONTROLLER_H

#include "bullet_physics.h"

typedef struct SpaceBoxController SpaceBoxController;

SpaceBoxController *space_box_controller_new(btRigidBody *rigid_body);
void space_box_controller_on_key_event(SpaceBoxController *controller, int key, int scancode, int action, int mods);
void space_box_controller_update(SpaceBoxController *controller);
void space_box_controller_delete(SpaceBoxController *controller);

#endif
