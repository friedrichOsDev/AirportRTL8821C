#ifndef RTL8821CHAL_HPP
#define RTL8821CHAL_HPP

#include <Kernel/IOKit/IOLib.h>
#include <Kernel/IOKit/pci/IOPCIDevice.h>
#include <Kernel/IOKit/IOInterrupts.h>
#include <Kernel/IOKit/IOLocks.h>
#include <libkern/OSAtomic.h>

// register definitions
#define REG_SYS_CTRL	0x000 // system control register
#define BIT_FEN_EN	(1 << 26) // function enable bit
#define REG_DMEM_CTRL   0x1080 // memory control register
#define BIT_WL_RST	(1 << 16) // wlan reset bit
#define REG_AGCTR_A 0xc08 // agc control register a
#define REG_AGCTR_B	0xe08 // agc control register b
#define REG_TXDFIR  0xc20 // tx digital filter register
#define REG_TRSW    0xca0 // transceiver switch register
// TODO: add more register definitions

class RTL8821CHAL : public OSObject {
    OSDeclareDefaultStructors(RTL8821CHAL)

public:
    // factory
    static RTL8821CHAL* withProvider(IOPCIDevice *device);

    // lifecycle
    virtual bool init() override;
    virtual bool start(IOPCIDevice *device); // map resources
    virtual void stop();
    virtual void free() override;

    // register access
    void write8(uint32_t addr, uint8_t val);
    uint8_t read8(uint32_t addr) const;
    void write16(uint32_t addr, uint16_t val);
    uint16_t read16(uint32_t addr) const;
    void write32(uint32_t addr, uint32_t val);
    uint32_t read32(uint32_t addr) const;
    void write32Mask(uint32_t addr, uint32_t data, uint32_t mask);

    // TODO: port from linux driver
    bool loadFirmware(const uint8_t *fw, size_t len);
    bool readEeprom(uint32_t offset, void *buf, size_t len) const;
    bool powerOn();
    bool powerOff();
    bool initRf();
    bool calibrate();
    bool initRings();
    void teardownRings();
    bool submitTxDescriptor(void *pkt, uint32_t len);
    void enableInterrupts();
    void disableInterrupts();
    void handleInterrupt(); // called by driver

private:
    IOPCIDevice *fPciDevice;
    IOMemoryMap *fMmioMap;
    volatile UInt8 *fMmioBase; 
    IOLock *fLock; 
};

#endif // RTL8821CHAL_HPP