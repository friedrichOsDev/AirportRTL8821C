#ifndef RTL8821CHAL_HPP
#define RTL8821CHAL_HPP

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

#endif // RTL8821CHAL_HPP