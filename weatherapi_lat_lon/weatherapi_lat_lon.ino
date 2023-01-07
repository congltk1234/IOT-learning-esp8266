#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); 

const char* ssid = "stc";    // tên wifi
const char* password = "suthanhcon";  // mật khẩu wifi

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "fee6882e46a02ff030490637056c4fa0";

// Kinh độ, vĩ độ:
String lat = "16.458603360042282";
String lon = "107.58114075277862";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;

String jsonBuffer;

void setup() {
  Serial.begin(115200);
  
  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn

  WiFiConnect();
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      // String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?lat="+lat+"&lon="+lon+"&units=metric&appid=" + openWeatherMapApiKey;
      jsonBuffer = httpGETRequest(serverPath.c_str());

      JSONVar myObject = JSON.parse(jsonBuffer);
      
      Serial.print("Latitude = ");
      Serial.println(lat);
      Serial.print("Longitude = ");
      Serial.println(lon);

      
      Serial.print("City: ");
      String city = JSON.stringify(myObject["name"]);
      Serial.print(city); Serial.print(" - "); Serial.println(myObject["sys"]["country"]);
      lcd.setCursor(0,0); //con trỏ vị trí số 0 hàng 1, hiện ô số 1
      lcd.print("City: " + city);
      
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      lcd.setCursor(0,1); //con trỏ vị trí số 0 hàng 2, hiện ô số 1
      lcd.print("Temp: ");
      lcd.print(myObject["main"]["temp"]);
      lcd.print("C");
      
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);

      Serial.println("========================");
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
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


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  payload = http.getString();
  
  // Free resources
  http.end();

  return payload;
}
