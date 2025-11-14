#include "RTL8821CHAL.hpp"

OSDefineMetaClassAndStructors(RTL8821CHAL, OSObject)

RTL8821CHAL* RTL8821CHAL::withProvider(IOPCIDevice *device) {
    RTL8821CHAL *me = new RTL8821CHAL;
    if (!me || !me->init()) {
        if (me) me->release();
        return nullptr;
    }
    return me;
}

bool RTL8821CHAL::init() {
    if (!OSObject::init()) return false;
    fPciDevice = nullptr;
    fMmioMap = nullptr;
    fMmioBase = nullptr;
    fLock = nullptr;
    return true;
}

bool RTL8821CHAL::start(IOPCIDevice *device) {
    if (!device) return false;
    
    fPciDevice = device;
    fPciDevice->retain();

    fLock = IOLockAlloc();
    if (!fLock) {
        IOLog("RTL8821CHAL: ERROR: lock allocation failed\n");
        goto cleanup_pci_retain;
    }

    fMmioMap = fPciDevice->mapDeviceMemoryWithIndex(0);
    if (!fMmioMap) {
        IOLog("RTL8821CHAL: ERROR: failed to map device memory\n");
        goto cleanup_lock;
    }

    fMmioBase = reinterpret_cast<volatile UInt8 *>(fMmioMap->getVirtualAddress());
    if (!fMmioBase) {
        IOLog("RTL8821CHAL: ERROR: mmio base is null\n");
        goto cleanup_mmio_map;
    }

    IOLog("RTL8821CHAL: mmio map success\n");
    return true;

cleanup_mmio_map:
    fMmioMap->release();
    fMmioMap = nullptr;
cleanup_lock:
    IOLockFree(fLock);
    fLock = nullptr;
cleanup_pci_retain:
    fPciDevice->release();
    fPciDevice = nullptr;
    return false;
}

void RTL8821CHAL::stop() {
    IOLog("RTL8821CHAL: stopping\n");

    // free lock
    if (fLock) { IOLockFree(fLock); fLock = nullptr; }
    // free mmio map
    if (fMmioMap) { fMmioMap->release(); fMmioMap = nullptr; fMmioBase = nullptr; }
    // free pci device
    if (fPciDevice) { fPciDevice->release(); fPciDevice = nullptr; }
}

void RTL8821CHAL::free() {
    stop(); // free resourcses
    OSObject::free();
}

void RTL8821CHAL::write8(uint32_t addr, uint8_t val) {
    if (!fMmioBase || !fLock) return;
    IOLockLock(fLock);
    *(volatile uint8_t *)(fMmioBase + addr) = val;
    OSSynchronizeIO();
    IOLockUnlock(fLock);
}

uint8_t RTL8821CHAL::read8(uint32_t addr) const {
    if (!fMmioBase || !fLock) return;
    IOLockLock(fLock);
    uint8_t v = *(volatile uint8_t *)(fMmioBase + addr);
    OSSynchronizeIO();
    IOLockUnlock(fLock);
    return v;
}

void RTL8821CHAL::write16(uint32_t addr, uint16_t val) {
    if (!fMmioBase || !fLock) return;
    IOLockLock(fLock);
    *(volatile uint16_t *)(fMmioBase + addr) = val;
    OSSynchronizeIO();
    IOLockUnlock(fLock);
}

uint16_t RTL8821CHAL::read16(uint32_t addr) const {
    if (!fMmioBase || !fLock) return 0;
    IOLockLock(fLock);
    uint16_t v = *(volatile uint16_t *)(fMmioBase + addr);
    OSSynchronizeIO();
    IOLockUnlock(fLock);
    return v;
}

void RTL8821CHAL::write32(uint32_t addr, uint32_t val) {
    if (!fMmioBase || !fLock) return;
    IOLockLock(fLock);
    *(volatile uint32_t *)(fMmioBase + addr) = val;
    OSSynchronizeIO();
    IOLockUnlock(fLock);
}

uint32_t RTL8821CHAL::read32(uint32_t addr) const {
    if (!fMmioBase || !fLock) return 0;
    IOLockLock(fLock);
    uint32_t v = *(volatile uint32_t *)(fMmioBase + addr);
    OSSynchronizeIO();
    IOLockUnlock(fLock);
    return v;
}

void RTL8821CHAL::write32Mask(uint32_t addr, uint32_t data, uint32_t mask) {
    if (!fMmioBase || !fLock) return;
    IOLockLock(fLock);
    volatile uint32_t *reg = (volatile uint32_t *)(fMmioBase + addr);

    uint32_t cur = *reg;
    uint32_t next = (cur & ~mask) | (data & mask);
    
    if (cur != next) { *reg = next; }

    OSSynchronizeIO();
    IOLockUnlock(fLock);
}

bool RTL8821CHAL::loadFirmware(const uint8_t *fw, size_t len) {
    if (!fw || len == 0) {
        IOLog("RTL8821CHAL: ERROR: fw data is null or empty\n");
        return false;
    }
    
    if (!fMmioBase || !fLock) {
        IOLog("RTL8821CHAL: ERROR: mmio base or lock is null\n");
        return false;
    }

    const uint32_t FW_DMA_START_ADDR = 0x1000; // TODO: change to real addr
    
    // TODO: Vorbereitung des Chips
    IOLog("RTL8821CHAL: Starte Firmware-Upload (%zu Bytes)...\n", len);
    // 1. Chip in den Download-Modus setzen 
    // 2. Zieladresse im Chip setzen 
    
    IOLockLock(fLock); 
    
    // write fw in 32-bit chunk
    for (size_t i = 0; i < len; i += 4) {
        uint32_t val = 0;
        
        size_t chunk = (len - i >= 4) ? 4 : (len - i);
        
        for (size_t j = 0; j < chunk; j++) {
            val |= ((uint32_t)fw[i + j]) << (j * 8);
        }
        
        *(volatile uint32_t *)(fMmioBase + FW_DMA_START_ADDR + i) = val;
    }
    
    OSSynchronizeIO();
    IOLockUnlock(fLock);

    // TODO: Abschluss des Vorgangs
    // 3. Warten, bis der Upload abgeschlossen ist
    // 4. Chip anweisen, die Firmware zu starten
    
    IOLog("RTL8821CHAL: Firmware erfolgreich geladen.\n");
    return true;
}