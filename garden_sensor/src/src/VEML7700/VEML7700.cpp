#include "VEML7700.h"
#include <Wire.h>

//Initialise config register for a reading
void VEML7700::initialise(){
	//Send initial command to config register
	sendData(0x00, VEML7700_INIT_CONFIG_CMD);
	VEML7700_gain = 1;
	VEML7700_IT = 0;
	ALS_GAIN = 0b10;
	ALS_IT = 0b0000;
	ALS_PERS = 0b00;
	ALS_INT_EN = 0b0;
	ALS_SD = 0b0;
	
	//Disable power saving modes for simplicity
	sendData(0x03, 0x0000);
	delay(getIT_ms() + 5);

}

//Returns: -1=error, 0=LUX_VEML calc, 1=LUX_CALC calc, 2=Error, value too high
//TODO: Make this function more readable, add timeout to all while loops
int VEML7700::ALScalibrate(){
	//Var to be returned
  int outcome = -1;
  uint16_t ALS;
  while(1){    
    //Read from the als register
    readData(0x04, ALS);
    
    if(ALS <= 100){
      //Shut down ALS
      ALS_SD = 0b1;
      updateConfigRegister();
      delay(3);
      //Increment gain
      bool gain_max = incrementGain();
       
      if(gain_max){
        //Increment IT
        bool IT_max = increment_IT();
        
        //If IT is max
        if(IT_max){
          //caclulate LUX_VEML
          outcome = 0;
          return outcome;
        }
        
        //IT is not max
        else{
          //Turn on the ALS and return to the start of the while loop
          ALS_SD = 0b0;
          updateConfigRegister();
          delay(getIT_ms() + 5);
          continue;
        }
      } //END OF gain_max==true CONDITIONAL
      
      //Gain is not max
      else{
        //Turn on ALS and start next while iteration
        ALS_SD = 0b0;
        updateConfigRegister();
        delay(getIT_ms() + 5);
        continue;
      }
    } //END OF ALS <=100 CONDITIONAL
    
    //ALS is greater than 100
    else{
      //Reduce IT until suitable and return the outcome of the calibration
      return ALS_ITNegativeAdjust();
    }
  } //END OF WHILE LOOP
} //END OF FUNCTION

int VEML7700::WHITEcalibrate(){
	//Var to be returned
  int outcome = -1;
  uint16_t WHITE;
  while(1){    
    //Read from the als register
    readData(0x05, WHITE);
    
    if(WHITE <= 100){
      //Shut down ALS
      ALS_SD = 0b1;
      updateConfigRegister();
      delay(3);
      //Increment gain
      bool gain_max = incrementGain();
       
      if(gain_max){
        //Increment IT
        bool IT_max = increment_IT();
        
        //If IT is max
        if(IT_max){
          //caclulate LUX_VEML
          outcome = 0;
          return outcome;
        }
        
        //IT is not max
        else{
          //Turn on the ALS and return to the start of the while loop
          ALS_SD = 0b0;
          updateConfigRegister();
          delay(getIT_ms() + 5);
          continue;
        }
      } //END OF gain_max==true CONDITIONAL
      
      //Gain is not max
      else{
        //Turn on ALS and start next while iteration
        ALS_SD = 0b0;
        updateConfigRegister();
        delay(getIT_ms() + 5);
        continue;
      }
    } //END OF ALS <=100 CONDITIONAL
    
    //ALS is greater than 100
    else{
      //Reduce IT until suitable and return the outcome of the calibration
      return WHITE_ITNegativeAdjust();
    }
  } //END OF WHILE LOOP
} //END OF FUNCTION
	
//Once gain has been adjusted to a suitable value below its max,
//decrease IT to a suitable value
//TODO: Add timeout to infinite loop
int VEML7700::ALS_ITNegativeAdjust(){
	  while(1){
    //Turn on ALS
    ALS_SD = 0b0;
    updateConfigRegister();
    delay(getIT_ms() + 5);  //Make sure we're giving it enough time to get started

    //Take a reading from the ALS
    uint16_t ALS;
    readData(0x04, ALS);
    
    if(ALS > 10000){
      //Turn off ALS
      ALS_SD = 0b1;
      updateConfigRegister();
      delay(3);
      //Decrement IT
      bool IT_min = decrement_IT();
      if(IT_min){
        //calc Lux_calc
        return 2;
      }
      else{continue;}
    } //END OF ALS>10000 CONDITIONAL
    
    //ALS <= 10000
    else{
      //Calculate lux_calc
      return 1;
    }
  } //END OF WHILE LOOP
} //END OF FUNCTION

