#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("TFT Init...");
  
  tft.init();
  tft.setRotation(1); // 1 - альбомная, 0 - портретная
  
  Serial.println("Fill red...");
  tft.fillScreen(TFT_RED);
  delay(1000);
  
  Serial.println("Fill green...");
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  
  Serial.println("Fill blue...");
  tft.fillScreen(TFT_BLUE);
}

void loop() {
  // Ничего не делаем, экран должен просто светиться синим
}
