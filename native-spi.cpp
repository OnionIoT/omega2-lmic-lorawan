#include "native-spi.h"
#include <stdio.h>

// nativeSpi::nativeSpi() {
// 	nativeSpi(NATIVE_SPI_DEFAULT_BUS, NATIVE_SPI_DEFAULT_DEVICE, NATIVE_SPI_DEFAULT_SPEED);
// }

nativeSpi::nativeSpi(int busNum_in, int deviceId_in, int speedInHz_in) {

	busNum = busNum_in;
	deviceId = deviceId_in;
	speedInHz = speedInHz_in;

	delayInUs = 0;
	bitsPerWord = NATIVE_SPI_DEFAULT_BITS_PER_WORD;

	mode = NATIVE_SPI_DEFAULT_MODE;
	modeBits = NATIVE_SPI_DEFAULT_MODE_BITS;
	
	setupDevice();
}

int nativeSpi::getDevice()
{
	int 	status;
	char 	pathname[255];

	// define the path to open
	status = snprintf(pathname, sizeof(pathname), NATIVE_SPI_DEV_PATH, busNum, deviceId);

	// check the filename
	if (status < 0 || status >= sizeof(pathname)) {
		// add errno
		return EXIT_FAILURE;
	}

	// create a file descriptor for the I2C bus
	if ( (spi_device = open(pathname, O_RDWR)) < 0) {
		printf("ERROR: could not open sysfs device '%s'\n", pathname);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int nativeSpi::releaseDevice() {
	if ( close(spi_device) < 0 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int nativeSpi::setupDevice()
{
	int 	status, ret;
	status 	= getDevice();

	if (status == EXIT_SUCCESS) {
		// set the SPI mode
		ret = ioctl(spi_device, SPI_IOC_WR_MODE32, &(modeBits) );
		if (ret == -1) {
			printf("ERROR: Cannot set SPI mode 0x%02x\n", modeBits);
			return EXIT_FAILURE;
		}
		
		ret = ioctl(spi_device, SPI_IOC_RD_MODE32, &(modeBits) );
		if (ret == -1){
			printf("ERROR: Cannot set SPI mode 0x%02x\n", modeBits);
			return EXIT_FAILURE;
		}

		// set the bits per word
		ret = ioctl(spi_device, SPI_IOC_WR_BITS_PER_WORD, &(bitsPerWord) );
		if (ret == -1) {
			printf("ERROR: Cannot set %d bits per word\n", bitsPerWord);
			return EXIT_FAILURE;
		}
		
		ret = ioctl(spi_device, SPI_IOC_RD_BITS_PER_WORD, &(bitsPerWord) );
		if (ret == -1) {
			printf("ERROR: Cannot set %d bits per word\n", bitsPerWord);
			return EXIT_FAILURE;
		}
		
		// set max speed in Hz
#ifdef NATIVE_SPI_DEBUG 
		printf("> Setting SPI speed to %d Hz\n", speedInHz);
#endif // NATIVE_SPI_DEBUG
		ret = ioctl(spi_device, SPI_IOC_WR_MAX_SPEED_HZ, &(speedInHz) );
		if (ret == -1) {
			printf("ERROR: Cannot set max speed %d Hz\n", speedInHz);
			return EXIT_FAILURE;
		}
		
		ret = ioctl(spi_device, SPI_IOC_RD_MAX_SPEED_HZ, &(speedInHz) );
		if (ret == -1) {
			printf("ERROR: Cannot set max speed %d Hz\n", speedInHz);
			return EXIT_FAILURE;
		}

		// clean-up
		status 	|= releaseDevice();
	}
	
	return status;
}


void nativeSpi::transfer(uint8_t* txBuf, size_t txLen, uint8_t* rxBuf, size_t rxLen) {
	int status, a;
	struct spi_ioc_transfer k[2];
	
	// get device
	status = getDevice();
	if (status != EXIT_SUCCESS) {
		return;
	}
	
	/* I/O transaction */
	memset(&k, 0, sizeof(k)); /* clear k */

	k[0].tx_buf = (unsigned long) txBuf;
	k[1].rx_buf = (unsigned long) rxBuf;
	k[0].len = txLen;
	k[1].len = rxLen;
	k[0].cs_change = 0;
	//k[1].cs_change = SPI_CS_CHANGE;
	k[1].cs_change = 0;
	k[0].speed_hz = speedInHz,
	k[1].speed_hz = speedInHz,

	a = ioctl(spi_device, SPI_IOC_MESSAGE(2), &k);
	
	releaseDevice();
}


void nativeSpi::write(uint8_t* txBuf, size_t txLen) {
	int status, a;
	struct spi_ioc_transfer k;
	
	// get device
	status = getDevice();
	if (status != EXIT_SUCCESS) {
		return;
	}
	
	/* I/O transaction */
	memset(&k, 0, sizeof(k)); /* clear k */

	k.tx_buf = (unsigned long) txBuf;
	// k.rx_buf = (unsigned long) txBuf;
	k.len = txLen;
	k.cs_change = 0;
	k.speed_hz = speedInHz,

	a = ioctl(spi_device, SPI_IOC_MESSAGE(1), &k);
	
	releaseDevice();
}