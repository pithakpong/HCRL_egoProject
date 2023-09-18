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
IPAddress local_IP(192, 168, 4,3);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4,4);

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


//Website

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <html>
<head>
    <style>
      body {
            display: flex;
            justify-content: center;
            align-items: start;
            height: 100vh; /* This ensures the content is centered vertically. */
            margin: auto;
        }
     .maze {
            display: inline-block;
            margin: auto;
           
        }

        .maze-row {
            display: flex;
        }

        .maze-cell {
            width: 50px;
            height: 50px;
         
            background-color: black; /* Change the background color as needed */
        }

        .wall {
            background-color: blue; /* Change the wall color as needed */
        }

        .start {
            background-color: green; /* Change the start color as needed */
        }

        .end {
            background-color: red; /* Change the end color as needed */
        }
          .player {
             position: absolute;
            width: 40px;
            height: 40px;
          
            border-radius: 50%;
            
            transform: translate(10%, 10%);
            
        }
        .mazePos{
            background-image: linear-gradient(rgb(128, 23, 41), purple);
            position: relative;
            height: 716px;
            width: 893px;
            justify-content: center;
            align-items: center;
            margin-top: auto;
            border-bottom: 20px solid rgba(4, 4, 4, 0.182); 
            border-right: 20px solid rgba(0, 0, 0, 0.132); 
            box-shadow: 4ex 4ex 0px rgba(0, 0, 0, 0.207);
        } 
#player1{
  background-color: yellow;
  }
  #player2{
  background-color: white;
  }
  #player3{
  background-color: green;
  }
        /* CSS styles as before... */
    </style>
</head>
<body>
  
    <div class="mazePos">
        <div class="player" id="player1"></div>
    <div class="player" id="player2"></div>
    <div class="player" id="player3"></div>
    <div class="maze"  id="maze1"></div>
    <div class="maze" id="maze2"></div>
    <div class="maze" id="maze3"></div>
