#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    void sleep_ms(int ms) { Sleep(ms); }
#else
    #include <unistd.h>
    void sleep_ms(int ms) { usleep(ms * 1000); }
#endif

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ---------------------------------------------------------
// EASTER EGG FUNCTIONS
// ---------------------------------------------------------

// Easter Egg 1: Digital Rain (Matrix Mode)
void trigger_matrix_easter_egg() {
    clear_screen();
    printf("\x1b[32m"); // Text Warna Hijau Matrix
    
    char chars[] = "01ZEONOSFUST109876543210ABCDEF";
    int num_chars = strlen(chars);

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 60; j++) {
            printf("%c ", chars[rand() % num_chars]);
        }
        printf("\n");
        sleep_ms(50);
    }

    printf("\n");
    printf("       \\ \\___/ /\n");
    printf("      (  [\x1b[36mO\x1b[32m] [\x1b[36mO\x1b[32m] )   <-- \"WAKE UP, CAPTAIN... THE MATRIX HAS YOU.\"\n");
    printf("       \\   u   /\n");
    printf("        \\_____/\n\n");
    printf("\x1b[0m"); // Reset warna
}

// Easter Egg 2: Coffee Command
void trigger_coffee_easter_egg() {
    printf("\n");
    printf("\x1b[33m"); // Warna Kuning / Cokelat
    printf("      (  )   (   )  )\n");
    printf("       ) (   )  (  (\n");
    printf("      (____/\x1b[0m_____\x1b[33m\\__)\n");
    printf("      |                |___\n");
    printf("      |  \x1b[36mZEON CAFFEINE\x1b[33m |   |  \x1b[32m[FUST]\x1b[0m Here is your Coffee, Captain!\n");
    printf("      |   \x1b[32mBOOST +100%%\x1b[33m  |___|  System energy fully restored ☕⚡\n");
    printf("      \\________________/\n");
    printf("\x1b[0m");
}

// Easter Egg 3: Konami Code / Overclock
void trigger_overclock_easter_egg() {
    clear_screen();
    printf("\x1b[35m"); // Warna Magenta / Rainbow Effect
    printf("=========================================================\n");
    printf("          ⚡ OVERCLOCK MODE UNLOCKED! (300%%) ⚡         \n");
    printf("=========================================================\n");
    printf("\x1b[31m  [CPU]\x1b[0m Core 1..8: 5.2 GHz [MAX]\n");
    printf("\x1b[33m  [GPU]\x1b[0m Render Pipeline: HYPER-SPEED\n");
    printf("\x1b[32m  [RAM]\x1b[0m Memory Latency: 0.0001 ms\n");
    printf("=========================================================\n");
    printf("       \\ \\___/ /\n");
    printf("      (  \x1b[31m(#)\x1b[35m \x1b[31m(#)\x1b[35m )   <-- \"MAXIMUM POWER ENGAGED!\"\n");
    printf("       \\   U   /\n");
    printf("=========================================================\n");
    printf("\x1b[0m");
}

// Easter Egg 4: Lore Fust
void trigger_fust_origin() {
    printf("\n");
    printf("\x1b[36m"); // Warna Cyan
    printf("=========================================================\n");
    printf("                 [ FUST SENTINEL ORIGIN ]                \n");
    printf("=========================================================\n");
    printf(" Name       : Fust\n");
    printf(" Species    : Cyber Owl (Sentinel Class)\n");
    printf(" Creation   : ZEONOS Security Core Protocol\n");
    printf(" Mission    : Protect Captain's System from intruders,\n");
    printf("              memory leaks, and unauthorized access.\n");
    printf(" Loyalty    : 100%% Absolute to Captain Zeon.\n");
    printf("=========================================================\n");
    printf("\x1b[0m");
}

// ---------------------------------------------------------
// MAIN TERMINAL LOOP
// ---------------------------------------------------------

int main() {
    char input[128];

    clear_screen();
    printf("=========================================================\n");
    printf("              ZEONOS INTERACTIVE SHELL v1.0              \n");
    printf("               Type 'help' for command list              \n");
    printf("=========================================================\n\n");

    while (1) {
        printf("\x1b[32mcaptain@zeonos\x1b[0m:\x1b[34m~\x1b[0m$ ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Hapus newline \n di akhir input
        input[strcspn(input, "\r\n")] = 0;

        if (strlen(input) == 0) continue;

        // Cek Perintah Standar
        if (strcmp(input, "help") == 0) {
            printf("\n Available commands:\n");
            printf("  - help     : Show this menu\n");
            printf("  - clear    : Clear terminal screen\n");
            printf("  - status   : Check system status\n");
            printf("  - exit     : Close shell\n");
            printf("  \x1b[90m(Psst... try searching for hidden secrets!)\x1b[0m\n\n");
        } 
        else if (strcmp(input, "clear") == 0) {
            clear_screen();
        } 
        else if (strcmp(input, "status") == 0) {
            printf("\n\x1b[32m[SYSTEM OK]\x1b[0m ZEONOS Kernel online. Fust Sentinel active.\n\n");
        } 
        else if (strcmp(input, "exit") == 0) {
            printf("\n\x1b[33m[FUST]\x1b[0m Shell closed. Goodbye Captain!\n");
            break;
        }
        // -----------------------------------------------------
        // EASTER EGG TRIGGERS (SECRET COMMANDS)
        // -----------------------------------------------------
        else if (strcmp(input, "matrix") == 0) {
            trigger_matrix_easter_egg();
        } 
        else if (strcmp(input, "sudo make me a coffee") == 0 || strcmp(input, "coffee") == 0) {
            trigger_coffee_easter_egg();
        } 
        else if (strcmp(input, "uuddlrlrba") == 0 || strcmp(input, "overclock") == 0) {
            trigger_overclock_easter_egg();
        } 
        else if (strcmp(input, "who is fust") == 0 || strcmp(input, "fust origin") == 0) {
            trigger_fust_origin();
        } 
        else {
            printf("\x1b[31mCommand '%s' not found. Type 'help' for assistance.\x1b[0m\n", input);
        }
    }

    return 0;
}
