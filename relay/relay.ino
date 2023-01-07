#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); 

#define RELAY_PIN D5 // Relay nối với Pin D5


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);

  
  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn
  lcd.setCursor(0,0);  //Con trở ở vị trí 0 hàng 1, hiện ô 1
  lcd.print("Trang thai:");
}

void loop() {
  /// Bật 
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("ON");
  lcd.setCursor(12,0); //Con trở ở vị trí 12, hiện ô 13
  lcd.print("ON ");
  delay(5000);  /// Nghỉ 5s

  /// Tắt
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("OFF");
  lcd.setCursor(12,0); //Con trở ở vị trí 12, hiện ô 13
  lcd.print("OFF");  
  delay(5000);  /// Nghỉ 5s
}
