#include <efi.h>
#include <efilib.h>

// ==========================================
// HELPER & UTILITIES
// ==========================================

// Fungsi ganti warna teks UEFI
void set_color(EFI_SYSTEM_TABLE *ST, UINTN Attribute) {
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, Attribute);
}

// Tampilan Banner Logo ASCII Warna-Warni
void draw_cyber_banner(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, EFI_LIGHTGREEN);
    Print(L"  ___   _____   _____   ___ _____ _____ \n");
    Print(L" / _ \\ /  ___| /  ___| / _ \\___  /  ___|\n");
    Print(L"/ /_\\ \\\\ `--.  \\ `--. / /_\\ \\ / /\\ `--. \n");
    Print(L"|  _  | `--. \\  `--. \\|  _  |/ /  `--. \\\n");
    Print(L"| | | |/\\__/ / /\\__/ /| | | / /  /\\__/ \\\n");
    Print(L"\\_| |_/\\____/  \\____/ \\_| |_/\\_/  \\____/ \n");
    
    set_color(ST, EFI_CYAN);
    Print(L"===========================================\n");
    Print(L"       ATRUM OS - BAREMETAL EDITION        \n");
    Print(L"===========================================\n\n");
    set_color(ST, EFI_LIGHTGRAY);
}

// ==========================================
// APLIKASI & MODUL OS
// ==========================================

// 1. Terminal / Shell Interaktif
void app_terminal_shell(EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    set_color(ST, EFI_YELLOW);
    Print(L"=== TERMINAL SHELL v1.0 ===\n");
    Print(L"Ketik 'help' untuk daftar perintah, atau 'exit' untuk keluar.\n\n");
    set_color(ST, EFI_LIGHTGRAY);

    EFI_INPUT_KEY Key;
    CHAR16 buffer[100];
    UINTN buf_idx = 0;

    while (1) {
        set_color(ST, EFI_LIGHTCYAN);
        Print(L"user@atrum-os> ");
        set_color(ST, EFI_LIGHTGRAY);

        buf_idx = 0;
        
        while (1) {
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

            if (Key.UnicodeChar == L'\r') { // Enter
                Print(L"\n");
                buffer[buf_idx] = L'\0';
                break;
            } 
            else if (Key.UnicodeChar == L'\b') { // Backspace
                if (buf_idx > 0) {
                    buf_idx--;
                    Print(L"\b \b");
                }
            } 
            else if (Key.UnicodeChar >= 32 && buf_idx < 99) {
                buffer[buf_idx++] = Key.UnicodeChar;
                Print(L"%c", Key.UnicodeChar);
            }
        }

        if (StrCmp(buffer, L"help") == 0) {
            Print(L"Daftar Perintah:\n");
            Print(L"  help   - Menampilkan bantuan ini\n");
            Print(L"  about  - Info pembuat OS\n");
            Print(L"  clear  - Bersihkan layar\n");
            Print(L"  exit   - Kembali ke menu utama\n\n");
        } 
        else if (StrCmp(buffer, L"about") == 0) {
            Print(L"Atrum OS v1.0 - Custom Bare-Metal OS berbasis UEFI C.\n\n");
        } 
        else if (StrCmp(buffer, L"clear") == 0) {
            uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
        } 
        else if (StrCmp(buffer, L"exit") == 0) {
            break;
        } 
        else if (buf_idx > 0) {
            set_color(ST, EFI_RED);
            Print(L"Command '%s' tidak ditemukan!\n\n", buffer);
            set_color(ST, EFI_LIGHTGRAY);
        }
    }
}

// 2. Animasi Efek Matrix
void app_matrix_effect(EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    set_color(ST, EFI_LIGHTGREEN);

    Print(L"[EFEK MATRIX] Tekan tombol apa saja untuk keluar...\n\n");

    EFI_INPUT_KEY Key;
    UINTN counter = 0;
    
    while (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key) == EFI_NOT_READY) {
        CHAR16 random_char = L'A' + (counter % 26);
        Print(L"%c %d ", random_char, counter % 9);
        
        counter++;
        if (counter % 15 == 0) Print(L"\n");
        
        for (volatile int i = 0; i < 3000000; i++); // Delay kecil
    }

    set_color(ST, EFI_LIGHTGRAY);
}

