#include <Arduino.h>
#include <EEPROM.h>
#include <Seeed_BME280.h>
#include <DS1307.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include "variables.h"

Button redButton = {false, false, 0, 2};
Button greenButton = {false, false, 0, 3};
ChainableLED led(7, 8, 1);
SoftwareSerial gps(5, 6);
DS1307 clock;
BME280 sensor;
Mode currentMode;
Config config;

int fileNumber = 0;
unsigned long startTimer = millis();

void setup() {
    EEPROM.get(0, config);
    Serial.begin(9600);
    gps.begin(9600);
    clock.begin();
    SD.begin(4);
    led.init();
    sensor.init();
    pinMode(redButton.pin, INPUT_PULLUP);
    pinMode(greenButton.pin, INPUT_PULLUP);

    if(demarrage(redButton, led)) {
        setModeConfig(led, config, clock);
    }

    setModeStandard(led, currentMode);
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

    String data = acquerirDonnees(config, clock, sensor, gps, currentMode, startTimer, fileNumber);

    Serial.print(data);
}
