// 12 Mar 2014
// this works with ComArduino.py and ComArduinoA4e.rb
// this version uses a start marker 254 and an end marker of 255
//  it uses 253 as a special byte to be able to reproduce 253, 254 and 255
// it also sends data to the PC using the same system
//   if the number of bytes is 0 the PC will assume a debug string and just print it to the screen

//================
#include <avr/wdt.h> // Watchdog timer
#include <avr/io.h> 

#define startMarker 254
#define endMarker 255
#define specialByte 253
#define maxMessage 50
#define BIT(n,i) (n>>i&1)

// the program could be rewritten to use local variables instead of some of these globals
//  however globals make the code simpler
//  and simplify memory management

byte bytesRecvd = 0;
byte dataSentNum = 0; // the transmitted value of the number of bytes in the package i.e. the 2nd byte received
byte dataRecvCount = 0;


char dataRecvd[maxMessage]; 
char dataSend[maxMessage];  
char tempBuffer[maxMessage];

byte dataSendCount = 0; // the number of 'real' bytes to be sent to the PC
byte dataTotalSend = 0; // the number of bytes to send to PC taking account of encoded bytes

boolean inProgress = false;
boolean startFound = false;
boolean allReceived = false;
boolean memReceived = false;
char debugMsg[20] = "Arduino Reset";
int memAddress;
String dataSendStr;
int changedArr[16];
float y0 = sqrt(2) - 1;
float a0 = 2*pow((y0), 2);
int n_iter = 10; 

int loopCounter = 0;

byte testVar = 1000;

char houseKeep[] = "Frederic";
char houseKeep1[] = "Jochim";
char houseKeep2[] = "Bas";
char houseKeep3[] = "Alexander";

//================

void setup() {
  pinMode(13, OUTPUT); // the onboard LED
  Serial.begin(9600);
  debugToPC(debugMsg);
  char buff[16];
  sprintf(buff, "V %d", &testVar); 
  debugToPC(buff);
  delay(50);
  //blinkLED(10); // just so we know it's alive
  //watchdogSetup();
}

//================

void loop() {

  getSerialData();
  //char buff1[50];
  //sprintf(buff1, "TEST %d, %d", testVar, memAddress); 
  
  //debugToPC(buff1);
  
  processData(dataRecvd);
  //dataToPC(dataRecvd);
  //checkIfMemAddress(tempBuffer);
  delay(200);
  float pi;
  pi = getBorweinPi(y0, a0, n_iter);
  char result[16]; // Buffer big enough for 7-character float
  dtostrf(pi, 1, 6, result); //float goes up to 8 bytes in total (the comma is also a byte) 
  sendHouseKeep(result);

  loopCounter ++;
  //bitFlip(276);
  delay(500);
  
}

//================
void watchdogSetup(void)
{
  cli();

  //wdt_reset();
/*
 * WDTCSR configuration:
 * WDIE = 1 : Interrupt Enable
 * WDE  = 1 : Reset Enable
 * See table for time-out variations:
 * WDP3 = 0 : For 1000ms Time-out
 * WDP2 = 1 : For 1000ms Time-out
 * WDP3 = 1 : For 1000ms Time-out
 * WDP3 = 0 : For 1000ms Time-out
 */

 // Enter Watchdog Configuration Mode
 WDTCSR |= (1<<WDCE)|(1<<WDE);
 // Set Watchdog Settings:
 WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0);

 sei();
}
  

void getSerialData() {

     // Receives data into tempBuffer[]
     //   saves the number of bytes that the PC said it sent - which will be in tempBuffer[1]
     //   uses decodeHighBytes() to copy data from tempBuffer to dataRecvd[]
     
     // the Arduino program will use the data it finds in dataRecvd[]

  if(Serial.available() > 0) {

    byte x = Serial.read();
    if (x == startMarker) { 
      bytesRecvd = 0; 
      inProgress = true;
      // blinkLED(2);
      // debugToPC("start received");
    }
      
    if(inProgress) {
      tempBuffer[bytesRecvd] = x;
      bytesRecvd ++;
    }

    if (x == endMarker) {
      inProgress = false;
      allReceived = true;
      
      
      dataSentNum = tempBuffer[1];
      char buff2[32];
      sprintf(buff2, "NumBytes = %d", dataSentNum); 
      dataToPC(buff2);
      decodeHighBytes(tempBuffer);
      //checkIfMemAddress();
      //sprintf(dataRecvd, "MA0276x");
      
    }
  }
}

//============================

void processData(char arr[]) {
  
    // processes the data that is in dataRecvd[]
  if (allReceived) {
    checkIfMemAddress(arr);
    //pingFromPC(arr);
    if (memReceived) {
      //blinkLED(10);
      dataSendStr = "Mem Rcvd";
      bitFlip(memAddress);
      //blinkLED(1);
    }
    else {
      dataSendStr = "Arduino working";
    }
    dataSendStr.toCharArray(dataSend, maxMessage);
    
    dataToPC(dataSend);
    //wdt_reset();
    delay(100);
    allReceived = false; 
    
  }
}

