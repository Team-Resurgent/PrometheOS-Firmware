// cpu_gpu.cpp
#include "cpu_gpu.h"
#include <xtl.h>
#include <stdio.h>   // for sprintf
#include <string.h>  // for memcpy

namespace {

extern "C" ULONG __stdcall HalReadWritePCISpace(
    ULONG Bus, ULONG Slot, ULONG Offset,
    PVOID Buffer, ULONG Length, BOOLEAN Write);

static __forceinline ULONG PciSlot(ULONG device, ULONG function)
{
    return (device << 5) | (function & 0x7);
}

static DWORD PciReadDword(ULONG bus, ULONG dev, ULONG fun, ULONG off)
{
    DWORD val = 0;
    (void)HalReadWritePCISpace(bus, PciSlot(dev, fun), off, &val, sizeof(val), FALSE);
    return val;
}

// ----- MSR helper -----
static __forceinline unsigned __int64 rdmsr_xdk(unsigned long msr)
{
    unsigned long lo, hi;
    __asm {
        mov ecx, msr
        rdmsr
        mov lo, eax
        mov hi, edx
    }
    return ((unsigned __int64)hi << 32) | (unsigned __int64)lo;
}

// CPU ratio * 10 out of MSR 0x2A
static unsigned CpuRatioX10_FromMsr2A()
{
    const unsigned long MSR_EBL_CR_POWERON = 0x2A;
    unsigned __int64 reg = rdmsr_xdk(MSR_EBL_CR_POWERON);
    unsigned char pat = (unsigned char)((reg >> 22) & 0x2F);

    switch (pat) {
        case 0x01: return 30;  case 0x05: return 35;
        case 0x02: return 40;  case 0x06: return 45;
        case 0x00: return 50;  case 0x04: return 55;
        case 0x0B: return 60;  case 0x0F: return 65;
        case 0x09: return 70;  case 0x0D: return 75;
        case 0x0A: return 80;  case 0x26: return 85;
        case 0x20: return 90;  case 0x24: return 95;
        case 0x2B: return 100; case 0x2F: return 105;
        case 0x2A: return 130; case 0x2C: return 140;
    }
    return 0;
}

// ----- CPUMPLL nibble decoder (for MEM/VCO) -----
typedef struct {
    unsigned mem_div;    // high nibble of byte2
    unsigned fsb_multi;  // low  nibble of byte2
    unsigned pll_n;      // byte1 (FSB numerator)
    unsigned pll_m;      // byte0 (FSB denominator)
} CpuPllFields;

static inline CpuPllFields DecodeCpuPll(DWORD cpumpll)
{
    CpuPllFields f;
    f.pll_m =  (cpumpll      ) & 0xFFu;          // byte0
    f.pll_n = ((cpumpll >> 8 ) & 0xFFu);         // byte1
    unsigned byte2 = (cpumpll >> 16) & 0xFFu;    // byte2 = 0xMN
    f.fsb_multi =  byte2        & 0x0Fu;         // low nibble
    f.mem_div   = (byte2 >> 4)  & 0x0Fu;         // high nibble
    return f;
}

// Tiny helper: take the middle 6 hex chars of a DWORD as uppercase (e.g. "#A1B2C3")
static inline void FormatHash6FromDword(char out6[7], DWORD v)
{
    // Format 8 hex digits, keep positions [2..7)
    char buf[9];
    sprintf(buf, "%08X", v);
    memcpy(out6, buf + 2, 6);
    out6[6] = '\0';
}

// ----- Raw readers -----
static const double XTAL_HZ = 16666666.6667; // 16.666... MHz

static bool ReadFsbAndCpu(double& outFsbMHz, double& outCpuMHz)
{
    const ULONG BUS = 0, DEV = 3, FUN = 0, CPUMPLL_OFF = 0x6C;
    DWORD cpumpll = PciReadDword(BUS, DEV, FUN, CPUMPLL_OFF);

    unsigned fsb_div  =  cpumpll        & 0xFF;
    unsigned fsb_mult = (cpumpll >> 8)  & 0xFF;
    if (!fsb_div || !fsb_mult) return false;

    double fsb_hz = XTAL_HZ * ((double)fsb_mult / (double)fsb_div);
    outFsbMHz = fsb_hz / 1.0e6;

    unsigned ratio_x10 = CpuRatioX10_FromMsr2A();
    if (!ratio_x10) return false;

    double cpu_hz = fsb_hz * (ratio_x10 / 10.0);
    outCpuMHz = cpu_hz / 1.0e6;
    return true;
}

static bool ReadGpu(double& outGpuMHz)
{
    #define NV2A_MMIO_BASE             0xFD000000u
    #define NV_PRAMDAC_NVPLL_COEFF_OFF 0x00680500u

    volatile DWORD* nvpll_ptr = (volatile DWORD*)(NV2A_MMIO_BASE + NV_PRAMDAC_NVPLL_COEFF_OFF);
    DWORD nvpll = *nvpll_ptr;

    unsigned m =  nvpll        & 0xFFu;
    unsigned n = (nvpll >> 8)  & 0xFFu;
    unsigned p = (nvpll >> 16) & 0x07u;
    if (!m) return false;

    double gpu_hz = ((double)n * XTAL_HZ / (double)(1u << p)) / (double)m;
    outGpuMHz = gpu_hz / 1.0e6;
    return true;
}

// ----- Cached values, updated ~1 Hz -----
static DWORD  s_lastTick = 0;
static double s_cpuMHz   = 0.0;
static double s_gpuMHz   = 0.0;
static double s_fsbMHz   = 0.0;

// New cached values
static double s_memMHz   = 0.0;   // derived memory clock
static double s_vcoMHz   = 0.0;   // derived CPU VCO clock
static DWORD  s_cpumpll  = 0;     // raw CPUMPLL DWORD
static DWORD  s_nvpll    = 0;     // raw NVPLL DWORD

} // anonymous namespace

