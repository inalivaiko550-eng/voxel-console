#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// --- Настройки мира ---
// Карта 16x16 (1 - стена, 0 - пусто)
const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;
const uint8_t worldMap[MAP_HEIGHT][MAP_WIDTH] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,0,0,0,1,1,1,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// --- Игрок ---
float playerX = 3.5;
float playerY = 3.5;
float playerAngle = 0.0;

// --- Пины джойстика ---
#define JOY_X 34
#define JOY_Y 35
#define JOY_BTN 27

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0); // Если картинка перевернута, поменяй на 1, 2 или 3
  tft.fillScreen(TFT_BLACK);
  
  pinMode(JOY_BTN, INPUT_PULLUP);
}

// Функция проверки, есть ли стена в координатах
bool isWall(int x, int y) {
  if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true;
  return worldMap[y][x] == 1;
}

// Алгоритм пускания лучей (Raycasting)
float castRay(float startX, float startY, float angle) {
  float rayDirX = cos(angle);
  float rayDirY = sin(angle);
  
  float dist = 0.0;
  while (dist < 20.0) {
    dist += 0.05; // Шаг луча
    int checkX = int(startX + rayDirX * dist);
    int checkY = int(startY + rayDirY * dist);
    
    if (isWall(checkX, checkY)) {
      return dist;
    }
  }
  return 20.0;
}

void loop() {
  // --- Управление (аналоговый джойстик) ---
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  // Поворот (влево/вправо)
  if (xVal < 1000) playerAngle -= 0.08;
  if (xVal > 3000) playerAngle += 0.08;

  // Движение (вперед/назад)
  float moveSpeed = 0.15;
  if (yVal < 1000) { // Вперед
    float newX = playerX + cos(playerAngle) * moveSpeed;
    float newY = playerY + sin(playerAngle) * moveSpeed;
    if (!isWall(int(newX), int(playerY))) playerX = newX;
    if (!isWall(int(playerX), int(newY))) playerY = newY;
  }
  if (yVal > 3000) { // Назад
    float newX = playerX - cos(playerAngle) * moveSpeed;
    float newY = playerY - sin(playerAngle) * moveSpeed;
    if (!isWall(int(newX), int(playerY))) playerX = newX;
    if (!isWall(int(playerX), int(newY))) playerY = newY;
  }

  // --- Рендеринг ---
  tft.fillScreen(TFT_BLACK); // Очистка экрана

  float fov = 60.0 * (PI / 180.0); // Угол обзора (60 градусов)
  int screenWidth = tft.width();
  int screenHeight = tft.height();

  // Проходим по каждому пикселю по ширине экрана
  for (int x = 0; x < screenWidth; x++) {
    // Угол текущего луча
    float rayAngle = playerAngle - (fov / 2.0) + ((float)x / screenWidth) * fov;
    
    // Пускаем луч
    float dist = castRay(playerX, playerY, rayAngle);
    
    // Высота стены обратно пропорциональна расстоянию
    int wallHeight = int(screenHeight / dist);
    int startY = (screenHeight / 2) - (wallHeight / 2);
    int endY = startY + wallHeight;

    // Защита от выхода за границы экрана
    if (startY < 0) startY = 0;
    if (endY > screenHeight) endY = screenHeight;

    // Цвет вокселя (делаем эффект тумана: чем дальше, тем темнее)
    int brightness = 255 - int(dist * 15);
    if (brightness < 0) brightness = 0;
    uint16_t color = tft.color565(brightness, brightness, brightness);

    // Рисуем вертикальную линию (это и есть 3D стена)
    tft.drawFastVLine(x, startY, endY - startY, color);
  }

  delay(10); // Небольшая задержка для стабильности
}
