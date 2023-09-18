#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <M5Stack.h>
#include <HTTPClient.h>
const int mazeWidth = 17;
const int mazeHeight = 13;
const char* ssid = "Kid";
const char* password = "123456789";
int playerX = 1;
int playerY = 1;
float smoothedGyroX = 0.0;
float smoothedGyroY = 0.0;
const float smoothingFactor = 0.9; // Adjust this value for desired smoothing
IPAddress local_IP(192, 168, 4,1);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1,1);

IPAddress subnet(255, 255, 0, 0);
AsyncWebServer server(80);
struct Data {
  int radius;
  float BallpositionX;
  float BallpositionY;
  int ballcoloR;
  bool foundgoal;
  int mapindex;
};
Data sendData;
Data sendData2;
struct Data getsData = {9, 0.0, 0.0, TFT_GREEN, false};
int maze[3][mazeHeight][mazeWidth] = {{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1},
    {1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
},
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 2, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
},  
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 2, 0, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  }    
};

void change_map()
{
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.fillCircle(getsData.BallpositionX, getsData.BallpositionY, getsData.radius,TFT_BLACK);
  getsData.BallpositionX = 0;
  getsData.BallpositionY = 0;
  M5.Lcd.fillCircle(getsData.BallpositionX, getsData.BallpositionY, getsData.radius,getsData.ballcoloR);
  getsData.mapindex  += 1;
  getsData.foundgoal = false;
}
void setup() {
  M5.begin();
  M5.Lcd.begin();
  M5.IMU.Init();
  WiFi.softAP(ssid, password);
  Serial.println(WiFi.localIP());
  // Route for getting sensor data
  server.on("/kid", HTTP_GET, [](AsyncWebServerRequest *request){
    
    // Create a JSON object and populate it with the data
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["radius"] = getsData.radius;
    jsonDoc["BallpositionX"] = getsData.BallpositionX;
    jsonDoc["BallpositionY"] = getsData.BallpositionY;
    jsonDoc["ballcoloR"] = getsData.ballcoloR;
    jsonDoc["foundgoal"] = getsData.foundgoal;
    jsonDoc["mapindex"] = getsData.mapindex;
    // Serialize the JSON data into a string
    String jsonString;
    serializeJson(jsonDoc, jsonString);

    request->send(200, "application/json", jsonString);
  });


  // Start the server
  server.begin();
}

void drawPlayer() {
  int playerRadius = 9; 
  int playerCenterX = playerX * 18 + 9; 
  int playerCenterY = playerY * 18 + 9; 
  getsData.radius =9;
  getsData.BallpositionX = (float)playerCenterX;
  getsData.BallpositionY = (float)playerCenterY;
  getsData.ballcoloR = TFT_YELLOW;
  M5.Lcd.fillCircle(getsData.BallpositionX, getsData.BallpositionY, getsData.radius,getsData.ballcoloR);
}

void drawMaze() {
    for (int y = 0; y < mazeHeight; y++) {
     for (int x = 0; x < mazeWidth; x++) {
      if (maze[getsData.mapindex %3][y][x] == 1) {
        M5.Lcd.fillRect(x * 18, y * 18, 18, 18, TFT_BLUE);
      } else if (maze[getsData.mapindex%3][y][x] == 2) { // Check for the end point (2)
        M5.Lcd.fillRect(x * 18, y * 18, 18, 18, TFT_RED);
      }
    }
  }
}