//============================

void decodeHighBytes(char arr[]) {

  //  copies to dataRecvd[] only the data bytes i.e. excluding the marker bytes and the count byte
  //  and converts any bytes of 253 etc into the intended numbers
  //  Note that bytesRecvd is the total of all the bytes including the markers
  dataRecvCount = 0;
  for (int n = 2; n < dataSentNum - 1 ; n++) { // 2 skips the start marker and the count byte, -1 omits the end marker
    byte x = arr[n];
    if (x == specialByte) {
       // debugToPC("FoundSpecialByte");
       n++;
       x = x + arr[n];
    }
    dataRecvd[dataRecvCount] = x;
    dataRecvCount ++;
  }
  
}

//====================

void dataToPC(char arr[]) {
    
      // expects to find data in dataSend[]
      //   uses encodeHighBytes() to copy data to tempBuffer
      //   sends data to PC from tempBuffer
    encodeHighBytes(arr);
    Serial.write(startMarker);
    Serial.write(dataTotalSend);
    Serial.write(arr);
    Serial.write(endMarker);
    // blinkLED(1);
    
}

//============================

void encodeHighBytes(char arr[]) {
  // Copies to temBuffer[] all of the data in dataSend[]
  //  and converts any bytes of 253 or more into a pair of bytes, 253 0, 253 1 or 253 2 as appropriate
  int messageLength = sizeof(arr)/sizeof(char);
  dataTotalSend = 0;
  for (int n = 0; n < messageLength; n++) {
    if (dataSend[n] >= specialByte) {
      arr[dataTotalSend] = specialByte;
      dataTotalSend++;
      arr[dataTotalSend] = arr[n] - specialByte;
    }
    else {
      arr[dataTotalSend] = arr[n];
    }
    dataTotalSend++;
  }
}

//=========================

void debugToPC( char arr[]) {
    byte nb = 0;
    Serial.write(startMarker);
    Serial.write(nb);
    Serial.print(arr);
    Serial.write(endMarker);
}

//=========================

void debugToPC( byte num) {
    byte nb = 0;
    Serial.write(startMarker);
    Serial.write(nb);
    Serial.print(num);
    Serial.write(endMarker);
}

//=========================

void blinkLED(byte numBlinks) {
    for (byte n = 0; n < numBlinks; n ++) {
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
    }
}

float getBorweinPi(float y0, float a0, int n_iter)
{
    int count = 0;
    float y = y0;
    float a = a0;
    while(count<n_iter)
    {
        y = (1-pow(1-pow(y,4),0.25))/(1+pow(1-pow(y,4),0.25));
        a = a*pow(1+y,4)-pow(2,(2*count+3))*y*(1+y+pow(y,2));
        count++;
    }
    return 1/a;
}

void bitFlip(int nearAddress)
{
  byte *temp;
  temp = (byte*)nearAddress;
  byte toChange = *temp;
  
  int randI = random(0,8);

  *temp = toChange ^ ((1<<randI));
  //blinkLED(randI);
  //blinkLED(2);

}  


void checkIfMemAddress(char arr[]){
  int arrLength = 4;
  //Serial.println(arrLength);
  char memAddressCh[arrLength];
  if ((arr[0] == 'M') && (arr[1] == 'A')){
    blinkLED(1);
    byte n = 2;
    boolean notDone = true;
    while(notDone) {
      if (arr[n] != 'x'){
        memAddressCh[n-2] = arr[n];
        n ++;
      }
      else {
        notDone = false;
        }
      //blinkLED(arr[n]);
      //delay(1000);
      }
    //Serial.println(memAddressCh);
    //memAddressCh[-1] = NULL;
    memReceived = true;
    memAddress = atoi(memAddressCh);
    //blinkLED(5);
    char buff4[32];
    sprintf(buff4, "memAdd = %d", memAddress);
    dataToPC(buff4);
    }
  else {
    memReceived = false;
 
    }
}

void sendHouseKeep(char result[]){
  //dataToPC(houseKeep);
  //dataToPC(houseKeep1);
  //dataToPC(houseKeep2);
  //dataToPC(houseKeep3);
  char buff[16];
  sprintf(buff, "V %d", testVar);
  dataToPC(buff);
  
  //time_t Time = now();
  
  //houseKeep = String(weekday(Time));
  //dataToPC(houseKeep);
  //wdt_reset();
  //dataToPC(result);
  }

void pingFromPC(char arr[]){
  if ((arr[0] == 107) && (arr[1] == 105) && (arr[2] == 99) && (arr[3] == 107)){
    //blinkLED(1);
    //wdt_reset();
    }
  }

//void bin2dec(int arr[]){
//  int arraySize = sizeof(arr)/sizeof(int);
//  word dec = 0;
//  for (i = 0; i<arraySize; i ++) {
//    
//    }
//    
//}


