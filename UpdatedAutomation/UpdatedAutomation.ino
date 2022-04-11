float sensorValue = 0; 
double tempLow = 0;
double tempHi = 0;
double lightHours = 0;
double moistLow = 0;
double moistHi = 0;
int waterPump = 0;
int heater = 0;
int lightLamp = 0;
int fan = 0;
const int BUFFER_SIZE = 100;
String Final_Message = "";


#include <math.h>
#include <stdbool.h>
//global values 
#define ONE_MINUTE                            60000 

//Moistrue Global Variables 
#define         MoisturePin                  (A0)
float moistSensorVal = 0;

//Temperature Global Variables 
int B = 4275;               // B value of the thermistor
//int R0 = 100000;          // R0 = 100k
#define         TemperaturePin               (A5)     // Grove - Temperature Sensor connect to A0

//CO2 sensor variables 
#define         CO2Pin                       (A9)     //define which analog input channel you are going to use
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
//    Serial.println("tempSensorVal: ");
//    Serial.println(tempSensorVal);
    
    float R = 1023.0/tempSensorVal-1.0;
    R = 100000*R;
    float temperature = 1.0/(log(R/100000)/B+1/298.15)-273.15; // convert to temperature via datasheet
//    Serial.println("Temp:");
//    Serial.println(temperature);
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


char sendMsgToPi() {
//Msg types:
//  D: Definion of plant -  when an user selects the type of a plant - coming from database
//  S: Set up by User - when an user changes environment such as temp or light
//  N: No msg available - no updates of Message  
  char msgType = 'N';
  if (Serial.available()){
   
        String a = Serial.readString();
           
        Serial.println(String("echo: ") + a);
//        delay(10);
//        Serial.print("Arduino");
        return msgType;

    } else {
//       Serial.println("nothing to read");
//       Serial.write("Arduino - not run ");
       return msgType;
    }

}



bool  lastMsg() {
  char *strings[BUFFER_SIZE];
  char *ptr = NULL; 
  int index = 0;
  
  if (Serial.available()) {
      char msg[BUFFER_SIZE];
      String stringMsg = Serial.readString();
      Serial.print("Input String: \t");
      Serial.println(stringMsg);
      if (stringMsg.length() < 15) {
        Final_Message = stringMsg.substring(0,stringMsg.length()-2);
      } else {
           for (int i = 0; i < stringMsg.length(); i++) {
              msg[i] = stringMsg.charAt(i);
           }    
           ptr = strtok(msg, ";");
//           Serial.print("first ptr \t");
//           Serial.println(ptr);
          while (ptr != NULL) {
              strings[index] = ptr;
              index++;
              ptr = strtok(NULL,";");
//              Serial.print("i ptr \t");
//              Serial.println(ptr);
           }
            
            index = index-2;
//            Serial.print("index: ");
//            Serial.println(index); 
            
            String test1 = strings[index];
//            Serial.print("strings[index] message: ");
//            Serial.println(strings[index]); 
            Final_Message = strings[index];
//                Serial.print("test1: \t");
//      Serial.println(test1);  
             
      //      for (int i =0; i <= index ; i++) {
      //        Final_Message = Final_Message.substring(0,Final_Message.length()-1) + char(*strings[i]); 
      //      }
      //      Final_Message = Final_Message.substring(0,Final_Message.length()-1) + '\0';
      //      Serial.print("Final message: ");
      //      Serial.println(Final_Message);   
      }
      
      
      Serial.print("Final Message: \t");
      Serial.println(Final_Message);    
      return true;
  } else { return false;}
  
}


