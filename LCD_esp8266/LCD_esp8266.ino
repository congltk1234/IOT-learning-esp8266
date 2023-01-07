#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); 

void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);

  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn
}

void loop() {

  Serial.println("Dòng 1: BTH 2.1");
  //Con trở ở vị trí 0 hàng 1, hiện ô 1
  lcd.setCursor(0,0);
  lcd.print("BTH 2.1");

  Serial.println("Dòng 2: NHOM 1");
  //Con trở ở vị trí 0 hàng 2, hiện ô 1
  lcd.setCursor(0,1);
  lcd.print("NHOM 1");

  Serial.println("===================");
  delay(1000);
}
