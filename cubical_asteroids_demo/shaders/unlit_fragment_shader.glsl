#version 330
in vec2 uv_varying;
uniform sampler2D texture_uniform;
uniform vec2 texture_scale_uniform;
out vec4 fragment;
void main() {
	fragment = texture2D(texture_uniform, uv_varying * texture_scale_uniform);
};
