#include <efi.h>
#include <efilib.h>

// ==========================================
// CONFIGURATION & CONSTANTS
// ==========================================

#define OS_NAME         L"ZEON OS"
#define OS_VERSION      L"1.2.0-baremetal"
#define OS_CODENAME     L"FUST ENGINE"
#define MAX_FILES       10
#define BUFFER_SIZE     128

// Color Palette Definition
#define COLOR_DEFAULT   EFI_LIGHTGRAY
#define COLOR_PROMPT    EFI_LIGHTGREEN
#define COLOR_CYAN      EFI_LIGHTCYAN
#define COLOR_YELLOW    EFI_YELLOW
#define COLOR_RED       EFI_RED
#define COLOR_WHITE     EFI_WHITE

// ==========================================
// VIRTUAL FILE SYSTEM (VFS) STRUCTURE
// ==========================================

typedef struct {
    CHAR16 name[32];
    CHAR16 content[BUFFER_SIZE];
    BOOLEAN is_used;
    UINTN size;
} VirtualFile;

VirtualFile ramdisk[MAX_FILES];

// ==========================================
// CORE SYSTEM HELPERS
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

// Delay Microseconds Helper
void sleep_ms(UINTN milliseconds) {
    for (volatile UINTN i = 0; i < milliseconds * 100000; i++) {
        __asm__ __volatile__("nop");
    }
}

// ==========================================
// VIRTUAL FILE SYSTEM (VFS) ENGINE
// ==========================================

void vfs_init() {
    for (UINTN i = 0; i < MAX_FILES; i++) {
        ramdisk[i].is_used = FALSE;
        ramdisk[i].size = 0;
        ramdisk[i].name[0] = L'\0';
        ramdisk[i].content[0] = L'\0';
    }

    // Default System Files
    StrCpy(ramdisk[0].name, L"hostname");
    StrCpy(ramdisk[0].content, L"zeon-station-01");
    ramdisk[0].is_used = TRUE;
    ramdisk[0].size = StrLen(ramdisk[0].content) * sizeof(CHAR16);

    StrCpy(ramdisk[1].name, L"welcome.txt");
    StrCpy(ramdisk[1].content, L"Selamat datang di ZEON OS Baremetal System!");
    ramdisk[1].is_used = TRUE;
    ramdisk[1].size = StrLen(ramdisk[1].content) * sizeof(CHAR16);

    StrCpy(ramdisk[2].name, L"engine.cfg");
    StrCpy(ramdisk[2].content, L"ENGINE=FUST_GRAPHICS_V2\nMODE=1080P_GRID");
    ramdisk[2].is_used = TRUE;
    ramdisk[2].size = StrLen(ramdisk[2].content) * sizeof(CHAR16);
}

void vfs_ls(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_CYAN);
    Print(L"STATUS    SIZE      FILENAME\n");
    Print(L"------    ----      --------\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN file_found = 0;
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used) {
            Print(L"[RAM]     %03d B    %s\n", ramdisk[i].size, ramdisk[i].name);
            file_found++;
        }
    }

    if (file_found == 0) {
        Print(L"(Direktori kosong)\n");
    }
    Print(L"\n");
}

void vfs_cat(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(filename, ramdisk[i].name) == 0) {
            set_color(ST, COLOR_WHITE);
            Print(L"%s\n\n", ramdisk[i].content);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }
    set_color(ST, COLOR_RED);
    Print(L"cat: '%s': File tidak ditemukan.\n\n", filename);
    set_color(ST, COLOR_DEFAULT);
}

