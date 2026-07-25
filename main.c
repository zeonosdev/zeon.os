// <i love my user>
#include <efi.h>
#include <efilib.h>

// ==========================================
// CONFIG & DEFINITIONS
// ==========================================

#define COLOR_DEFAULT   EFI_LIGHTGRAY
#define COLOR_MENU_SEL  EFI_YELLOW
#define COLOR_BANNER    EFI_LIGHTGREEN
#define COLOR_SUBTEXT   EFI_CYAN

// ==========================================
// HELPER & UTILITIES
// ==========================================

void set_color(EFI_SYSTEM_TABLE *ST, UINTN Attribute) {
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, Attribute);
}

void clear_screen(EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
}

EFI_GRAPHICS_OUTPUT_PROTOCOL* get_gop(EFI_SYSTEM_TABLE *ST) {
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    EFI_STATUS status;

    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status) || !gop) {
        return NULL;
    }
    return gop;
}

// Banner Utama ZEON OS
void draw_cyber_banner(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_BANNER);
    Print(L"  _______ ____  _  _    ___  ____  \n");
    Print(L" |_  / __/ __ \\/ \\| |  / _ \\/ __/  \n");
    Print(L"  / / _// /_/ / .` | | / // /\\ \\   \n");
    Print(L" /___/___/\\____/_|\\_|  \\___/___/   \n");
    
    set_color(ST, COLOR_SUBTEXT);
    Print(L"===========================================\n");
    Print(L"       ZEON OS - BAREMETAL EDITION         \n");
    Print(L"===========================================\n\n");
    set_color(ST, COLOR_DEFAULT);
}

// ==========================================
// MODUL WALLPAPER GRAPHICS (GOP)
// ==========================================

// 1. Wallpaper FUST (Synthwave Speed Grid)
void draw_wallpaper_fust(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) {
        Print(L"Gagal memuat Graphic Output Protocol!\n");
        return;
    }

    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 *fb = (UINT32*)gop->Mode->FrameBufferBase;

    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            UINT32 color = 0x000000;

            if (y < height / 2) {
                // Horizon Langit
                UINT32 red = (y * 255) / (height / 2);
                color = (red << 16) | (0x00 << 8) | 0x44; 
            } else {
                // Speed Grid Lines
                UINT32 line_y = y - (height / 2);
                if ((line_y % 18 < 3) || (x % 45 == 0)) {
                    color = 0xFF5500; // FUST Orange Line
                } else {
                    color = 0x0A0015; // Ground Color
                }
            }

            fb[y * width + x] = color;
        }
    }
}

// 2. Wallpaper CYBER (Neon Grid Cyberpunk)
void draw_wallpaper_cyber(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) {
        Print(L"Gagal memuat Graphic Output Protocol!\n");
        return;
    }

    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 *fb = (UINT32*)gop->Mode->FrameBufferBase;

    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            UINT32 color = 0x050510;

            if (x % 50 == 0 || y % 50 == 0) {
                if ((x / 50 + y / 50) % 2 == 0) {
                    color = 0x00F0FF; // Neon Cyan
                } else {
                    color = 0xFF007F; // Neon Magenta
                }
            } else if ((x % 50 >= 23 && x % 50 <= 27) && (y % 50 >= 23 && y % 50 <= 27)) {
                color = 0x00FF66; // Bright Green Dot
            }

            fb[y * width + x] = color;
        }
    }
}

// ==========================================
// APLIKASI & FITUR OS
// ==========================================

// Terminal Shell
void app_terminal_shell(EFI_SYSTEM_TABLE *ST) {
    clear_screen(ST);
    set_color(ST, EFI_YELLOW);
    Print(L"=== ZEON OS TERMINAL SHELL v1.0 ===\n");
    Print(L"Ketik 'help' untuk bantuan, atau 'exit' untuk keluar.\n\n");
    set_color(ST, COLOR_DEFAULT);

    EFI_INPUT_KEY Key;
    CHAR16 buffer[100];
    UINTN buf_idx = 0;

    while (1) {
        set_color(ST, EFI_LIGHTCYAN);
        Print(L"user@zeon-os> ");
        set_color(ST, COLOR_DEFAULT);

        buf_idx = 0;
        while (1) {
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

            if (Key.UnicodeChar == L'\r') {
                Print(L"\n");
                buffer[buf_idx] = L'\0';
                break;
            } else if (Key.UnicodeChar == L'\b') {
                if (buf_idx > 0) {
                    buf_idx--;
                    Print(L"\b \b");
                }
            } else if (Key.UnicodeChar >= 32 && buf_idx < 99) {
                buffer[buf_idx++] = Key.UnicodeChar;
                Print(L"%c", Key.UnicodeChar);
            }
        }

        if (StrCmp(buffer, L"help") == 0) {
            Print(L"Daftar Perintah Shell:\n");
            Print(L"  help  - Menampilkan bantuan ini\n");
            Print(L"  about - Informasi sistem ZEON OS\n");
            Print(L"  clear - Membersihkan layar terminal\n");
            Print(L"  exit  - Keluar dari terminal\n\n");
        } else if (StrCmp(buffer, L"about") == 0) {
            Print(L"ZEON OS v1.0\n");
            Print(L"Sistem Operasi Bare-Metal dikembangkan dengan UEFI C.\n\n");
        } else if (StrCmp(buffer, L"clear") == 0) {
            clear_screen(ST);
        } else if (StrCmp(buffer, L"exit") == 0) {
            break;
        } else if (buf_idx > 0) {
            set_color(ST, EFI_RED);
            Print(L"Command '%s' tidak ditemukan!\n\n", buffer);
            set_color(ST, COLOR_DEFAULT);
        }
    }
}

