# AirportRTL8821C

A macOS kernel extension (kext) aiming to add support for the Realtek RTL8821CE PCIe Wi‑Fi adapter for Hackintosh systems.

__Important__: this driver is a work in progress and is not complete. Do not expect a fully working Wi‑Fi adapter yet. The repository currently contains skeletons and stubs for the main driver, HAL and PCIe layer.

## Status
- Incomplete — core functionality and device initialization are still under development.
- Basic source files and PCIe/driver scaffolding exist, but full TX/RX, power management, firmware handling and macOS integration are not finished.

## Files of interest
- Main driver entry: [`AirportRTL8821C`](AirportRTL8821C.hpp) — [AirportRTL8821C.cpp](AirportRTL8821C.cpp) / [AirportRTL8821C.hpp](AirportRTL8821C.hpp)  
- Hardware Abstraction Layer: [`RTL8821CHAL`](RTL8821CHAL.hpp) — [RTL8821CHAL.cpp](RTL8821CHAL.cpp) / [RTL8821CHAL.hpp](RTL8821CHAL.hpp)  
- PCIe support: [`RTL8821CPCIe`](RTL8821CPCIe.hpp) — [RTL8821CPCIe.cpp](RTL8821CPCIe.cpp) / [RTL8821CPCIe.hpp](RTL8821CPCIe.hpp)  
- Kext metadata: [Info.plist](Info.plist)  
- This README: [readme.md](readme.md)

## Building
- This project isn't a Xcode project.
- There is no building logic in the moment.

## Installing (development/testing)
- Only use with OpenCore Bootloader or if you know what you are doing

## Contributing
- Contributions and pull requests are welcome. Please open issues describing bugs or desired features.
- If you work on device initialization, HAL, firmware loading or power management, reference the files listed above to integrate changes.

## License & Copyright
See the repository for license details. Info.plist contains the bundle metadata: [Info.plist](Info.plist).

---

Development status: experimental. Use at your own risk.