void vfs_touch(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    // Cek apakah file sudah ada
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(filename, ramdisk[i].name) == 0) {
            set_color(ST, COLOR_YELLOW);
            Print(L"touch: File '%s' sudah ada.\n\n", filename);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }

    // Cari slot kosong
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (!ramdisk[i].is_used) {
            StrCpy(ramdisk[i].name, filename);
            StrCpy(ramdisk[i].content, L"(File Kosong)");
            ramdisk[i].is_used = TRUE;
            ramdisk[i].size = 0;
            set_color(ST, COLOR_PROMPT);
            Print(L"File '%s' berhasil dibuat.\n\n", filename);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }

    set_color(ST, COLOR_RED);
    Print(L"touch: RAMDisk Penuh! Maksimal %d file.\n\n", MAX_FILES);
    set_color(ST, COLOR_DEFAULT);
}

void vfs_rm(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(filename, ramdisk[i].name) == 0) {
            ramdisk[i].is_used = FALSE;
            set_color(ST, COLOR_PROMPT);
            Print(L"File '%s' berhasil dihapus.\n\n", filename);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }
    set_color(ST, COLOR_RED);
    Print(L"rm: File '%s' tidak ditemukan.\n\n", filename);
    set_color(ST, COLOR_DEFAULT);
}

// ==========================================
// FUST GRAPHICS ENGINE (WALLPAPER)
// ==========================================

void render_fust_wallpaper(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) {
        set_color(ST, COLOR_RED);
        Print(L"FUST Engine Error: Graphics Output Protocol gagal dimuat!\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 *fb = (UINT32*)gop->Mode->FrameBufferBase;

    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            UINT32 color = 0x000000;

            if (y < height / 2) {
                // FUST Sunset Horizon Gradient
                UINT32 red = (y * 255) / (height / 2);
                color = (red << 16) | (0x00 << 8) | 0x44; 
            } else {
                // FUST Neon Speed Grid Lines
                UINT32 line_y = y - (height / 2);
                if ((line_y % 18 < 3) || (x % 45 == 0)) {
                    color = 0xFF5500; // FUST Bright Orange
                } else {
                    color = 0x0A0015; // Dark Cyber Ground
                }
            }

            fb[y * width + x] = color;
        }
    }
}

void render_cyber_wallpaper(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) return;

    UINT32 width = gop->Mode->Info->HorizontalResolution;
    UINT32 height = gop->Mode->Info->VerticalResolution;
    UINT32 *fb = (UINT32*)gop->Mode->FrameBufferBase;

    for (UINT32 y = 0; y < height; y++) {
        for (UINT32 x = 0; x < width; x++) {
            UINT32 color = 0x050510;

            if (x % 50 == 0 || y % 50 == 0) {
                if ((x / 50 + y / 50) % 2 == 0) {
                    color = 0x00F0FF; // Cyber Cyan
                } else {
                    color = 0xFF007F; // Cyber Pink
                }
            } else if ((x % 50 >= 23 && x % 50 <= 27) && (y % 50 >= 23 && y % 50 <= 27)) {
                color = 0x00FF66; // Matrix Green Center
            }

            fb[y * width + x] = color;
        }
    }
}

// ==========================================
// BUILT-IN APPLICATIONS & MODULES
// ==========================================

void app_neofetch(EFI_SYSTEM_TABLE *ST, EFI_RUNTIME_SERVICES *RT) {
    EFI_TIME Time;
    uefi_call_wrapper(RT->GetTime, 2, &Time, NULL);

    set_color(ST, COLOR_PROMPT);
    Print(L"  _______ ____  _  _   "); set_color(ST, COLOR_CYAN); Print(L" root@zeon-station\n");
    set_color(ST, COLOR_PROMPT);
    Print(L" |_  / __/ __ \\/ \\| |  "); set_color(ST, COLOR_DEFAULT); Print(L" -----------------\n");
    set_color(ST, COLOR_PROMPT);
    Print(L"  / / _// /_/ / .` | | "); set_color(ST, COLOR_YELLOW); Print(L" OS Name   : "); set_color(ST, COLOR_DEFAULT); Print(L"%s\n", OS_NAME);
    set_color(ST, COLOR_PROMPT);
    Print(L" /___/___/\\____/_|\\_|  "); set_color(ST, COLOR_YELLOW); Print(L" Version   : "); set_color(ST, COLOR_DEFAULT); Print(L"%s\n", OS_VERSION);
    Print(L"                       "); set_color(ST, COLOR_YELLOW); Print(L" Engine    : "); set_color(ST, COLOR_DEFAULT); Print(L"%s\n", OS_CODENAME);
    Print(L"                       "); set_color(ST, COLOR_YELLOW); Print(L" System Clock: "); set_color(ST, COLOR_DEFAULT); Print(L"%02d:%02d:%02d UTC\n", Time.Hour, Time.Minute, Time.Second);
    Print(L"                       "); set_color(ST, COLOR_YELLOW); Print(L" Shell Environment: "); set_color(ST, COLOR_DEFAULT); Print(L"zeon-shell\n\n");
    set_color(ST, COLOR_DEFAULT);
}

