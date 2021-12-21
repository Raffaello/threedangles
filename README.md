# threedangles

Three-D-angles is a 3D triangles engine built from scratch.

The project has been inspired by a video series of OLC (OneLoneCoder):
- [YouTube](https://www.youtube.com/watch?v=ih20l3pJoeU)
- [GitHub](https://github.com/OneLoneCoder)


It will feature the following algorithms:

- [x] Line drawing (bresenham)
- [/] Triangle filling (bresenham, pineda)
- [/] Triangle clipping
- [x] Transformations
- [/] Camera positioning
- [/] Loading 3D file for objects
- [ ] Textures
- [ ] Z-buffer (Depth-Buffer)
- [x] Flat-Shading
- [ ] Vertex Normals
- [ ] Gouraud Shading
- [ ] Phong Shading
- rasterization
- [ ] Left-hand rules
- Quaternions

The implementation will be based on :

- [x] compiler based
- [ ] SIMD instructions
- [ ] GPU

# Note on the implementation

- Using SDL2 for window creation, pixel drawing and input events, so it can be cross-OS in a simpler way,
  without worrying about the "initial boiler plate OS dependant code".
  SDL2 might be removed completely after few iterations.

## windows
- WinRT: https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.imaging.writeablebitmap.pixelbuffer?view=winrt-22000#examples
- WinGDI: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setpixel

## SIMD
- [ ] SSE/AVX for float 32 bit and Vec4 / Mat4 operations (vectorization)

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
