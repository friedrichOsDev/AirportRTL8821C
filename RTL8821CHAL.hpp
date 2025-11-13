#ifndef RTL8821CHAL_HPP
#define RTL8821CHAL_HPP

#include <Kernel/IOKit/IOLib.h>
#include <Kernel/IOKit/pci/IOPCIDevice.h>
#include <Kernel/IOKit/IOInterrupts.h>
#include <libkern/OSAtomic.h>

#ifndef OSMemoryBarrier
#define OSMemoryBarrier() __sync_synchronize()
#endif

// System Control and Chip-Reset
#define REG_SYS_CTRL	0x000 // system control register
#define BIT_FEN_EN	(1 << 26) // function enable bit
#define REG_DMEM_CTRL   0x1080 // memory control register
#define BIT_WL_RST	(1 << 16) // wlan reset bit

// RF/PHY Register (power up)
#define REG_AGCTR_A 0xc08 // agc control register a
#define REG_AGCTR_B	0xe08 // agc control register b
#define REG_TXDFIR  0xc20 // tx digital filter register
#define REG_TRSW    0xca0 // transceiver switch register

// Hardware Abstraction Layer Class
class RTL8821CHAL : public OSObject {
    OSDeclareDefaultStructors(RTL8821CHAL)

public:
    // factory
    static RTL8821CHAL* withProvider(IOPCIDevice *device);

    // lifecycle
    virtual bool init() override;
    virtual bool start(IOPCIDevice *device); // map resources, allocate DMA, register interrupts
    virtual void stop(); // disable interrupts, free resources
    virtual void free() override;

    // register access
    void write8(uint32_t addr, uint8_t val);
    uint8_t read8(uint32_t addr);
    void write32(uint32_t addr, uint32_t val);
    uint32_t read32(uint32_t addr);
    void write32Mask(uint32_t addr, uint32_t data, uint32_t mask);

    // firmware / nvram
    bool loadFirmware(const uint8_t *fw, size_t len);
    bool readEeprom(uint32_t offset, void *buf, size_t len);

    // PHY / RF / power
    bool powerOn();
    bool powerOff();
    bool initRf();
    bool calibrate();

    // DMA / discriptors
    bool initRings();
    void teardownRings();
    bool submitTxDescriptor(void *pkt, uint32_t len);
    // callback for upper layer to fetch RX packet: implemented via delegate in driver

    // interrupts
    void enableInterrupts();
    void disableInterrupts();
    void handleInterrupt();

private:
    // hardware handles
    IOPCIDevice *fPciDevice;
    IOMemoryMap *fMmioMap;
    volatile uint8_t *fMmioBase;
    IOInterruptSource *fIntSource;
    // DMA/Ring bookkeeping (IOBufferMemoryDescriptor etc.)
    // Locks and state
    IOLock *fLock;
};

#endif // RTL8821CHAL_HPP