char readMsgFromPi() {
  //"D,temp low, temp high, hours of light, moisture low, moisture high"
  //"S,(0/1) for waterPump,(0/1) for heater, (0,1) for lightlamp, (0,1) for fan)
  //Msg types:
  //  D: Definion of plant -  when an user selects the type of a plant - coming from database
  //  S: Set up by User - when an user changes environment such as temp or light
  //  N: No msg available - no updates of Message  
    char msgType = 'N';
    bool msgFlag = lastMsg();
//    Serial.print("msgFlag = \t");
//    Serial.println(msgFlag);
//    Serial.print("Final_Message= \t");
//    Serial.println(Final_Message);
    
    if (msgFlag){ 
          
          //char msg[] = "D,100,200,200,8,20,40";
          char msg[] = "                           ";
          for (int i = 0; i < Final_Message.length(); i++) {
              msg[i] = Final_Message .charAt(i);
            }
          char msgType;        
          char *strings[7];
          char *ptr = NULL; 
          int index = 0;
          ptr = strtok(msg, ",");
          while (ptr != NULL) {
            strings[index] = ptr;
            index++;
            ptr = strtok(NULL,",");
          }
    
        //Serial.println(strings[0]); 
        msgType = *strings[0];
        if (msgType == 'D') {
            tempLow = atof(strings[1]);
            tempHi = atof(strings[2]);
            lightHours = atof(strings[3]);
            moistLow = atof(strings[4]);
            moistHi = atof(strings[5]) ; 
          } else if ( msgType == 'S') {
            waterPump = atoi(strings[1]); 
            heater = atoi(strings[2]);
            lightLamp = atoi(strings[3]);
            fan = atoi(strings[4]);
          } else {
            Serial.println("Idendifier doesn't work\t message is the following: ");
            Serial.println(msg);
            msgType = 'N';
            return *strings[0];
          }
       
  //        Serial.println(String("echo: ") + a);
  //        delay(10);
  //        Serial.print("Arduino");
          return *strings[0];
  
      } else {
  //       Serial.println("nothing to read");
  //       Serial.write("Arduino - not run ");
         return msgType;
      }
  
 }

 void System_Heartbeat(bool fMode)
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
        
            if(fMode)
            {
              eHeartbeatState = HEARTBEAT_READ;
            }
            
            break;
  
        case HEARTBEAT_READ:
        
            if(fMode)
            {
              //Call moisture sensor module here, if it returns true, needs water, go to water pump state
    
               Serial.println("Heartbeat Read");
              // Serial.println(fTemp);
              // Serial.println("\t");
    
              if(fMoisture < 200)
              {
                  eHeartbeatState = HEARTBEAT_WATER_PUMP;
              }
    
              //call temperature sensor module here, if it returns true, need heat, go to heat lamp state
              if(fTemp < 17)
              {
                  eHeartbeatState = HEARTBEAT_LIGHT;
              }
    
              //call CO2 sensor module here, if it returns true, need air, go to fan state
              if(fCarbon > 1300)
              {
                  eHeartbeatState = HEARTBEAT_FAN;
              }
            }
            else
            {
              eHeartbeatState = HEARTBEAT_INIT;
            }
            break;
  
        case HEARTBEAT_WATER_PUMP:
        
            if(fMode)
            {
              CurrentMillis = millis();
              //Write to the output pin for Water
              digitalWrite(5, HIGH);
  
              if((fMoisture > 200) || ((CurrentMillis - PreviousMillis) >= ONE_MINUTE))
              {
                digitalWrite(5, LOW);
                
                PreviousMillis = CurrentMillis;
                // send to the end state to reset the state machine
                eHeartbeatState = HEARTBEAT_END;
              }
            }
            else
            {
              eHeartbeatState = HEARTBEAT_INIT;
            }
            break;
  
        case HEARTBEAT_FAN:
        
            if(fMode)
            {
              CurrentMillis = millis();
              Serial.println("Heartbeat Fan ON:");       

              digitalWrite(4, HIGH);
              
              if(fCarbon < 1300)
              {
                
                digitalWrite(4, LOW); 
  
                PreviousMillis = CurrentMillis;
                
                // send to the end state to reset the state machine
                eHeartbeatState = HEARTBEAT_END;
              }
            }
            else
            {
              eHeartbeatState = HEARTBEAT_INIT;
            }
            break;
  
        case HEARTBEAT_LIGHT:
        
             if(fMode)
             {
               CurrentMillis = millis();
                   
              //Write to the output pin for Water
              digitalWrite(7, HIGH);
  
              if((fTemp > 17) || ((CurrentMillis - PreviousMillis) >= ONE_MINUTE))
              {
              
                digitalWrite(7, LOW);
    
                PreviousMillis = CurrentMillis;
                
                // send to the end state to reset the state machine
                eHeartbeatState = HEARTBEAT_END;
              }
             }
             else
             {
              eHeartbeatState = HEARTBEAT_INIT;
             }
            break;
  
        case HEARTBEAT_END:
  
            eHeartbeatState = HEARTBEAT_INIT;
  
            break;
        }
    
}



