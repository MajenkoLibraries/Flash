#ifndef _FLASH_H
#define _FLASH_H

#define PAGE_SIZE               1024        // # of 32-bit Instructions per Page
#define BYTE_PAGE_SIZE          (4 * PAGE_SIZE) // Page size in Bytes
#define ROW_SIZE                128         // # of 32-bit Instructions per Row
#define BYTE_ROW_SIZE           (4 * ROW_SIZE) // # Row size in Bytes
#define NUM_ROWS_PAGE           8              //Number of Rows per Page

#include <WProgram.h>
#include <sys/kmem.h>

class FlashClass {
	private:
		static const uint32_t nvmopNop 			= 0x4000;
		static const uint32_t nvmopWriteWord 	= 0x4001;
		static const uint32_t nvmopWriteRow 	= 0x4003;
		static const uint32_t nvmopErasePage 	= 0x4004;

		bool __attribute__((nomips16)) doNvmOp(uint32_t nvmop);
		bool clearNvmError();
		bool isNvmError();

        uint32_t _flash_buffer[PAGE_SIZE];
        uint32_t _page_address;

	public:
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
