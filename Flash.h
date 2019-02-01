#ifndef _FLASH_H
#define _FLASH_H

#include <Arduino.h>
#include <sys/kmem.h>

class FlashClass {
	private:
		static const uint32_t nvmopNop 			= 0x4000;
		static const uint32_t nvmopWriteWord 	= 0x4001;
		static const uint32_t nvmopWriteRow 	= 0x4003;
		static const uint32_t nvmopErasePage 	= 0x4004;
        static const uint32_t nvmopEraseProgmem = 0x4007;

		bool __attribute__((nomips16)) doNvmOp(uint32_t nvmop);
		bool clearNvmError();
		bool isNvmError();

        uint32_t _flash_buffer[__FLASH_PAGE__];
        uint32_t _page_address;

	public:
        bool eraseProgmem();
		bool erasePage(void *adr);
		bool writeWord(void *adr, uint32_t val);
		bool writeRow(void *adr, void *val);
        bool loadPage(void *adr);
        bool savePage();
        
        uint32_t getPageAddress() { return _page_address; }
        bool writePageWord(void *adr, uint32_t val);
        bool verifyPage();
};

extern FlashClass Flash;
#endif
