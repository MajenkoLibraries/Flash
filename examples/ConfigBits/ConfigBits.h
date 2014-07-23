#ifndef _CONFIG_BITS_H
#define _CONFIG_BITS_H

struct conf0 {
	union {
		uint32_t raw;
		struct {
			unsigned debugger:2;
			#define DEBUGGER_ENABLED 0b10
			#define DEBUGGER_DISABLED 0b00
			unsigned :1;
			unsigned icesel:1;
			#define ICESEL_PGE1 0b0
			#define ICESEL_PGE2 0b1
			unsigned :8;
			unsigned pwp:8;
			unsigned :4;
			unsigned bwp:1
			#define BWP_WRITEABLE 0b1
			#define BWP_PROTECTED 0b0
			unsigned :3
			unsigned cp:1
			#define CP_ENABLED 0b0
			#define CP_DISABLED 0b1
			unsigned :3
		};
	};
} __attribute__((packed));

#endif
