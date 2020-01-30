#include "toolkit.h"
#include "camera.h"
#include "renderer.h"

#include <GLFW/glfw3.h>

// TODO: Switch to SDL2 instead of GLFW?

struct window {
    GLFWwindow *glfw_window;
};

atomic_uint window_count;

void window_increment(void) {
    unsigned int previous_count = atomic_fetch_add(&window_count, 1);

    if (previous_count == 0) {
        glfwInit();

        // OpenGL 3.1 removed the immediate mode stuff.
        // OpenGL 3.3, the shader versions starts matching the gl version.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    }
}

void window_decrement(void) {
    unsigned int previous_count = atomic_fetch_sub(&window_count, 1);

    if (previous_count == 1) {
        glfwTerminate();
    }
}

struct window *window_create(int width, int height, const char *title) {
    window_increment();

    struct window *window = malloc(sizeof *window);
    if (!window) {
        window_decrement();
        return NULL;
    }

    window->glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window->glfw_window) {
        free(window);
        window_decrement();
        return NULL;
    }

    // Disable swap interval (framerate limiter)
    // glfwSwapInterval(0);

    // Disable cursor
    glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // as opposed to GLFW_CURSOR_NORMAL

    // Enable raw cursor motion
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window->glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetCursorPos(window->glfw_window, 0, 0);

    return window;
}

void window_use(struct window *window) {
    GLFWwindow *current_window = glfwGetCurrentContext();

    if (window->glfw_window != current_window) {
        window_unuse(window);
    }

    glfwMakeContextCurrent(window->glfw_window);
}

void window_unuse(struct window *window) {
    TK_UNUSED(window);

    glfwMakeContextCurrent(NULL);
}

void window_destroy(struct window *window) {
    glfwDestroyWindow(window->glfw_window);
    free(window);
    window_decrement();
}

void window_handle_events(struct window *window, struct renderer *renderer, struct camera *camera) {
    glfwPollEvents();

    if (glfwGetKey(window->glfw_window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_move(camera, 0, 0, -0.08f);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_move(camera, 0, 0, 0.08f);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_move(camera, -0.08f, 0, 0);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_move(camera, 0.08f, 0, 0);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_1) == GLFW_PRESS) {
        renderer_set_wireframe(renderer, GL_POINT);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_2) == GLFW_PRESS) {
        renderer_set_wireframe(renderer, GL_LINE);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_3) == GLFW_PRESS) {
        renderer_set_wireframe(renderer, GL_FILL);
    }

    double x, y;
    glfwGetCursorPos(window->glfw_window, &x, &y);
    if (x != 0 || y != 0) {
        camera_rotate(camera, y * -0.002f, x * -0.002f, 0);
        glfwSetCursorPos(window->glfw_window, 0, 0);
    }

    if (glfwGetKey(window->glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window->glfw_window, GLFW_TRUE);
    }
}

void window_framebuffer_size(struct window *window, int *width, int *height) {
    glfwGetFramebufferSize(window->glfw_window, width, height);
}

void window_refresh(struct window *window) {
    glfwSwapBuffers(window->glfw_window);
}

bool window_should_close(struct window *window) {
    return glfwWindowShouldClose(window->glfw_window);
}
