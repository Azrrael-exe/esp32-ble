#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <callbacks.h>

#include <ADIS16209.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#define TRIGGER_BUTTON 17
#define TRIGGER_SIGNAL 14
#define FLASH_SIGNAL 27
#define LASER_OUTPUT 26

#define CS_PIN   21
#define RST_PIN  15
#define DC_PIN   2

ADIS16209 INCL(16, 17, 4);
Adafruit_SSD1351 tft = Adafruit_SSD1351(21, 2, 15);

int INCX = 0;
int INCY = 0;

BLEServer* server = NULL;

BLECharacteristic* input = NULL;
BLECharacteristic* batery = NULL;
BLECharacteristic* event = NULL;

long batery_timmer = millis();
long imu_timer = millis();
long status_timer = millis();

long trigger_timmer = 0;
long screen_timer = 0;

uint8_t state = 0;
uint8_t screen_state = 0;

int val = 0;

void showCameraTrigger(){
  tft.fillScreen(BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Camera \nTrigger");
}

void showDataSend(){
  tft.fillScreen(BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Data Send");
}

void initialScreen(){
  tft.fillScreen(BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("System \nReady\n\n");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
     
  tft.begin();

  Serial.println("init");


  INCL.configSPI();
  delay(100);                   // Give the part time to start up
  INCL.regWrite(MSC_CTRL,0x6);  // Enable Data Ready on INCL
  delay(20);
  INCL.regWrite(AVG_CNT,0x8);   // Set Digital Filter on INCL
  delay(20);
  INCL.regWrite(SMPL_PRD,0x14), // Set Decimation on INCL
  delay(20);
  
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

  pinMode(4, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, INPUT);

  pinMode(TRIGGER_BUTTON, INPUT);
  pinMode(TRIGGER_SIGNAL, OUTPUT);
  pinMode(FLASH_SIGNAL, OUTPUT);
}

void loop() {
  if(millis() - batery_timmer > 10000){
    batery_timmer = millis();
    uint8_t value = random(0,100);
    char buffer[3];
    sprintf(buffer, "%03d", value);
    batery->setValue((uint8_t*)buffer, 3);
    batery->notify();
    char sec_buff[20];
    sprintf(sec_buff, "Val: %03d", val);
    Serial.println(sec_buff);
  }

  if(millis() - imu_timer > 100){
    imu_timer = millis();
    INCX = INCL.regRead(XINCL_OUT);
    INCY = INCL.regRead(YINCL_OUT);
    INCX = INCL.inclineScale(INCX);
    INCY = INCL.inclineScale(INCY);   
  }

  if((millis() - status_timer > 1000) && (screen_state == 0)){
    status_timer = millis();
    initialScreen();
  }

  if(digitalRead(TRIGGER_BUTTON) && (state==0)){
    Serial.print("State: ");Serial.println(state);
    
    state = 1;
    screen_state = 1;
    trigger_timmer = millis();

    char imu_buffer[7];
    sprintf(imu_buffer,"%03d,%03d", INCX, INCY);
    event->setValue((uint8_t*)imu_buffer, 7);
    event->notify();
    Serial.println(imu_buffer);
    
    showCameraTrigger();
  }
  
  // --- Trigger Secuence ---
  if(state != 0){
    if((state == 1) && (millis() - trigger_timmer >=180)){
      Serial.print("State: ");Serial.println(state);
      state = 2;
      trigger_timmer = millis();
      digitalWrite(TRIGGER_SIGNAL, 1);
    }
    if((state == 2) && (millis()-trigger_timmer >= 10)){
      Serial.print("State: ");Serial.println(state);
      state = 3;
      digitalWrite(TRIGGER_SIGNAL, 0);
      digitalWrite(FLASH_SIGNAL, 1);
    }
    if((state == 3) && (millis()-trigger_timmer >= 10)){
      Serial.print("State: ");Serial.println(state);
      state = 0;
      digitalWrite(FLASH_SIGNAL, 0);
    }
  }

  // --- Screen Handling ---
  if(screen_state != 0){
    if(screen_state == 1){
      Serial.print("Screen State: ");Serial.println(screen_state);
      screen_state = 2;
      screen_timer = millis();
      showCameraTrigger();
    }
    if((screen_state == 2) && (millis()-screen_timer >= 2000)){
      Serial.print("Screen State: ");Serial.println(screen_state);
      screen_state = 3;
      screen_timer = millis();
      showDataSend();
    }
    if((screen_state == 3) && (millis()-screen_timer >= 2000)){
      Serial.print("Screen State: ");Serial.println(screen_state);
      screen_state = 0;
      initialScreen();
    }
  }
}
