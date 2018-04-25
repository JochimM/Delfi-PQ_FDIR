boolean memReceived = false;
int memAddress;
char memAddressTry[] = "MA0276x";

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT); // the onboard LED
  Serial.begin(9600);

  Serial.println("Starting up...");
}

void loop() {
  // put your main code here, to run repeatedly:
  checkIfMemAddress(memAddressTry);
  Serial.println(memAddress);
  
  delay(1000);
}

void checkIfMemAddress(char arr[]){
  int arrLength = 4;
  char memAddressCh[arrLength];
  if ((arr[0] == 'M') && (arr[1] == 'A')){
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
      }
    memReceived = true;
    memAddress = atoi(memAddressCh);
    }
  else {
    memReceived = false;
    }
}
