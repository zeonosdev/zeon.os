#ifndef EXTRA_FEATURES_H
#define EXTRA_FEATURES_H

#include <efi.h>
#include <efilib.h>

// Helper untuk ganti warna teks UEFI
void set_color(EFI_SYSTEM_TABLE *ST, UINTN Attribute) {
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, Attribute);
}

// 1. Tampilan Banner Logo ASCII Warna-Warni
void draw_cyber_banner(EFI_SYSTEM_TABLE *ST) {
    set_color(ST, EFI_LIGHTGREEN);
    Print(L"  ___   _____   _____   ___ _____ _____ \n");
    Print(L" / _ \\ /  ___| /  ___| / _ \\___  /  ___|\n");
    Print(L"/ /_\\ \\\\ `--.  \\ `--. / /_\\ \\ / /\\ `--. \n");
    Print(L"|  _  | `--. \\  `--. \\|  _  |/ /  `--. \\\n");
    Print(L"| | | |/\\__/ / /\\__/ /| | | / /  /\\__/ /\n");
    Print(L"\\_| |_/\\____/  \\____/ \\_| |_/\\_/  \\____/ \n");
    
    set_color(ST, EFI_CYAN);
    Print(L"===========================================\n");
    Print(L"         CYBERPUNK OS - BAREMETAL          \n");
    Print(L"===========================================\n\n");
    set_color(ST, EFI_LIGHTGRAY);
}

// 2. Animasi Efek Matrix (Layar Penuh Teks Hijau Jatuh)
void app_matrix_effect(EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    set_color(ST, EFI_LIGHTGREEN);

    Print(L"[EFEK MATRIX] Tekan tombol apa saja untuk keluar...\n\n");

    EFI_INPUT_KEY Key;
    UINTN counter = 0;
    
    // Loop nampilin karakter acak sampai tombol keyboard ditekan
    while (uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key) == EFI_NOT_READY) {
        CHAR16 random_char = L'A' + (counter % 26);
        Print(L"%c %d ", random_char, counter % 9);
        
        counter++;
        if (counter % 15 == 0) Print(L"\n");
        
        // Stalling kecil biar nggak terlalu cepat
        for (volatile int i = 0; i < 5000000; i++);
    }

    set_color(ST, EFI_LIGHTGRAY);
}

// 3. Mode Terminal / Command Shell
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
        Print(L"user@my-os> ");
        set_color(ST, EFI_LIGHTGRAY);

        buf_idx = 0;
        
        // Membaca input ketikan string
        while (1) {
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

            if (Key.UnicodeChar == L'\r') { // Tombol Enter
                Print(L"\n");
                buffer[buf_idx] = L'\0';
                break;
            } 
            else if (Key.UnicodeChar == L'\b') { // Tombol Backspace
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

        // Cek Perintah Teks
        if (StrCmp(buffer, L"help") == 0) {
            Print(L"Daftar Perintah:\n");
            Print(L"  help   - Menampilkan bantuan\n");
            Print(L"  about  - Info pembuat OS\n");
            Print(L"  clear  - Bersihkan layar\n");
            Print(L"  exit   - Kembali ke menu utama\n\n");
        } 
        else if (StrCmp(buffer, L"about") == 0) {
            Print(L"OS ini dikembangkan secara bare-metal berbasis UEFI C.\n\n");
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

#endif
