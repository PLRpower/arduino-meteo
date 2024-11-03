#include "main.h"

bool useColor1 = true;
bool longerState = false;
bool alreadyInit = false;

void initBlinkInterrupt() {
    if(!alreadyInit) {
        longerState = false;
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1  = 0;
        OCR1A = (62500 * 0.5) - 1;
        TCCR1B |= (1 << WGM12) | (1 << CS12);
        TIMSK1 |= (1 << OCIE1A);
        alreadyInit = true;
    }
}

void stopBlinkInterrupt() {
    TIMSK1 &= ~(1 << OCIE1A);
    setMode(currentMode);
    alreadyInit = false;
}

ISR(TIMER1_COMPA_vect) {
    setLedColor(useColor1 ? blinkColor1 : blinkColor2);
    if(useColor1 && longerBlink && !longerState) {
        longerState = true;
    } else {
        useColor1 = !useColor1;
        longerState = false;
    }
}

void redButtonInterrupt() {
    bool isPressed = digitalRead(redButton.pin) == LOW;

    if (isPressed && !redButton.state) {  // Si le bouton vient d'être appuyé
        redButton.pressStart = millis();
        redButton.state = true;
    } else if (!isPressed) {  // Si le bouton est relâché
        redButton.state = false;
    }
}

void greenButtonInterrupt() {
    bool isPressed = digitalRead(greenButton.pin) == LOW;

    if (isPressed && !greenButton.state) {  // Si le bouton vient d'être appuyé
        greenButton.pressStart = millis();
        greenButton.state = true;
    } else if (!isPressed) {  // Si le bouton est relâché
        greenButton.state = false;
    }
}