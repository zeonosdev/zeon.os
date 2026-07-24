#include <stdio.h>
#include <string.h>

// Hash resmi dari build asli ZEONOS (dihasilkan saat kompilasi resmi)
const char* OFFICIAL_KERNEL_HASH = "a8f5c2e190...zeonos_official_key";

// Fungsi untuk mengecek keaslian kernel
int check_system_integrity() {
    char current_hash[64];
    
    // Hitung hash dari memori kernel saat ini
    calculate_kernel_hash(current_hash);

    // Bandingkan hash lokal dengan Hash Resmi
    if (strcmp(current_hash, OFFICIAL_KERNEL_HASH) == 0) {
        return 1; // ASLI
    } else {
        return 0; // MODIF
    }
}
// DONT MODIF THIS
void print_system_banner() {
    if (check_system_integrity()) {
        printf("\033[32m[SYSTEM STATUS: OFFICIAL ZEONOS BUILD]\033[0m\n");
    } else {
        printf("\033[31m[WARNING: MODIFIED / CUSTOM BUILD DETECTED]\033[0m\n");
        printf("Note: This copy of ZEONOS contains modified source code.\n");
    }
}