int VEML7700::WHITE_ITNegativeAdjust(){
	  while(1){
    //Turn on ALS
    ALS_SD = 0b0;
    updateConfigRegister();
    delay(getIT_ms() + 5);  //Make sure we're giving it enough time to get started

    //Take a reading from the WHITE
    uint16_t WHITE;
    readData(0x05, WHITE);
    
    if(WHITE > 10000){
      //Turn off ALS
      ALS_SD = 0b1;
      updateConfigRegister();
      delay(3);
      //Decrement IT
      bool IT_min = decrement_IT();
      if(IT_min){
        //calc Lux_calc
        return 2;
      }
      else{continue;}
    } //END OF WHITE>10000 CONDITIONAL
    
    //WHITE <= 10000
    else{
      //Calculate lux_calc
      return 1;
    }
  } //END OF WHILE LOOP
} //END OF FUNCTION


//Increments the IT counter and updates the IT bits
//Returns true if at or above max IT
bool VEML7700::increment_IT(){
	  if(VEML7700_IT <= 2){
     VEML7700_IT++;
    //Convert numeric value to bits
    getITBits(VEML7700_IT, ALS_IT);
    //Change IT bits in config command
    updateConfigRegister();
  }
  //Return true if IT is greater than or equal to 4
  if(VEML7700_IT >= 3){
    return true;
  }
  else{ return false; }
  
}

//Decrements the IT counter and updates the IT bits
//Returns true if at or below min IT
bool VEML7700::decrement_IT(){
  if(VEML7700_IT >= -1){
    VEML7700_IT--;
    uint8_t ITBits;
    //Convert numeric value to bits
    getITBits(VEML7700_IT, ALS_IT);
    //Change IT bits in config command
    updateConfigRegister();
  }
  //Return true if IT is less than or equal to -2
  if(VEML7700_IT <= -2){
    return true;
  }
  else{ return false;}
}

//Increments gain and updates the gain bits
//Returns true if at or above max gain
bool VEML7700::incrementGain(){
	//Increment gain
  if(VEML7700_gain <= 3){
    VEML7700_gain++;
    //Get bits corresponding to new gain
    uint8_t gainBits;
    getGainBits(VEML7700_gain, ALS_GAIN);
    updateConfigRegister();
  }
  if(VEML7700_gain >= 4){
    return true;
  }
  else { return false; }
}

//Gives IT bits corresponding to IT counter through ITBits
void VEML7700::getITBits(int ITNumber, uint8_t &ITBits){
	  switch(ITNumber){

    //25ms
    case -2:
      ITBits = 0b1100;
    break;

    //50ms
    case -1:
      ITBits = 0b1000;
    break;

    //100ms
    case 0:
      ITBits = 0b0000;
    break;

    //200ms
    case 1:
      ITBits = 0b0001;
    break;

    //400ms
    case 2:
      ITBits = 0b0010;
    break;

    //800ms
    case 3:
      ITBits = 0b0011;
    break;
  }
}

//Gives gain bits corresponding to gain counter through gainBits
void VEML7700::getGainBits(int gainNumber, uint8_t &gainBits){
  switch(gainNumber){
    //Gain = 1/8
    case 1:
      gainBits = 0b10;
    break;

    //Gain = 1/4
    case 2:
      gainBits = 0b11;
    break;

    //Gain = 1
    case 3:
      gainBits = 0b00;
    break;

    //Gain = 2
    case 4:
      gainBits = 0b01;
    break;
  }
}

//Send 16 bits of data to a register of the VEML7700
//TODO: ADD ERROR HANDLING
void VEML7700::sendData(uint8_t register_address, uint16_t data){
  //Write slave address and receive response
  Wire.beginTransmission(VEML7700_ADDRESS);
  //Specify register
  Wire.write(register_address);
  //Write LSB of data to register and receive ack
  Wire.write(uint8_t(data & 0xff));
  //Write MSB of data to register and receive ack
  Wire.write(uint8_t(data >> 8));
  Wire.endTransmission();
}

//Read 16 bits of data from a register of the VEML7700
//TODO: ADD ERROR HANDLING
void VEML7700::readData(uint8_t register_address, uint16_t &data){
  //Write slave address and receive response
  Wire.beginTransmission(VEML7700_ADDRESS);
  //Specify register
  Wire.write(register_address);
  //Send restart message after communication and do not release bus
  Wire.endTransmission(false);
  //Request 2 bytes from the sensor
  Wire.requestFrom(VEML7700_ADDRESS, 0x02);
  //Receive LSB
  data = Wire.read();
  //Receive MSB
  data |= uint16_t(Wire.read()) << 8;
}

//Update the config register with the current values of 
//ALS_GAIN, ALS_IT, ALS_PERS, ALS_IT_EN, ALS_SD
void VEML7700::updateConfigRegister(){
  uint16_t cmd = ((ALS_GAIN << 11) | (ALS_IT << 6) | (ALS_PERS << 4) | (ALS_INT_EN << 1) | (ALS_SD << 0));
  sendData(0x00, cmd);
}

