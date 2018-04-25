boolean memReceived = false;
int memAddress;
char memAddressTry[] = "MA0276x";

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT); // the onboard LED
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkIfMemAddress(memAddressTry);
  Serial.println(memAddress);
  
  delay(1000);
}

void checkIfMemAddress(char arr[]){
  int arrLength = 4;
  //Serial.println(arrLength);
  char memAddressCh[arrLength];
  if ((arr[0] == 'M') && (arr[1] == 'A')){
    //blinkLED(1);
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
    //dataToPC(memAddressCh);
    }
  else {
    memReceived = false;
 
    }
}
