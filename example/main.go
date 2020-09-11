package main

import (
	"fmt"
	"github.com/go-gl/glfw/v3.3/glfw"
	"github.com/nitrix/laygl"
	"log"
	"runtime/debug"
	"time"
)

// TODO: Get rid of mgl32 at top-level.

func main() {
	// Create a window.
	window, err := laygl.NewFullScreenWindow("Learn OpenGL")
	//window, err := laygl.NewWindow(1280, 720, "Learn OpenGL")
	if err != nil {
		log.Fatalln("Unable to create fullscreen window:", err)
	}
	defer window.Destroy()

	// Create the renderer.
	renderer, err := laygl.NewRenderer(window)
	if err != nil {
		log.Fatalln("Unable to create renderer:", err)
	}

	// Load the model.
	//model, err := laygl.LoadModel("helmet", "assets/cowboy.glb")
	model, err := laygl.LoadModel("helmet", "assets/helmet.glb")
	if err != nil {
		log.Fatalln("Unable to load cowboy model:", err)
	}

	// Create light
	light := laygl.DefaultLight()

	// Entity
	entity := laygl.NewEntityFromModel(model)

	// Camera
	camera := laygl.NewCamera()
	camera.MoveAt(0, 0, 20)
	camera.LookAt(0, 0, 0)

	// Scene
	scene := laygl.NewScene()
	scene.AddCamera(camera)
	scene.AddEntity(entity)
	scene.AddLight(&light)

	// Reduce the memory residency after loading assets into GPU memory.
	debug.FreeOSMemory()

	fps := 0
	lastTime := time.Now()

	entity.Translate(0, 0, -10) // FIXME: Why the directional light struggle at the origin?

	// Main loop
	previousTime := glfw.GetTime()
	for !window.ShouldClose() {
		window.PollEvents()

		// Update
		t := glfw.GetTime()
		elapsed := t - previousTime
		previousTime = t

		entity.Rotate(0, float32(elapsed) * 0.25, 0)

		// Render
		renderer.Render(scene)
		window.Refresh()

		// Print FPS
		fps++
		if time.Since(lastTime) > time.Second {
			fmt.Println("FPS:", fps)
			fps = 0
			lastTime = time.Now()
		}
	}
}