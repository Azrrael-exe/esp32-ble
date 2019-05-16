#include <Arduino.h>
#include <ADIS16209.h>
#include <Adafruit_SSD1351.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

#include <pinout.h>

Adafruit_SSD1351 tft = Adafruit_SSD1351(128, 128, &SPI, TFT_CS, TFT_DC, TFT_RST);
ADIS16209 INCL(IMU_CS, IMU_DR, IMU_RST);

int INCX = 0;
int INCY = 0;
long imu_timer = millis();

void setup() {
  setIO();
  Serial.begin(115200);  
  tft.begin();
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  
  INCL.configSPI();
  delay(100);                   // Give the part time to start up
  INCL.regWrite(MSC_CTRL,0x6);  // Enable Data Ready on INCL
  delay(20);
  INCL.regWrite(AVG_CNT,0x8);   // Set Digital Filter on INCL
  delay(20);
  INCL.regWrite(SMPL_PRD,0x14), // Set Decimation on INCL
  delay(20);
}

void loop() {
    // --- IMU read task ---
  if(millis() - imu_timer > 100){
    imu_timer = millis();
    INCX = INCL.regRead(XINCL_OUT);
    INCY = INCL.regRead(YINCL_OUT);
    INCX = INCL.inclineScale(INCX);
    INCY = INCL.inclineScale(INCY);   
    Serial.print("X: ");Serial.println(INCX);
    Serial.print("Y: ");Serial.println(INCY);
    Serial.println("--- --- ---");
    
    tft.setCursor(0,0);
    tft.print(INCX);
    tft.setCursor(0,20);
    tft.print(INCY);
  }
}