// Matrix Effect
void app_matrix_effect(EFI_SYSTEM_TABLE *ST) {
    clear_screen(ST);
    set_color(ST, EFI_LIGHTGREEN);
    Print(L"[MATRIX EFEK] Tekan tombol apa saja untuk keluar...\n\n");

    EFI_INPUT_KEY Key;
    UINTN counter = 0;

    while (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key) == EFI_NOT_READY) {
        CHAR16 random_char = L'A' + (counter % 26);
        Print(L"%c %d ", random_char, counter % 9);
        counter++;

        if (counter % 15 == 0) {
            Print(L"\n");
        }

        for (volatile int i = 0; i < 2500000; i++);
    }

    set_color(ST, COLOR_DEFAULT);
}

// Game Tebak Angka
void app_game_tebak_angka(EFI_SYSTEM_TABLE *ST) {
    clear_screen(ST);
    set_color(ST, EFI_LIGHTYELLOW);
    Print(L"=== GAME TEBAK ANGKA ===\n\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN target = 7;
    EFI_INPUT_KEY Key;

    Print(L"Tebak angka antara 1 sampai 9: ");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
    Print(L"%c\n\n", Key.UnicodeChar);

    if (Key.UnicodeChar == (L'0' + target)) {
        set_color(ST, EFI_LIGHTGREEN);
        Print(L"[MENANG] Luar biasa! Tebakan lo BENAR!\n");
    } else {
        set_color(ST, EFI_RED);
        Print(L"[KALAH] Sayang sekali! Angka yang benar adalah: %d\n", target);
    }

    set_color(ST, COLOR_DEFAULT);
    Print(L"\nTekan tombol apa saja untuk kembali...");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

// Jam Real-time
void app_jam_sistem(EFI_SYSTEM_TABLE *ST, EFI_RUNTIME_SERVICES *RT) {
    clear_screen(ST);
    set_color(ST, EFI_LIGHTCYAN);
    Print(L"=== JAM REAL-TIME SISTEM ===\n\n");
    set_color(ST, COLOR_DEFAULT);

    EFI_TIME Time;
    uefi_call_wrapper(RT->GetTime, 2, &Time, NULL);

    Print(L"Tanggal Sistem : %02d-%02d-%04d\n", Time.Day, Time.Month, Time.Year);
    Print(L"Waktu Sistem   : %02d:%02d:%02d (UTC)\n\n", Time.Hour, Time.Minute, Time.Second);

    Print(L"Tekan tombol apa saja untuk kembali...");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

// ==========================================
// KERNEL MAIN (ENTRY POINT DARI BOOT.S)
// ==========================================

EFI_STATUS kernel_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    UINTN MenuOption = 0;
    UINTN MaxMenu = 6;
    EFI_INPUT_KEY Key;

    while (1) {
        clear_screen(ST);
        draw_cyber_banner(ST);

        Print(L"Gunakan Panah Atas/Bawah & Enter untuk memilih:\n\n");

        // Menu Option 1
        if (MenuOption == 0) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [1] Terminal Shell\n", (MenuOption == 0) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Menu Option 2
        if (MenuOption == 1) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [2] Wallpaper FUST (Synthwave Speed Grid)\n", (MenuOption == 1) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Menu Option 3
        if (MenuOption == 2) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [3] Wallpaper CYBER (Neon Grid)\n", (MenuOption == 2) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Menu Option 4
        if (MenuOption == 3) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [4] Animasi Efek Matrix\n", (MenuOption == 3) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Menu Option 5
        if (MenuOption == 4) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [5] Game Tebak Angka\n", (MenuOption == 4) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Menu Option 6
        if (MenuOption == 5) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [6] Jam Real-Time Sistem\n", (MenuOption == 5) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Menu Option 7
        if (MenuOption == 6) set_color(ST, COLOR_MENU_SEL);
        Print(L" %s [7] Shutdown Komputer\n", (MenuOption == 6) ? L"->" : L"  ");
        set_color(ST, COLOR_DEFAULT);

        // Navigasi Keyboard
        WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

        if (Key.ScanCode == 0x01 && MenuOption > 0) {
            MenuOption--;
        } else if (Key.ScanCode == 0x02 && MenuOption < MaxMenu) {
            MenuOption++;
        } else if (Key.UnicodeChar == L'\r') {
            if (MenuOption == 0) {
                app_terminal_shell(ST);
            } else if (MenuOption == 1) {
                draw_wallpaper_fust(ST);
                WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            } else if (MenuOption == 2) {
                draw_wallpaper_cyber(ST);
                WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            } else if (MenuOption == 3) {
                app_matrix_effect(ST);
            } else if (MenuOption == 4) {
                app_game_tebak_angka(ST);
            } else if (MenuOption == 5) {
                app_jam_sistem(ST, RT);
            } else if (MenuOption == 6) {
                uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            }
        }
    }

    return EFI_SUCCESS;
}
