#version 330

uniform mat4 model_xform;
uniform mat4 combined_xform;

in vec3 position;
in vec3 normal;
in vec2 texture_coord;

out vec3 world_normal;
out vec2 text_coord;
out vec3 world_position;

void main(void)
{
	text_coord = texture_coord;
	world_normal = mat3(model_xform) * normal;
	world_position = mat4x3(model_xform) * vec4(position, 1.0);
    gl_Position = combined_xform * vec4(position, 1.0);
}
