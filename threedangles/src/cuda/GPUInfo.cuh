#pragma once

#include <cuda_runtime.h>
#include <thrust/version.h>
#include <thrust/host_vector.h>
#include <string>

namespace cuda
{
    class GPUInfo
    {
    public:
        GPUInfo();

        const int cudart_version;
        const int thrust_version;
        int getErrorsCount() const noexcept;
        thrust::host_vector<std::string> getErrors() const noexcept;
        int getDeviceCount() const noexcept;
        thrust::host_vector<cudaDeviceProp> getDeviceProperties() const noexcept;
    private:
        thrust::host_vector<std::string> _errors;
        thrust::host_vector<cudaDeviceProp> _deviceProperties;
    };
}
