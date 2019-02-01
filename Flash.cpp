#include <Flash.h>

bool FlashClass::erasePage(void* adr) {
    NVMADDR = KVA_TO_PA((unsigned int) adr);
    return doNvmOp(nvmopErasePage);
}

bool FlashClass::writeWord(void* adr, uint32_t val) {
    NVMADDR = KVA_TO_PA((unsigned int) adr);
#if defined(__PIC32MZ__)
    NVMDATA0 = val;
#else
    NVMDATA = val;
#endif
    return doNvmOp(nvmopWriteWord);
}

bool FlashClass::writeRow(void* adr, void *val) {
	bool blank = true;
	for (int i = 0; i < __FLASH_ROW__; i++) {
		if (((uint8_t *)val)[i] != 0xFF) {
			blank = false;		
		}
	}
	if (blank) {
		return 0;
	}
    NVMADDR = KVA_TO_PA((unsigned int) adr);
    NVMSRCADDR = KVA_TO_PA((uint32_t)val);
    return doNvmOp(nvmopWriteRow);
}

bool FlashClass::clearNvmError() {
	return doNvmOp(nvmopNop);
}

bool FlashClass::isNvmError() {
	return (NVMCON & (_NVMCON_WRERR_MASK | _NVMCON_LVDERR_MASK)) ? true : false;
}

bool __attribute__((nomips16)) FlashClass::doNvmOp(uint32_t nvmop) {
    int         intSt;
    uint32_t    tm;


    // M00TODO: When DMA operations are supported in the core, need
    // to add code here to suspend DMA during the NVM operation.

    intSt = disableInterrupts();
    NVMCON = _NVMCON_WREN_MASK | nvmop;

    tm = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() - tm < ((F_CPU * 6) / 2000000));

    NVMKEY      = 0xAA996655;
    NVMKEY      = 0x556699AA;
    NVMCONSET   = _NVMCON_WR_MASK;

    while (NVMCON & _NVMCON_WR_MASK) {
		continue;
    }

    NVMCONCLR = _NVMCON_WREN_MASK;

    //M00TODO: Resume a suspended DMA operation

    restoreInterrupts(intSt);
    return(isNvmError());
}

bool FlashClass::loadPage(void *adr) {
    uint32_t *ptr = (uint32_t *)adr;

    ptr = (uint32_t *)((uint32_t)ptr & ~((__FLASH_PAGE__*4)-1));
    _page_address = (uint32_t)ptr;

    for (int i = 0; i < __FLASH_PAGE__; i++) {
        _flash_buffer[i] = *ptr;
        ptr++;
    }
    return true;
}

bool FlashClass::savePage() {
    uint32_t *ptr = (uint32_t *)_page_address;
    erasePage(ptr);
    for (int i = 0; i < __FLASH_PAGE__; i++) {
        writeWord(ptr, _flash_buffer[i]);
        ptr++;
    }
    return true;
}

bool FlashClass::writePageWord(void *adr, uint32_t val) {
    uint32_t a = (uint32_t)adr - _page_address;
    if (a > __FLASH_PAGE__ * 4) {
        return false;
    }
    _flash_buffer[a/4] = val;
    return true;
}

bool FlashClass::verifyPage() {
    uint32_t *ptr = (uint32_t *)_page_address;
    for (int i = 0; i < __FLASH_PAGE__; i++) {
        if (*ptr != _flash_buffer[i]) {
            return false;
        }
        ptr++;
    }
    return true;
}

bool FlashClass::eraseProgmem() {
    return doNvmOp(nvmopEraseProgmem);
}

FlashClass Flash;