void app_matrix_effect(EFI_SYSTEM_TABLE *ST) {
    clear_screen(ST);
    set_color(ST, COLOR_PROMPT);
    Print(L"=== FUST MATRIX ENGINE ACTIVATED ===\n");
    Print(L"Tekan tombol apa saja untuk menghentikan...\n\n");

    EFI_INPUT_KEY Key;
    UINTN counter = 0;

    while (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key) == EFI_NOT_READY) {
        CHAR16 random_char = L'A' + (counter % 26);
        Print(L"%c %d ", random_char, counter % 9);
        counter++;

        if (counter % 16 == 0) {
            Print(L"\n");
        }

        sleep_ms(25);
    }

    set_color(ST, COLOR_DEFAULT);
    Print(L"\n\nMatrix dikembalikan ke terminal.\n\n");
}

void app_calculator(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_YELLOW);
    Print(L"=== KALKULATOR SEDERHANA ZEON OS ===\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN a = 15;
    UINTN b = 5;

    Print(L"Angka A: %d | Angka B: %d\n", a, b);
    Print(L"Penjumlahan (%d + %d) = %d\n", a, b, a + b);
    Print(L"Pengurangan (%d - %d) = %d\n", a, b, a - b);
    Print(L"Perkalian   (%d * %d) = %d\n", a, b, a * b);
    Print(L"Pembagian   (%d / %d) = %d\n\n", a, b, a / b);
}

void app_game_tebak_angka(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_YELLOW);
    Print(L"=== MINI GAME: TEBAK ANGKA ===\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN target = 7;
    EFI_INPUT_KEY Key;

    Print(L"Masukkan angka tebakan lo (1 - 9): ");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
    Print(L"%c\n", Key.UnicodeChar);

    if (Key.UnicodeChar == (L'0' + target)) {
        set_color(ST, COLOR_PROMPT);
        Print(L"[MENANG] Luar biasa! Angka tebakan lo tepat: %d\n\n", target);
    } else {
        set_color(ST, COLOR_RED);
        Print(L"[KALAH] Tebakan salah! Angka yang benar adalah: %d\n\n", target);
    }
    set_color(ST, COLOR_DEFAULT);
}

void display_help(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_CYAN);
    Print(L"=== DAFTAR PERINTAH ZEON OS SHELL ===\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"  neofetch    - Menampilkan informasi sistem & statistik OS\n");
    Print(L"  fust        - Merender FUST Speed Grid Wallpaper Engine\n");
    Print(L"  cyber       - Merender Cyberpunk Neon Grid Wallpaper\n");
    Print(L"  matrix      - Menjalankan simulasi animasi Matrix\n");
    Print(L"  calc        - Jalankan kalkulator bawaan sistem\n");
    Print(L"  game        - Mainkan mini game tebak angka\n");
    Print(L"  ls          - Tampilkan daftar file di RAMDisk\n");
    Print(L"  cat <file>  - Baca isi teks dari file\n");
    Print(L"  touch <file>- Buat file baru di dalam RAMDisk\n");
    Print(L"  rm <file>   - Hapus file dari RAMDisk\n");
    Print(L"  clear       - Bersihkan tampilan layar terminal\n");
    Print(L"  poweroff    - Matikan sistem komputer\n\n");
}

