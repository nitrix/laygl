#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "layman.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

_Thread_local const struct layman_renderer *current_renderer;

static struct layman_matrix_4f calculate_projection_matrix(const struct layman_renderer *renderer) {
	float aspect_ratio = (float) renderer->viewport_width / (float) renderer->viewport_height;
	float scale_y = (1.0f / tanf(renderer->fov / 2.0f / 180.0f * M_PI)) * aspect_ratio;
	float scale_x = scale_y / aspect_ratio;
	float frustrum_length = renderer->near_plane - renderer->far_plane;

	struct layman_matrix_4f m = {
		.d = {
			[0] = scale_x,
			[5] = scale_y,
			[10] = ((renderer->far_plane + renderer->near_plane) / frustrum_length),
			[11] = -1,
			[14] = ((2 * renderer->near_plane * renderer->far_plane) / frustrum_length)
		},
	};

	return m;
}

struct layman_renderer *layman_renderer_create(void) {
	struct layman_renderer *renderer = malloc(sizeof *renderer);
	if (!renderer) {
		return NULL;
	}

	// TODO: Dynamic dimensions.
	renderer->viewport_width = 1280;
	renderer->viewport_height = 720;

	// TODO: Change the FOV.
	renderer->fov = 90.0f;
	renderer->far_plane = 1000.0f;
	renderer->near_plane = 0.1f;

	renderer->start_time = glfwGetTime();
	renderer->viewProjectionMatrixLocation = -1;
	renderer->modelMatrixLocation = -1;
	renderer->normalMatrixLocation = -1;

	renderer->exposure = 1;

	return renderer;
}

void layman_renderer_destroy(struct layman_renderer *renderer) {
	if (!renderer) {
		return;
	}

	free(renderer);
}

void layman_renderer_switch(const struct layman_renderer *renderer) {
	if (current_renderer == renderer) {
		return;
	} else {
		current_renderer = renderer;
	}

	glClearColor(0, 0, 0, 1); // Black.

	// TODO: Support a wireframe mode.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Multisampling.
	glEnable(GL_MULTISAMPLE);

	// Necessary to avoid the seams of the cubemap being visible.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

static void render_mesh(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene, const struct layman_mesh *mesh) {
	layman_mesh_switch(mesh);

	// Uniforms.
	layman_shader_bind_uniform_material(mesh->shader, mesh->material);
	layman_shader_bind_uniform_camera(mesh->shader, camera);
	layman_shader_bind_uniform_lights(mesh->shader, scene->lights, scene->lights_count);

	// TODO: Horrible, please don't do this every frames!
	if (renderer->viewProjectionMatrixLocation == -1) {
		renderer->viewProjectionMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ViewProjectionMatrix");
		renderer->modelMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_ModelMatrix");
		renderer->normalMatrixLocation = glGetUniformLocation(mesh->shader->program_id, "u_NormalMatrix");
		renderer->exposureLocation = glGetUniformLocation(mesh->shader->program_id, "u_Exposure");
	}

	// TODO: More uniforms, tidy this up.
	// TODO: Should all move into the model file and stuff.
	double elapsed = layman_renderer_elapsed(renderer);
	struct layman_matrix_4f projectionMatrix = calculate_projection_matrix(renderer);
	glUniformMatrix4fv(renderer->viewProjectionMatrixLocation, 1, false, projectionMatrix.d); // TODO: Missing view matrix?
	struct layman_matrix_4f modelMatrix = layman_matrix_4f_identity();
	layman_matrix_4f_rotate_x(&modelMatrix, -M_PI_2);
	layman_matrix_4f_rotate_y(&modelMatrix, M_PI * elapsed * 0.25f);
	layman_matrix_4f_translate(&modelMatrix, LAYMAN_VECTOR_3F(0, 0, -3));
	glUniformMatrix4fv(renderer->modelMatrixLocation, 1, false, modelMatrix.d);
	// struct layman_matrix_4f normalMatrix = layman_matrix_4f_identity();
	// glUniformMatrix4fv(renderer->normalMatrixLocation, 1, false, normalMatrix.d);
	glUniformMatrix4fv(renderer->normalMatrixLocation, 1, false, modelMatrix.d); // OMG, this fixed the lighting problem!
	glUniform1f(renderer->exposureLocation, renderer->exposure);

	// Render.
	// FIXME: Support more than just unsigned shorts.
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_SHORT, NULL);
}

double layman_renderer_elapsed(const struct layman_renderer *renderer) {
	return glfwGetTime() - renderer->start_time;
}

void layman_renderer_render(struct layman_renderer *renderer, const struct layman_camera *camera, const struct layman_scene *scene) {
	layman_renderer_switch(renderer);

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render all entities.
	for (size_t i = 0; i < scene->entity_count; i++) {
		const struct layman_entity *entity = scene->entities[i];

		// Render all meshes.
		for (size_t i = 0; i < entity->model->meshes_count; i++) {
			struct layman_mesh *mesh = entity->model->meshes[i];

			// Render mesh.
			render_mesh(renderer, camera, scene, mesh);
		}
	}
}
