# threedangles

Three-D-angles is a 3D triangles engine built from scratch.

It will feature the following algorithms:

- [x] Line drawing (bresenham)
- [/] Triangle filling (bresenham)
- [/] Triangle clipping
- [x] Transformations
- [/] Camera positioning
- [/] Loading 3D file for objects
- [ ] Textures
- [x] Flat-Shading
- [ ] Gouraud Shading
- [ ] Phong Shading
- rasterization
- Left-hand rules
- Quaternions

The implementation will be based on :

- [x] compiler based (can be using whatever instruction the compiler decide)
- [ ] SIMD instructions (Intel)
- [ ] GPU (CUDA)

So it would be possible to benchmark those too.

It might be later on implemented an analougue 3D engine feature using DirectX/OpenGL/Vulkan for
a comparision/benchmark on the algorithms implemented here.

# Note on the implementation

- Using SDL2 for window creation, pixel drawing and input events, so it can be cross-OS in a simpler way,
  without worrying about the "initial boiler plate OS dependant code".
  SDL2 might be removed completely after few iterations.

## windows
- WinRT: https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.imaging.writeablebitmap.pixelbuffer?view=winrt-22000#examples
- WinGDI: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setpixel

## SIMD
- [ ] SSE/AVX for float 32 bit and Vec4 / Mat4 operations

## CUDA
This might require to redo all the `engine` into `cudaEngine`:

- sorting
- vectors, matrix and triangle storing and operations 
- screen and surface, textures
- operations in the pipeline and how organize data might be required to reorder it and increment parallelism.

## C/C++ compiler optimization
- [ ] `__vectorcall`, `__fastcall`
- [ ] various compiler switches

#### Note on compilers
It looks like compilers are already using some SIMD from default C/C++ code.

- [ ] add specific compiler options on CMAKE configuration, to enhanche performances.


