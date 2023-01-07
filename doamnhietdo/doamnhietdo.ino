#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); 
#include <DHT.h>

const int DHTPIN = D5;//Chân Out của cảm biến nối chân số 5 ESP8266

const int DHTTYPE = DHT22;   // Khai báo kiểu cảm biến là DHT21     


DHT dht(DHTPIN, DHTTYPE); //Khai báo thư viện chân cảm biến và kiểu cảm biến

void setup() 
{
  Serial.begin(115200);

  dht.begin(); //Khởi động cảm biến

  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn
  lcd.setCursor(0,0);
  lcd.print("DO AM:");
}

void loop() 
{
  float doam = dht.readHumidity(); //Đọc độ ẩm

  float doC = dht.readTemperature(); //Đọc nhiệt độ C

  float doF = dht.readTemperature(true); //Đọc nhiệt độ F

  // Kiểm tra cảm biến có hoạt động hay không
  if (isnan(doam) || isnan(doC) || isnan(doF)) 
  {
    Serial.println("Không có giá trị trả về từ cảm biến DHT");
    return;
  }

  Serial.print("Độ ẩm: ");
  Serial.print(doam);
  lcd.setCursor(7,0); //con trỏ vị trí số 7 hàng 1, hiện ô số 8
  lcd.print(doam);
  lcd.setCursor(12,0); //Con trở ở vị trí 12, hiện ô 13
  lcd.print("%");
  
  Serial.print("%  Nhiệt độ: ");
  Serial.print(doC);
  Serial.print("°C | ");
  Serial.print(doF);
  Serial.println("°F");
  lcd.setCursor(0,1);  //Con trở ở vị trí 0 hàng 2, hiện ô 13
  lcd.print(doC);
  lcd.setCursor(5,1);
  lcd.print("'C-");
  lcd.print(doF);
  lcd.print("'F");

  delay(1000);
}
