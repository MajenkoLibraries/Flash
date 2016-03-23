#include <ANSI.h>
#include <Flash.h>
#include "ConfigBits.h"

ANSI ansi(Serial);

struct conf0 config0;
struct conf1 config1;
struct conf2 config2;
struct conf3 config3;

bool confirm() {
    Serial.print("Are you sure? (y/N): ");
    while (!Serial.available());
    char c = Serial.read();
    if (c >= 'a' && c <= 'z') {
        c -= 0x20;  
    }
    if (c == 'Y') {
        Serial.println("Yes");
        return true;
    }
    Serial.println("No");
    return false;
}

void reboot() {
    volatile int * p = (volatile int *)&RSWRST;

    // Unlock the system
    disableInterrupts();
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    // Perform the software reset
    RSWRSTSET=_RSWRST_SWRST_MASK;
    *p;

    // Wait for the rest to take place
    while(1);
}

void loadSettings() {
    config0.raw = DEVCFG0;
    config1.raw = DEVCFG1;
    config2.raw = DEVCFG2;
    config3.raw = DEVCFG3;
}

void saveSettings() {
    Flash.loadPage((void *)&DEVCFG3);
    Flash.writePageWord((void *)&DEVCFG0, config0.raw);
    Flash.writePageWord((void *)&DEVCFG1, config1.raw);
    Flash.writePageWord((void *)&DEVCFG2, config2.raw);
    Flash.writePageWord((void *)&DEVCFG3, config3.raw);
    Flash.savePage();   
}

const char *ws_ps[32] = {
    "1ms", "2ms", "4ms", "8ms", "16ms", "32ms", "64ms", "128ms", "256ms", "512ms", "1.024s",
    "2.048s", "4.096s", "8.192s", "16.384s", "32.768s", "65.536s", "131.072s", "262.144s", "524.288s", "1048.576s", "X", "X", "X", "X",
    "X", "X", "X", "X", "X", "X", "X"
};

void setup() {
    loadSettings();
    Serial.begin(115200);
}

void loop() {
    ansi.cls();
    ansi.setCursor(0, 0);
    heading("Main Menu");
    Serial.println();
    Serial.println(" D) Debugger");
    Serial.println(" W) Watchdog");
    Serial.println(" U) USB");
    Serial.println(" O) Oscillator");
    Serial.println(" P) Peripherals");
    Serial.println();
    Serial.println(" L) Load settings");
    Serial.println(" S) Save settings");
    Serial.println(" R) Reboot");   
    Serial.println();
    Serial.print("Select an entry: ");
    char c = getch();
    switch(c) {
        case 'L':
            if (confirm()) {
                loadSettings();
                Serial.println("Loaded.");
            }
            break;
        case 'S':
            if (confirm()) {
                saveSettings();
                Serial.println("Saved.");
            }
            break;
        case 'R':
            if (confirm()) {
                Serial.println("Rebooting...");
                Serial.flush();
                reboot();
            }
            break;

        case 'D':
            debugger();
            break;
        case 'W':
            watchdog();
            break;
        case 'U':
            usb();
            break;
        case 'O':
            oscillator();
            break;
        case 'P':
            peripherals();
            break;
    }
}

char getch() {
    while (!Serial.available());
    char c = Serial.read();
    if (c >= 'a' && c <= 'z') {
        c -= 0x20;  
    }
    return c;
}

void subMenuFooter() {
    Serial.println();
    Serial.println(" X) Exit to main menu");
    Serial.println(" L) Load settings");
    Serial.println(" S) Save settings");
    Serial.println(" R) Reboot");   
    Serial.println();
    Serial.print("Select an entry to change: ");
}

void heading(const char *t) {
    int len = strlen(t);
    for (int x = 0; x < 40 - len / 2; x++) {
        Serial.write(' ');
    }
    Serial.println(t);
    for (int x = 0; x < 80; x++) {
        Serial.write("-");
    }
    Serial.println();
}

