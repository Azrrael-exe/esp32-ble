#define TRIGGER_BUTTON 12
#define LASER_BUTTON 13

#define TRIGGER_SIGNAL 25
#define FLASH_SIGNAL 26
#define CAMERA_ON 32

// SCREEN
#define TFT_CS  5
#define TFT_DC  10
#define TFT_RST 9
// IMU 
#define IMU_CS 21
#define IMU_RST 22
#define IMU_DR 0

void setIO(){
    pinMode(TRIGGER_BUTTON, INPUT);
    pinMode(LASER_BUTTON, INPUT);

    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);

    pinMode(IMU_CS, OUTPUT);
    pinMode(IMU_RST, OUTPUT);
}

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF
