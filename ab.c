// Fungsi ganti warna teks UEFI
void set_color(UINTN color) {
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, color);
}

// Contoh pemakaian di Banner:
set_color(EFI_CYAN); // Ubah warna jadi Cyan
Print(L"  __  ____  __  ____ _____  _____ _____ \n");
Print(L" |  \\/  \\ \\/ / / __ \\  __ \\/ ____|_   _|\n");
Print(L" | \\  / |\\  / | |  | | |__) | (___   | |  \n");
Print(L" | |\\/| | /  \\ | |  | |  _  / \\___ \\  | |  \n");
Print(L" |_|  |_|/_/\\_\\| |__| | | \\ \\ ____) |_| |_ \n");
Print(L"                \\____/|_|  \\_\\_____/|_____|\n\n");

set_color(EFI_WHITE); // Balikin ke warna putih biasa
