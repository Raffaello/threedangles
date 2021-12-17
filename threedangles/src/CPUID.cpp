#include <CPUID.hpp>
#include <bitset>
#include <vector>
#include <array>
#include <cstring>

CPUID::CPUID()
{
    std::vector<std::array<int, 4>> data_;
    std::vector<std::array<int, 4>> exData_;
    std::array<int, 4> cpui;

    std::bitset<32> f_1_ECX_;
    std::bitset<32> f_1_EDX_;
    std::bitset<32> f_7_EBX_;
    //std::bitset<32> f_7_ECX_;
    //std::bitset<32> f_81_ECX_;
    //std::bitset<32> f_81_EDX_;

    // Calling __cpuid with 0x0 as the function_id argument
// gets the number of the highest valid function ID.
#if defined(__clang__) || defined(__GNUC__)
    __cpuid(0, cpui[0], cpui[1], cpui[2], cpui[3]);
#else
    __cpuid(cpui.data(), 0);
#endif

    int nIds_ = cpui[0];

    for (int i = 0; i < nIds_; i++) {
        __cpuidex(cpui.data(), i, 0);
        data_.push_back(cpui);
    }

    // Capture vendor string
    char vendor[0x20];
    memset(vendor, 0, sizeof(vendor));
    *reinterpret_cast<int*>(vendor) = data_[0][1];
    *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
    *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
    vendor_ = vendor;

    // load bitset with flags for function 0x00000001
    if (nIds_ >= 1)
    {
        f_1_ECX_ = data_[1][2];
        f_1_EDX_ = data_[1][3];
    }

    // load bitset with flags for function 0x00000007
    if (nIds_ >= 7)
    {
        f_7_EBX_ = data_[7][1];
        //f_7_ECX_ = data_[7][2];
    }

    // --------------------------------------

    // Calling __cpuid with 0x80000000 as the function_id argument
    // gets the number of the highest valid extended ID.cpui
#if defined(__clang__) || defined(__GNUC__)
    __cpuid(0x80000000, cpui[0], cpui[1], cpui[2], cpui[3]);
#else
    __cpuid(cpui.data(), 0x80000000);
#endif
    int nExIds_ = cpui[0];

    char brand[0x40];
    memset(brand, 0, sizeof(brand));

    for (int i = 0x80000000; i <= nExIds_; ++i)
    {
        __cpuidex(cpui.data(), i, 0);
        exData_.push_back(cpui);
    }

    // load bitset with flags for function 0x80000001
    if (nExIds_ >= 0x80000001)
    {
        //f_81_ECX_ = exData_[1][2];
        //f_81_EDX_ = exData_[1][3];
    }

    // Interpret CPU brand string if reported
    if (nExIds_ >= 0x80000004)
    {
        memcpy(brand, exData_[2].data(), sizeof(cpui));
        memcpy(brand + 16, exData_[3].data(), sizeof(cpui));
        memcpy(brand + 32, exData_[4].data(), sizeof(cpui));
        brand_ = brand;
    }

    // ----------------------------------------------------------

    _MMX = f_1_EDX_[23];
    _SSE = f_1_EDX_[25];
    _SSE2 = f_1_EDX_[26];
    _SSE3 = f_1_ECX_[0];
    _SSSE3 = f_1_ECX_[9];
    _SSE41 = f_1_ECX_[19];
    _SSE42 = f_1_ECX_[20];
    _AVX = f_1_ECX_[28];
    _AVX2 = f_7_EBX_[5];
    _AVX512F = f_7_EBX_[16];
    _AVX512PF = f_7_EBX_[26];
    _AVX512ER = f_7_EBX_[27];
    _AVX512CD = f_7_EBX_[28];
}

bool CPUID::MMX() const noexcept
{
    return _MMX;
}

bool CPUID::SSE() const noexcept
{
    return _SSE;
}

bool CPUID::SSE2() const noexcept
{
    return _SSE2;
}

bool CPUID::SSE3() const noexcept
{
    return _SSE3;
}

bool CPUID::SSSE3() const noexcept
{
    return _SSSE3;
}

bool CPUID::SSE41() const noexcept
{
    return _SSE41;
}

bool CPUID::SSE42() const noexcept
{
    return _SSE42;
}

bool CPUID::AVX() const noexcept
{
    return _AVX;
}

bool CPUID::AVX2() const noexcept
{
    return _AVX2;
}

bool CPUID::AVX512F() const noexcept
{
    return _AVX512F;
}

bool CPUID::AVX512PF() const noexcept
{
    return _AVX512PF;
}

bool CPUID::AVX512ER() const noexcept
{
    return _AVX512ER;
}

bool CPUID::AVX512CD() const noexcept
{
    return _AVX512CD;
}

const std::string& CPUID::brand() const noexcept
{
    return brand_;
}

const std::string& CPUID::vendor() const noexcept
{
    return vendor_;
}
