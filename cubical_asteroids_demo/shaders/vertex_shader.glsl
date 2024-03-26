#version 330
uniform mat4 MVP;
uniform mat4 model_matrix;
in vec2 uv;
in vec3 vPos;
in vec3 normal;
out vec3 normal_varying;
out vec2 uv_varying;
void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
	 normal_varying = (model_matrix * vec4(normal, 0.0)).xyz;
	 uv_varying = uv;
};
