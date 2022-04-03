
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
#include <math.h>
//global values 
 

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



char readMsgFromPi() {
  //"D,temp low, temp high, hours of light, moisture low, moisture high"
  //"S,(0/1) for waterPump,(0/1) for heater, (0,1) for lightlamp, (0,1) for fan)
  //Msg types:
  //  D: Definion of plant -  when an user selects the type of a plant - coming from database
  //  S: Set up by User - when an user changes environment such as temp or light
  //  N: No msg available - no updates of Message  
    char msgType = 'N';
    if (Serial.available()){ 
          String stringMsg = Serial.readString();
          Serial.print("Input String: \t");
          Serial.println(stringMsg);
          
          //char msg[] = "D,100,200,200,8,20,40";
          char msg[] = "                           ";
          for (int i = 0; i < stringMsg.length(); i++) {
              msg[i] = stringMsg.charAt(i);
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



void setup() { 
 Serial.begin(57600);
 
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

 
const int BUFFER_SIZE = 100;
char buf[BUFFER_SIZE];



void loop() { 
    
    static int moistureVal = 0;
    static int tempVal = 0;
    static int CO2Val = 0;
    moistureVal = findMoisture() ;
    tempVal = findTemp() ;
    CO2Val = findCO2() ;
    
    if (Serial.available()) {
          String stringMsg = Serial.readString();
    //      Stream msg = Serial.readByteUntil();
          Serial.print("Input String: \t");
          Serial.println(stringMsg);
          
    } else {
          Serial.println("No message from python");  
    
    }
    
    /*
    if (Serial.available() > 0) {
      // read the incoming bytes:
      int rlen = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
      // prints the received data
      //Serial.print("Input: ");
      for(int i = 0; i < rlen; i++) {
      //  Serial.print(buf[i]);
      }
    } else {
        Serial.println("no message from python");  
    }
*/
  /*
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
  delay(10);


    
//    Serial.println("Moisture: ");
//    Serial.println(moistureVal);
//    Serial.println("\t");
//    Serial.println("Temperature: "); 
//    Serial.println(tempVal);
//    Serial.println("\t");
//    Serial.println("CO2 Level: ");
//    Serial.println(CO2Val);
//    Serial.println("\n"); 
//    char type = readMsgFromPi();
//    Serial.println(moistureVal + String(",") + tempVal + String(",") + CO2Val);
//    Serial.print("return");
//    Serial.println(type);
    
//int waterPump = 0;
//int heater = 0;
//int lightLamp = 0;
//int fan = 0;
/*    if ( waterPump == 1) {
       digitalWrite(4,waterPump)
    } else {
      
    }

    if ( heater == 1) {
        digitalWrite(5,heater);
    }

    if ( lightLamp == 1) {
        digitalWrite(6,lightLamp);
    }

    if ( fan == 1) {
        digitalWrite(7,fan);
    }
*/      
  /*
  digitalWrite(4,waterPump);
  delay(100);
  digitalWrite(5,heater);
  delay(100);
  digitalWrite(6,lightLamp);
  delay(100);
  digitalWrite(7,fan);
  delay(100);
*/
   // Serial.print("waterPumpVal \t");
   // Serial.println(waterPump);
   // Serial.print("lightVal \t");
   // Serial.println(lightLamp);
    

          
  //  delay(5000);

//S,1,0,1,0

    
}
