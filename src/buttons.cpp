#include "variables.h"

void handleButton(Button& button) {
    if (digitalRead(button.pin) == LOW) {  // Si le bouton est appuyé
        if (!button.state) {  // Si le bouton vient d'être appuyé
            button.pressStart = millis();
            button.state = true;
        } else if (millis() - button.pressStart >= 5000) {  // Si le bouton est appuyé depuis plus de 5 secondes
            button.pressed5s = true;
        }
    } else if (button.state) {  // Si le bouton est relâché
        button.state = false;
        button.pressed5s = false;
    }
}

void resetButton(Button& button) {
    button.state = false;
    button.pressed5s = false;
}