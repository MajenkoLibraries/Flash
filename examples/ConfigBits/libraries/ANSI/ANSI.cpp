#include <ANSI.h>

void ANSI::setCursor(int x, int y) {
	char tmp[10];
	sprintf(tmp, "\e[%d;%dH", y + 1, x + 1);
	_dev->print(tmp);
}

void ANSI::cls() {
	_dev->print("\e[2J");
}