namespace cpugpu
{
    void poll()
    {
        DWORD now = GetTickCount();
        if (s_lastTick != 0 && (now - s_lastTick) < 1000) return; // ~1 Hz
        s_lastTick = now;

        double fsb, cpu, gpu;
        if (ReadFsbAndCpu(fsb, cpu)) {
            s_fsbMHz = fsb;
            s_cpuMHz = cpu;

            // Read CPUMPLL raw and derive VCO/MEM from its nibble fields.
            const ULONG BUS = 0, DEV = 3, FUN = 0, CPUMPLL_OFF = 0x6C;
            s_cpumpll = PciReadDword(BUS, DEV, FUN, CPUMPLL_OFF);

            CpuPllFields F = DecodeCpuPll(s_cpumpll);
            if (F.fsb_multi != 0) {
                // VCO = FSB * FSB_MULTI * 2
                s_vcoMHz = s_fsbMHz * (double)F.fsb_multi * 2.0;
                // MEM = VCO / (MEM_DIV * 2)  ==  FSB * FSB_MULTI / MEM_DIV
                if (F.mem_div != 0) {
                    s_memMHz = s_vcoMHz / ((double)F.mem_div * 2.0);
                } else {
                    s_memMHz = 0.0;
                }
            } else {
                s_vcoMHz = 0.0;
                s_memMHz = 0.0;
            }
        }

        if (ReadGpu(gpu)) {
            s_gpuMHz = gpu;

            // Also cache NVPLL raw for hex display
            #define NV2A_MMIO_BASE             0xFD000000u
            #define NV_PRAMDAC_NVPLL_COEFF_OFF 0x00680500u
            volatile DWORD* nvpll_ptr = (volatile DWORD*)(NV2A_MMIO_BASE + NV_PRAMDAC_NVPLL_COEFF_OFF);
            s_nvpll = *nvpll_ptr;
        }
    }

    unsigned cpuRatio() { return CpuRatioX10_FromMsr2A(); }

    // ----- Existing getters -----
    double getCpuMHz() { return s_cpuMHz; }
    double getGpuMHz() { return s_gpuMHz; }
    double getFsbMHz() { return s_fsbMHz; }

    // ----- New getters: MEM & VCO -----
    double getMemMHz() { return s_memMHz; }
    double getVcoMHz() { return s_vcoMHz; }

    // ----- New getters: raw PLL DWORDs -----
    DWORD getCpumpll() { return s_cpumpll; }
    DWORD getNvpll()   { return s_nvpll;   }

    // ----- New helpers: 6-char hash-style hex for UI labels -----
    // out6 must be >= 7 bytes (null-terminated)
    void getCpumpllHash6(char out6[7]) { FormatHash6FromDword(out6, s_cpumpll); }
    void getNvpllHash6  (char out6[7]) { FormatHash6FromDword(out6, s_nvpll);   }
}
