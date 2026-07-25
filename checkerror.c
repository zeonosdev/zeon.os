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

// Easter Egg: Fake Critical Error
void trigger_fake_error() {
    printf("\n");
    printf("\x1b[31m[CRITICAL ERROR] Core Kernel Leak Detected at 0x0000FFFF!\x1b[0m\n");
    sleep_ms(600);
    printf("\x1b[31m[WARNING] Quantum Overheat Failure: Temperature at 999°C!\x1b[0m\n");
    sleep_ms(800);
    printf("\x1b[31m[ALERT] System Purge in 3... 2... 1...\x1b[0m\n");
    sleep_ms(1200);

    // Bikin efek jeda dramatis, lalu Fust muncul menenangkan
    printf("\n");
    printf("\x1b[33m       \\ \\___/ /\n");
    printf("      (  (> ) ( <) )   <-- \"GOTCHA, CAPTAIN! 😜\"\n");
    printf("       \\   u   /\n");
    printf("        \\_____/\x1b[0m\n\n");
    printf("\x1b[32m[FUST STATUS]\x1b[0m Relax, Captain! All systems are 100%% safe.\n");
    printf("\x1b[32m              This was just a fake error simulation!\x1b[0m\n\n");
}

int main() {
    char input[128];

    printf("ZEONOS Terminal Ready. Type 'check error' to check error.\n\n");

    while (1) {
        printf("\x1b[32mcaptain@zeonos\x1b[0m:\x1b[34m~\x1b[0m$ ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\r\n")] = 0;

        if (strcmp(input, "exit") == 0) break;

        // Trigger Error Palsu
        if (strcmp(input, "check error") == 0) {
            trigger_fake_error();
        } else {
            printf("Command '%s' executed normally.\n", input);
        }
    }

    return 0;
}
