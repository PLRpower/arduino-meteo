#include <Arduino.h>
#include <EEPROM.h>
#include <ChainableLED.h>
#include <DS1307.h>
#include <Seeed_BME280.h>
#include <SoftwareSerial.h>
#include <SD.h>

#include "main.h"

Button redButton = {false, 0, 2};
Button greenButton = {false, 0, 3};
ChainableLED led(7, 8, 1);
Config config;
DS1307 clock;
BME280 sensor;
SoftwareSerial gps(5, 6);
unsigned long startTimer = millis();
Color blinkColor1 = GREEN;
Color blinkColor2 = OFF;
bool longerBlink = false;

void sauvegarderDonnees(const char* data) {
    clock.getTime();
    char fileName[14];
    sprintf(fileName, "20%02d%02d_0.LOG", clock.month, clock.dayOfMonth);
    File file = SD.open(fileName, FILE_WRITE);
    if (file) {
        if (file.size() + sizeof(data) > config.fileMaxSize.value) {
            file.close();
            char newFile[14];
            int fileNumber = 0;
            do {
                sprintf(newFile, "20%02d%02d_%d.LOG", clock.month, clock.dayOfMonth, fileNumber++);
            } while (SD.exists(newFile));
        }
        file.println(data);
        file.close();
    } else {
        initBlinkInterrupt();
    }
}

void acquerirDonnees(char* donnees, size_t taille) {
    clock.getTime();
    int hour = clock.hour;
    int minute = clock.minute;
    int second = clock.second;
    float temperture = sensor.getTemperature();
    float humidity = sensor.getHumidity();
    float pressure = sensor.getPressure();
    if(currentMode != ECONOMIQUE) {
        gps.read();
    }
}

bool start() {
    initBlinkInterrupt();
    unsigned long millisStart = millis();
    while (millis() - millisStart < 5000) {
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

    if(start()) {
        Serial.println(F("Début du mode configuration"));
        setMode(CONFIG);
    } else {
        setMode(STANDARD);
    }

    EEPROM.get(0, config);
    gps.begin(9600);
    sensor.init();
    clock.begin();
    if(!SD.begin(4)) {
        Serial.println(F("Erreur : LECTURE/ECRITURE"));
    }
}

void loop() {
    if (currentMode == CONFIG) {
        checkUserInput();
    } else {
        if(redButton.state && millis() - redButton.pressStart > 5000) {  // Si le bouton est appuyé depuis plus de 5 secondes
            setMode(currentMode == MAINTENANCE ? previousMode : MAINTENANCE);
            redButton.state = false;
        } else if(greenButton.state && millis() - greenButton.pressStart > 5000) {
            setMode(currentMode == ECONOMIQUE ? STANDARD : ECONOMIQUE);
            greenButton.state = false;
        }

        int interval = currentMode == ECONOMIQUE ? config.logInterval.value * 2 : config.logInterval.value;
        if (millis() - startTimer >= interval * 1000) {
            char data[50];
            acquerirDonnees(data, sizeof(data));
            if(currentMode == MAINTENANCE) {
                Serial.println(data);
            } else {
                sauvegarderDonnees(data);
            }
        }
    }
}