#include <Arduino.h>
#include "variables.h"
#include "DS1307.h"

Button redButton = {false, false, 0, 2};
Button greenButton = {false, false, 0, 3};
ChainableLED led(7, 8, 1);
Mode currentMode;
Config config;
DS1307 clock;

void printTime()
{
    clock.getTime();
    Serial.print(clock.hour, DEC);
    Serial.print(":");
    Serial.print(clock.minute, DEC);
    Serial.print(":");
    Serial.print(clock.second, DEC);
    Serial.print("  ");
    Serial.print(clock.dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(clock.month, DEC);
    Serial.print("/");
    Serial.print(clock.year + 2000, DEC);
    Serial.print(" ");
    switch (clock.dayOfWeek)
    {
        case MON:
            Serial.print("LUN");
        break;
        case TUE:
            Serial.print("MAR");
        break;
        case WED:
            Serial.print("MER");
        break;
        case THU:
            Serial.print("JEU");
        break;
        case FRI:
            Serial.print("VEN");
        break;
        case SAT:
            Serial.print("SAM");
        break;
        case SUN:
            Serial.print("DIM");
        break;
    }
    Serial.println(" ");
}

void setup() {
    Serial.begin(9600);
    led.init();
    pinMode(redButton.pin, INPUT_PULLUP);
    pinMode(greenButton.pin, INPUT_PULLUP);
    clock.begin();

    if(demarrage(redButton, led)) {  // Si le bouton rouge est appuyé au démarrage
        setModeConfig(led, config, clock);  // Mode configuration
    }

    setModeStandard(led, currentMode);  // Mode standard si le bouton rouge n'est pas appuyé ou après 30 secondes d'inactivité
}

void loop() {
    printTime();

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
