#ifndef WALLPAPERS_H
#define WALLPAPERS_H

#include <efi.h>
#include <efilib.h>

// Helper untuk mengambil pointer Framebuffer GOP
EFI_GRAPHICS_OUTPUT_PROTOCOL* get_gop(EFI_SYSTEM_TABLE *ST) {
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status) || !gop) return NULL;
    return gop;
}

// 1. WALLPAPER "FAST / SPEED" (Synthwave / Speed Grid Horizon)
void draw_wallpaper_fast(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) return;

    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 *fb = (UINT32*)gop->Mode->FrameBufferBase;

    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            UINT32 color = 0x000000; // Default Hitam

            if (y < height / 2) {
                // Sky Gradient (Merah Keunguan ke Hitam)
                UINT32 red = (y * 255) / (height / 2);
                color = (red << 16) | (0x00 << 8) | 0x33; 
            } else {
                // Speed Horizon Lines (Efek Garis Kecepatan)
                UINT32 line_y = y - (height / 2);
                if ((line_y % 20 < 4) || (x % 40 == 0)) {
                    // Garis Speed Neon Oranye
                    color = 0xFF6600; 
                } else {
                    // Ground Gelap
                    color = 0x110022; 
                }
            }

            fb[y * width + x] = color;
        }
    }
}

// 2. WALLPAPER "CYBER / CYBERPUNK" (Neon Grid & Matrix Pattern)
void draw_wallpaper_cyber(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) return;

    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 *fb = (UINT32*)gop->Mode->FrameBufferBase;

    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            UINT32 color = 0x050510; // Dark Cyber background

            // Grid Neon Cyberpunk (Cyan & Magenta)
            if (x % 50 == 0 || y % 50 == 0) {
                if ((x / 50 + y / 50) % 2 == 0) {
                    color = 0x00F0FF; // Neon Cyan
                } else {
                    color = 0xFF007F; // Neon Magenta
                }
            } 
            // Efek Titik Simpul / Crosshair Cyber
            else if ((x % 50 >= 23 && x % 50 <= 27) && (y % 50 >= 23 && y % 50 <= 27)) {
                color = 0x00FF66; // Bright Green Dot
            }

            fb[y * width + x] = color;
        }
    }
}

#endif
