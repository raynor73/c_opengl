#version 330
struct DirectionalLight {
    vec3 color;
    vec3 direction;
};
uniform DirectionalLight directional_light;
in vec3 normal_varying;
in vec2 uv_varying;
uniform sampler2D texture_uniform;
out vec4 fragment;
void main()
{
	fragment =
            texture2D(texture_uniform, uv_varying) * vec4(directional_light.color, 1.0) *
            dot(normalize(normal_varying), -directional_light.direction);
};
