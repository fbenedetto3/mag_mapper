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

float Vref, Temp,RangeXY,RangeZ;
int ARi,Bxi,Byi,Bzi,Ti;
uint8_t range;

char strX[20]; //at least as long as # of char +1
char strY[20]; //are these too long/short. Idk but seems to work/not matter
char strZ[20]; 
char strVarX[20]; //at least as long as # of char +1
char strVarY[20]; //are these too long/short. Idk but seems to work/not matter
char strVarZ[20]; 
char buffer[50];

int nReads = 30;
float Bx[nReads];
float By[nReads];
float Bz[nReads];




char incomingByte = '0';

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
        for (int i = 0; i < nReads; i++){
                // SELECT RANGE MANUALLY
        range=0;    // 0 = +/- 100 mT, 1 = +/- 300 mT, 2 = +/- 1 T, 3 = +/- 3 T

        //Serial.print(" >>> Analog Mode:   range: ");
        if (Serial)
        switch (range){
          case 0:
            RangeXY=18.18;  //range from table 6 (5V)
            RangeZ=20.73;
            //Serial.println("+/- 100 mT");
            break;
          case 1:
            RangeXY=6,27;  //range from table   6 (5V)
            RangeZ=7.04;
            //Serial.println("+/- 300 mT");
            break;
          case 2:
            RangeXY=1.89;  //range from table  6 (5V)
            RangeZ=2.13;
            //Serial.println("+/- 1000 mT");
            break;
            case 3:
          default:
            RangeXY=0.62;  //range from table  6 (5V)
            RangeZ=0.72;
            //Serial.println("+/- 3000 mT");
            break;
          }
          // here only the first 2 bits (RA0 and RA1) are set 
          AnalogSetOptions(range); // Set OPTIONS bits (RA0, RA1, MA0, MA1, LP, INV, EMR)



//Wait to see if read command is sent
 if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    if (incomingByte == 'M') {


          ARi=analogRead(A_REF_PIN);
          Bxi=analogRead(A_BX_PIN);
          Byi=analogRead(A_BY_PIN);
          Bzi=analogRead(A_BZ_PIN);
          Ti=analogRead(A_TEMPERATURE_PIN);
          // conversion to voltages(without extended measuring range)
          Vref=float(ARi)/511*5000; // reference voltage [mV]
          
          // 10 bit Arduino ADC * 5000 mV and additional mV offset

          Bx[i] = float(Bxi-ARi-3.2)/511*5000/RangeXY/2;
          By[i] = float(Byi-ARi-4.5)/511*5000/RangeXY/2 ;
          Bz[i]=float(Bzi-ARi)/511*5000/RangeZ/2;
          Serial.println(Bx[i], 1);

      }
      float BxSum = 0;
      float BySum = 0;
      float BzSum = 0;
      float BxSumSq = 0;
      float BySumSq = 0;
      float BzSumSq = 0;

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
      Serial.print(meanX, 3);
      Serial.print(", ");
      meanY = BySum/nReads;
      Serial.print(meanY, 3);
      Serial.print(", ");
      meanZ = BzSum/nReads;
      Serial.print(meanZ, 3);
      Serial.println();
        BxSum = 0;
        BySum = 0;
        BzSum = 0;








      // for (int i = 0; i < nReads; i++){
      //   BxSumSq += (Bx[i] - meanX)*(Bx[i] - meanX);
      //   //Serial.println(BxSumSq,1);
      //   BySumSq += (By[i] - meanY)*(By[i] - meanY);
      //   BzSumSq += (Bz[i] - meanZ)*(Bz[i] - meanZ);
      // }
      // varX = BxSumSq/(nReads-1);
      // Serial.print(varX, 3);
      // Serial.print(", ");
      // varY = BxSumSq/(nReads-1);
      // Serial.print(varY, 3);
      // Serial.print(", ");
      // varZ = BxSumSq/(nReads-1);
      // Serial.println(varZ, 3);
      // dtostrf(meanX, -5, 7, strX); 
      //  dtostrf(meanY, -5, 7, strY); 
      //  dtostrf(meanZ, -5, 7, strZ); 
      //  dtostrf(meanX, -5, 7, strVarX); 
      //  dtostrf(meanY, -5, 7, strVarY); 
      //  dtostrf(meanZ, -5, 7, strVarZ); 
      //sprintf(buffer, "%s, %s, %s, %s, %s, %s", strX, strY, strZ, strVarX, strVarY, strVarZ);
       
    }
  }
}