void debugger() {
    while (1) {
        ansi.cls();
        ansi.setCursor(0, 0);
        heading("Debugger Settings");
        Serial.println();
        Serial.print(" D) Debugger .......................... ");
        switch (config0.debug) {
            case 0b11: Serial.println("Disabled"); break;
            default: Serial.println("Enabled"); break;
        }
        Serial.print(" I) ICE Pins .......................... ");
        Serial.println(config0.icesel ? "PGE2" : "PGE1");
        subMenuFooter();
        char c = getch();
        switch(c) {
            case 'X':
                return;
        case 'L':
            if (confirm()) {
                loadSettings();
                Serial.println("Loaded.");
            }
            break;
        case 'S':
            if (confirm()) {
                saveSettings();
                Serial.println("Saved.");
            }
            break;
        case 'R':
            if (confirm()) {
                Serial.println("Rebooting...");
                Serial.flush();
                reboot();
            }
            break;
        case 'D':
            if (config0.debug == 0b11) {
                config0.debug = 0b10;
            } else {
                config0.debug = 0b11;
            }
            break;
        case 'I':
            config0.icesel = 1-config0.icesel;
            break;
        }
    }
}

void watchdog() {
    while (1) {
        ansi.cls();
        ansi.setCursor(0, 0);
        heading("Watchdog Settings");
        Serial.println();
        Serial.print(" W) Watchdog .......................... ");
        switch (config1.fwdten) {
            case 0: 
                Serial.println("Software Controlled"); 
                break;
            case 1: 
                Serial.println("Always Enabled"); 
                break;
        }
        Serial.print(" T) Timeout ........................... ");
        if (ws_ps[config1.wdtps][0] == 'X') {
            config1.wdtps = 0b10100;
        }
        Serial.println(ws_ps[config1.wdtps]);

        Serial.print(" C) Clock Switching and Monitoring .... ");
        switch (config1.fcksm) {
            case 0:
                Serial.println("Switch enabled, Monitor enabled");
                break;
            case 1:
                Serial.println("Switch enabled, Monitor disabled");
                break;
            case 2:
                Serial.println("Switch disabled, Monitor disabled");
                break;
            case 3:
                Serial.println("Switch disabled, Monitor disabled");
                break;
        }
        
        subMenuFooter();
        char c = getch();
        switch(c) {
            case 'X':
                return;
        case 'L':
            if (confirm()) {
                loadSettings();
                Serial.println("Loaded.");
            }
            break;
        case 'S':
            if (confirm()) {
                saveSettings();
                Serial.println("Saved.");
            }
            break;
        case 'R':
            if (confirm()) {
                Serial.println("Rebooting...");
                Serial.flush();
                reboot();
            }
            break;
        case 'W':
            config1.fwdten++;
            break;
        case 'T':
            config1.wdtps++;
            if (config1.wdtps > 0b10100) {
                config1.wdtps = 0b00000;
            }
            break;
        case 'C':
            config1.fcksm++;
            if (config1.fcksm == 3) {
                config1.fcksm = 0;
            }
            break;
        }
    }
}

void usb() {
    while (1) {
        ansi.cls();
        ansi.setCursor(0, 0);
        heading("USB Settings");
        Serial.println();
        Serial.print(" P) USB PLL ........................... ");
        Serial.println(config2.upllen ? "Disabled" : "Enabled");
        Serial.print(" D) PLL Input Divider ................. ");
        switch (config2.upllidiv) {
            case 0:
                Serial.println("/1");
                break;
            case 1:
                Serial.println("/2");
                break;
            case 2:
                Serial.println("/3");
                break;
            case 3:
                Serial.println("/4");
                break;
            case 4:
                Serial.println("/5");
                break;
            case 5:
                Serial.println("/6");
                break;
            case 6:
                Serial.println("/10");
                break;
            case 7:
                Serial.println("/12");
                break;
        }

        Serial.print(" V) VBUSON Pin Control ................ ");
        Serial.println(config3.fvbusonio ? "Controlled by USB" : "Normal IO Port");
        
        Serial.print(" I) USBID Pin Control ................. ");
        Serial.println(config3.fusbidio ? "Controlled by USB" : "Normal IO Port");

        subMenuFooter();
        char c = getch();
        switch(c) {
            case 'X':
                return;
        case 'L':
            if (confirm()) {
                loadSettings();
                Serial.println("Loaded.");
            }
            break;
        case 'S':
            if (confirm()) {
                saveSettings();
                Serial.println("Saved.");
            }
            break;
        case 'R':
            if (confirm()) {
                Serial.println("Rebooting...");
                Serial.flush();
                reboot();
            }
            break;
        case 'P':
            config2.upllen++;
            break;
        case 'D':
            config2.upllidiv++;
            break;
        case 'V':
            config3.fvbusonio++;
            break;
        case 'I':
            config3.fusbidio++;
            break;
        }
    }
}

