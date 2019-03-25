
class InputCallback: public BLECharacteristicCallbacks {
    private:
        int* value;
    public:
        InputCallback(int* v):BLECharacteristicCallbacks(){
            this->value = v;
        };
        void onWrite(BLECharacteristic *pCharacteristic);
};

void InputCallback::onWrite(BLECharacteristic *pCharacteristic) {
    String inp = pCharacteristic->getValue().c_str();
    *value = inp[0];
}