#include <Flash.h>
#include "ConfigBits.h"

uint32_t *config0addr = (uint32_t *)0xBFC02FFC;
uint32_t *config1addr = (uint32_t *)0xBFC02FF8;
uint32_t *config2addr = (uint32_t *)0xBFC02FF4;
uint32_t *config3addr = (uint32_t *)0xBFC02FF0;

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
	config0.raw = *config0addr;
	config1.raw = *config1addr;
	config2.raw = *config2addr;
	config3.raw = *config3addr;
}

void saveSettings() {
	Flash.loadPage(config3addr);
	Flash.writePageWord(config0addr, config0.raw);
	Flash.writePageWord(config1addr, config1.raw);
	Flash.writePageWord(config2addr, config2.raw);
	Flash.writePageWord(config3addr, config3.raw);
	Flash.savePage();	
}

const char *ws_ps[32] = {
	"1ms", "2ms", "4ms", "8ms", "16ms", "32ms", "64ms", "128ms", "256ms", "512ms", "1.024s",
	"2.048s", "4.096s", "8.192s", "16.384s", "32.768s", "65.536s", "131.072s", "262.144s", "524.288s", "1048.576s", "X", "X", "X", "X",
	"X", "X", "X", "X", "X", "X", "X"
};

void displaySettings() {
	Serial.print("D) Debugger .......................... ");
	switch (config0.debug) {
		case 0b00: Serial.println("Enabled"); break;
		case 0b01: Serial.println("Enabled"); break;
		case 0b10: Serial.println("Enabled"); break;
		case 0b11: Serial.println("Disabled"); break;
	}
	Serial.print("I) ICE Pins .......................... ");
	Serial.println(config0.icesel ? "PGE2" : "PGE1");
	Serial.print("W) Watchdog Timeout .................. ");
	if (ws_ps[config1.wdtps][0] == 'X') {
		config1.wdtps = 0b10100;
	}
	Serial.println(ws_ps[config1.wdtps]);
	Serial.print("M) MII Interface ..................... ");
	Serial.println(config3.fmiien ? "MII" : "RMII");
	Serial.print("E) Ethernet Pins ..................... ");
	Serial.println(config3.fethio ? "Default" : "Alternate");
	Serial.print("C) CAN Pins .......................... ");
	Serial.println(config3.fcanio ? "Default" : "Alternate");
	Serial.print("U) USBID Pin.......................... ");
	Serial.println(config3.fusbidio ? "USBID" : "IO");
	Serial.print("V) VBUSON Pin......................... ");
	Serial.println(config3.fvbusonio ? "VBUSON" : "IO");
}

void setup() {
	loadSettings();
	Serial.begin(115200);
}

void loop() {
	Serial.println();
	displaySettings();
	Serial.println();
	Serial.println("L) Load settings");
	Serial.println("S) Save settings");
	Serial.println("R) Reboot");
	Serial.println();
	Serial.print("Select an entry to change: ");
	while (!Serial.available());
	char c = Serial.read();
	if (c >= 'a' && c <= 'z') {
		c -= 0x20;	
	}
	Serial.println(c);
	switch (c) {
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
		case 'W':
			config1.wdtps++;
			if (config1.wdtps > 0b10100) {
				config1.wdtps = 0b00000;
			}
			break;
		case 'M':
			config3.fmiien = 1-config3.fmiien;
			break;
		case 'E':
			config3.fethio = 1-config3.fethio;
			break;
		case 'C':
			config3.fcanio = 1-config3.fcanio;
			break;
		case 'U':
			config3.fusbidio = 1-config3.fusbidio;
			break;
		case 'V':
			config3.fvbusonio = 1-config3.fvbusonio;
			break;
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
	}
}

