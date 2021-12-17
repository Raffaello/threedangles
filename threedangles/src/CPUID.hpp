#pragma once

#if defined(__clang__) || defined(__GNUC__)
#include <cpuid.h>
// Add the method if not present in GCC
#if __GNUC__ < 11
static __inline void
__cpuidex(int __cpuid_info[4], int __leaf, int __subleaf)
{
    __cpuid_count(__leaf, __subleaf, __cpuid_info[0], __cpuid_info[1],
        __cpuid_info[2], __cpuid_info[3]);
}
#endif
#else
#include <intrin.h>
#endif
#include <string>

class CPUID
{
public:
    CPUID();

    bool MMX()      const noexcept;
    bool SSE()      const noexcept;
    bool SSE2()     const noexcept;
    bool SSE3()     const noexcept;
    bool SSSE3()    const noexcept;
    bool SSE41()    const noexcept;
    bool SSE42()    const noexcept;
    bool AVX()      const noexcept;
    bool AVX2()     const noexcept;
    bool AVX512F()  const noexcept;
    bool AVX512PF() const noexcept;
    bool AVX512ER() const noexcept;
    bool AVX512CD() const noexcept;

    const std::string& brand()  const noexcept;
    const std::string& vendor() const noexcept;

private:
    bool _MMX;
    bool _SSE;
    bool _SSE2;
    bool _SSE3;
    bool _SSSE3;
    bool _SSE41;
    bool _SSE42;
    bool _AVX;
    bool _AVX2;
    bool _AVX512F;
    bool _AVX512PF;
    bool _AVX512ER;
    bool _AVX512CD;
    std::string brand_;
    std::string vendor_;

};