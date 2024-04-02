/*   MV2_analog.ino
 * 
 *   example code how to set and read analog data from 
 * 
 *   METROLAB MV2 3 axis Hall sensor: 
 *   
 *   code for standalone operation 
 *   with an Arduino MEGA or UNO (generic design)
 *   
 *   I did a coarse calibration versus another Hall probe and the values seem to be correct within 5% or better.
 *   Becasue I had to magnetic shield of suitable size I could not check the offsets.
 *   There is a systemtatic difference of 0.5 relative to the calibration data in Table 6 (5V),  
 *   addionally the temperature is also somewhat odd, couldn't calibrate this much.
 *   
 *   Therefore, this code is just an example how to get started (reading all three field components with 10bit resolution
 *   and manually adjustable range). Extended range and other temp. compensations were not tried, but I'm sure you will 
 *   figure out if needed.
 *   
 *   PS: These is also a code for digital operation
 * 
 *   (c) P. Blümler, Physics Uni Mainz, Germany 2022
 *       bluemler@uni-mainz.de
 */
 
 #include <SPI.h> 

#define NDIGITAL_ANALOG_PIN     8
#define VDD_PIN                 A5 
#define A_BX_PIN                A0
#define A_BY_PIN                A1
#define A_BZ_PIN                A2
#define A_TEMPERATURE_PIN       A3
#define A_REF_PIN               A4
#define A_EMR_PIN               2
#define A_INV_PIN               4
#define A_LP_PIN                7

#define A_MA0_PIN               10  //pins on Arduino UNO
#define A_MA1_PIN               11
#define A_RA0_PIN               12
#define A_RA1_PIN               13
/*  
 *  #define A_MA0_PIN               53  //pins on Arduino MEGA
    #define A_MA1_PIN               51
    #define A_RA0_PIN               50
    #define A_RA1_PIN               52
 */

 char strX[20]; //at least as long as # of char +1
char strY[20]; //are these too long/short. Idk but seems to work/not matter
char strZ[20]; 
char strVarX[20]; //at least as long as # of char +1
char strVarY[20]; //are these too long/short. Idk but seems to work/not matter
char strVarZ[20]; 
char buffer[50];

float Vref,Bx, By,Bz,Temp,RangeXY,RangeZ;
int ARi,Bxi,Byi,Bzi,Ti;
uint8_t range;

void setup() {
    //Configure common pins
    pinMode(NDIGITAL_ANALOG_PIN, OUTPUT);
    pinMode(VDD_PIN, INPUT);
    pinMode(A_BX_PIN, INPUT);
    pinMode(A_BY_PIN, INPUT);
    pinMode(A_BZ_PIN, INPUT);
    pinMode(A_TEMPERATURE_PIN, INPUT);
    pinMode(A_REF_PIN, INPUT);
    pinMode(A_INV_PIN, OUTPUT);
    //Configure option pins
    pinMode(A_MA0_PIN, OUTPUT);
    pinMode(A_MA1_PIN, OUTPUT);
    pinMode(A_RA0_PIN, OUTPUT);
    pinMode(A_RA1_PIN, OUTPUT);
    pinMode(A_LP_PIN, OUTPUT);
    pinMode(A_EMR_PIN, OUTPUT);
   
    digitalWrite(NDIGITAL_ANALOG_PIN, HIGH); //enable Analog mode
    // Initialize all other controls to LOW (logic 0)
    digitalWrite(A_INV_PIN, LOW);
    digitalWrite(A_MA0_PIN, LOW);
    digitalWrite(A_MA1_PIN, LOW);
    digitalWrite(A_RA0_PIN, LOW);
    digitalWrite(A_RA1_PIN, LOW);
    digitalWrite(A_LP_PIN, LOW);
    digitalWrite(A_EMR_PIN, LOW);
    Serial.begin(9600);
}

void AnalogSetOptions(uint8_t Options)  //Set options bits : RA0, RA1, MA0, MA1, LP, INV, EMR
{   digitalWrite(A_RA0_PIN, bitRead(Options, 0));
    digitalWrite(A_RA1_PIN, bitRead(Options, 1));
    digitalWrite(A_MA0_PIN, bitRead(Options, 2));
    digitalWrite(A_MA1_PIN, bitRead(Options, 3));
    digitalWrite(A_LP_PIN,  bitRead(Options, 4));
    digitalWrite(A_INV_PIN, bitRead(Options, 5));
    digitalWrite(A_EMR_PIN, bitRead(Options, 6));
}


void loop() {
  // SELECT RANGE MANUALLY
  range=0;    // 0 = +/- 100 mT, 1 = +/- 300 mT, 2 = +/- 1 T, 3 = +/- 3 T

  //Serial.print(" >>> Analog Mode:   range: ");
  switch (range){
    case 0:
      RangeXY=18.18;  //range from table 6 (5V)
      RangeZ=20.73;
      //Serial.println("+/- 100 mT");
      break;
    case 1:
      RangeXY=6,27;  //range from table   6 (5V)
      RangeZ=7.04;
      break;
    case 2:
      RangeXY=1.89;  //range from table  6 (5V)
      RangeZ=2.13;
      break;
    default:
      RangeXY=0.62;  //range from table  6 (5V)
      RangeZ=0.72;
      break;
    }
    // here only the first 2 bits (RA0 and RA1) are set 
    AnalogSetOptions(range); // Set OPTIONS bits (RA0, RA1, MA0, MA1, LP, INV, EMR)
    ARi=analogRead(A_REF_PIN);
    Bxi=analogRead(A_BX_PIN);
    Byi=analogRead(A_BY_PIN);
    Bzi=analogRead(A_BZ_PIN);
    Ti=analogRead(A_TEMPERATURE_PIN);
    // conversion to voltages(without extended measuring range)
    Vref=float(ARi)/511*5000; // reference voltage [mV]
    Bx=float(Bxi-ARi)/511*5000;  // 10 bit Arduino ADC * 5000 mV
    By=float(Byi-ARi)/511*5000;
    Bz=float(Bzi-ARi)/511*5000;
    Temp=float(Ti)/511*5000;
    dtostrf(Bx/RangeXY/2, -5, 7, strX);
    dtostrf(By/RangeXY/2, -5, 7, strY);
    dtostrf(Bz/RangeZ/2, -5, 7, strZ);

    sprintf(buffer, "%s, %s, %s", strX, strY, strZ);
    Serial.println(buffer);
       
    delay(100);
}
