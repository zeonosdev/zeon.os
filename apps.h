#ifndef APPS_H
#define APPS_H

#include <efi.h>
#include <efilib.h>

// 1. Aplikasi Tebak Angka
void app_game_tebak_angka(EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    Print(L"=====================================\n");
    Print(L"      GAME: TEBAK ANGKA (1 - 10)     \n");
    Print(L"=====================================\n\n");

    // Menggunakan timer UEFI sebagai acuan angka acak (misal angka rahasianya: 7)
    UINTN target = 7; 
    EFI_INPUT_KEY Key;

    Print(L"Tebak angka antara 1 sampai 9!\n");
    Print(L"Tekan tombol angka di keyboard: ");

    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

    Print(L"%c\n\n", Key.UnicodeChar);

    if (Key.UnicodeChar == (L'0' + target)) {
        Print(L"[MENANG] Tebakan lo BENAR! Angkanya emang %d!\n", target);
    } else {
        Print(L"[KALAH] SALAH! Angka yang bener adalah %d.\n", target);
    }

    Print(L"\nTekan tombol apa saja untuk kembali ke menu...");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

// 2. Aplikasi Jam & Tanggal Real-Time dari CMOS/BIOS
void app_jam_sistem(EFI_SYSTEM_TABLE *ST, EFI_RUNTIME_SERVICES *RT) {
    uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
    Print(L"=====================================\n");
    Print(L"        INFORMASI WAKTU REAL-TIME     \n");
    Print(L"=====================================\n\n");

    EFI_TIME Time;
    // Mengambil waktu langsung dari chip motherboard
    uefi_call_wrapper(RT->GetTime, 2, &Time, NULL);

    Print(L"Tanggal : %02d-%02d-%04d\n", Time.Day, Time.Month, Time.Year);
    Print(L"Waktu   : %02d:%02d:%02d (UTC)\n\n", Time.Hour, Time.Minute, Time.Second);

    Print(L"Tekan tombol apa saja untuk kembali ke menu...");
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

#endif
