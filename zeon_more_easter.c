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
// NEW EASTER EGG FUNCTIONS
// ---------------------------------------------------------

// 1. Cat Fust Easter Egg
void trigger_cat_easter_egg() {
    printf("\n\x1b[35m");
    printf("       /\\_/\\\n");
    printf("      ( o.o )\x1b[0m   <-- \"Meow! Fust's official cyber cat says hi!\"\n");
    printf("\x1b[35m       > ^ < \n\x1b[0m\n");
}

// 2. Fust Dance Animation
void trigger_dance_easter_egg() {
    clear_screen();
    for (int i = 0; i < 3; i++) {
        clear_screen();
        printf("\x1b[33m\n");
        printf("       \\ \\___/ /\n");
        printf("      (  (> )(<) )   <-- \"ZEONOS DANCE PARTY!\" 🎵\n");
        printf("       \\   u   /\n");
        printf("       /_______\\\n\x1b[0m");
        sleep_ms(300);

        clear_screen();
        printf("\x1b[36m\n");
        printf("       \\ \\___/ /\n");
        printf("      (  (< )(>) )   <-- \"BOOGIE FEVER!\" 🎶\n");
        printf("       \\   u   /\n");
        printf("       \\_______/\n\x1b[0m");
        sleep_ms(300);
    }
    printf("\n");
}

// 3. Ping Moon (Interplanetary Ping)
void trigger_ping_moon() {
    printf("\n\x1b[34m[NETWORK]\x1b[0m Pinging Moon Base Alpha [173.194.2.1] with 32 bytes of data:\n");
    sleep_ms(400);
    printf("Reply from 173.194.2.1: bytes=32 time=1280ms TTL=54\n");
    sleep_ms(400);
    printf("Reply from 173.194.2.1: bytes=32 time=1310ms TTL=54\n");
    sleep_ms(400);
    printf("Reply from 173.194.2.1: bytes=32 time=1295ms TTL=54\n\n");
    printf("\x1b[32m[FUST]\x1b[0m Moon latency is stable at ~1.3 seconds, Captain!\n\n");
}

// 4. Fake Hack NASA
void trigger_hack_nasa() {
    printf("\n\x1b[31m[INITIATING HACK]\x1b[0m Connecting to NASA Satellite Core...\n");
    sleep_ms(500);
    printf("\x1b[33m[BYPASSING FIREWALL]\x1b[0m 25%%... 50%%... 75%%...\n");
    sleep_ms(700);
    printf("\n\x1b[41m\x1b[37m [SECURITY BLOCKED] \x1b[0m\n");
    printf("       \\ \\___/ /\n");
    printf("      (  (O) (O) )    <-- \"NICE TRY CAPTAIN! BUT FUST SAYS NO ILLEGAL HACKING!\"\n");
    printf("       \\   u   /\n");
    printf("        \\_____/\n\n");
}

// 5. Fortune / Daily Wisdom
void trigger_fortune() {
    char *fortunes[] = {
        "A smooth compilation is heading your way today!",
        "Fust predicts 0 memory leaks in your next code.",
        "Great power comes with great overclocks.",
        "Stay hydrated! Even kernels need cool temperatures."
    };
    int index = rand() % 4;

    printf("\n\x1b[36m[FUST WISDOM]\x1b[0m 🔮 %s\n\n", fortunes[index]);
}

// ---------------------------------------------------------
// MAIN TERMINAL SHELL LOOP
// ---------------------------------------------------------

int main() {
    char input[128];

    clear_screen();
    printf("=========================================================\n");
    printf("         ZEONOS TERMINAL - ADVANCED EASTER EGGS         \n");
    printf("         Type commands or search for hidden gems!        \n");
    printf("=========================================================\n\n");

    while (1) {
        printf("\x1b[32mcaptain@zeonos\x1b[0m:\x1b[34m~\x1b[0m$ ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\r\n")] = 0;

        if (strlen(input) == 0) continue;

        if (strcmp(input, "exit") == 0) {
            printf("\n\x1b[33m[FUST]\x1b[0m See you Captain!\n");
            break;
        } 
        else if (strcmp(input, "clear") == 0) {
            clear_screen();
        }
        // -----------------------------------------------------
        // NEW EASTER EGG TRIGGERS
        // -----------------------------------------------------
        else if (strcmp(input, "cat fust") == 0) {
            trigger_cat_easter_egg();
        } 
        else if (strcmp(input, "zeon dance") == 0) {
            trigger_dance_easter_egg();
        } 
        else if (strcmp(input, "ping moon") == 0) {
            trigger_ping_moon();
        } 
        else if (strcmp(input, "hack nasa") == 0) {
            trigger_hack_nasa();
        } 
        else if (strcmp(input, "fortune") == 0) {
            trigger_fortune();
        } 
        else {
            printf("\x1b[31mCommand '%s' not found.\x1b[0m\n", input);
        }
    }

    return 0;
}
