#include "layman.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	struct layman_application *app = layman_application_create(1280, 720, "Example", false);
	// struct layman_application *app = layman_application_create(0, 0, "Example", true);
	if (!app) {
		fprintf(stderr, "Unable to create the layman application\n");
		return EXIT_FAILURE;
	}

	layman_application_use(app);

	// TODO: No hardcoded models.
	struct layman_model *model = layman_model_load("DamagedHelmet.glb");
	if (!model) {
		fprintf(stderr, "Unable to load model\n");
		return EXIT_FAILURE; // TODO: Handle error better.
	}

	struct layman_entity *entity = layman_entity_create_from_model(model);
	if (!entity) {
		// TODO: Handle error.
	}

	struct layman_light *light = layman_light_create(LAYMAN_LIGHT_TYPE_DIRECTIONAL);
	if (!entity) {
		// TODO: Handle error.
	}

	layman_scene_add_entity(app->scene, entity);
	layman_scene_add_light(app->scene, light);

	layman_application_run(app); // Main loop.

	layman_application_unuse(app);

	layman_entity_destroy(entity);
	layman_model_destroy(model);

	layman_application_destroy(app);

	return EXIT_SUCCESS;
}
