int ledPin1 = 16;                 // LED connected to digital pin 16
int ledPin2 = 2;

void TURN_ON_LED1(void)
{
  Serial.println("LED 1 ON / LED 2 OFF");
  digitalWrite(ledPin1, HIGH);   // sets the LED on
  digitalWrite(ledPin2, LOW);    // sets the LED off
}

void TURN_ON_LED2(void)
{
  Serial.println("LED 1 OFF / LED 2 ON");
  digitalWrite(ledPin2, HIGH);   // sets the LED on
  digitalWrite(ledPin1, LOW);    // sets the LED off
}


void setup()
{
  Serial.begin(115200);
  Serial.println("BTH1: ON/OFF LED");
  pinMode(ledPin1, OUTPUT);      // sets the digital pin as output
  pinMode(ledPin2, OUTPUT);   
}

void loop()
{
  TURN_ON_LED1();
  delay(3000);     // waits for 3s
  TURN_ON_LED2();
  delay(3000);                  // waits for 3s
}
