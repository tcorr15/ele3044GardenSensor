#ifndef VEML7700_H
#define VEML7700_H
#include <Arduino.h>

//Define important VEML7700 registers and addresses
#define VEML7700_ADDRESS 0x10

//Define initial configuration
#define VEML7700_INIT_CONFIG_CMD  0x1000

class VEML7700{
	private:
	int ALS_ITNegativeAdjust();
	int WHITE_ITNegativeAdjust();
	bool increment_IT();
	bool decrement_IT();
	bool incrementGain();
	void getITBits(int ITNumber, uint8_t &ITBits);
	void getGainBits(int gainNumber, uint8_t &gainBits);
	void sendData(uint8_t register_address, uint16_t data);
	void readData(uint8_t register_address, uint16_t &data);
	void updateConfigRegister();
	double getResolution();
	int getIT_ms();
	
	public:
	uint8_t ALS_GAIN, ALS_IT, ALS_PERS, ALS_INT_EN, ALS_SD;
	int VEML7700_gain;
	int VEML7700_IT;
	void initialise();
	int ALScalibrate();
	int WHITEcalibrate();
	double getAmbient();
	double getWhite();
};

#endif
