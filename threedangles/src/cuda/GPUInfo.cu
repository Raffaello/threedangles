#include <cuda/GPUInfo.cuh>
#include <cassert>

namespace cuda
{
    GPUInfo::GPUInfo() : cudart_version(CUDART_VERSION), thrust_version(THRUST_VERSION)
    {
        int deviceCount;
        if (cudaGetDeviceCount(&deviceCount) != cudaSuccess) {
            _errors.push_back(cudaGetErrorString(cudaGetLastError()));
            return;
        }

        for (int i = 0; i < deviceCount; i++)
        {
            cudaDeviceProp devProp;
            if (cudaGetDeviceProperties(&devProp, i) != cudaSuccess) {
                _errors.push_back(cudaGetErrorString(cudaGetLastError()));
                continue;
            }

            _deviceProperties.push_back(devProp);
        }

        assert(deviceCount == _deviceProperties.size());
    }

    int GPUInfo::getErrorsCount() const noexcept
    {
        return _errors.size();
    }

    thrust::host_vector<std::string> GPUInfo::getErrors() const noexcept
    {
        return _errors;
    }

    int GPUInfo::getDeviceCount() const noexcept
    {
        return _deviceProperties.size();
    }

    thrust::host_vector<cudaDeviceProp> GPUInfo::getDeviceProperties() const noexcept
    {
        return _deviceProperties;
    }
}
