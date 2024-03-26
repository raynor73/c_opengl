#version 330
uniform mat4 mvp_uniform;
in vec2 uv_attribute;
in vec3 position_attribute;
out vec2 uv_varying;
void main() {
	gl_Position = mvp_uniform * vec4(position_attribute, 1.0);
	uv_varying = uv_attribute;
};
