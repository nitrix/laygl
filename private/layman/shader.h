#ifndef LAYMAN_PRIVATE_SHADER_H
#define LAYMAN_PRIVATE_SHADER_H

struct layman_shader {
	GLuint program_id;

	// Material uniforms.
	GLint uniform_base_color_factor;
	GLint uniform_base_color_sampler;
	GLint uniform_normal_sampler;
	GLint uniform_normal_scale;
	GLint uniform_metallic_roughness_sampler;
	GLint uniform_metallic_factor;
	GLint uniform_roughness_factor;
	GLint uniform_occlusion_sampler;
	GLint uniform_occlusion_strength;
	GLint uniform_emissive_sampler;
	GLint uniform_emissive_factor;

	// Environment IBL.
	GLint uniform_environment_mip_count;
	GLint uniform_environment_lambertian;
	GLint uniform_environment_ggx;
	GLint uniform_environment_ggx_lut;
	GLint uniform_environment_charlie;
	GLint uniform_environment_charlie_lut;

	// Camera uniforms.
	GLint uniform_camera;

	// Light uniforms.
	GLint uniform_lights_type[MAX_LIGHTS];
	GLint uniform_lights_position[MAX_LIGHTS];
	GLint uniform_lights_direction[MAX_LIGHTS];
	GLint uniform_lights_color[MAX_LIGHTS];
	GLint uniform_lights_intensity[MAX_LIGHTS];
};

void layman_shader_switch(const struct layman_shader *shader);

#endif