void setup() { 
 Serial.begin(57600);
 //Serial.begin(115200);
  //Water Pump: 4
  //Heater: 5
  //Light 6
  //Fan 7
  int BASE = 4;
  int NUM = 4;
 for (int i = BASE; i < BASE + NUM; i ++) 
 {
   pinMode(i, OUTPUT);   //Set the number I/O port to outputs
 }
 pinMode(12, OUTPUT);
} 

 



void loop() { 

    static unsigned long PreviousTime = 0;
    unsigned long CurrentTime;
    static int moistureVal = 0;
    static int tempVal = 0;
    static int CO2Val = 0;
    static bool fOperatingMode;
    moistureVal = findMoisture() ;
    tempVal = findTemp() ;
    CO2Val = findCO2() ;
    CurrentTime = millis();
    /*
    if (Serial.available()) {
          String stringMsg = Serial.readString();
    //      Stream msg = Serial.readByteUntil();
          Serial.print("Input String: \t");
          Serial.println(stringMsg);
          
    } else {
          Serial.println("No message from python");  
    
    }
    */
    
   /* 
    if (Serial.available() > 0) {
      // read the incoming bytes:
      int rlen = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
      // prints the received data
      //Serial.print("Input: ");
      for(int i = 0; i < rlen; i++) {
      //Serial.print(buf[i]);
      }
    } else {
      //Serial.println("no message from python");  
    }
  
   char *strings[BUFFER_SIZE];
   char *ptr = NULL; 
   int index = 0;
   ptr = strtok(buf, ";");
   while (ptr != NULL) {
      strings[index] = ptr;
      index++;
      ptr = strtok(NULL,";");
  }
  
  for (int n = 0; n < index; n++)
  {
     Serial.print("order: ");
     Serial.print(n);
     Serial.print("\t");
     Serial.println(strings[n]);
  } 
  */


  
  //delay(10);


    
//    Serial.println("Moisture: ");
//    Serial.println(moistureVal);
//    Serial.println("\t");
//    Serial.println("Temperature: "); 
//    Serial.println(tempVal);
//    Serial.println("\t");
//    Serial.println("CO2 Level: ");
//    Serial.println(CO2Val);
//    Serial.println("\n"); 
    
    Serial.println(moistureVal + String(",") + tempVal + String(",") + CO2Val);
//    Serial.print("return");
//    Serial.println(type);
 
 
    
  char type = readMsgFromPi();
  //  bool flag = lastMsg();

  if (type == 'S')
  {
    digitalWrite(4,fan);
    delay(10);
    digitalWrite(5,waterPump);
    delay(10);
    digitalWrite(6,lightLamp);
    delay(10);
    digitalWrite(7,heater);
    delay(10);
  }
  if (type == 'S')
  {
    PreviousTime = CurrentTime;
  }

  if ((type == 'N') && ((CurrentTime - PreviousTime) >= ONE_MINUTE))
  {
    fOperatingMode = true;
  }
  else
  {
    fOperatingMode = false;
  }
    System_Heartbeat(fOperatingMode);
  /*
    Serial.print("waterPumpVal \t");
    Serial.print(waterPump);
    Serial.print("\tlightVal \t");
    Serial.print(lightLamp);
    Serial.print("\t heater \t");
    Serial.print(heater);
    Serial.print("\t fan \t");
    Serial.println(fan);
  */
         
  //  delay(5000);

//light 
//fan works (pin4)
//S,1,0,1,0
//S,1,0,0,0;S,1,0,1,0;S,1,1,1,0
}
