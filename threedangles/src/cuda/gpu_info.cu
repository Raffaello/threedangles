#include <cuda/gpu_info.cuh>
#include <cuda_runtime.h>
#include <iostream>
#include <thrust/version.h>

void display_gpu_info()
{
    using std::cout;
    using std::endl;
    using std::cerr;
    
    cout << "gpu info" << endl;
    
    int devCount;
    if (cudaGetDeviceCount(&devCount) != cudaSuccess) {
        cerr << cudaGetLastError() << endl;
        return;
    }
    cout << "CUDA :" << CUDART_VERSION << " - THRUST: " << THRUST_VERSION << endl;
    cout << "CUDA device count:" << devCount << endl;
    for (int i = 0; i < devCount; i++)
    {
        cudaDeviceProp devProp;
        if (cudaGetDeviceProperties(&devProp, i) != cudaSuccess) {
            cerr << cudaGetLastError() << endl;
            continue;
        }

        cout
            << "Device:     " << i << endl
            << "Name:       " << devProp.name << " - " << devProp.major << "." << devProp.minor << endl
            << "Global Mem: " << devProp.totalGlobalMem << endl
            << "Shared Mem: " << devProp.sharedMemPerBlock << endl
            ;
    }
}

