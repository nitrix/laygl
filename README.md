## Learn OpenGL (learngl)

Just a simple project while I'm learning OpenGL.

![Wakfu example character](docs/screenshot.png)

### Currently implemented

- Creating a window and renderer with a modern OpenGL context.
- Entities have a model, texture, position, rotation and scale.
- Matrix and vector math utilities.
- Support .obj file format.
- Wireframe toggleable modes for debugging.
- Phong lighting (ambient + diffuse + specular).
- Back-face culling optimization.
- Entities time-based movement system.
- Third person camera.

### Short term

- Numerous code refactors
    - Spaghetti camera/renderer/entity.
    - Toolkit collection/dynarray.
- Drop SOIL, use libpng directly.
- SDL for window events and OpenGL context.
- Optimizations
    - Convert obj module and move texture module into a loader module.
    - Batch rendering using some kind of scene manager.

### Long term

- Terrain
- Transparency
- Fog
- Multi-texturing
- Mipmapping
- Terrain collision
- Texture atlases
- GUI
- Multiple lights
- Point lights
- Day/night cycle
- Cell shading
- Normal mapping
- Font rendering
- Distance field text rendering
- Particle effects
- Instanced rendering
- Procedural terrain
- Shadow mapping
- Percentage closer filtering
- Antialiasing and anisotropic filtering
- Post-processing effects
- Gaussian Blur
- Multisampled FBOs
- Specular maps
- Bloom effect
- Multiple render targets
- Geometry shaders
- Cube map reflections
- Render to cube map texture
- Skeletal animation
- Lens flare
- Occlusion queries
- Water