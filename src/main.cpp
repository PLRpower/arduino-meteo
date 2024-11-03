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
MeteoData data;
unsigned long gpsTimer;
unsigned long startTimer = millis();
bool longerBlink = false;
bool errors[6] = {false};

void sauvegarderDonnees(const MeteoData &data) {
    clock.getTime();
    char fileName[] = "20xxxx_0.LOG";
    fileName[2] = '0' + clock.month / 10;
    fileName[3] = '0' + clock.month % 10;
    fileName[4] = '0' + clock.dayOfMonth / 10;
    fileName[5] = '0' + clock.dayOfMonth % 10;

    File32 file = SD.open(fileName, FILE_WRITE);
    if (file && (file.size() + sizeof(data) > config.fileMaxSize.value)) {
        int fileIndex = 1;
        do {
            fileName[7] = '0' + fileIndex++;
        } while (SD.exists(fileName));

        File newFile = SD.open(fileName, FILE_WRITE);
        if (newFile) {
            char buffer[32];
            while (file.available()) {
                int bytesRead = file.readBytes(buffer, sizeof(buffer));
                newFile.write(buffer, bytesRead);
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
    file.print(';'); file.print(data.light); file.print(';');

    if (data.latitude != 0.0 && data.longitude != 0.0) {
        file.print(data.latitude); file.print(';'); file.println(data.longitude);
    } else {
        file.println(F("NA;NA;"));
    }
    file.close();
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
            if(gpsState == WAITING) {
                data = acquerirDonnees();
                gpsTimer = millis();
            } else if (gpsState == LOADING) {
                acquerirGPS();
            } else {
                gpsState = WAITING;
                startTimer = millis();
                if(currentMode == MAINTENANCE) {
                    Serial.print(F("Heure : ")); Serial.print(data.hour);
                    Serial.print(':'); Serial.print(data.minute);
                    Serial.print(':'); Serial.print(data.second);
                    Serial.print(F(" | Température : ")); Serial.print(data.temperature);
                    Serial.print(F("°C | Humidité : ")); Serial.print(data.humidity);
                    Serial.print(F("% | Pression : ")); Serial.print(data.pressure);
                    Serial.print(F("Pa | Luminosité : ")); Serial.print(data.light);
                    if(data.latitude == NULL || data.longitude == NULL) {
                        Serial.println(F(" | Latitude : NA | Longitude : NA"));
                    } else {
                        Serial.print(F(" | Latitude : ")); Serial.print(data.latitude);
                        Serial.print(F(" | Longitude : ")); Serial.println(data.longitude);
                    }
                } else {
                    sauvegarderDonnees(data);
                }
            }
        }
    }
}