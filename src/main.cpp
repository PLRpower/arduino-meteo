#include <Arduino.h>
#include <EEPROM.h>
#include <ChainableLED.h>
#include <DS1307.h>
#include <Seeed_BME280.h>
#include <SoftwareSerial.h>
#include <SdFat.h>

#include "main.h"

Button redButton = {false, 0, 2};
Button greenButton = {false, 0, 3};
ChainableLED led(7, 8, 1);
Config config;
DS1307 clock;
BME280 sensor;
SoftwareSerial gps(5, 6);
Mode currentMode = STANDARD;
Mode previousMode;
Color blinkColor1 = GREEN;
Color blinkColor2 = OFF;
SdFat32 SD;
unsigned long startTimer = millis();
bool longerBlink = false;
bool errors[6] = {false};


void sauvegarderDonnees(const MeteoData &data) {
    clock.getTime();
    char fileName[14];
    sprintf(fileName, "20%02d%02d_0.LOG", clock.month, clock.dayOfMonth);
    File32 file = SD.open(fileName, FILE_WRITE);
    if (file) {
        if(file.size() + sizeof(data) > config.fileMaxSize.value) {
            int fileIndex = 1;
            char newFileName[14];
            do {
                sprintf(newFileName, "20%02d%02d_%d.LOG", clock.month, clock.dayOfMonth, fileIndex++);
            } while (SD.exists(newFileName)); // Incrémente jusqu'à trouver un nom non existant

            File newFile = SD.open(newFileName, FILE_WRITE);
            if (newFile) {
                // Copie des données de oldFile vers newFile
                while (file.available()) {
                    newFile.write(file.read());
                }
                newFile.close();
            }
            file.close();
            SD.remove(fileName);
            file = SD.open(fileName, FILE_WRITE);
        }
        file.print(data.hour);
        file.print(':'); file.print(data.minute);
        file.print(':'); file.print(data.second);
        file.print(';'); file.print(data.temperature);
        file.print(';'); file.print(data.humidity);
        file.print(';'); file.print(data.pressure);
        file.print(';'); file.print(data.light);
        file.print(';'); file.print(data.latitude);
        file.print(';'); file.println(data.longitude);
        file.close();
    }
}

bool starting() {
    initBlinkInterrupt();
    unsigned long startTimer = millis();
    while (millis() - startTimer < 5000) {
        if(redButton.state) {
            stopBlinkInterrupt();
            return true;
        }
    }
    stopBlinkInterrupt();
    return false;
}

void setup() {
    Serial.begin(9600);
    Serial.setTimeout(30000);
    attachInterrupt(digitalPinToInterrupt(redButton.pin), redButtonInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(greenButton.pin), greenButtonInterrupt, CHANGE);

    if(starting()) {
        Serial.println(F("Début du mode configuration"));
        setMode(CONFIG);
    } else {
        setMode(STANDARD);
    }

    EEPROM.get(0, config);
    gps.begin(9600);
    sensor.init();
    clock.begin();
    SD.begin(4);
}

void loop() {
    if (currentMode == CONFIG) {
        checkUserInput();
    } else {
        if(redButton.state && millis() - redButton.pressStart > 5000) {  // Si le bouton est appuyé depuis plus de 5 secondes
            if(currentMode == MAINTENANCE) {
                setMode(previousMode);
            } else {
                previousMode = currentMode;
                setMode(MAINTENANCE);
            }
            redButton.state = false;
        } else if(greenButton.state && millis() - greenButton.pressStart > 5000) {
            if(currentMode != MAINTENANCE) {
                setMode(currentMode == ECONOMIQUE ? STANDARD : ECONOMIQUE);
            }
            greenButton.state = false;
        }

        const unsigned int interval = config.logInterval.value * 1000 * (currentMode == ECONOMIQUE ? 2 : 1);
        if (millis() - startTimer >= interval) {
            startTimer = millis();
            const MeteoData data = acquerirDonnees();
            if(currentMode == MAINTENANCE) {
                Serial.print(F("Heure : ")); Serial.print(data.hour);
                Serial.print(':'); Serial.print(data.minute);
                Serial.print(':'); Serial.print(data.second);
                Serial.print(F(" | Température : ")); Serial.print(data.temperature);
                Serial.print(F("°C | Humidité : ")); Serial.print(data.humidity);
                Serial.print(F("% | Pression : ")); Serial.print(data.pressure);
                Serial.print(F("Pa | Luminosité : ")); Serial.print(data.light);
                Serial.print(F(" | Latitude : ")); Serial.print(data.latitude);
                Serial.print(F(" | Longitude : ")); Serial.println(data.longitude);
            } else {
                sauvegarderDonnees(data);
            }
        }
    }
}