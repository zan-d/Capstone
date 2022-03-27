

float sensorValue = 0; 

#include <math.h>

//Moistrue Global Variables 
#define         MoisturePin                  (A0)
float moistSensorVal = 0;

//Temperature Global Variables 
int B = 4275;               // B value of the thermistor
//int R0 = 100000;          // R0 = 100k
#define         TemperaturePin               (A1)     // Grove - Temperature Sensor connect to A0

//CO2 sensor variables 
#define         CO2Pin                       (A2)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (2)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier

/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in
                                                     //normal operation

/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE           (0.335) //define the output of the sensor in volts when the concentration of CO2 is 400PPM, voltage/8.5
#define         REACTION_VOLTAGE             (0.022) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2

/*****************************Globals***********************************************/
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTAGE/(2.602-3))};

int findMoisture() {
    static long moistSensorVal = 0;
    for (int i = 0; i <= 100; i++) 
    { 
        moistSensorVal = moistSensorVal + analogRead(MoisturePin); 
        delay(1); 
    } 
    moistSensorVal = moistSensorVal/100.0; 
    return moistSensorVal;
        
}
float findTemp() { 
    static long tempSensorVal = 0;
    for (int i = 0; i< 100; i++)
    {
      tempSensorVal = tempSensorVal + analogRead(TemperaturePin);
    
      delay(1);
    }
    tempSensorVal = tempSensorVal / 100.0;
    Serial.println("tempSensorVal: ");
    Serial.println(tempSensorVal);
    
    float R = 1023.0/tempSensorVal-1.0;
    R = 100000*R;
    float temperature = 1.0/(log(R/100000)/B+1/298.15)-273.15; // convert to temperature via datasheet
    Serial.println("Temp:");
    Serial.println(temperature);
    return temperature;
}
float MGRead(int CO2Pin)
{
    int i;
    double v=0;

    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(CO2Pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;
}

int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else {
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}
int findCO2() {
    static int CO2Level = 0;
    static float volts = 0.0;

    volts = MGRead(CO2Pin);
    CO2Level = MGGetPercentage(volts,CO2Curve);
    return CO2Level;
}



void setup() { 
 Serial.begin(9600);
 
} 

void loop() { 
    
    static int moistureVal = 0;
    static int tempVal = 0;
    static int CO2Val = 0;
    moistureVal = findMoisture() ;
    tempVal = findTemp() ;
    CO2Val = findCO2() ;
    Serial.println("Moisture: ");
    Serial.println(moistureVal);
    Serial.println("\t");
    Serial.println("Temperature: "); 
    Serial.println(tempVal);
    Serial.println("\t");
    Serial.println("CO2 Level: ");
    Serial.println(CO2Val);
    Serial.println("\n"); 
    delay(1000);
}
