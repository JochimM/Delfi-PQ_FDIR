uint16_t value = 1000;
uint16_t *location = &value;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600); 
  
  Serial.println("Starting up...");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(*location);
  Serial.println(*location,BIN);
  
  int randI = random(0,16);

  bitFlip(location);
}

void bitFlip(uint16_t *nearAddress)
{
  uint16_t toChange = *nearAddress;

  int randI = random(0,16);

  uint16_t Changed = toChange ^ ((1<<randI));

  Serial.println(Changed);
  Serial.println(Changed,BIN);
  delay(10000);
}  


