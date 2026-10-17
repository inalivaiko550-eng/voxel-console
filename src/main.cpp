#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// --- Настройки мира ---
#define MAP_SIZE 16
const char* map[] = {
  "################",
  "#..............#",
  "#..####........#",
  "#..............#",
  "#..............#",
  "#..............#",
  "#..............#",
  "################"
};

// --- Игрок ---
float playerX = 3.0;
float playerY = 3.0;
float playerA = 0.0;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
}

float castRay(float x, float y, float angle) {
  float sinA = sin(angle);
  float cosA = cos(angle);
  float dist = 0.0;
  
  while (dist < 16.0) {
    dist += 0.1;
    int mapX = int(x + cosA * dist);
    int mapY = int(y + sinA * dist);
    
    if (mapX < 0 || mapX >= MAP_SIZE || mapY < 0 || mapY >= MAP_SIZE) return dist;
    if (map[mapY][mapX] == '#') return dist;
  }
  return 16.0;
}

void loop() {
  tft.fillScreen(TFT_BLACK);
  
  // Рендер
  for (int x = 0; x < tft.width(); x++) {
    float rayAngle = playerA - (PI / 6.0) + (float)x / tft.width() * (PI / 3.0);
    float dist = castRay(playerX, playerY, rayAngle);
    
    int lineHeight = (int)(tft.height() / dist);
    int startY = (tft.height() - lineHeight) / 2;
    
    // Цвет стены зависит от расстояния (туман)
    uint16_t color = tft.color565(255 / (dist + 1), 255 / (dist + 1), 255 / (dist + 1));
    tft.drawFastVLine(x, startY, lineHeight, color);
  }
  
  // Простое движение по кругу для теста (потом заменишь на джойстик)
  playerA += 0.05;
  delay(10); 
}
