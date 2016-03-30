#version 330

struct Light
{
    vec3 position;
    float range;
    vec3 intensity;
};

uniform vec3 camera_position;
uniform vec3 material_colour;
uniform sampler2D shininess_texture;
uniform Light lights[7];
uniform vec3 ambient_intensity;
uniform int specularOn;
uniform int checkered;

in vec3 world_normal;
in vec2 text_coord;
in vec3 world_position;

out vec4 fragment_colour;

// This method works out the intensity of a point light
// Given a boolean, it will add specular as well as diffuse light
vec3 pointSourceIntensity(in Light light, in vec3 source_colour)
{
	vec3 L = normalize(light.position - world_position);
	vec3 N = normalize(world_normal);

	float attenuation = 1 - smoothstep(0.0, light.range - 40.0, distance(world_position, light.position));

	vec3 diffuse_colour = vec3(source_colour * clamp(max(dot(L, N), 0.0), 0.0, 1.0) * 0.05 * light.intensity * attenuation);
	vec3 returned_colour = diffuse_colour;
	
	if(specularOn == 1)
	{
		float specular_intensity = texture2D(shininess_texture, text_coord).r;
		vec3 V = normalize(camera_position - world_position);
		vec3 Rv = reflect(-V, N);

		returned_colour += vec3(vec3(1.0, 1.0, 1.0) * pow(clamp(max(dot(L, Rv), 0.0) * sign(dot(L, N)), 0.0, 1.0), specular_intensity * 16.0) * attenuation);
	}

	return returned_colour;
}

void main(void)
{
	vec3 combined_intensity = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 7; i++)
	{
		combined_intensity += pointSourceIntensity(lights[i], material_colour);
	}
	
	// Creates a checkered effect on the texture
	if(checkered == 1)
	{
		vec3 colour_a = vec3(1.0, 0.0, 0.0);
		vec3 colour_b = vec3(1.0, 1.0, 0.0);
		const float block_size = 0.2;
		vec2 uv = mod(text_coord, block_size) / block_size;
		bool use_a = (uv.x > 0.5) ^^ (uv.y > 0.5);
		vec3 colour = use_a ? colour_a : colour_b;
		combined_intensity += colour;
	}

	vec3 light_intensity = vec3((ambient_intensity * material_colour) + combined_intensity);
    fragment_colour = vec4(light_intensity, 1.0);
}
