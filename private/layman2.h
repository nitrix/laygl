#ifndef PRIVATE_LAYMAN_H
#define PRIVATE_LAYMAN_H

#include "layman.h"
#include <glad/glad.h>

struct layman_scene {
	const struct layman_entity **entities;
	size_t entity_count;
	size_t entity_capacity;
};

struct layman_entity {
	const struct layman_model *model;
	struct layman_vector_3f position;
};

struct layman_model {
	struct layman_mesh **meshes;
	size_t meshes_count;
};

struct layman_mesh {
	GLuint vao;
	GLuint vbo_positions;
	GLuint vbo_normals;
	GLuint vbo_uvs;
	GLuint ebo_indices;
	GLuint vbo_tangents;
	GLuint vbo_bitangents;
	size_t indices_count;

	struct layman_shader *shader;
	const struct layman_material *material;
};

struct layman_material {
	struct layman_vector_3f base_color_factor;
	struct layman_texture *base_color_texture;
	struct layman_texture *metallic_roughness_texture;
	float metallic_factor;
	float roughness_factor;
	struct layman_texture *normal_texture;
	struct layman_texture *occlusion_texture;
	struct layman_texture *emissive_texture;
	struct layman_vector_3f emissive_factor;
};

struct layman_texture {
	GLuint id;
	enum layman_texture_kind kind;
};

struct layman_shader {
	GLuint program_id;

	// Uniforms.
	GLint uniform_base_color_factor;
	GLint uniform_base_color_texture;
	GLint uniform_normal_texture;
	GLint uniform_metallic_roughness_texture;
	GLint uniform_occlusion_texture;
	GLint uniform_emissive_texture;
};

#endif
