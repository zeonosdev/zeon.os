#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    UINTN MenuOption = 0;
    EFI_INPUT_KEY Key;

    while (1) {
        // Bersihkan layar
        uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);

        // Header Menu Utama
        Print(L"=====================================\n");
        Print(L"          MY CUSTOM OS MENU          \n");
        Print(L"=====================================\n\n");
        Print(L"Pilih Aplikasi (Gunakan Panah Atas/Bawah & Enter):\n\n");

        // Tampilan Pilihan Menu
        if (MenuOption == 0) Print(L" -> [1] Aplikasi Kalkulator\n");
        else                 Print(L"    [1] Aplikasi Kalkulator\n");

        if (MenuOption == 1) Print(L" -> [2] Informasi Sistem / PC\n");
        else                 Print(L"    [2] Informasi Sistem / PC\n");

        if (MenuOption == 2) Print(L" -> [3] Shutdown / Matikan PC\n");
        else                 Print(L"    [3] Shutdown / Matikan PC\n");

        // Tunggu input dari Keyboard
        WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

        // Navigasi Panah
        if (Key.ScanCode == 0x01) { // Panah Atas
            if (MenuOption > 0) MenuOption--;
        } 
        else if (Key.ScanCode == 0x02) { // Panah Bawah
            if (MenuOption < 2) MenuOption++;
        }
        else if (Key.UnicodeChar == L'\r') { // Tombol Enter
            uefi_call_wrapper(ST->ConOut->ClearScreen, 2, ST->ConOut);
            
            // Logika saat aplikasi dipilih
            if (MenuOption == 0) {
                Print(L"=== APLIKASI KALKULATOR ===\n");
                Print(L"1 + 1 = 2 (Dummy App)\n\nTekan tombol apa saja...");
                WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            } 
            else if (MenuOption == 1) {
                Print(L"=== INFORMASI SISTEM ===\n");
                Print(L"OS Mode: UEFI x86_64 Bare-Metal\n\nTekan tombol apa saja...");
                WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            } 
            else if (MenuOption == 2) {
                Print(L"Mematikan sistem...\n");
                uefi_call_wrapper(RT->ResetSystem, 4, EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            }
        }
    }

    return EFI_SUCCESS;
}
