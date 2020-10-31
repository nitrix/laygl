package laygl

import "C"
import (
	"github.com/go-gl/gl/v4.6-core/gl"
	"github.com/go-gl/mathgl/mgl32"
	"unsafe"
)

const AntiAliasingSamples = 8
const IBLSamples = 32

type Renderer struct {
	projection mgl32.Mat4
	environment *Environment
	framebuffer *Framebuffer
	hdrShader *Shader
	ditherTexture *Texture
	width, height int
}

var bayerMatrix = []uint8 {
	10, 32, 8, 40, 2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44, 4, 36, 14, 46, 6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	3, 35, 11, 43, 1, 33, 9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47, 7, 39, 13, 45, 5, 37,
	63, 31, 55, 23, 61, 29, 53, 21,
}

func NewRenderer(window *Window) (*Renderer, error) {
	renderer := &Renderer{}

	renderer.Resize(window.Dimensions())
	window.OnResize(func() {
		renderer.Resize(window.Dimensions())
	})

	gl.ClearColor(0, 0, 0, 1.0)

	//gl.Enable(gl.DEPTH_TEST)
	//gl.Enable(gl.CULL_FACE)
	//gl.CullFace(gl.BACK)

	gl.Enable(gl.DEPTH_TEST)
	//gl.DepthFunc(gl.LEQUAL)
	gl.Enable(gl.MULTISAMPLE)

	environment, err := NewEnvironment()
	if err != nil {
		return nil, err
	}

	renderer.environment = environment
	renderer.framebuffer = NewFramebuffer(window.Dimensions())

	renderer.hdrShader, err = LoadShader("shaders/hdr.vert", "shaders/hdr.frag")
	if err != nil {
		return nil, err
	}

	renderer.ditherTexture = &Texture{}

	gl.GenTextures(1, &renderer.ditherTexture.id)
	gl.BindTexture(gl.TEXTURE_2D, renderer.ditherTexture.id)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
	gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
	gl.TexImage2D(gl.TEXTURE_2D, 0, gl.R8UI, 8, 8, 0, gl.RED_INTEGER, gl.UNSIGNED_BYTE, unsafe.Pointer(&bayerMatrix[0]))

	return renderer, nil
}

func (r *Renderer) Wireframe(enabled bool) {
	if enabled {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.LINE)
	} else {
		gl.PolygonMode(gl.FRONT_AND_BACK, gl.FILL)
	}
}

func (r *Renderer) Resize(width, height int) {
	r.width, r.height = width, height
	r.projection = mgl32.Perspective(mgl32.DegToRad(45.0), float32(width)/float32(height), 0.1, 1000.0)
	gl.Viewport(0, 0, int32(width), int32(height))
}

func (r *Renderer) Render(scene *Scene) {
	r.framebuffer.Use()
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	r.renderEntities(scene)
	r.framebuffer.Unuse()

	//gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	//r.renderEntities(scene)

	// Present.
	r.hdrShader.Use()
	gl.ActiveTexture(gl.TEXTURE0)
	gl.BindTexture(gl.TEXTURE_2D_MULTISAMPLE, r.framebuffer.colorBuffer)
	gl.ActiveTexture(gl.TEXTURE1)
	gl.BindTexture(gl.TEXTURE_2D, r.ditherTexture.id)

	gl.Uniform2ui(r.hdrShader.findUniformByName("dimensions"), uint32(r.width), uint32(r.height))
	gl.Uniform1i(r.hdrShader.findUniformByName("hdrBuffer"), 0)
	gl.Uniform1i(r.hdrShader.findUniformByName("bayer_matrix"), 1)

	// Render a quad that covers the entire screen so we can actually use the texture produced by the main shader.
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	gl.DrawArrays(gl.TRIANGLES, 0, 3)
	r.hdrShader.Unuse()

	//gl.BindFramebuffer(gl.READ_FRAMEBUFFER, r.framebuffer.fbo)
	//gl.BindFramebuffer(gl.DRAW_FRAMEBUFFER, 0)
	//gl.BlitFramebuffer(0, 0, int32(r.width), int32(r.height), 0, 0, int32(r.width), int32(r.height), gl.DEPTH_BUFFER_BIT, gl.NEAREST)

	//r.renderEntities(scene)
}

func (r *Renderer) renderEntities(scene *Scene) {
	for model, entities := range scene.entities {
		// Render each meshes of the model.
		for _, mesh := range model.meshes {
			mesh.Use()

			// Prepare the shader.
			mesh.shader.BindUniformProjection(r.projection)
			mesh.shader.BindUniformCamera(scene.activeCamera)
			mesh.shader.BindUniformLights(scene.lights)
			mesh.shader.BindUniformMaterial(mesh.material)
			mesh.shader.BindUniformEnvironment(r.environment)
			mesh.shader.BindUniformTextureSamplers(
				mesh.albedoTexture,
				mesh.normalMapTexture,
				mesh.metallicRoughnessMapTexture,
				mesh.ambientOcclusionMapTexture,
			)

			for _, entity := range entities {
				// The entity transformMatrix is the only thing that we should be updating for each entity.
				mesh.shader.BindUniformTransform(&entity.transformMatrix)

				// FIXME: UNSIGNED_SHORT?
				gl.DrawElements(gl.TRIANGLES, int32(mesh.indiceCount), gl.UNSIGNED_SHORT, gl.PtrOffset(0))
			}

			mesh.Unuse()
		}
	}
}