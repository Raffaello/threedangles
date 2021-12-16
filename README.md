# threedangles

Three-D-angles is a 3D triangles engines built from scratch.

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

The algorithms are all implemented from scratch without using any modern library or Engine.

Their implementation will be based on :

- [x] compiler based (can be using whatever instruction the compiler decide)
- [ ] SIMD instructions (Intel)
- [ ] GPU (CUDA)

So it would be possible to benchmark those too.

It might be later on implemented an analougue 3D engine feature using DirectX/OpenGL/Vulkan for
a comparision/benchmark on the algorithms implemented here.

# Note on the implementation

- Using SDL2 for window creation (and pixel drawing, soon to be removed) so it can be cross-OS in a simpler way,
  without worrying about the "initial boiler plate OS dependant code".
  SDL2 might be removed completely after few iterations of the engine.

## windows
- WinRT: https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.imaging.writeablebitmap.pixelbuffer?view=winrt-22000#examples
- WinGDI: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setpixel

## SIMD
- [ ] SSE/AVX for float 32 bit and Vec4 / Mat4 operations


## C/C++ compiler optimization
- [ ] `__vectorcall`, `__fastcall`

#### Note on compilers
It looks like compilers are already using some SIMD from default C/C++ code.

- [ ] add specific compiler options on CMAKE configuration, to enhanche performances.


