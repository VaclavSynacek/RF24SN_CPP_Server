RF24SN C++ Server
=================

Full implementation of [RF24SN](https://github.com/VaclavSynacek/RF24SN) on top of [RF24-rpi](https://github.com/jscrane/RF24-rpi)
 - a Raspberry Pi port of arduino [RF24](https://maniacbug.github.io/RF24/index.html) driver.
Currently only runs on Raspberry Pi.

For full description of protocol, client server setup or alternative implementations, see [RF24SN](https://github.com/VaclavSynacek/RF24SN)


## Installation:

Install [RF24-rpi](https://github.com/jscrane/RF24-rpi) according to [this guide](https://github.com/jscrane/RF24-rpi#setup-the-library).
Install libmosquitto
```Shell
apt-get install libmosquito-dev
```

Then compile
```Shell
git clone https://github.com/VaclavSynacek/RF24SN_CPP_Server.git
cd RF24SN_CPP_Server
make
```

## Usage:
```Shell
sudo ./RF24SN
```

The sudo is required in standard Raspbian instalation unless access to /dev/spidevX.X and the GPIO pins has been granted to other user (via [quick2wire](https://github.com/quick2wire/quick2wire-gpio-admin) or similar).


## Wiring

The SPI wires (yellow) have to go exactly to their counterparts:
* MOSI to MOSI
* MISO to MISO
* SCK to SC(L)K

The VCC (red) has to go to any **3.3V** pin. Connecting it to 5V pin will damage the nRF24L01.

GRN (black) can go to any ground.

The CSN (blue) has to go to either CS0 or CS1. This determines the spi device. To use the /dev/spidev0.**0** use the CS **0**.

The CE(cyan) can go to any GPIO pin. The diagram follows the F24SN defaults - CE 25.

The IRQ pin is not used by this implementation.