void oscillator() {
    while (1) {
        ansi.cls();
        ansi.setCursor(0, 0);
        heading("Oscillator Settings");
        Serial.println();
        Serial.print(" O) Oscillator ........................ ");

        switch (config1.fnosc) {
            case 0:
                Serial.println("Fast RC");
                break;
            case 1:
                Serial.println("Fast RC with Div-N and PLL");
                break;
            case 2:
                Serial.println("Primary");
                break;
            case 3:
                Serial.println("Primary with PLL");
                break;
            case 4:
                Serial.println("Secondary");
                break;
            case 5:
                Serial.println("Low-Power RC");
                break;
            case 6:
                Serial.println("Fast RC with Div-16");
                break;
            case 7:
                Serial.println("Fast RC with Div-N");
                break;
        }
        Serial.print(" 1) Primary Oscillator Mode ........... ");
        switch (config1.poscmod) {
            case 0:
                Serial.println("External");
                break;
            case 1:
                Serial.println("XT");
                break;
            case 2:
                Serial.println("HS");
                break;
            case 3:
                Serial.println("Disabled");
                break;
        }
        Serial.print(" 2) Secondary Oscillator .............. ");
        Serial.println(config1.fsoscen ? "Enabled" : "Disabled");
        
        Serial.print(" I) PLL Input Divider ................. ");
        switch (config2.fpllidiv) {
            case 0:
                Serial.println("/1");
                break;
            case 1:
                Serial.println("/2");
                break;
            case 2:
                Serial.println("/3");
                break;
            case 3:
                Serial.println("/4");
                break;
            case 4:
                Serial.println("/5");
                break;
            case 5:
                Serial.println("/6");
                break;
            case 6:
                Serial.println("/10");
                break;
            case 7:
                Serial.println("/12");
                break;
        }

        Serial.print(" M) PLL Multiplier .................... ");
        switch (config2.fpllmul) {
            case 0:
                Serial.println("x15");
                break;
            case 1:
                Serial.println("x16");
                break;
            case 2:
                Serial.println("x17");
                break;
            case 3:
                Serial.println("x18");
                break;
            case 4:
                Serial.println("x19");
                break;
            case 5:
                Serial.println("x20");
                break;
            case 6:
                Serial.println("x21");
                break;
            case 7:
                Serial.println("x24");
                break;
        }

        Serial.print(" D) PLL Output Divider ................ ");
        switch (config2.fpllodiv) {
            case 0:
                Serial.println("/1");
                break;
            case 1:
                Serial.println("/2");
                break;
            case 2:
                Serial.println("/3");
                break;
            case 3:
                Serial.println("/4");
                break;
            case 4:
                Serial.println("/5");
                break;
            case 5:
                Serial.println("/6");
                break;
            case 6:
                Serial.println("/10");
                break;
            case 7:
                Serial.println("/12");
                break;
        }

        Serial.print(" P) Peripheral Bus Divider ............ /");
        Serial.println(1 << config1.fpbdiv, DEC);

        Serial.print(" C) Clock Output ...................... ");
        Serial.println(config1.osciofnc ? "Disabled" : "Enabled");
        
        Serial.print(" T) Two Speed Startup ................. ");
        Serial.println(config1.ieso ? "Enabled" : "Disabled");

        subMenuFooter();
        char c = getch();
        switch(c) {
            case 'X':
                return;
            case 'L':
                if (confirm()) {
                    loadSettings();
                    Serial.println("Loaded.");
                }
                break;
            case 'S':
                if (confirm()) {
                    saveSettings();
                    Serial.println("Saved.");
                }
                break;
            case 'R':
                if (confirm()) {
                    Serial.println("Rebooting...");
                    Serial.flush();
                    reboot();
                }
                break;
            case 'O':
                config1.fnosc++;
                break;
            case '1':
                config1.poscmod++;
                break;
            case '2':
                config1.fsoscen++;
                break;
            case 'I':
                config2.fpllidiv++;
                break;
            case 'M':
                config2.fpllmul++;
                break;
            case 'D':
                config2.fpllodiv++;
                break;
            case 'P':
                config1.fpbdiv++;
                break;
            case 'C':
                config1.osciofnc++;
                break;
            case 'T':
                config1.ieso++;
                break;
        }
    }
}

