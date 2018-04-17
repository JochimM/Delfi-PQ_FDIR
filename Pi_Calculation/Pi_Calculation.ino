float y0 = sqrt(2) - 1;
float a0 = 2*pow(y0,2);
int n_iter = 10; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Starting up...");

}

void loop() {
  // put your main code here, to run repeatedly:
  float pi;
  pi = getBorweinPi(y0, a0, n_iter);
  char result[16]; // Buffer big enough for 7-character float
  dtostrf(pi, 1, 14, result); //float goes up to 8 bytes in total (the comma is also a byte) 
  Serial.println(result);

  delay(1000);
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