// ==========================================
// KERNEL MAIN & SHELL LOOP
// ==========================================

EFI_STATUS kernel_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    // Inisialisasi File System di Memory
    vfs_init();
    clear_screen(ST);

    // Banner Booting
    set_color(ST, COLOR_PROMPT);
    Print(L"--------------------------------------------------\n");
    Print(L"   ZEON OS BAREMETAL KERNEL [FUST ENGINE LOADED]   \n");
    Print(L"--------------------------------------------------\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"Ketik 'neofetch' atau 'help' untuk memulai perintah.\n\n");

    EFI_INPUT_KEY Key;
    CHAR16 buffer[BUFFER_SIZE];
    UINTN buf_idx = 0;

    // Shell Infinite Loop
    while (1) {
        // Render UNIX/Zeon Prompt Style: root@zeon:~#
        set_color(ST, COLOR_PROMPT);
        Print(L"root@zeon");
        set_color(ST, COLOR_DEFAULT);
        Print(L":");
        set_color(ST, COLOR_CYAN);
        Print(L"~");
        set_color(ST, COLOR_RED);
        Print(L"# ");
        set_color(ST, COLOR_DEFAULT);

        // Sub-loop untuk Menerima Keyboard Character
        buf_idx = 0;
        while (1) {
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

            if (Key.UnicodeChar == L'\r') { // Enter Key
                Print(L"\n");
                buffer[buf_idx] = L'\0';
                break;
            } else if (Key.UnicodeChar == L'\b') { // Backspace
                if (buf_idx > 0) {
                    buf_idx--;
                    Print(L"\b \b");
                }
            } else if (Key.UnicodeChar >= 32 && buf_idx < (BUFFER_SIZE - 1)) { // Normal Char
                buffer[buf_idx++] = Key.UnicodeChar;
                Print(L"%c", Key.UnicodeChar);
            }
        }

        // ==========================================
        // COMMAND PARSER SYSTEM
        // ==========================================

        if (StrCmp(buffer, L"help") == 0) {
            display_help(ST);
        } else if (StrCmp(buffer, L"neofetch") == 0) {
            app_neofetch(ST, RT);
        } else if (StrCmp(buffer, L"fust") == 0) {
            render_fust_wallpaper(ST);
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            clear_screen(ST);
        } else if (StrCmp(buffer, L"cyber") == 0) {
            render_cyber_wallpaper(ST);
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            clear_screen(ST);
        } else if (StrCmp(buffer, L"matrix") == 0) {
            app_matrix_effect(ST);
        } else if (StrCmp(buffer, L"calc") == 0) {
            app_calculator(ST);
        } else if (StrCmp(buffer, L"game") == 0) {
            app_game_tebak_angka(ST);
        } else if (StrCmp(buffer, L"ls") == 0) {
            vfs_ls(ST);
        } else if (StrnCmp(buffer, L"cat ", 4) == 0) {
            vfs_cat(ST, buffer + 4);
        } else if (StrnCmp(buffer, L"touch ", 6) == 0) {
            vfs_touch(ST, buffer + 6);
        } else if (StrnCmp(buffer, L"rm ", 3) == 0) {
            vfs_rm(ST, buffer + 3);
        } else if (StrCmp(buffer, L"clear") == 0) {
            clear_screen(ST);
        } else if (StrCmp(buffer, L"poweroff") == 0 || StrCmp(buffer, L"shutdown") == 0) {
            set_color(ST, COLOR_YELLOW);
            Print(L"Mematikan ZEON OS secara aman...\n");
            sleep_ms(500);
            uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        } else if (buf_idx > 0) {
            set_color(ST, COLOR_RED);
            Print(L"zeon-shell: command '%s' tidak ditemukan!\n\n", buffer);
            set_color(ST, COLOR_DEFAULT);
        }
    }

    return EFI_SUCCESS;
}
