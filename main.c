#include <efi.h>
#include <efilib.h>

// ==========================================
// CONFIGURATION & DEFINITIONS
// ==========================================

#define OS_NAME         L"ZEON OS"
#define OS_VERSION      L"2.0.0-baremetal"
#define OS_CODENAME     L"FUST GRAPHICS CORE"
#define MAX_FILES       15
#define BUFFER_SIZE     256
#define FILE_MAX_TEXT   1024

// Color Palette Definitions
#define COLOR_DEFAULT   EFI_LIGHTGRAY
#define COLOR_PROMPT    EFI_LIGHTGREEN
#define COLOR_CYAN      EFI_LIGHTCYAN
#define COLOR_YELLOW    EFI_YELLOW
#define COLOR_RED       EFI_RED
#define COLOR_WHITE     EFI_WHITE
#define COLOR_BG_EDIT   EFI_BACKGROUND_BLUE
#define https://google.com/   676767

// ==========================================
// VIRTUAL FILE SYSTEM (VFS) STRUCTURE
// ==========================================

typedef struct {
    CHAR16 name[48];
    CHAR16 content[FILE_MAX_TEXT];
    BOOLEAN is_used;
    UINTN size;
} VirtualFile;

VirtualFile ramdisk[MAX_FILES];

// ==========================================
// SYSTEM HELPERS & UTILITIES
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

void sleep_ms(UINTN milliseconds) {
    for (volatile UINTN i = 0; i < milliseconds * 100000; i++) {
        __asm__ __volatile__("nop");
    }
}

// Split String Helper Sederhana
void split_args(CHAR16 *input, CHAR16 *cmd, CHAR16 *arg1, CHAR16 *arg2) {
    UINTN i = 0, j = 0;
    cmd[0] = L'\0';
    arg1[0] = L'\0';
    arg2[0] = L'\0';

    // Parse Command
    while (input[i] != L'\0' && input[i] != L' ') {
        cmd[j++] = input[i++];
    }
    cmd[j] = L'\0';

    if (input[i] == L'\0') return;
    i++; // Skip Space

    // Parse Arg1
    j = 0;
    while (input[i] != L'\0' && input[i] != L' ') {
        arg1[j++] = input[i++];
    }
    arg1[j] = L'\0';

    if (input[i] == L'\0') return;
    i++; // Skip Space

    // Parse Arg2
    j = 0;
    while (input[i] != L'\0') {
        arg2[j++] = input[i++];
    }
    arg2[j] = L'\0';
}

// ==========================================
// VIRTUAL FILE SYSTEM (VFS) CORE
// ==========================================

void vfs_init() {
    for (UINTN i = 0; i < MAX_FILES; i++) {
        ramdisk[i].is_used = FALSE;
        ramdisk[i].size = 0;
        ramdisk[i].name[0] = L'\0';
        ramdisk[i].content[0] = L'\0';
    }

    // Default System Files
    StrCpy(ramdisk[0].name, L"main.c");
    StrCpy(ramdisk[0].content, L"// ZEON OS Kernel Source Code\nvoid kernel_main() {\n    // FUST Engine Initialized\n}");
    ramdisk[0].is_used = TRUE;
    ramdisk[0].size = StrLen(ramdisk[0].content) * sizeof(CHAR16);

    StrCpy(ramdisk[1].name, L"config.fust");
    StrCpy(ramdisk[1].content, L"RENDERER=FUST_GRID\nRESOLUTION=1080P\nFPS=60");
    ramdisk[1].is_used = TRUE;
    ramdisk[1].size = StrLen(ramdisk[1].content) * sizeof(CHAR16);

    StrCpy(ramdisk[2].name, L"readme.txt");
    StrCpy(ramdisk[2].content, L"Selamat datang di ZEON OS!\nGunakan perintah 'edit <file>' untuk mengedit kodingan.");
    ramdisk[2].is_used = TRUE;
    ramdisk[2].size = StrLen(ramdisk[2].content) * sizeof(CHAR16);
}

void vfs_ls(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_CYAN);
    Print(L"TYPE      SIZE      FILENAME\n");
    Print(L"----      ----      --------\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN count = 0;
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used) {
            Print(L"[RAM]     %04d B    %s\n", ramdisk[i].size, ramdisk[i].name);
            count++;
        }
    }

    if (count == 0) {
        Print(L"(Direktori kosong)\n");
    }
    Print(L"\n");
}

