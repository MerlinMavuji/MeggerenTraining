
class ButtonHandler {

private:

    const int buttonPin;

    bool lastButtonState;

    bool buttonPressed;

    void (*callbackFunction)();

 

public:

    ButtonHandler(int pin, void (*callback)()) : buttonPin(pin), lastButtonState(HIGH), buttonPressed(false), callbackFunction(callback) {}

 

    void begin() {

        pinMode(buttonPin, INPUT_PULLUP);
    }

 

    void update() {

        bool currentButtonState = digitalRead(buttonPin);

 

        if (currentButtonState == LOW && lastButtonState == HIGH) {

            delay(50); // Debounce

            if (digitalRead(buttonPin) == LOW) {

                buttonPressed = !buttonPressed;

                if (callbackFunction) {

                    callbackFunction();

                } 

            }

        }

 

        lastButtonState = currentButtonState;

    }

};