void peripherals() {
    while (1) {
        ansi.cls();
        ansi.setCursor(0, 0);
        heading("Peripheral Settings");
        Serial.println();

        Serial.print(" C) CAN I/O Pins ...................... ");
        Serial.println(config3.fcanio ? "Default" : "Alternate");

        Serial.print(" E) Ethernet I/O Pins ................. ");
        Serial.println(config3.fethio ? "Default" : "Alternate");

        Serial.print(" M) Ethernet MII ...................... ");
        Serial.println(config3.fmiien ? "Enabled" : "Disabled");

        Serial.print(" U) User ID ........................... ");
        char tmp[10];
        sprintf(tmp, "0x%04X", config3.userid);
        Serial.println(tmp);

        subMenuFooter();
        char c = getch();
        switch(c) {
            case 'X':
                return;
            case 'L':
                if (confirm()) {
                    loadSettings();
                    Serial.println("Loaded.");
                }
                break;
            case 'S':
                if (confirm()) {
                    saveSettings();
                    Serial.println("Saved.");
                }
                break;
            case 'R':
                if (confirm()) {
                    Serial.println("Rebooting...");
                    Serial.flush();
                    reboot();
                }
                break;
            case 'C':
                config3.fcanio++;
                break;
            case 'E':
                config3.fethio++;
                break;
            case 'M':
                config3.fmiien++;
                break;
            case 'U':
                config3.userid = inputHex(config3.userid);
                break;
        }
    }
}

int inputHex(int val) {
    uint16_t hexval = val;
    char tmp[50];
    Serial.println();

    while (1) {
        sprintf(tmp, "\rEnter HEX value: 0x%04X", hexval);
        Serial.print(tmp);
        char c = getch();
        switch (c) {
            case '0':
                hexval <<= 4;
                break;
            case '1':
                hexval <<= 4;
                hexval |= 1;
                break;
            case '2':
                hexval <<= 4;
                hexval |= 2;
                break;
            case '3':
                hexval <<= 4;
                hexval |= 3;
                break;
            case '4':
                hexval <<= 4;
                hexval |= 4;
                break;
            case '5':
                hexval <<= 4;
                hexval |= 5;
                break;
            case '6':
                hexval <<= 4;
                hexval |= 6;
                break;
            case '7':
                hexval <<= 4;
                hexval |= 7;
                break;
            case '8':
                hexval <<= 4;
                hexval |= 8;
                break;
            case '9':
                hexval <<= 4;
                hexval |= 9;
                break;
            case 'A':
                hexval <<= 4;
                hexval |= 10;
                break;
            case 'B':
                hexval <<= 4;
                hexval |= 11;
                break;
            case 'C':
                hexval <<= 4;
                hexval |= 12;
                break;
            case 'D':
                hexval <<= 4;
                hexval |= 13;
                break;
            case 'E':
                hexval <<= 4;
                hexval |= 14;
                break;
            case 'F':
                hexval <<= 4;
                hexval |= 15;
                break;
            case '\n':
                return hexval;
            case '\r':
                return hexval;
        }
    }
}

