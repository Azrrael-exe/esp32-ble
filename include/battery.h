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
    this->updateTimer();
}

float Battery::readAdc(){
    float vol = analogRead(this->pin) * (5.0/4096);
    return vol;
}

void Battery::iterate(){
    float vol = readAdc();
    if(count < 10){
        ready = false;
        count++;
        temp_voltaje += vol;
    } 
    else{
        count = 0;
        voltaje = temp_voltaje/10.0;
        temp_voltaje = 0;
        ready = true;
    }
}

int Battery::readBattery(){
    if(voltaje > 4.0) return 99;
    else if(voltaje < 3.0) return 0;
    else {
        return map((voltaje-3.0), 0, 1.0, 0, 98);
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