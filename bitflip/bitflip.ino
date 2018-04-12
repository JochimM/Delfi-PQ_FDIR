#define BIT(n,i) (n>>i&1)
char changedArr[16];
uint16_t *location;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
  uint16_t variable = 1000;
  location = &variable;
  bitFlip(location);
  
}

void bitFlip(uint16_t *nearAddress)
{
  uint16_t toChange = *nearAddress;

  Serial.println(toChange);

  int randI = random(0,16);
  
  for (int i = 0; i < 16; i ++) {
    // Serial.println(randI);
    changedArr[i] = BIT(toChange, i);
    }
  if (changedArr[randI] == 1){
  changedArr[randI] = 0;}
  if (changedArr[randI] == 0){
  changedArr[randI] = 1;}
  
  Serial.println(changedArr);
  delay(1000);
}
