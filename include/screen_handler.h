#include <Adafruit_SSD1351.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

#include <logo.h>

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF


class ScreenHandler: public Adafruit_SSD1351 {
    public:
        ScreenHandler(int tft_cs, int tft_dc, int tft_rst): Adafruit_SSD1351(tft_cs, tft_dc, tft_rst) {

        }
        void init(int color);
        void handle(int state);
        void base(bool clear, int color);
        void initScreen(bool clear, int color);
        void laser(bool clear, int color);
        void imageTaken(bool clear, int color);
        void imageTransfer(bool clear, int color);
        void charging(int state);
        void splash(bool clear);
        void battery(int value);
        void updateTimer();
        long readTimer();
        int getState();
    private:
        int state;
        int backgroud;
        long timer;
};

void ScreenHandler::handle(int state){
    ;
}

void ScreenHandler::init(int color){
    this->begin();
    delay(10);
    this->backgroud = color;
    this->setTextSize(2);
    this->fillScreen(backgroud);
}

void ScreenHandler::initScreen(bool clear=false, int color=WHITE){
    if(!clear){
        this->setCursor(34,20);
        this->setTextColor(color);
        this->print("System");
        this->setCursor(22,41);
        this->print("Starting");
    }
    else {
        this->fillRect(22,20, 114, 57, backgroud);
    }
}   

void ScreenHandler::base(bool clear=false, int color=WHITE){
    if(!clear){
        this->setCursor(34,20);
        this->setTextColor(color);
        this->print("System");
        this->setCursor(39,41);
        this->print("Ready");
        this->setTextSize(1);
        this->setCursor(95, 112);
        this->setTextColor(WHITE);
        char buffer[5];
        sprintf(buffer, "B:00%%");
        this->print(buffer);
        this->setTextSize(2);
    }
    else {
        this->fillRect(34, 20, 94, 57, backgroud);
    }
    this->state = 0;
}

void ScreenHandler::splash(bool clear=false){
    if(!clear){
        this->drawBitmap(0, 0, logo, 128, 128, BLUE);
    }
    else{
        this->fillScreen(backgroud);
    }
}

void ScreenHandler::laser(bool clear=false, int color=RED){
    if(!clear){
        this->setTextSize(1);
        this->setCursor(4,112);
        this->setTextColor(color);
        this->print("Laser On!");
        this->setTextSize(2);
    }
    else {
        this->fillRect(4, 112, 54, 120, backgroud);
    }
}

void ScreenHandler::battery(int value){
    this->fillRect(105, 112, 23, 8, backgroud);
    this->setTextSize(1);
    this->setCursor(95, 112);
    this->setTextColor(WHITE);
    char buffer[5];
    sprintf(buffer, "B:%02i%%", value);
    this->print(buffer);
    this->setTextSize(2);
}

void ScreenHandler::charging(int state = 0){
    this->fillRect(55, 102, 69, 8, BLACK);
    this->setTextSize(1);
    switch (state) {
    case 1: // Charging
        this->setCursor(75, 102);
        this->setTextColor(RED);
        this->print("Charging");
        break;
    case 2: // Full Charge
        this->setCursor(55, 102);
        this->setTextColor(GREEN);
        this->print("Full Charge");
        break;
    default: // Clear
        this->fillRect(55, 102, 69, 8, BLACK);
        break;
    }
    this->setTextSize(2);
}

void ScreenHandler::imageTaken(bool clear=false, int color=RED){
    if(!clear){
        this->setCursor(36,20);
        this->setTextColor(color);
        this->print("Imagen");
        this->setCursor(36,41);
        this->print("Taken");
    }
    else {
        this->fillRect(36,20, 94, 57, backgroud);
    }
    this->state = 1;
}

void ScreenHandler::imageTransfer(bool clear=false, int color=YELLOW){
    if(!clear){
        this->setCursor(36,20);
        this->setTextColor(color);
        this->print("Imagen");
        this->setCursor(24,41);
        this->print("Transfer");
    }
    else {
        this->fillRect(24,20, 104, 57, backgroud);
    }
    this->state = 2;
}

int ScreenHandler::getState(){
    return this->state;
}

void ScreenHandler::updateTimer(){
    this->timer = millis();
}

long ScreenHandler::readTimer(){
    return (millis()-this->timer);
}
