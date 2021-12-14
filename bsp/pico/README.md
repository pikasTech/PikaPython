# pikascript in pico

This bsp is based on https://github.com/GorgonMeducer/Pico_Template.

## Usage

1. Boot the Pico with the BOOTSEL button pressed.

2. Drag and  drop  **pico-debug-gimmecache.uf2**   to RPI-RP2 mass-storage driver in the explorer. It immediately reboots as a CMSIS-DAP adapter. Pico-debug loads as a RAM only .uf2 image, meaning that it is never written to flash and doesn't replace existing user code.

3. Open the MDK/mdk projcet.

4. Compile and Debug

5. The  **printf** is connected to  **UART0** , and  **TX is GP0** ,  **RX is GP1** .