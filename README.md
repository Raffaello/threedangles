# threedangles

Three-D-angles is a 3D triangles engines built from scratch.

It will feature the following algorithms:

- [ ] Triangle drawing (bresenham)
- [x] Triangle filling (bresenham)
- [ ] Triangle clipping
- [x] Transformations
- [ ] Camera positioning
- [ ] Loading 3D file for objects
- [ ] Textures
- [x] Flat-Shading
- [ ] Gouraud Shading
- [ ] Phong Shading
- rasterization
- Left-hand rules

The algorithms will be all implemented from scratch without using any modern library or Engine.

Their implementation will be based on :

- CPU/FPU
- SIMD instructions
- GPU

So it would be possible to benchmark those too.

It might be later on implemented an analougue 3D engine feature using DirectX/OpenGL/Vulkan for
a comparision/benchmark on the algorithms implemented here.

# Note on the implementation

- Using SDL2 for window creation (and pixel drawing, soon to be removed) so it can be cross-OS in a simpler way,
  without worrying about the "initial boiler plate OS dependant code".
  SDL2 might be removed completely after few iterations of the engine.
