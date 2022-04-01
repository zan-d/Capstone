

float sensorValue = 0; 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

//Output Pin Variables
int BASE = 4 ;  //The first relay is connected to the I / O port
int NUM = 4;   //Total number of relays

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
  //  Serial.println("tempSensorVal: ");
  //  Serial.println(tempSensorVal);
    
    float R = 1023.0/tempSensorVal-1.0;
    R = 100000*R;
    float temperature = 1.0/(log(R/100000)/B+1/298.15)-273.15; // convert to temperature via datasheet
   // Serial.println("Temp:");
   // Serial.println(temperature);
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

void System_Heartbeat(bool fMode, bool fWaterButton, bool fFanButton, bool fLightButton)
{
  //Serial.println("Moisture: Test 1");
    float fMoisture;
    float fTemp;
    float fCarbon;
    float newTemp;

    static unsigned long PreviousMillis = 0;
    unsigned long CurrentMillis;

   fMoisture = findMoisture();
   fTemp = findTemp();
   fCarbon = findCO2();
    
    unsigned long timer;
    unsigned long waterTime;
    unsigned long fanTime;
    unsigned long heatTime;

    typedef enum
    {
        HEARTBEAT_INIT = 0,
        HEARTBEAT_READ,
        HEARTBEAT_WATER_PUMP,
        HEARTBEAT_FAN,
        HEARTBEAT_LIGHT,
        HEARTBEAT_END
    }eHeartbeat;

    static eHeartbeat eHeartbeatState = HEARTBEAT_INIT;

      switch(eHeartbeatState)
      {
        case HEARTBEAT_INIT:
            Serial.println("Heartbeat Init");

            waterTime = 0;
            fanTime = 0;
            heatTime = 0;
  
            eHeartbeatState = HEARTBEAT_READ;
  
            break;
  
        case HEARTBEAT_READ:
  
            //Call moisture sensor module here, if it returns true, needs water, go to water pump state
  
             Serial.println("Heartbeat Read");
            // Serial.println(fTemp);
            // Serial.println("\t");
  
            if((fMoisture < 0 && fMode == false) || (fWaterButton == true && fMode == true))
            {
                eHeartbeatState = HEARTBEAT_WATER_PUMP;
            }
  
            //call temperature sensor module here, if it returns true, need heat, go to heat lamp state
            if((fTemp < 24 && fMode == false) || (fLightButton == true && fMode == true))
            {
                eHeartbeatState = HEARTBEAT_LIGHT;
            }
  
            //call CO2 sensor module here, if it returns true, need air, go to fan state
            if((fCarbon < 100 && fMode == false) || (fFanButton == true && fMode == true))
            {
                eHeartbeatState = HEARTBEAT_FAN;
            }
  
            break;
  
        case HEARTBEAT_WATER_PUMP:

            CurrentMillis = millis();
            Serial.println("Heartbeat Water Pump");
            Serial.println(fMoisture);
            // call waterpump module function here
            // waterPump();

            waterTime = millis();
            Serial.println(waterTime);
            //Write to the output pin for Water
            digitalWrite(4, HIGH);

            if((fMoisture > 100 && fMode == false) || (fWaterButton == false && fMode == true) || ((CurrentMillis - PreviousMillis) >= 5000))
            {
              digitalWrite(4, LOW);
              
              PreviousMillis = CurrentMillis;
              // send to the end state to reset the state machine
              eHeartbeatState = HEARTBEAT_END;
            }
            break;
  
        case HEARTBEAT_FAN:

            CurrentMillis = millis();
            Serial.println("Heartbeat Heat Fan");
            Serial.println(fCarbon);
            // call heat fan module function here
            // oxyFan();
            
            fanTime = 0;

            digitalWrite(7, HIGH);
            
            if((fCarbon < 100 && fMode == false)  || (fFanButton == false && fMode == true) || ((CurrentMillis - PreviousMillis) >= 5000))
            {
              
              digitalWrite(7, LOW); 

              PreviousMillis = CurrentMillis;
              // send to the end state to reset the state machine
              eHeartbeatState = HEARTBEAT_END;
            }
            break;
  
        case HEARTBEAT_LIGHT:
             CurrentMillis = millis();
             Serial.println("Heartbeat Heat Lamp");
             Serial.println(fTemp);
             Serial.println(CurrentMillis);
             Serial.println(PreviousMillis);
             


            ////Write to the output pin for Water
            digitalWrite(5, HIGH);

            if((fTemp > 24 && fMode == false) || (fLightButton == false && fMode == true) || ((CurrentMillis - PreviousMillis) >= 5000))
            {
            
            digitalWrite(5, LOW);

            PreviousMillis = CurrentMillis;
            // send to the end state to reset the state machine
            eHeartbeatState = HEARTBEAT_END;
            }
            break;
  
        case HEARTBEAT_END:
  
            eHeartbeatState = HEARTBEAT_INIT;
  
            break;
        }
    
}

int b;
char payload[3];
bool fWaterSignal = false;
bool fFanSignal = false;
bool fLightSignal = false;
bool fAutoMode = false;

void setup() { 
 Serial.begin(9600);

  //Water Pump: 4
  //Heater: 5
  //Light 6
  //Fan 7
 for (int i = BASE; i < BASE + NUM; i ++) 
 {
   pinMode(i, OUTPUT);   //Set the number I/O port to outputs
 }
 pinMode(12, OUTPUT);
} 

void loop() { 
int c;
            
bool sendser = false;
    if (Serial.available())
    {
    //    int b;
//        char payload[3];
        String a = Serial.readString();
        b = a.toInt();
//        sprintf(payload, "%d", b);
//        Serial.println(payload);
          sendser = true;
        delay(1000);
    }
  c = b;
        if(sendser){
        sprintf(payload, "%d", c);
        Serial.println(payload);
        sendser = false;
        }
      
    System_Heartbeat(fAutoMode, fWaterSignal,fFanSignal,fLightSignal);
//    static int moistureVal = 0;
//    static int tempVal = 0;
//    static int CO2Val = 0;
//    moistureVal = findMoisture() ;
//    tempVal = findTemp() ;
//    CO2Val = findCO2() ;
//    Serial.println("Moisture: ");
//    Serial.println(moistureVal);
//    Serial.println("\t");
//    Serial.println("Main Temperature: "); 
//    Serial.println(tempVal);
//    Serial.println("\t");
//    Serial.println("CO2 Level: ");
//    Serial.println(CO2Val);
//    Serial.println("\n"); 
//    delay(1000);
}
