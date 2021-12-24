# threedangles

Three-D-angles is a 3D triangles engine built from scratch.

This is a for fun, educational project.

The project has been inspired by a OLC's video serie (OneLoneCoder):
- [YouTube](https://www.youtube.com/watch?v=ih20l3pJoeU)
- [GitHub](https://github.com/OneLoneCoder)

It will feature the following algorithms:

- [x] Line drawing (bresenham)
- [x] Triangle filling (bresenham, pineda)
- [x] Triangle clipping
- [x] Transformations
- [x] Camera positioning
- [x] Loading 3D object files
- [ ] Textures
- [x] Painter's algorithm
- [x] Z-buffer (Depth-Buffer)
- [x] Flat-Shading
- [x] Vertex Normals
- [x] Gouraud Shading
- [ ] Phong Shading
- rasterization
- [ ] Left-hand rules
- [ ] Quaternions
- [ ] Materials
- [ ] advanced lighting
- [ ] bezier curves
- [ ] bezier triangles
- [ ] splines
- [ ] NURBS

The implementation will be based on :

- [x] CPU / compiler based 
- [ ] SIMD instructions
- [ ] GPU

## Compiling

Use `CMake` and eventually `vcpkg` for package management.

## Note on the implementation

- Using SDL2 for window creation, pixel drawing and input events, so it can be cross-OS in a simpler way,
  without worrying about the "initial boiler plate OS dependant code".
  SDL2 might be removed completely after few iterations.

### SDL2-Image

Used to load textures.

### Windows
- WinRT: https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.imaging.writeablebitmap.pixelbuffer?view=winrt-22000#examples
- WinGDI: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setpixel

### SIMD
Using MMX/SSE/AVX for float 32 bit and Vec4 / Mat4 operations (vectorization) and rasterization.

- [ ] MMX
- [ ] SSE/SSE2/...
- [ ] AVX/AVX512/...

### CUDA
This might require to redo all the `engine` into `cudaEngine`:

- sorting
- vectors, matrix and triangle storing and operations
- screen and surface, textures
- operations in the pipeline and how organize data might be required to reorder it and increment parallelism.

#### note
It might just be pointless doing a "software 3D rendering Engine using GPGPU", apart some fun on it, but at that point would be better doing properly
with Vulkan/DirectX/OpenGL.

I am not sure if i will do it right now or in the future, as after wrote the algorithms,
using CUDA won't change too much, especially with the usage of the `thrust` library.

### C/C++ compiler optimization
- [ ] `__vectorcall`, `__fastcall`
- [ ] various compiler switches

#### Note on compilers
It looks like compilers are already using some SIMD from default C/C++ code.

- [ ] add specific compiler options on CMAKE configuration, to enhanche performances.
