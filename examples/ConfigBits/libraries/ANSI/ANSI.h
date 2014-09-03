#ifndef _ANSI_H
#define _ANSI_H

#if (ARDUINO >= 100)
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

class ANSI {
	private:
		Stream *_dev;
		// Private functions and variables here
	public:
		ANSI() : _dev(&Serial) {};
		ANSI(Stream &dev) : _dev(&dev) {};
		ANSI(Stream *dev) : _dev(dev) {};
		void cls();
		void setCursor(int, int);
};

#endif
