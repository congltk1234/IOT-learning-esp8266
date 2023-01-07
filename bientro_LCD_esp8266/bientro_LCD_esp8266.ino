#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); 

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

int sensorValue = 0;

void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);

  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn
  lcd.setCursor(0,0);
  lcd.print("Bien tro:");
}

void loop() {
  // Đọc giá trị Biến trở từ chân Analog A0
  sensorValue = analogRead(analogInPin);
 
  // Hiển thị kết quả trên serial
  Serial.print("sensor = ");
  Serial.println(sensorValue);

  lcd.setCursor(10,0); //Con trở ở vị trí 10, hiện ô 11
  lcd.print(sensorValue);
  
  delay(1000);
}