void vfs_cat(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    if (filename[0] == L'\0') {
        set_color(ST, COLOR_RED);
        Print(L"Penggunaan: cat <nama_file>\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(filename, ramdisk[i].name) == 0) {
            set_color(ST, COLOR_WHITE);
            Print(L"--- ISI FILE: %s ---\n", ramdisk[i].name);
            set_color(ST, COLOR_DEFAULT);
            Print(L"%s\n\n", ramdisk[i].content);
            return;
        }
    }

    set_color(ST, COLOR_RED);
    Print(L"cat: File '%s' tidak ditemukan.\n\n", filename);
    set_color(ST, COLOR_DEFAULT);
}

void vfs_touch(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    if (filename[0] == L'\0') {
        set_color(ST, COLOR_RED);
        Print(L"Penggunaan: touch <nama_file>\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(filename, ramdisk[i].name) == 0) {
            set_color(ST, COLOR_YELLOW);
            Print(L"touch: File '%s' sudah ada.\n\n", filename);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }

    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (!ramdisk[i].is_used) {
            StrCpy(ramdisk[i].name, filename);
            ramdisk[i].content[0] = L'\0';
            ramdisk[i].is_used = TRUE;
            ramdisk[i].size = 0;
            set_color(ST, COLOR_PROMPT);
            Print(L"File '%s' berhasil dibuat.\n\n", filename);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }

    set_color(ST, COLOR_RED);
    Print(L"touch: RAMDisk Penuh!\n\n");
    set_color(ST, COLOR_DEFAULT);
}

void vfs_rename(EFI_SYSTEM_TABLE *ST, CHAR16 *old_name, CHAR16 *new_name) {
    if (old_name[0] == L'\0' || new_name[0] == L'\0') {
        set_color(ST, COLOR_RED);
        Print(L"Penggunaan: rename <nama_lama> <nama_baru>\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(old_name, ramdisk[i].name) == 0) {
            StrCpy(ramdisk[i].name, new_name);
            set_color(ST, COLOR_PROMPT);
            Print(L"Nama file berhasil diubah dari '%s' menjadi '%s'.\n\n", old_name, new_name);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }

    set_color(ST, COLOR_RED);
    Print(L"rename: File '%s' tidak ditemukan.\n\n", old_name);
    set_color(ST, COLOR_DEFAULT);
}

void vfs_rm(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    if (filename[0] == L'\0') {
        set_color(ST, COLOR_RED);
        Print(L"Penggunaan: rm <nama_file>\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

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
// INTERACTIVE TEXT EDITOR (MINI CODE IDE)
// ==========================================

void app_code_editor(EFI_SYSTEM_TABLE *ST, CHAR16 *filename) {
    if (filename[0] == L'\0') {
        set_color(ST, COLOR_RED);
        Print(L"Penggunaan: edit <nama_file>\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    INTN target_idx = -1;
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(filename, ramdisk[i].name) == 0) {
            target_idx = i;
            break;
        }
    }

    // Buat file otomatis jika belum ada
    if (target_idx == -1) {
        for (UINTN i = 0; i < MAX_FILES; i++) {
            if (!ramdisk[i].is_used) {
                StrCpy(ramdisk[i].name, filename);
                ramdisk[i].content[0] = L'\0';
                ramdisk[i].is_used = TRUE;
                ramdisk[i].size = 0;
                target_idx = i;
                break;
            }
        }
    }

    if (target_idx == -1) {
        set_color(ST, COLOR_RED);
        Print(L"edit: Slot RAMDisk Penuh!\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    clear_screen(ST);
    set_color(ST, COLOR_YELLOW);
    Print(L"===========================================================\n");
    Print(L"      ZEON OS TEXT EDITOR / CODE IDE v1.0                  \n");
    Print(L"      FILE: %s                                              \n", ramdisk[target_idx].name);
    Print(L"      [ESC] Simpan & Keluar | [ENTER] Baris Baru           \n");
    Print(L"===========================================================\n\n");
    set_color(ST, COLOR_DEFAULT);

    CHAR16 edit_buffer[FILE_MAX_TEXT];
    StrCpy(edit_buffer, ramdisk[target_idx].content);
    UINTN buf_len = StrLen(edit_buffer);

    Print(L"01 | %s", edit_buffer);

    EFI_INPUT_KEY Key;
    UINTN line_count = 1;

    while (1) {
        WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

        // ESC Key (ScanCode 0x17) -> Save and Exit
        if (Key.ScanCode == 0x17) {
            StrCpy(ramdisk[target_idx].content, edit_buffer);
            ramdisk[target_idx].size = StrLen(edit_buffer) * sizeof(CHAR16);
            clear_screen(ST);
            set_color(ST, COLOR_PROMPT);
            Print(L"[EDITOR] File '%s' berhasil disimpan!\n\n", ramdisk[target_idx].name);
            set_color(ST, COLOR_DEFAULT);
            break;
        }
        else if (Key.UnicodeChar == L'\r') {
            if (buf_len < FILE_MAX_TEXT - 2) {
                edit_buffer[buf_len++] = L'\n';
                edit_buffer[buf_len] = L'\0';
                line_count++;
                Print(L"\n%02d | ", line_count);
            }
        }
        else if (Key.UnicodeChar == L'\b') {
            if (buf_len > 0) {
                buf_len--;
                edit_buffer[buf_len] = L'\0';
                Print(L"\b \b");
            }
        }
        else if (Key.UnicodeChar >= 32 && buf_len < FILE_MAX_TEXT - 1) {
            edit_buffer[buf_len++] = Key.UnicodeChar;
            edit_buffer[buf_len] = L'\0';
            Print(L"%c", Key.UnicodeChar);
        }
    }
}

// ==========================================
// FUST GRAPHICS ENGINE & WALLPAPERS
// ==========================================

void render_fust_wallpaper(EFI_SYSTEM_TABLE *ST) {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop(ST);
    if (!gop) {
        set_color(ST, COLOR_RED);
        Print(L"FUST Engine Error: Graphics Output Protocol Gagal!\n");
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
                UINT32 red = (y * 255) / (height / 2);
                color = (red << 16) | (0x00 << 8) | 0x44; 
            } else {
                UINT32 line_y = y - (height / 2);
                if ((line_y % 18 < 3) || (x % 45 == 0)) {
                    color = 0xFF5500; // FUST Bright Orange
                } else {
                    color = 0x0A0015; 
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
                color = ((x / 50 + y / 50) % 2 == 0) ? 0x00F0FF : 0xFF007F;
            } else if ((x % 50 >= 23 && x % 50 <= 27) && (y % 50 >= 23 && y % 50 <= 27)) {
                color = 0x00FF66;
            }
            fb[y * width + x] = color;
        }
    }
}

// ==========================================
// BUILT-IN APPLICATIONS
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
    Print(L"                       "); set_color(ST, COLOR_YELLOW); Print(L" Real Time : "); set_color(ST, COLOR_DEFAULT); Print(L"%02d:%02d:%02d UTC\n", Time.Hour, Time.Minute, Time.Second);
    Print(L"                       "); set_color(ST, COLOR_YELLOW); Print(L" Shell     : "); set_color(ST, COLOR_DEFAULT); Print(L"zeon-shell v2.0\n\n");
    set_color(ST, COLOR_DEFAULT);
}

void app_matrix_effect(EFI_SYSTEM_TABLE *ST) {
    clear_screen(ST);
    set_color(ST, COLOR_PROMPT);
    Print(L"=== FUST MATRIX ANIMATION ===\n");
    Print(L"Tekan sembarang tombol untuk keluar...\n\n");

    EFI_INPUT_KEY Key;
    UINTN counter = 0;

    while (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key) == EFI_NOT_READY) {
        CHAR16 random_char = L'A' + (counter % 26);
        Print(L"%c %d ", random_char, counter % 9);
        counter++;
        if (counter % 16 == 0) Print(L"\n");
        sleep_ms(20);
    }

    set_color(ST, COLOR_DEFAULT);
    Print(L"\n\nMatrix selesai.\n\n");
}

void app_calculator(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_YELLOW);
    Print(L"=== KALKULATOR Aritmatika ZEON ===\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN a = 24, b = 6;
    Print(L"Hasil untuk A = 24 dan B = 6:\n");
    Print(L"  A + B = %d\n", a + b);
    Print(L"  A - B = %d\n", a - b);
    Print(L"  A * B = %d\n", a * b);
    Print(L"  A / B = %d\n\n", a / b);
}

void app_game_tebak_angka(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_YELLOW);
    Print(L"=== GAME TEBAK ANGKA ZEON ===\n");
    set_color(ST, COLOR_DEFAULT);

    UINTN target = 7;
    EFI_INPUT_KEY Key;

    Print(L"Tebak angka (1-9): ");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
    Print(L"%c\n", Key.UnicodeChar);

    if (Key.UnicodeChar == (L'0' + target)) {
        set_color(ST, COLOR_PROMPT);
        Print(L"[BENAR] Tebakan lo tepat!\n\n");
    } else {
        set_color(ST, COLOR_RED);
        Print(L"[SALAH] Angka yang benar: %d\n\n", target);
    }
    set_color(ST, COLOR_DEFAULT);
}

void display_help(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_CYAN);
    Print(L"=== DAFTAR PERINTAH ZEON OS ===\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"  neofetch                 - Informasi statistik sistem\n");
    Print(L"  fust                     - Wallpaper FUST Speed Grid\n");
    Print(L"  cyber                    - Wallpaper Cyberpunk Grid\n");
    Print(L"  matrix                   - Animasi Matrix\n");
    Print(L"  calc                     - Kalkulator sistem\n");
    Print(L"  game                     - Mini game tebak angka\n");
    Print(L"  ls                       - List file RAMDisk\n");
    Print(L"  cat <file>               - Baca isi file\n");
    Print(L"  touch <file>             - Buat file baru\n");
    Print(L"  edit <file>              - Open Code Editor / IDE\n");
    Print(L"  rename <lama> <baru>     - Ubah nama file\n");
    Print(L"  rm <file>                - Hapus file\n");
    Print(L"  clear                    - Bersihkan layar\n");
    Print(L"  poweroff                 - Matikan komputer\n\n");
}

// ==========================================
// KERNEL MAIN ENTRY POINT
// ==========================================

EFI_STATUS kernel_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    vfs_init();
    clear_screen(ST);

    set_color(ST, COLOR_PROMPT);
    Print(L"--------------------------------------------------\n");
    Print(L"   ZEON OS BAREMETAL KERNEL v2.0 [FUST CORE]      \n");
    Print(L"--------------------------------------------------\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"Ketik 'neofetch' atau 'help' untuk daftar perintah.\n\n");

    EFI_INPUT_KEY Key;
    CHAR16 input_buffer[BUFFER_SIZE];
    CHAR16 cmd[32], arg1[64], arg2[64];
    UINTN buf_idx = 0;

    while (1) {
        // ZEON Shell Prompt: root@zeon:~#
        set_color(ST, COLOR_PROMPT);
        Print(L"root@zeon");
        set_color(ST, COLOR_DEFAULT);
        Print(L":");
        set_color(ST, COLOR_CYAN);
        Print(L"~");
        set_color(ST, COLOR_RED);
        Print(L"# ");
        set_color(ST, COLOR_DEFAULT);

        // Sub-loop Keyboard Input
        buf_idx = 0;
        while (1) {
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

            if (Key.UnicodeChar == L'\r') {
                Print(L"\n");
                input_buffer[buf_idx] = L'\0';
                break;
            } else if (Key.UnicodeChar == L'\b') {
                if (buf_idx > 0) {
                    buf_idx--;
                    Print(L"\b \b");
                }
            } else if (Key.UnicodeChar >= 32 && buf_idx < (BUFFER_SIZE - 1)) {
                input_buffer[buf_idx++] = Key.UnicodeChar;
                Print(L"%c", Key.UnicodeChar);
            }
        }

        // Parsing Command & Arguments
        split_args(input_buffer, cmd, arg1, arg2);

        if (StrCmp(cmd, L"help") == 0) {
            display_help(ST);
        } else if (StrCmp(cmd, L"neofetch") == 0) {
            app_neofetch(ST, RT);
        } else if (StrCmp(cmd, L"fust") == 0) {
            render_fust_wallpaper(ST);
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            clear_screen(ST);
        } else if (StrCmp(cmd, L"cyber") == 0) {
            render_cyber_wallpaper(ST);
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            clear_screen(ST);
        } else if (StrCmp(cmd, L"matrix") == 0) {
            app_matrix_effect(ST);
        } else if (StrCmp(cmd, L"calc") == 0) {
            app_calculator(ST);
        } else if (StrCmp(cmd, L"game") == 0) {
            app_game_tebak_angka(ST);
        } else if (StrCmp(cmd, L"ls") == 0) {
            vfs_ls(ST);
        } else if (StrCmp(cmd, L"cat") == 0) {
            vfs_cat(ST, arg1);
        } else if (StrCmp(cmd, L"touch") == 0) {
            vfs_touch(ST, arg1);
        } else if (StrCmp(cmd, L"edit") == 0) {
            app_code_editor(ST, arg1);
        } else if (StrCmp(cmd, L"rename") == 0) {
            vfs_rename(ST, arg1, arg2);
        } else if (StrCmp(cmd, L"rm") == 0) {
            vfs_rm(ST, arg1);
        } else if (StrCmp(cmd, L"clear") == 0) {
            clear_screen(ST);
        } else if (StrCmp(cmd, L"poweroff") == 0 || StrCmp(cmd, L"shutdown") == 0) {
            set_color(ST, COLOR_YELLOW);
            Print(L"Mematikan ZEON OS secara aman...\n");
            sleep_ms(300);
            uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        } else if (buf_idx > 0) {
            set_color(ST, COLOR_RED);
            Print(L"zeon-shell: command '%s' tidak ditemukan!\n\n", cmd);
            set_color(ST, COLOR_DEFAULT);
        }
    }

    return EFI_SUCCESS;
}
