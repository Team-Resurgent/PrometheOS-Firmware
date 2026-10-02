// cpu_gpu.h
#pragma once
#include <xtl.h>

namespace cpugpu
{
    // Call once per frame (cheap; internally rate-limited to ~1 Hz)
    void poll();

    // Latest readings in MHz (0.0 if unavailable)
    double getCpuMHz();
    double getGpuMHz();
    double getFsbMHz();

    // New: derived clocks (MHz)
    double getMemMHz();   // Memory clock derived from CPUMPLL fields
    double getVcoMHz();   // CPU VCO clock derived from CPUMPLL fields

    // New: raw PLL register snapshots (last polled)
    DWORD getCpumpll();   // CPUMPLL DWORD at PCI 0:3:0 offset 0x6C
    DWORD getNvpll();     // NVPLL  DWORD at NV2A MMIO 0x00680500

    // New: helpers for UI labels � middle 6 hex chars (uppercase), out6 must be >= 7 bytes
    void getCpumpllHash6(char out6[7]);
    void getNvpllHash6  (char out6[7]);

    unsigned cpuRatio();
}