// 3. Game Tebak Angka
void app_game_tebak_angka(EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    set_color(ST, EFI_LIGHTYELLOW);
    Print(L"=====================================\n");
    Print(L"      GAME: TEBAK ANGKA (1 - 9)      \n");
    Print(L"=====================================\n\n");
    set_color(ST, EFI_LIGHTGRAY);

    UINTN target = 7; 
    EFI_INPUT_KEY Key;

    Print(L"Tebak angka rahasia antara 1 sampai 9!\n");
    Print(L"Tekan angka di keyboard: ");

    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

    Print(L"%c\n\n", Key.UnicodeChar);

    if (Key.UnicodeChar == (L'0' + target)) {
        set_color(ST, EFI_LIGHTGREEN);
        Print(L"[MENANG] Tebakan lo BENAR! Angkanya emang %d!\n", target);
    } else {
        set_color(ST, EFI_RED);
        Print(L"[KALAH] SALAH! Angka yang bener adalah %d.\n", target);
    }

    set_color(ST, EFI_LIGHTGRAY);
    Print(L"\nTekan tombol apa saja untuk kembali...");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

// 4. Jam & Tanggal Real-Time dari Motherboard
void app_jam_sistem(EFI_SYSTEM_TABLE *ST, EFI_RUNTIME_SERVICES *RT) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    set_color(ST, EFI_LIGHTCYAN);
    Print(L"=====================================\n");
    Print(L"        INFORMASI WAKTU REAL-TIME     \n");
    Print(L"=====================================\n\n");
    set_color(ST, EFI_LIGHTGRAY);

    EFI_TIME Time;
    uefi_call_wrapper(RT->GetTime, 2, &Time, NULL);

    Print(L"Tanggal : %02d-%02d-%04d\n", Time.Day, Time.Month, Time.Year);
    Print(L"Waktu   : %02d:%02d:%02d (UTC)\n\n", Time.Hour, Time.Minute, Time.Second);

    Print(L"Tekan tombol apa saja untuk kembali...");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

// ==========================================
// ENTRY POINT / MAIN PROGRAM
// ==========================================

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    UINTN MenuOption = 0;
    UINTN MaxMenu = 4; // Total 5 Opsi (0 sampai 4)
    EFI_INPUT_KEY Key;

    while (1) {
        uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);

        // Cetak Banner Atas
        draw_cyber_banner(ST);

        Print(L"Gunakan Panah Atas/Bawah & Enter untuk memilih:\n\n");

        // Opsi 1: Terminal Shell
        if (MenuOption == 0) set_color(ST, EFI_YELLOW);
        Print(L" %s [1] Terminal / Command Shell\n", (MenuOption == 0) ? L"->" : L"  ");
        set_color(ST, EFI_LIGHTGRAY);

        // Opsi 2: Matrix Effect
        if (MenuOption == 1) set_color(ST, EFI_YELLOW);
        Print(L" %s [2] Animasi Efek Matrix\n", (MenuOption == 1) ? L"->" : L"  ");
        set_color(ST, EFI_LIGHTGRAY);

        // Opsi 3: Game Tebak Angka
        if (MenuOption == 2) set_color(ST, EFI_YELLOW);
        Print(L" %s [3] Game Tebak Angka\n", (MenuOption == 2) ? L"->" : L"  ");
        set_color(ST, EFI_LIGHTGRAY);

        // Opsi 4: Jam Real-Time
        if (MenuOption == 3) set_color(ST, EFI_YELLOW);
        Print(L" %s [4] Informasi Jam & Tanggal System\n", (MenuOption == 3) ? L"->" : L"  ");
        set_color(ST, EFI_LIGHTGRAY);

        // Opsi 5: Shutdown
        if (MenuOption == 4) set_color(ST, EFI_YELLOW);
        Print(L" %s [5] Matikan Komputer (Shutdown)\n", (MenuOption == 4) ? L"->" : L"  ");
        set_color(ST, EFI_LIGHTGRAY);

        // Membaca Navigasi Keyboard
        WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

        if (Key.ScanCode == 0x01 && MenuOption > 0) {
            MenuOption--; // Panah Atas
        } 
        else if (Key.ScanCode == 0x02 && MenuOption < MaxMenu) {
            MenuOption++; // Panah Bawah
        } 
        else if (Key.UnicodeChar == L'\r') { // Enter
            if (MenuOption == 0)      app_terminal_shell(ST);
            else if (MenuOption == 1) app_matrix_effect(ST);
            else if (MenuOption == 2) app_game_tebak_angka(ST);
            else if (MenuOption == 3) app_jam_sistem(ST, RT);
            else if (MenuOption == 4) {
                uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            }
        }
    }

    return EFI_SUCCESS;
}
