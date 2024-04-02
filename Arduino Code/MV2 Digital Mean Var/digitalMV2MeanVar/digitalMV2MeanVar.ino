/*   MV2_digital.ino
 * 
 *   example code how to set and read data from 
 * 
 *   METROLAB MV2 3 axis Hall sensor: 
 *   
 *   code for standalone operation 
 *   with an Arduino MEGA or UNO (generic design)
 *   
 *   I did a coarse calibration versus another Hall probe and the values seem to be correct within 5% or better.
 *   Becasue I had to magnetic shield of suitable size I could not check the offsets.
 *   The temperature is also somewhat odd, because the values given in the data sheet (table 10) gave strange values.
 *   
 *   Therefore, this code is just an example how to get started (reading all three field components with 16bit resolution
 *   and manually adjustable range). Extended range and other temp. compensations were not tried, but I'm sure you will 
 *   figure out if needed.
 *   
 *   PS: These is also a code for analog operation
 * 
 *   (c) P. Blümler, Physics Uni Mainz, Germany 2022
 *       bluemler@uni-mainz.de
 */
    const int xPin = A0; 
    const int yPin = A1;
    const int zPin = A2;
    float xZero = digitalRead(xPin);
    float yZero = digitalRead(yPin);
    float zZero = digitalRead(zPin);

#include <SPI.h> 
#define NDIGITAL_ANALOG_PIN     8
#define D_DR_PIN                2
#define D_INIT_PIN              7
#define D_CHIP_SELECT_PIN       10
#define D_SPI_MOSI                11
#define D_SPI_MISO              12
#define D_SPI_CLK               13

const byte READ = 0x1C;   //Read address MV2     
const byte WRITE = 0x2C;  //Write address MV2
float Temp,RangeXY,RangeZ;
uint16_t data;
int range_set;
byte RA_byte;

char strX[20]; //at least as long as # of char +1
char strY[20]; //are these too long/short. Idk but seems to work/not matter
char strZ[20]; 
char strVarX[20]; //at least as long as # of char +1
char strVarY[20]; //are these too long/short. Idk but seems to work/not matter
char strVarZ[20]; 
char buffer[50];

const int nReads = 30;
float Bx[nReads];
float By[nReads];
float Bz[nReads];

char incomingByte = '0';


void setup() {
    Serial.begin(9600);
    digitalWrite(NDIGITAL_ANALOG_PIN,LOW); //Select digital mode
    SPI.begin();
    pinMode(D_DR_PIN , INPUT);
    pinMode(D_CHIP_SELECT_PIN, OUTPUT);
    writeRegister(0xC1, 0x00);  //init
    writeRegister(0x2C, 0x2C);  //set to read Bx 3T 16 bit (to init)
    writeRegister(0x2D, 0x02);  //permanent MISO
    writeRegister(0x2E, 0x08);  //temp. comp 0
    delay(100); }
    

//little routine to read a SPI register 
uint16_t readRegister(byte Register, byte Value) {
   uint16_t result = 0;   // result to return
   uint16_t dataToSend = (Register << 8) | Value;
   digitalWrite(D_CHIP_SELECT_PIN, LOW);
   result = SPI.transfer16(dataToSend);
   digitalWrite(D_CHIP_SELECT_PIN, HIGH);
   return (result);}


//little routine to write a SPI register 
void writeRegister(byte Register, byte Value) {
   uint16_t dataToSend = (WRITE << 8) | Register ;
   digitalWrite(D_CHIP_SELECT_PIN, LOW);
   SPI.transfer16(dataToSend); //Send register location
   SPI.transfer(Value);  //Send value to record into register
   digitalWrite(D_CHIP_SELECT_PIN, HIGH);}
 
void loop() {
  float Bxs = 0;
  float Bys = 0;
  float Bzs = 0;
  // SELECT RANGE MANUALLY
  range_set=1; // 0 = +/- 100 mT, 1 = +/- 300 mT, 2 = +/- 1 T, 3 = +/- 3 T

  //Serial.print(" >>> Digital Mode:   range: ");
  
  switch (range_set){ //offsets need to be detemined experimentally
    case 0:
      RA_byte=0x00;
      RangeXY=267;  //range from table 10
      RangeZ=289;
     // Serial.println("+/- 100 mT");
      break;
    case 1:
      RA_byte=0x04;
      RangeXY=91.2;  //range from table 10
      RangeZ=98.5;
      //Serial.println("+/- 300 mT");
      break;
    case 2:
      RA_byte=0x08;
      RangeXY=27.6;  //range from table 10
      RangeZ=29.5;
      //Serial.println("+/- 1000 mT");
      break;
    default:
      RA_byte=0x0C;
      RangeXY=9.1;  //range from table 10
      RangeZ=10;
      //Serial.println("+/- 3000 mT");
      break;
  }

 if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

      if (incomingByte == 'M') {
        Serial.println("M recieved");
        for (int i = 0; i < nReads; i++){
          data = readRegister(0x2C, 0x21+RA_byte);
          Bx[i] =((float(data)-32768+280)/RangeXY);
          data = readRegister(0x2C, 0x22+RA_byte);
          By[i] =((float(data)-32768+385)/RangeXY);
          data = readRegister(0x2C, 0x23+RA_byte);
          Bz[i] =((float(data)-32768-125)/RangeZ);
          delay(10);
        }
        float BxSum = 0;
        float BySum = 0;
        float BzSum = 0;
        float varXSum = 0;
        float varYSum = 0;
        float varZSum = 0;

        for (int i = 0; i < nReads; i++){
          BxSum += Bx[i];
          BySum += By[i];
          BzSum += Bz[i];
        }

        float meanX = 0;
        float meanY = 0;
        float meanZ = 0;
        float varX = 0;
        float varY = 0;
        float varZ = 0;

        meanX = BxSum/nReads;
        Serial.print(meanX, 5);
        Serial.print(", ");
        meanY = BySum/nReads;
        Serial.print(meanY, 5);
        Serial.print(", ");
        meanZ = BzSum/nReads;
        Serial.print(meanZ, 5);
        Serial.print(", ");

        for (int i = 0; i < nReads; i++){
            varXSum += (Bx[i]/RangeXY/2 - meanX)*(Bx[i]/RangeXY/2 - meanX);
            varYSum += (By[i]/RangeXY/2 - meanY)*(By[i]/RangeXY/2 - meanY);
            varZSum += (Bz[i]/RangeZ/2 - meanZ)*(Bz[i]/RangeZ/2 - meanZ);
          }

        varX = varXSum/(nReads-1);
        Serial.print(varX, 5);
        Serial.print(", ");
        varY = varYSum/(nReads-1);
        Serial.print(varY, 5);
        Serial.print(", ");
        varZ = varZSum/(nReads-1);
        Serial.println(varZ, 5);

    }
 }
     else {
      if (digitalRead(D_DR_PIN) == HIGH) { //if ready
        data = readRegister(0x2C, 0x21+RA_byte); 
        Bxs = (float(data)-32768)/RangeXY;
        Serial.print(Bxs, 5);
        Serial.print(", ");
        data = readRegister(0x2C, 0x22+RA_byte);
        Bys =((float(data)-32768+385)/RangeXY);
        Serial.print(Bys, 3);
        Serial.print(", ");
        data = readRegister(0x2C, 0x23+RA_byte);
        Bzs =((float(data)-32768-125)/RangeZ);
        Serial.println(Bzs, 3);
        delay(200);
      }
      else { 
        Serial.println("Not Ready");
        delay(100);
      }
    }
}