void updatePlayerPosition() {
  // Read the gyroscope values
  float gyroX, gyroY, gyroZ;
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  // Apply smoothing to gyro inputs
  smoothedGyroX = smoothingFactor * smoothedGyroX + (1.0 - smoothingFactor) * gyroY;
  smoothedGyroY = smoothingFactor * smoothedGyroY + (1.0 - smoothingFactor) * gyroX;
  int playerRadius = 9; 
  int playerCenterX = playerX * 18 + 9; 
  int playerCenterY = playerY * 18 + 9; 
  if (getsData.foundgoal && sendData.foundgoal && sendData2.foundgoal)
     change_map();
  M5.Lcd.fillCircle(getsData.BallpositionX, getsData.BallpositionY, getsData.radius,TFT_BLACK);
  // Adjust player's position based on smoothed gyro values
  if (smoothedGyroX > 2.0 && !getsData.foundgoal) { // Move right
    if (maze[getsData.mapindex%3][playerY][playerX + 1] != 1) {
      if(maze[getsData.mapindex%3][playerY][playerX + 1] == 2)
        getsData.foundgoal = true;
          
      playerX++;
    }
  } else if (smoothedGyroX < -2.0 && !getsData.foundgoal) { // Move left
    if (maze[getsData.mapindex%3][playerY][playerX - 1] != 1) {
      if(maze[getsData.mapindex%3][playerY][playerX - 1] == 2)
        getsData.foundgoal = true;
      playerX--;
    }
  }
  if (smoothedGyroY > 2.0 && !getsData.foundgoal) { // Move down
    if (maze[getsData.mapindex%3][playerY + 1][playerX] != 1) {
       if(maze[getsData.mapindex%3][playerY + 1][playerX] == 2)
          getsData.foundgoal = true;
      playerY++;
    }
  } else if (smoothedGyroY < -2.0 && !getsData.foundgoal) { // Move up
    if (maze[getsData.mapindex%3][playerY - 1][playerX] != 1) {
      if(maze[getsData.mapindex%3][playerY - 1][playerX] == 2)
        getsData.foundgoal = true;
      playerY--;
    }
  }
}

void loop() {
  M5.update();
  updatePlayerPosition();
  drawMaze();
  drawPlayer();
   HTTPClient http;
  M5.Lcd.fillCircle(sendData.BallpositionX,sendData.BallpositionY,sendData.radius,TFT_BLACK);
  M5.Lcd.fillCircle(sendData2.BallpositionX,sendData2.BallpositionY,sendData2.radius,TFT_BLACK);
    // Send an HTTP GET request to the transmitter's IP address to retrieve data
    http.begin("http://192.168.4.2/namning"); // Use the IP address of the transmitter (AP)

    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      String payload = http.getString();

      // Deserialize the JSON data back into a struct
      StaticJsonDocument<200> jsonDoc;
      deserializeJson(jsonDoc, payload);

       sendData.radius = jsonDoc["radius"];
       sendData.BallpositionX = jsonDoc["BallpositionX"];
       sendData.BallpositionY = jsonDoc["BallpositionY"];
       sendData.ballcoloR = jsonDoc["ballcoloR"];
       sendData.foundgoal = jsonDoc["foundgoal"];
       sendData.mapindex = jsonDoc["mapindex"];
    }
   

    // Send an HTTP GET request to the transmitter's IP address to retrieve data
    http.begin("http://192.168.4.3/nin"); // Use the IP address of the transmitter (AP)

     httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      String payload = http.getString();

      // Deserialize the JSON data back into a struct
      StaticJsonDocument<200> jsonDoc;
      deserializeJson(jsonDoc, payload);

       sendData2.radius = jsonDoc["radius"];
       sendData2.BallpositionX = jsonDoc["BallpositionX"];
       sendData2.BallpositionY = jsonDoc["BallpositionY"];
       sendData2.ballcoloR = jsonDoc["ballcoloR"];
       sendData2.foundgoal = jsonDoc["foundgoal"];
       sendData2.mapindex = jsonDoc["mapindex"];
    }
    http.end();
  M5.Lcd.fillCircle(sendData.BallpositionX,sendData.BallpositionY,sendData.radius,sendData.ballcoloR);
  M5.Lcd.fillCircle(sendData2.BallpositionX,sendData2.BallpositionY,sendData2.radius,sendData2.ballcoloR);
  if (getsData.mapindex != sendData.mapindex || getsData.mapindex != sendData2.mapindex)
    {
      M5.Lcd.fillScreen(TFT_BLACK);
      getsData.foundgoal = false;
      getsData.mapindex = max(getsData.mapindex,sendData.mapindex);
      getsData.mapindex = max(getsData.mapindex,sendData2.mapindex);
    }
  delay(100); // Adjust this delay to control the ball's speed
}
