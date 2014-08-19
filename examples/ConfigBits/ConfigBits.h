#ifndef _CONFIG_BITS_H
#define _CONFIG_BITS_H

struct conf0 {
	union {
		uint32_t raw;
		struct {
			unsigned debug:2;
			#define DEBUGGER_ENABLED 0b10
			#define DEBUGGER_DISABLED 0b00
			unsigned :1;
			unsigned icesel:1;
			#define ICESEL_PGE1 0b0
			#define ICESEL_PGE2 0b1
			unsigned :8;
			unsigned pwp:8;
			unsigned :4;
			unsigned bwp:1;
			#define BWP_WRITEABLE 0b1
			#define BWP_PROTECTED 0b0
			unsigned :3;
			unsigned cp:1;
			#define CP_ENABLED 0b0
			#define CP_DISABLED 0b1
			unsigned :3;
		};
	};
} __attribute__((packed));

struct conf1 {
    union {
        uint32_t raw;
        struct {
            unsigned fnosc:3;
            unsigned :2;
            unsigned fsoscen:1;
            unsigned :1;
            unsigned ieso:1;
            unsigned poscmod:2; 
            unsigned osciofnc:1;
            unsigned :1;
            unsigned fpbdiv:2;
            unsigned fcksm:2;
            unsigned wdtps:5;
            unsigned :2;
            unsigned fwdten:1;
            unsigned :8;
        };
    };
} __attribute__((packed));

struct conf2 {
    union {
        uint32_t raw;
        struct {
            unsigned fpllidiv:3;
            unsigned :1;
            unsigned fpllmul:3;
            unsigned :1;
            unsigned upllidiv:3;
            unsigned :4;
            unsigned upllen:1;
            unsigned fpllodiv:3;
            unsigned :13;
        };
    };
} __attribute__((packed));

struct conf3 {
    union {
        uint32_t raw;
        struct {
            unsigned userid:16;
            unsigned fsrssel:3;
            unsigned :5;
            unsigned fmiien:1;
            unsigned fethio:1;
            unsigned fcanio:1;
            unsigned :3;
            unsigned fusbidio:1;
            unsigned fvbusonio:1;
        };
    };
} __attribute__((packed));

#endif
