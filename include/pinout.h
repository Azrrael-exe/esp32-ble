#define TRIGGER_INPUT 25
#define LASER_INPUT 27

#define TRIGGER_OUTPUT 26
#define FLASH_OUTPUT 32
#define CAMERA_OUTPUT 33

#define BATTERY_INPUT 35
#define CHARGING_INPUT 38
#define BREADY_INPUT 37

// SCREEN
#define TFT_CS  21
#define TFT_DC  2
#define TFT_RST 15
// IMU 
#define IMU_CS 10
#define IMU_RST 14
#define IMU_DR 0

// SPI PINS
#define ESP_MOSI 23
#define ESP_MISO 19
#define ESP_SCLK 18

void setIO(){
    pinMode(TRIGGER_OUTPUT, OUTPUT);
    pinMode(FLASH_OUTPUT, OUTPUT);
    pinMode(CAMERA_OUTPUT, OUTPUT);

    pinMode(BATTERY_INPUT, INPUT);
    pinMode(LASER_INPUT, INPUT);
    pinMode(TRIGGER_INPUT, INPUT);

    pinMode(CHARGING_INPUT, INPUT);
    pinMode(BREADY_INPUT, INPUT);

    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);

    pinMode(IMU_CS, OUTPUT);
    pinMode(IMU_RST, OUTPUT);
}
