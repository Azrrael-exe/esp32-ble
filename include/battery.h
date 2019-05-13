#include <driver/adc.h>

class Battery {
    public:
        Battery(int pin);
        void iterate();
        float readAdc();
        float readVoltaje();
        int readBattery();
        bool isReady();
        void updateTimer();
        long readTimer();
    private:
        float voltaje;
        float temp_voltaje;
        int count;
        bool ready;
        int pin;
        long timer;
};

Battery::Battery(int pin){
    this->pin=pin;
    this->voltaje = 0;
    this->count = 0;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_0);
    this->updateTimer();
}

float Battery::readAdc(){
    // int vol = adc1_get_raw(ADC1_CHANNEL_7);
    int vol = analogRead(35);
    Serial.print("vol:");Serial.println(vol);
    return vol * (5.0/4096);
}

void Battery::iterate(){
    float vol = readAdc();
    if(count < 5){
        ready = false;
        count++;
        temp_voltaje += vol;
    } 
    else{
        count = 0;
        voltaje = temp_voltaje/5.0;
        temp_voltaje = 0;
        ready = true;
    }
}

int Battery::readBattery(){
    if(voltaje > 4.0) return 99;
    else if(voltaje < 3.0) return 0;
    else {
        voltaje = voltaje - 3.0;
        return voltaje * 100;
    }
}

bool Battery::isReady(){
    return ready;
}

void Battery::updateTimer(){
    timer = millis();
}

long Battery::readTimer(){
    return millis()-timer;
}