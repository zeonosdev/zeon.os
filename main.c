#include <efi.h>
#include <efilib.h>

// ==========================================
// CONFIGURATION & DEFINITIONS
// ==========================================

#define OS_NAME         L"ZEON OS"
#define OS_VERSION      L"1.0-RELEASE"
#define OS_CODENAME     L"FUST GRAPHICS COMPLETE"
#define MAX_FILES       20
#define BUFFER_SIZE     256
#define FILE_MAX_TEXT   1024

// Color Palette Definitions
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

// Split String Parser Helper
void split_args(CHAR16 *input, CHAR16 *cmd, CHAR16 *arg1, CHAR16 *arg2) {
    UINTN i = 0, j = 0;
    cmd[0] = L'\0';
    arg1[0] = L'\0';
    arg2[0] = L'\0';

    while (input[i] != L'\0' && input[i] != L' ') {
        cmd[j++] = input[i++];
    }
    cmd[j] = L'\0';

    if (input[i] == L'\0') return;
    i++;

    j = 0;
    while (input[i] != L'\0' && input[i] != L' ') {
        arg1[j++] = input[i++];
    }
    arg1[j] = L'\0';

    if (input[i] == L'\0') return;
    i++;

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
    StrCpy(ramdisk[0].content, L"// ZEON OS Kernel Source Code\nvoid kernel_main() {\n    // FUST Engine Ready\n}");
    ramdisk[0].is_used = TRUE;
    ramdisk[0].size = StrLen(ramdisk[0].content) * sizeof(CHAR16);

    StrCpy(ramdisk[1].name, L"config.fust");
    StrCpy(ramdisk[1].content, L"RENDERER=FUST_GRID\nRESOLUTION=1080P\nFPS=60");
    ramdisk[1].is_used = TRUE;
    ramdisk[1].size = StrLen(ramdisk[1].content) * sizeof(CHAR16);

    StrCpy(ramdisk[2].name, L"readme.txt");
    StrCpy(ramdisk[2].content, L"Selamat datang di ZEON OS 100% Complete!\nGunakan 'explorer' atau 'edit <file>'.");
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

void vfs_cp(EFI_SYSTEM_TABLE *ST, CHAR16 *src, CHAR16 *dest) {
    if (src[0] == L'\0' || dest[0] == L'\0') {
        set_color(ST, COLOR_RED);
        Print(L"Penggunaan: cp <file_asal> <file_tujuan>\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    INTN src_idx = -1;
    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used && StrCmp(src, ramdisk[i].name) == 0) {
            src_idx = i;
            break;
        }
    }

    if (src_idx == -1) {
        set_color(ST, COLOR_RED);
        Print(L"cp: File asal '%s' tidak ditemukan!\n\n", src);
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (!ramdisk[i].is_used) {
            StrCpy(ramdisk[i].name, dest);
            StrCpy(ramdisk[i].content, ramdisk[src_idx].content);
            ramdisk[i].is_used = TRUE;
            ramdisk[i].size = ramdisk[src_idx].size;
            set_color(ST, COLOR_PROMPT);
            Print(L"File berhasil dikiaskan dari '%s' ke '%s'.\n\n", src, dest);
            set_color(ST, COLOR_DEFAULT);
            return;
        }
    }

    set_color(ST, COLOR_RED);
    Print(L"cp: Slot RAMDisk Penuh!\n\n");
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

void vfs_df(EFI_SYSTEM_TABLE *ST) {
    UINTN used_slots = 0;
    UINTN total_bytes = 0;

    for (UINTN i = 0; i < MAX_FILES; i++) {
        if (ramdisk[i].is_used) {
            used_slots++;
            total_bytes += ramdisk[i].size;
        }
    }

    set_color(ST, COLOR_CYAN);
    Print(L"=== STATISTIK MEMORI RAMDISK ===\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"Slot Terpakai : %d / %d\n", used_slots, MAX_FILES);
    Print(L"Slot Kosong   : %d\n", MAX_FILES - used_slots);
    Print(L"Total Ukuran  : %d Bytes\n\n", total_bytes);
}

// ==========================================
// INTERACTIVE FILE EXPLORER & CODE IDE
// ==========================================

void app_file_explorer(EFI_SYSTEM_TABLE *ST) {
    clear_screen(ST);
    set_color(ST, COLOR_YELLOW);
    Print(L"===========================================================\n");
    Print(L"            ZEON OS VISUAL FILE EXPLORER v1.0              \n");
    Print(L"===========================================================\n\n");
    set_color(ST, COLOR_DEFAULT);

    vfs_ls(ST);
    vfs_df(ST);

    Print(L"Tekan sembarang tombol untuk kembali ke terminal...");
    EFI_INPUT_KEY Key;
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
    clear_screen(ST);
}

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
        Print(L"edit: RAMDisk Penuh!\n\n");
        set_color(ST, COLOR_DEFAULT);
        return;
    }

    clear_screen(ST);
    set_color(ST, COLOR_YELLOW);
    Print(L"===========================================================\n");
    Print(L"      ZEON OS TEXT EDITOR / CODE IDE                       \n");
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

        if (Key.ScanCode == 0x17) { // ESC Key
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
    if (!gop) return;

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
                    color = 0xFF5500;
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
    Print(L"                       "); set_color(ST, COLOR_YELLOW); Print(L" Status    : "); set_color(ST, COLOR_DEFAULT); Print(L"100%% Baremetal Ready\n\n");
    set_color(ST, COLOR_DEFAULT);
}

void display_help(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, COLOR_CYAN);
    Print(L"=== DAFTAR PERINTAH ZEON OS ===\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"  neofetch                 - Informasi statistik sistem\n");
    Print(L"  explorer                 - Buka Visual File Manager\n");
    Print(L"  fust / cyber             - Render Wallpaper FUST / Cyber\n");
    Print(L"  ls                       - List file RAMDisk\n");
    Print(L"  cat <file>               - Baca isi file\n");
    Print(L"  touch <file>             - Buat file baru\n");
    Print(L"  edit <file>              - Buka Code Editor / IDE\n");
    Print(L"  cp <asal> <tujuan>       - Duplikat file\n");
    Print(L"  rename <lama> <baru>     - Ubah nama file\n");
    Print(L"  rm <file>                - Hapus file\n");
    Print(L"  df                       - Statistik memori VFS\n");
    Print(L"  clear                    - Bersihkan layar\n");
    Print(L"  poweroff                 - Shutdown komputer\n\n");
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
    Print(L"   ZEON OS BAREMETAL KERNEL v2.5 [COMPLETE CORE]  \n");
    Print(L"--------------------------------------------------\n");
    set_color(ST, COLOR_DEFAULT);
    Print(L"Ketik 'neofetch', 'explorer', atau 'help'.\n\n");

    EFI_INPUT_KEY Key;
    CHAR16 input_buffer[BUFFER_SIZE];
    CHAR16 cmd[32], arg1[64], arg2[64];
    UINTN buf_idx = 0;

    while (1) {
        set_color(ST, COLOR_PROMPT);
        Print(L"root@zeon");
        set_color(ST, COLOR_DEFAULT);
        Print(L":");
        set_color(ST, COLOR_CYAN);
        Print(L"~");
        set_color(ST, COLOR_RED);
        Print(L"# ");
        set_color(ST, COLOR_DEFAULT);

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

        split_args(input_buffer, cmd, arg1, arg2);

        if (StrCmp(cmd, L"help") == 0) {
            display_help(ST);
        } else if (StrCmp(cmd, L"neofetch") == 0) {
            app_neofetch(ST, RT);
        } else if (StrCmp(cmd, L"explorer") == 0) {
            app_file_explorer(ST);
        } else if (StrCmp(cmd, L"fust") == 0) {
            render_fust_wallpaper(ST);
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            clear_screen(ST);
        } else if (StrCmp(cmd, L"cyber") == 0) {
            render_cyber_wallpaper(ST);
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            clear_screen(ST);
        } else if (StrCmp(cmd, L"ls") == 0) {
            vfs_ls(ST);
        } else if (StrCmp(cmd, L"cat") == 0) {
            vfs_cat(ST, arg1);
        } else if (StrCmp(cmd, L"touch") == 0) {
            vfs_touch(ST, arg1);
        } else if (StrCmp(cmd, L"edit") == 0) {
            app_code_editor(ST, arg1);
        } else if (StrCmp(cmd, L"cp") == 0) {
            vfs_cp(ST, arg1, arg2);
        } else if (StrCmp(cmd, L"rename") == 0) {
            vfs_rename(ST, arg1, arg2);
        } else if (StrCmp(cmd, L"rm") == 0) {
            vfs_rm(ST, arg1);
        } else if (StrCmp(cmd, L"df") == 0) {
            vfs_df(ST);
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