//Returns the ALS resolution for the current ALS_GAIN and ALS_IT
double VEML7700::getResolution(){
  //Resolution lookup table
  double resolutions[4][6];
  //Row 1
  resolutions[0][0] = 0.0036;
  resolutions[1][0] = 0.0072;
  resolutions[2][0] = 0.0288;
  resolutions[3][0] = 0.0576;
  
  //Row 2
  resolutions[0][1] = 0.0072;
  resolutions[1][1] = 0.0144;
  resolutions[2][1] = 0.0576;
  resolutions[3][1] = 0.1152;
  
  //Row 3
  resolutions[0][2] = 0.0144; 
  resolutions[1][2] = 0.0288;
  resolutions[2][2] = 0.1152;
  resolutions[3][2] = 0.2304;
  
  //Row 4
  resolutions[0][3] = 0.0288;
  resolutions[1][3] = 0.0576;
  resolutions[2][3] = 0.2304;
  resolutions[3][3] = 0.4608;
  
  //Row 5
  resolutions[0][4] = 0.0576;
  resolutions[1][4] = 0.1152;
  resolutions[2][4] = 0.4608;
  resolutions[3][4] = 0.9216;
  
  //Row 6
  resolutions[0][5] = 0.1152;
  resolutions[1][5] = 0.2304;
  resolutions[2][5] = 0.9216;
  resolutions[3][5] = 1.8432;

  int gainIndex;
  int ITIndex;

  //Find gain index
  switch(ALS_GAIN){
    case 0b01:
      gainIndex = 0;
    break;

    case 0b00:
      gainIndex = 1;
    break;

    case 0b11:
      gainIndex = 2;
    break;

    case 0b10:
      gainIndex = 3;
    break;
  }

  //Find IT index
  switch(ALS_IT){
    case 0b0011:
      ITIndex = 0;
    break;

    case 0b0010:
      ITIndex = 1;
    break;

    case 0b0001:
      ITIndex = 2;
    break;

    case 0b0000:
      ITIndex = 3;
    break;

    case 0b1000:
      ITIndex= 4;
    break;

    case 0b1100:
      ITIndex = 5;
    break;
  }

  return resolutions[gainIndex][ITIndex];
}

//Returns the IT in ms for the current ALS_IT value
int VEML7700::getIT_ms(){
  switch(ALS_IT){
    //25ms
    case 0b1100:
      return 25;
    break;

    //50ms
    case 0b1000:
      return 50;
    break;

    //100ms
    case 0b0000:
      return 100;
    break;

    //200ms
    case 0b0001:
      return 200;
    break;

    //400ms
    case 0b0010:
      return 400;
    break;

    //800ms
    case 0b0011:
      return 800;
    break;
  }
}

//Initialises, calibrates and reads from VEML7700
//Returns the most accurate lux value calculated
double VEML7700::getAmbient(){
	//Initialise
  initialise();
	//Calibrate gain and integration time
  int result = ALScalibrate();
	//Ensure ALS is on
  ALS_SD = 0b0;
  updateConfigRegister();
  delay(getIT_ms() + 5);
  
	//Read the value of the ALS
  uint16_t ALS;
  readData(0x04, ALS);
	
	//Calc the approx lux value
  double VEML_lux = ALS * getResolution();
  double corrected_lux = VEML_lux;
	//If appropriate, apply non-linear correction formula
  if(result == 1){
    corrected_lux = 6.0135e-13*pow(VEML_lux,4)-9.3924e-9*pow(VEML_lux,3)+8.1488e-5*pow(VEML_lux,2)+1.0023*VEML_lux;
  }

  //Shut down ALS
  ALS_SD = 0b1;
  updateConfigRegister();
  delay(3);

	//Return most accurate lux value
  return corrected_lux;
}

double VEML7700::getWhite(){
	//Initialise
  initialise();
	//Calibrate gain and integration time
  int result = WHITEcalibrate();
	//Ensure ALS is on
  ALS_SD = 0b0;
  updateConfigRegister();
  delay(getIT_ms() + 5);
  
	//Read the value of the WHITE channel
  uint16_t WHITE;
  readData(0x05, WHITE);
	
	//Calc the approx lux value
  double VEML_lux = WHITE * getResolution();
  double corrected_lux = VEML_lux;
	//If appropriate, apply non-linear correction formula
  if(result == 1){
    corrected_lux = 6.0135e-13*pow(VEML_lux,4)-9.3924e-9*pow(VEML_lux,3)+8.1488e-5*pow(VEML_lux,2)+1.0023*VEML_lux;
  }

  //Shut down ALS
  ALS_SD = 0b1;
  updateConfigRegister();
  delay(3);

	//Return most accurate lux value
  return corrected_lux;
}
