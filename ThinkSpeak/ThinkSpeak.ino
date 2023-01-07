#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); ///LCD config

#include <DHT.h>
/// DHT config
const int DHTPIN = D5;//Chân Out của cảm biến nối chân số 5 ESP8266
const int DHTTYPE = DHT22;   // Khai báo kiểu cảm biến là DHT21     
DHT dht(DHTPIN, DHTTYPE); //Khai báo thư viện chân cảm biến và kiểu cảm biến

/// WiFi Settings
#include <ESP8266WiFi.h>
const char* ssid = "stc";    // tên wifi
const char* password = "suthanhcon";  // mật khẩu wifi
WiFiClient client;

// ThingSpeak Settings
const int channelID = 1986959; //
String writeAPIKey = "DF3D7Z3WY634UCAA"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 5 * 1000; // post data every 5 seconds

void setup() 
{
  Serial.begin(115200);

  dht.begin(); //Khởi động cảm biến
  WiFiConnect(); // Kết nối với WiFi
  
  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn
  lcd.setCursor(0,0);
  lcd.print("DO AM:");
  lcd.setCursor(0,1);
  lcd.print("NHIET DO:");
}

void loop() 
{
  // wait and then post again
  delay(postingInterval);
  
  float humi = dht.readHumidity(); //Đọc độ ẩm
  float temp = dht.readTemperature(); //Đọc nhiệt độ C

  // Kiểm tra cảm biến có hoạt động hay không
  if (isnan(humi) || isnan(temp)) 
  {
    Serial.println("Không có giá trị trả về từ cảm biến DHT");
    return;
  }

  Serial.print("Độ ẩm: ");
  Serial.print(humi);
  lcd.setCursor(7,0); //con trỏ vị trí số 7 hàng 1, hiện ô số 8
  lcd.print(humi);
  lcd.setCursor(12,0); //Con trở ở vị trí 12, hiện ô 13
  lcd.print("%");
  
  Serial.print("%  Nhiệt độ: ");
  Serial.print(temp);
  Serial.println("°C");
  lcd.setCursor(9,1);  //Con trở ở vị trí 0 hàng 2, hiện ô 13
  lcd.print(temp);
  lcd.print("'C");

  if (client.connect(server, 80)) {
    // Construct API request body
    String body = "field1=" + String(temp, 1) + "&field2=" + String(humi, 1);
  
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body.length());
    client.print("\n\n");
    client.print(body);
    client.print("\n\n");
    Serial.println("Uploaded to Thinkspeak successfully!");
  }
  client.stop();
  Serial.println("====================");
}


void WiFiConnect(){
  WiFi.begin(ssid, password);
  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
