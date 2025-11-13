#include "RTL8821CHAL.hpp"

OSDefineMetaClassAndStructors(RTL8821CHAL, OSObject)

RTL8821CHAL* RTL8821CHAL::withProvider(IOPCIDevice *device) {
    RTL8821CHAL *me = new RTL8821CHAL;
    if (!me || !me->init()) {
        if (me) me->free();
        return nullptr;
    }
    return me;
}

bool RTL8821CHAL::init() {
    if (!OSObject::init()) return false;
    fPciDevice = nullptr;
    fMmioMap = nullptr;
    fMmioBase = nullptr;
    fIntSource = nullptr;
    fLock = nullptr;
    return true;
}

bool RTL8821CHAL::start(IOPCIDevice *device) {
    if (!device) return false;
    fPciDevice = device;
    fPciDevice->retain();

    // map BAR0
    fMmioMap = fPciDevice->mapDeviceMemoryWithIndex(0);
    if (!fMmioMap) {
        IOLog("RTL8821CHAL: failed to map device memory\n");
        return false;
    }
    fMmioBase = reinterpret_cast<volatile uint8_t *>(fMmioMap->getVirtualAddress());
    if (!fMmioBase) {
        IOLog("RTL8821CHAL: mmio virtual address is null\n");
        fMmioMap->release();
        fMmioMap = nullptr;
        return false;
    }

    // create lock
    fLock = IOLockAlloc();
    if (!fLock) {
        IOLog("RTL8821CHAL: failed to allocate lock\n");
        fMmioMap->release();
        fMmioMap = nullptr;
        fMmioBase = nullptr;
        return false;
    }

    IOLog("RTL8821CHAL: started\n");
    return true;
}

void RTL8821CHAL::stop() {
    IOLog("RTL8821CHAL: stopping\n");

    // disable interrupts at HAL level if used
    disableInterrupts();

    if (fLock) {
        IOLockFree(fLock);
        fLock = nullptr;
    }

    if (fMmioMap) {
        fMmioMap->release();
        fMmioMap = nullptr;
        fMmioBase = nullptr;
    }

    if (fPciDevice) {
        fPciDevice->release();
        fPciDevice = nullptr;
    }
}

void RTL8821CHAL::free() {
    // ensure resources are freed
    if (fLock) {
        IOLockFree(fLock);
        fLock = nullptr;
    }
    if (fMmioMap) {
        fMmioMap->release();
        fMmioMap = nullptr;
        fMmioBase = nullptr;
    }
    if (fPciDevice) {
        fPciDevice->release();
        fPciDevice = nullptr;
    }
    OSObject::free();
}

void RTL8821CHAL::write8(uint32_t addr, uint8_t val) {
    if (!fMmioBase) return;
    *(volatile uint8_t *)(fMmioBase + addr) = val;
    OSMemoryBarrier();
}

uint8_t RTL8821CHAL::read8(uint32_t addr) {
    if (!fMmioBase) return 0;
    uint8_t v = *(volatile uint8_t *)(fMmioBase + addr);
    OSMemoryBarrier();
    return v;
}

void RTL8821CHAL::write32(uint32_t addr, uint32_t val) {
    if (!fMmioBase) return;
    *(volatile uint32_t *)(fMmioBase + addr) = val;
    OSMemoryBarrier();
}

uint32_t RTL8821CHAL::read32(uint32_t addr) {
    if (!fMmioBase) return 0;
    uint32_t v = *(volatile uint32_t *)(fMmioBase + addr);
    OSMemoryBarrier();
    return v;
}

void RTL8821CHAL::write32Mask(uint32_t addr, uint32_t data, uint32_t mask) {
    if (!fMmioBase) return;
    uint32_t cur = read32(addr);
    uint32_t next = (cur & ~mask) | (data & mask);
    if (next != cur) write32(addr, next);
}

// TODO: Firmware / EEPROM stubs
bool RTL8821CHAL::loadFirmware(const uint8_t *fw, size_t len) {
    // TODO: implement firmware upload and handshake with device
    IOLog("RTL8821CHAL: loadFirmware called (stub)\n");
    (void)fw; (void)len;
    return false;
}

bool RTL8821CHAL::readEeprom(uint32_t offset, void *buf, size_t len) {
    // TODO: implement EEPROM/efuse read
    IOLog("RTL8821CHAL: readEeprom called (stub)\n");
    (void)offset; (void)buf; (void)len;
    return false;
}

bool RTL8821CHAL::powerOn() {
    IOLog("RTL8821CHAL: powerOn called (stub)\n");
    return false;
}

bool RTL8821CHAL::powerOff() {
    IOLog("RTL8821CHAL: powerOff called (stub)\n");
    return false;
}

bool RTL8821CHAL::initRf() {
    IOLog("RTL8821CHAL: initRf called (stub)\n");
    return false;
}

bool RTL8821CHAL::calibrate() {
    IOLog("RTL8821CHAL: calibrate called (stub)\n");
    return false;
}

bool RTL8821CHAL::initRings() {
    IOLog("RTL8821CHAL: initRings called (stub)\n");
    return false;
}

void RTL8821CHAL::teardownRings() {
    IOLog("RTL8821CHAL: teardownRings called (stub)\n");
}

bool RTL8821CHAL::submitTxDescriptor(void *pkt, uint32_t len) {
    IOLog("RTL8821CHAL: submitTxDescriptor called (stub)\n");
    (void)pkt; (void)len;
    return false;
}

void RTL8821CHAL::enableInterrupts() {
    IOLog("RTL8821CHAL: enableInterrupts (stub)\n");
    // TODO: register IOInterruptEventSource or MSI handler
}

void RTL8821CHAL::disableInterrupts() {
    IOLog("RTL8821CHAL: disableInterrupts (stub)\n");
    // TODO: disable/teardown interrupt source
}

void RTL8821CHAL::handleInterrupt() {
    IOLog("RTL8821CHAL: handleInterrupt (stub)\n");
}