#include "layman2.h"

struct layman_light *layman_light_create(enum layman_light_type type) {
	struct layman_light *light = malloc(sizeof *light);
	if (!light) {
		return NULL;
	}

	light->type = type;
	light->position = LAYMAN_VECTOR_3F(0, 0, 0);
	light->direction = LAYMAN_VECTOR_3F(0, 0, -1);
	light->range = 10; // FIXME?
	light->color = LAYMAN_VECTOR_3F(1, 1, 1);
	light->intensity = 1;
	light->range = 1; // FIXME?
	light->innerConeCos = 0; // FIXME?
	light->outerConeCos = 0; // FIXME?

	return light;
}

void layman_light_destroy(struct layman_light *light) {
	free(light);
}
