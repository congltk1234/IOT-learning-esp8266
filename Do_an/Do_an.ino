#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,19,20); ///LCD config
#include "DHT.h"
#include "ThingSpeak.h"

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

/// DHT config
#define DHTTYPE DHT22   // DHT 22  
#define DHTPIN D5 //Chân Out của cảm biến nối chân số 5 ESP8266
const int SOIL_MOIST_PIN = A0;

/// WiFi Settings
const char* ssid = "stc";    // tên wifi
const char* password = "suthanhcon";  // mật khẩu WiFi
WiFiClient client;

// ThingSpeak Settings WRITE
const int channelID = 1986959; // ThingSpeak Channel ID
String writeAPIKey = "T6OBX91XS0ZQZ1TU"; // ThingSpeak write API key
const char* server = "api.thingspeak.com";


//
int statusCode = 0;
int field[3] = {1,2,3};

// Khởi tạo các biến
float humi;
float temp;
float soil;

// Xác định số lượng mẫu quan sát (kích thước mảng).
// Con số này càng lớn, giá trị lọc ra sẽ càng mượt, nhưng sẽ phản hồi càng chậm. 
// Sử sử dụng giá trị này để xác định kích thước của mảng đọc.
const int SMOOTHING_WINDOW_SIZE = 10; // 10 giá trị trong mảng
int curReadIndex = 0;                // vị trí ban đầu trong mảng
/// Humidity moving AVG config
float humi_samples[SMOOTHING_WINDOW_SIZE];  // Giá trị Độ ẩm được lưu trong mảng humi_samples
float humi_sampleTotal = 0;                 // the running total 
float humi_sampleAvg = 0;                   // the average

/// Temperature moving AVG config
float temp_samples[SMOOTHING_WINDOW_SIZE];  // Giá trị Nhiệt độ được lưu trong mảng temp_samples
float temp_sampleTotal = 0;                 // the running total
float temp_sampleAvg = 0;                   // the average

/// Soil moisture moving AVG config
float soil_samples[SMOOTHING_WINDOW_SIZE];  // Giá trị Độ ẩm đất được lưu trong mảng soil_samples
float soil_sampleTotal = 0;                 // the running total
float soil_sampleAvg = 0;                   // the average


DHT dht(DHTPIN, DHTTYPE); //Khai báo thư viện chân cảm biến và kiểu cảm biến

void setup() {
  Serial.begin(9600);
  WiFiConnect(); // Kết nối với WiFi
  dht.begin(); //Khởi động cảm biến
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
  lcd.init(); //Khởi động LCD                    
  lcd.backlight(); //Mở đèn
  lcd.setCursor(0,0);
  lcd.print("Humi:");
  lcd.setCursor(0,1);
  lcd.print("Temp:");
  
  // Khởi tạo các giá trị trong mảng = 0:
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++) {
    humi_samples[i] = 0;
    temp_samples[i] = 0;
    soil_samples[i] = 0;
  }
}

void loop() {
  // Đọc tín hiệu từ các cảm biến
  readSensors();
  // Tổng từng biến sẽ trừ đi giá trị cũ:  
  humi_sampleTotal = humi_sampleTotal - humi_samples[curReadIndex];
  temp_sampleTotal = temp_sampleTotal - temp_samples[curReadIndex];
  soil_sampleTotal = soil_sampleTotal - soil_samples[curReadIndex];
  
  // Thêm mới giá trị đo được từ cảm biến theo vị trí hiện tại vào mảng
  humi_samples[curReadIndex] = humi;
  temp_samples[curReadIndex] = temp;
  soil_samples[curReadIndex] = soil;
  
  // Tổng lại các giá trị trong mảng
  humi_sampleTotal = humi_sampleTotal + humi_samples[curReadIndex];
  temp_sampleTotal = temp_sampleTotal + temp_samples[curReadIndex];
  soil_sampleTotal = soil_sampleTotal + soil_samples[curReadIndex];
  
  // Tính giá trị trung bình trượt (Moving AVG):
  humi_sampleAvg = humi_sampleTotal / SMOOTHING_WINDOW_SIZE;
  temp_sampleAvg = temp_sampleTotal / SMOOTHING_WINDOW_SIZE;
  soil_sampleAvg = soil_sampleTotal / SMOOTHING_WINDOW_SIZE;

  // Hiển thị giá trị raw values/smooth values trên màn hình Serial
  Serial.print("HumidityRaw:");  Serial.print(humi);  Serial.print(", ");  Serial.print("HumidotySmooth:");  Serial.print(humi_sampleAvg);  Serial.print(", ");
  Serial.print("TemperatureRaw:");  Serial.print(temp);  Serial.print(", ");  Serial.print("TemperatureSmooth:");  Serial.print(temp_sampleAvg);  Serial.print(", ");
  Serial.print("SoilMoistureRaw:");  Serial.print(soil);  Serial.print(", ");  Serial.print("SoilMoistureSmooth:");  Serial.println(soil_sampleAvg);

  // Cập nhật vị trí mới cho lần lặp tiếp theo
  curReadIndex = curReadIndex + 1;
  
  // Sau khi đến vị trí cuối mảng...
  if (client.connect(server, 80) && curReadIndex >= SMOOTHING_WINDOW_SIZE ) {
    // ...Đặt biến vị trí về lại vị trí đầu mảng và cập nhật dữ liệu lên ThingSpeak:
    curReadIndex = 0;

    // Construct API request body
    String body = "field1=" + String(humi_sampleAvg, 1) + "&field2=" + String(temp_sampleAvg, 1) + "&field3=" + String(soil_sampleAvg,1);

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
//    Serial.print("upload to ThingSpeak");
  }

  
  statusCode = ThingSpeak.readMultipleFields(1995973,"NXRPXC3ZXM31HQ8H");
  /// Nhận giá trị từ ThingSpeak sau khi xử lý và hiển thị Giá trị trung bình từng ngày trên LCD
  if(statusCode == 200)
    {
      // Fetch the stored data

      float dailyHumi = ThingSpeak.getFieldAsFloat(field[0]); // Field 1
      float dailyTemp = ThingSpeak.getFieldAsFloat(field[1]); // Field 2
      float dailySoil = ThingSpeak.getFieldAsFloat(field[2]); // Field 3
      // Hiển thị giá trị trên màn hình LCD
      lcd.setCursor(5,0); lcd.print(dailyHumi); //con trỏ vị trí số 5 hàng 1, hiện giá trị độ ẩm ô số 6
      lcd.setCursor(5,1); lcd.print(dailyTemp); //con trỏ vị trí số 5 hàng 2, hiện giá trị nhiệt độ ô số 6
  //  lcd.setCursor(5,0); lcd.print(humi_sampleAvg); //con trỏ vị trí số 5 hàng 1, hiện ô số 6
  }
  else{
      Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }
  client.stop();

  delay(100); // read samples at ~20Hz (once every 50ms)
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

int getSoilMoist()
{
  int anaValue = 0;
  anaValue = analogRead(SOIL_MOIST_PIN); //Đọc giá trị cảm biến độ ẩm đất
  anaValue = map(anaValue, 1023, 0, 0, 100); //Ít nước:0%  ==> Nhiều nước 100%
  return anaValue;
}

void readSensors(void)
{
  temp = dht.readTemperature();   //Đọc nhiệt độ DHT22
  humi = dht.readHumidity();       //Đọc độ ẩm DHT22
  soil = getSoilMoist();        //Đọc cảm biến độ ẩm đất
}
