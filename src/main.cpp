#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <pinout.h>
#include <ADIS16209.h>
#include <callbacks.h>
#include <screen_handler.h>

#include <battery.h>
#include <io.h>
#include <SPI.h>

IO laser = IO(NULL, LASER_INPUT);
IO trigger = IO(TRIGGER_OUTPUT, TRIGGER_INPUT);
IO flash = IO(FLASH_OUTPUT, NULL);
IO camera = IO(CAMERA_OUTPUT, NULL);

IO charging = IO(NULL ,CHARGING_INPUT);
IO battery_ready = IO(NULL, BREADY_INPUT);

Battery battery = Battery(BATTERY_INPUT);

ADIS16209 INCL= ADIS16209(IMU_CS, IMU_DR, IMU_RST);
ScreenHandler tft = ScreenHandler(TFT_CS, TFT_DC, TFT_RST);

int INCX = 0;
int INCY = 0;

BLEServer* server = NULL;

BLECharacteristic* input = NULL;
BLECharacteristic* batery = NULL;
BLECharacteristic* event = NULL;

long batery_timmer = millis();
long imu_timer = millis();

long trigger_timmer = 0;
long screen_timer = 0;

uint8_t state = 0;
uint8_t screen_state = 0;

int val = 0;

void setup() {
  Serial.begin(115200);
  
  tft.init(BLACK);
  INCL.configSPI();
  delay(100);                   // Give the part time to start up
  INCL.regWrite(MSC_CTRL, 0x6);  // Enable Data Ready on INCL
  delay(20);
  INCL.regWrite(AVG_CNT, 0x8);   // Set Digital Filter on INCL
  delay(20);
  INCL.regWrite(SMPL_PRD, 0x14), // Set Decimation on INCL
  delay(20);

  setIO();
  
  BLEDevice::init("ESP32");
  server = BLEDevice::createServer();

  BLEService *service = server->createService("b5d17f4c-47cb-4831-ba37-4a01b8ce044d");

  input = service->createCharacteristic(
    "d89b1864-6086-4af1-84c6-89a93eee6650",
    BLECharacteristic::PROPERTY_READ    |
    BLECharacteristic::PROPERTY_WRITE   |
    BLECharacteristic::PROPERTY_NOTIFY  |
    BLECharacteristic::PROPERTY_INDICATE
  );

  batery = service->createCharacteristic(
    "445c53a2-9db5-4d6f-87df-257e1547110b",
    BLECharacteristic::PROPERTY_READ    |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  event = service->createCharacteristic(
    "cc944d71-a083-479f-8095-62a784dbedfc",
    BLECharacteristic::PROPERTY_READ    |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  input->addDescriptor(new BLE2902());
  batery->addDescriptor(new BLE2902());
  event->addDescriptor(new BLE2902());
  
  input->setCallbacks(new InputCallback(&val));

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID("b5d17f4c-47cb-4831-ba37-4a01b8ce044d");
  advertising->setScanResponse(false);
  advertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  tft.splash();
  delay(3000);
  tft.splash(true);
  tft.initScreen();
  camera.change(true);
  delay(4000);
  camera.change(false);
  tft.initScreen(true);
  tft.base();

}

void loop() {
  // --- Battery update task --- 
  if(battery.readTimer() >= 1000){
    battery.updateTimer();
    battery.iterate();
    if(battery.isReady()){
      int value = battery.readBattery();
      char buffer[2];
      sprintf(buffer, "%02i", value);
      Serial.println(buffer);
      batery->setValue((uint8_t*)buffer, 2);
      batery->notify();
      tft.battery(value);
    }
  }

  // --- IMU read task ---
  if(millis() - imu_timer > 100){
    imu_timer = millis();
    INCL.configSPI();
    INCX = INCL.regRead(XINCL_OUT);
    INCY = INCL.regRead(YINCL_OUT);
    INCX = INCL.inclineScale(INCX);
    INCY = INCL.inclineScale(INCY);
    Serial.print("X: ");Serial.println(INCX);
    Serial.print("Y: ");Serial.println(INCY);
    Serial.println("--- --- ---");   
  }

  // --- Laser read Task ---
  if(laser.hasChanged()){
    Serial.println(laser.readInput());
    tft.laser(!laser.readInput());
  }

  // --- Camera Trigger ---
  if(trigger.readInput() && (state==0)){
    Serial.println("trigger input!");
    trigger.updateTimer();
    tft.updateTimer();
    state = 1;
    char imu_buffer[7];
    sprintf(imu_buffer,"%03d,%03d", INCX, INCY);
    event->setValue((uint8_t*)imu_buffer, 7);
    event->notify();
  }

  // --- Battery Charging ---
  if(charging.hasChanged() || battery_ready.hasChanged()){
    int charging_state = 0;
    if(battery_ready.readInput()){
      charging_state = 2;
    }
    else if(charging.readInput()){
      charging_state = 1;
    }
    tft.charging(charging_state);
  }

  // --- Battery Full ---
  
  if(state == 1){
    if((trigger.readTimer() >= 180) && (!trigger.isActive())){
      trigger.updateTimer();
      trigger.change(true);
      flash.updateTimer();
      state = 2;
    }
  }

  if(state == 2){
    if((flash.readTimer()>=16) && (!flash.isActive())){
      Serial.println("flash on!");
      flash.change(true);
      flash.updateTimer();
    }

    if((flash.readTimer()>=100) && (flash.isActive())){
      Serial.println("flash off!");
      flash.change(false);
      state = 3;
    }
  }

  if(state == 3){
      if((trigger.readTimer() >= 300) && (trigger.isActive())){
      trigger.change(false);
      state = 4;
    }
  }


  // --- Screen Handling ---

  if((state == 1) && (tft.getState()==0)){
    tft.updateTimer();
    tft.base(true);
    tft.imageTaken();
  }
  if((state == 4) && (tft.getState()==1) && (tft.readTimer()>=1500)){
    tft.updateTimer();
    tft.imageTaken(true);
    tft.imageTransfer();
  }
  if((state == 4) && (tft.getState()==2) && (tft.readTimer()>=4000)){
    tft.updateTimer();
    tft.imageTransfer(true);
    tft.base();
    state = 0;
  }
}
