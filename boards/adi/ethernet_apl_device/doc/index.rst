.. zephyr:board:: ethernet_apl_device

Overview
********
The Ethernet APL Device board is an Advanced Physical Layer (APL) board with extensive supervision
capabilities, based on the MAX32690 microcontroller.

Hardware
********

- MAX32690 MCU:

  - Ultra-Efficient Microcontroller for Battery-Powered Applications

    - 120MHz Arm Cortex-M4 Processor with FPU
    - 7.3728MHz and 60MHz Low-Power Oscillators
    - External Crystal Support (32MHz required for BLE)
    - 32.768kHz RTC Clock (Requires External Crystal)
    - 8kHz Always-On Ultra-Low Power Oscillator
    - 3MB Internal Flash, 1MB Internal SRAM (832kB ECC ON)
    - TBDμW/MHz Executing from Cache at 1.1V
    - 1.8V and 3.3V I/O with No Level Translators
    - External Flash & SRAM Expansion Interfaces

  - Bluetooth 5.2 LE Radio

    - Dedicated, Ultra-Low-Power, 32-Bit RISC-V Coprocessor to Offload Timing-Critical Bluetooth Processing
    - Fully Open-Source Bluetooth 5.2 Stack Available
    - Supports AoA, AoD, LE Audio, and Mesh
    - High-Throughput (2Mbps) Mode
    - Long-Range (125kbps and 500kbps) Modes
    - Rx Sensitivity: -97.5dBm; Tx Power: +4.5dBm
    - Single-Ended Antenna Connection (50Ω)

  - Multiple Peripherals for System Control

    - 16-Channel DMA
    - Up To Five Quad SPI Master (60MHz)/Slave (48MHz)
    - Up To Four 1Mbaud UARTs with Flow Control
    - Up To Two 1MHz I2C Master/Slave
    - I2S Master/Slave
    - Eight External Channel, 12-bit 1MSPS SAR ADC w/ on-die temperature sensor
    - USB 2.0 Hi-Speed Device
    - 16 Pulse Train Engines
    - Up To Six 32-Bit Timers with 8mA High Drive
    - Up To Two CAN 2.0 Controllers
    - Up To Four Micro-Power Comparators
    - 1-Wire Master

  - Security and Integrity​

    - ChipDNA Physically Un-clonable Function (PUF)
    - Modular Arithmetic Accelerator (MAA), True Random Number Generator (TRNG)
    - Secure Nonvolatile Key Storage, SHA-256, AES-128/192/256
    - Secure Boot ROM

- External devices connected to the Ethernet APL Device:

  - On-Board HyperRAM
  - On-Board SPI Flash
  - MAX6613 temperature sensor
  - MAX42500 supervisor
  - ADFS7124 analog frontend for thermal sensing
  - SWD 10-Pin Header
  - On-Board Bluetooth 5.2 LE Radio antenna
  - MAXQ1065 Ultralow Power Cryptographic Controller with ChipDNA
  - Connector to external ADIN1110: Robust, Industrial, Low Power 10BASE-T1L Ethernet MAC-PHY
  - On-Board 3.5V, 3.3V, 1.8V, and 1.1V voltage regulators
  - Board Power Provided by APL connector
  - 6-pin external SPI connector
  - 4-pin extenral thermal analog input connector

Supported Features
==================

.. zephyr:board-supported-hw::

Programming and Debugging
*************************

Flashing
========

The MAX32690 MCU can be flashed by connecting an external debug probe to the
SWD port. SWD debug can be accessed through the Cortex 10-pin connector, P1.
Logic levels are fixed to VDDIO (1.8V).

Once the debug probe is connected to your host computer, then you can simply run the
``west flash`` command to write a firmware image into flash.

.. note::

  This board uses JLink as the default debug interface. You can also use
  OpenOCD by overriding the runner, appending ``--runner openocd`` to your
  ```west`` command(s). The debugger should be connected to the standard 2*5
  pin debug connector (P1) using an appropriate adapter board and cable.

Debugging
=========

Please refer to the `Flashing`_ section and run the ``west debug`` command
instead of ``west flash``.

References
**********

- `Ethernet APL Device web page`_

.. _Ethernet APL Device web page:
  https://wcm2.cldnet.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/AD-EthernetAPLDevice-SL.html
