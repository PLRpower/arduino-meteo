#include "main.h"

// Couleurs
bool useColor1 = true; // Indique la couleur en cours
bool longerState = false; // Pour suivre l'état de longer dans l'ISR

void initBlinkInterrupt() {
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    OCR1A = (62500 * 0.5) - 1; // Configuration du comparateur
    TCCR1B |= (1 << WGM12) | (1 << CS12); // CTC mode avec prescaler 256
    TIMSK1 |= (1 << OCIE1A); // Activer l'interruption
    interrupts(); // Réactiver les interruptions
}

void stopBlinkInterrupt() {
    TIMSK1 &= ~(1 << OCIE1A); // Désactiver l'interruption
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