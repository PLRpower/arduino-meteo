#include <Arduino.h>
#include "variables.h"

Button redButton = {false, false, 0, 2};
Button greenButton = {false, false, 0, 3};
ChainableLED led(7, 8, 1);
Mode currentMode;
Config config;pornhub

void setup() {
    Serial.begin(9600);
    led.init();
    pinMode(redButton.pin, INPUT_PULLUP);
    pinMode(greenButton.pin, INPUT_PULLUP);

    if(demarrage(redButton, led)) {  // Si le bouton rouge est appuyé au démarrage
        setModeConfig(led, config);  // Mode configuration
    }
    setModeStandard(led, currentMode);  // Mode standard si le bouton rouge n'est pas appuyé ou après 30 secondes d'inactivité
}

void loop() {
    handleButton(redButton);
    handleButton(greenButton);

    if (redButton.pressed5s) {
        currentMode == MODE_MAINTENANCE ? setPreviousMode(led, currentMode) : setModeMaintenance(led, currentMode);
        resetButton(redButton);
    } else if (greenButton.pressed5s) {
        if (currentMode == MODE_ECONOMIQUE) {
            setModeStandard(led, currentMode);
        } else if (currentMode == MODE_STANDARD) {
            setModeEconomique(led, currentMode);
        }
        resetButton(greenButton);
    }

    acquerirDonnees();

    sauvegarderDonnees(currentMode);
}
