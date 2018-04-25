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

  bitFlip(location);
}

void bitFlip(word *nearAddress)
{
  word toChange = *nearAddress;

  // Choose which bit to switch. Note the count is from right to left.
  int randI = 6;

  word Changed = toChange ^ ((1<<randI));

  Serial.println(Changed);
  Serial.println(Changed,BIN);
  delay(10000);
}  


