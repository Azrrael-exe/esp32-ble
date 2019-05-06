#define TRIGGER_INPUT 12
#define LASER_INPUT 13

#define TRIGGER_OUTPUT 25
#define FLASH_OUTPUT 26
#define CAMERA_OUTPUT 32

// SCREEN
#define TFT_CS  5
#define TFT_DC  10
#define TFT_RST 9
// IMU 
#define IMU_CS 21
#define IMU_RST 22
#define IMU_DR 0

void setIO(){
    pinMode(TRIGGER_OUTPUT, INPUT);
    pinMode(LASER_INPUT, INPUT);

    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);

    pinMode(IMU_CS, OUTPUT);
    pinMode(IMU_RST, OUTPUT);
}