</div>
  
 
    <script>
   
    const thresholdValue = 50;
  function fetchData() {
  fetch('/weballposition')
    .then(response => response.json())
    .then(data => {
    var playerPositions = data;
    // Extract X and Y positions for "Nin" (adjust property names as needed)
    var xPositionNin = playerPositions.BallpositionXNin;
    var yPositionNin = playerPositions.BallpositionYNin;
    var xPositionKid = playerPositions.BallpositionXKid;
    var yPositionKid = playerPositions.BallpositionYKid;
    var xPositionNamning = playerPositions.BallpositionXNamning;
    var yPositionNamning = playerPositions.BallpositionYNamning;
     var mapindex = playerPositions.IndexMaze;
     renderMaze(mazes[mapindex %3],"maze1");
    console.log(mapindex);
    players[1].x = (xPositionNin - 9)/18;
    players[1].y = (yPositionNin - 9)/18;
    players[0].x = (xPositionKid - 9)/18;
    players[0].y = (yPositionKid - 9)/18;
    players[2].x = (xPositionNamning - 9)/18;
    players[2].y = (yPositionNamning - 9)/18;
//    console.log(xPositionNin+" "+yPositionNin);
    for (let i = 0; i < players.length; i++) {
                const player = players[i];
                const playerElement = playerElements[i];
                playerElement.style.left = player.x * 50 + "px";
                playerElement.style.top = player.y * 50 + "px";
    }
//    // Determine the movement based on X and Y positions
//    // For example, if xPositionNin is greater than a certain threshold, move left:
//    if (xPositionNin > thresholdValue) {
//      movePlayer(currentLevel, 1, 1, 0); // Move left
//    }
//      // Extract positions for other players similarly
//
//      // Now you can use player1X, player1Y, etc., to control player movement
//      // Call movePlayer with the appropriate arguments based on the data
//      
    });
}
setInterval(fetchData, 100);

      const mazes = [
    // Maze 1
    [
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1],
        [1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1],
        [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1],
        [1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1],
        [1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1],
        [1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1],
        [1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1],
        [1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1],
        [1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1],
        [1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1],
        [1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    ],
    // Maze 2
    [
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 2, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1],
        [1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1],
        [1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1],
        [1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1],
        [1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1],
        [1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1],
        [1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1],
        [1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1],
        [1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1],
        [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
        [1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    ],
    // Maze 3
    [
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
        [1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1],
        [1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1],
        [1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1],
        [1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1],
        [1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 2, 0, 1, 1],
        [1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1],
        [1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1],
        [1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1],
        [1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1],
        [1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    ]
];

 
        // Function to render a maze
        function renderMaze(mazeData, containerId) {
            const container = document.getElementById(containerId);
            while (container.firstChild) {
        container.removeChild(container.firstChild);
    }

            mazeData.forEach(rowData => {
                const row = document.createElement("div");
                row.className = "maze-row";
                rowData.forEach(cellData => {
                    const cell = document.createElement("div");
                    cell.className = "maze-cell";
                    if (cellData === 1) {
                        cell.classList.add("wall");
                    } else if (cellData === 2) {
                        cell.classList.add("end");
                    }
                    row.appendChild(cell);
                });
                container.appendChild(row);
            });
        }
        var players = [
            { x: 1, y: 1 },
            { x: 1, y: 1 },
            { x: 1, y: 1 }
        ];

        // Player elements
       const playerElements = [
            document.getElementById("player1"),
            document.getElementById("player2"),
            document.getElementById("player3")
        ];

        // Function to render the players
        function renderPlayers() {
            hasAnyPlayerReachedFinish = players.every(player => {
        return mazes[currentLevel][player.y][player.x] === 2;
    });

   
            for (let i = 0; i < players.length; i++) {
                const player = players[i];
                const playerElement = playerElements[i];
                playerElement.style.left = player.x * 50 + "px";
                playerElement.style.top = player.y * 50 + "px";
            }
        }

        // Function to handle player movement
//        function movePlayer(mazenum,playerIndex,) {
//            const player = players[playerIndex];
//            const newX = player.x + dx;
//            const newY = player.y + dy;
//            // player1X
//            // Check if the new position is valid (not a wall)
//            if (mazes[mazenum][newY][newX] !== 1) {
//                player.x = newX;
//                player.y = newY;
//                renderPlayers();
//            }
//        }

     /*   // Function to handle keyboard input
        document.addEventListener("keydown", function (event) {
            switch (event.key) {
                // Player 1 controls (Arrow keys)
                case "ArrowUp":
                    movePlayer(currentLevel,0, 0, -1);
                    break;
                case "ArrowDown":
                    movePlayer(currentLevel,0, 0, 1);
                    break;
                case "ArrowLeft":
                    movePlayer(currentLevel,0, -1, 0);
                    break;
                case "ArrowRight":
                    movePlayer(currentLevel,0, 1, 0);
                    break;

                // Player 2 controls (WASD keys)
                case "w":
                    movePlayer(currentLevel,1, 0, -1);
                    break;
                case "s":
                    movePlayer(currentLevel,1, 0, 1);
                    break;
                case "a":
                    movePlayer(currentLevel,1, -1, 0);
                    break;
                case "d":
                    movePlayer(currentLevel,1, 1, 0);
                    break;

                // Player 3 controls (JKLI keys)
                case "i":
                    movePlayer(currentLevel,2, 0, -1);
                    break;
                case "k":
                    movePlayer(currentLevel,2, 0, 1);
                    break;
                case "j":
                    movePlayer(currentLevel,2, -1, 0);
                    break;
                case "l":
                    movePlayer(currentLevel,2, 1, 0);
                    break;
            }
        });*/

        // Initial rendering of players and maze
        renderPlayers();
        // Render the first maze level (maze1) by default
       
    </script>
</body>
</html>


)rawliteral"; 




//M5
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

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
      WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
        
    });

    
  // Route for getting sensor data
  server.on("/nin", HTTP_GET, [](AsyncWebServerRequest *request){
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
    
server.on("/weballposition", HTTP_GET, [](AsyncWebServerRequest *request){
    // Create a JSON object and populate it with the data
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["BallpositionXNin"] = getsData.BallpositionX;
    jsonDoc["BallpositionYNin"] = getsData.BallpositionY;
    jsonDoc["BallpositionXKid"] = sendData.BallpositionX;
    jsonDoc["BallpositionYKid"] = sendData.BallpositionY;
    jsonDoc["BallpositionXNamning"] = sendData2.BallpositionX;
    jsonDoc["BallpositionYNamning"] = sendData2.BallpositionY;
     jsonDoc["IndexMaze"] =  sendData.mapindex;
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
  getsData.ballcoloR = TFT_WHITE;
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
    http.begin("http://192.168.4.1/kid"); // Use the IP address of the transmitter (AP)

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
    http.begin("http://192.168.4.2/namning"); // Use the IP address of the transmitter (AP)

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
      getsData.foundgoal = false;
      getsData.mapindex = max(getsData.mapindex,sendData.mapindex);
      getsData.mapindex = max(getsData.mapindex,sendData2.mapindex);
    }
  delay(100); // Adjust this delay to control the ball's speed
}
