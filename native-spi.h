#ifndef SRC_NATIVE_SPI_H_
#define SRC_NATIVE_SPI_H_

#include <stddef.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

#include <unistd.h>
#include <cstring>
#include <string.h>

#define NATIVE_SPI_DEV_PATH				"/dev/spidev%d.%d"

#define NATIVE_SPI_DEFAULT_BUS			32766
#define NATIVE_SPI_DEFAULT_DEVICE		1

#define NATIVE_SPI_DEFAULT_SPEED			100000
#define NATIVE_SPI_DEFAULT_BITS_PER_WORD	0 				// corresponds to 8 bits per word
#define NATIVE_SPI_DEFAULT_MODE 			SPI_MODE_0
#define NATIVE_SPI_DEFAULT_MODE_BITS		(SPI_MODE_0 | SPI_TX_DUAL | SPI_RX_DUAL)

// #define NATIVE_SPI_DEBUG		1

class nativeSpi {
	public:
		/* By default instantiate with defaults for busNum, deviceId, speed */
    	nativeSpi() : nativeSpi(NATIVE_SPI_DEFAULT_BUS, NATIVE_SPI_DEFAULT_DEVICE, NATIVE_SPI_DEFAULT_SPEED) {};

		nativeSpi(int busNum_in, int deviceId_in, int speedInHz_in);

		void transfer(uint8_t* txBuf, size_t txLen, uint8_t* rxBuf, size_t rxLen);
		void write(uint8_t* txBuf, size_t txLen);

	private:
		int 	getDevice	();
		int		releaseDevice ();
		
		int 	setupDevice ();
		

		int busNum;
		int deviceId;

		int speedInHz;
		int delayInUs;
		int bitsPerWord;

		int mode;
		int modeBits;

		int spi_device;

};


#endif /* SRC_NATIVE_SPI_H_ */
