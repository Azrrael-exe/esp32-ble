class IO {
    public:
        IO(int ipin, int opin);
        void change(bool value);
        bool isActive();
        void updateTimer();
        long readTimer();
        bool readInput();
        bool hasChanged();
        bool activate;
    private:
        bool output_state;
        bool input_state;
        int ipin;
        int opin;
        long timer;
        bool has_changed;
};

IO::IO(int opin, int ipin){
    this->opin = opin;
    this->ipin = ipin;
    this->output_state = false;
    this->activate = false;
    pinMode(opin, OUTPUT);
    pinMode(ipin, INPUT);
    digitalWrite(this->opin, this->output_state);
}

void IO::change(bool state){
    this->output_state = state;
    digitalWrite(this->opin, this->output_state);
}

bool IO::isActive(){
    return this->output_state;
}

void IO::updateTimer(){
    this->timer = millis();
}

long IO::readTimer(){
    return (millis() - this->timer);
}

bool IO::readInput(){
    bool read = digitalRead(this->ipin);
    if(read != this->input_state){
        this->has_changed = true;
        this->input_state = read;
    }
    return input_state;
}

bool IO::hasChanged(){
    this->readInput();
    bool actual = this->has_changed;
    this->has_changed = false;
    return actual;
}