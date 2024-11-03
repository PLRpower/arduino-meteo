#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <DS1307.h>
#include <ChainableLED.h>


struct Parameter {
    int value;
    int minValue;
    int maxValue;
};

struct Config {
    Parameter logInterval = {10, 1, 60};  // Interval de log en minutes
    Parameter fileMaxSize = {4096, 1024, 16384};  // Taille maximale d'un fichier en octets
    Parameter timeout = {30, 10, 120};  // Timeout en secondes
    Parameter lumin = {1, 0, 1};  // Log de la luminosité
    Parameter luminLow = {255, 0, 1023};  // Seuil bas de luminosité
    Parameter luminHigh = {768, 0, 1023};
    Parameter tempAir = {1, 0, 1};
    Parameter minTempAir = {-10, -40, 85};
    Parameter maxTempAir = {60, -40, 85};
    Parameter hygr = {1, 0, 1};
    Parameter hygrMinT = {0, -40, 85};
    Parameter hygrMaxT = {50, -40, 85};
    Parameter pressure = {1, 0, 1};
    Parameter pressureMin = {850, 300, 1100};
    Parameter pressureMax = {1080, 300, 1100};
};

struct Button {
    bool state;
    unsigned long pressStart;
    byte pin;
};

struct MeteoData {
    byte hour, minute, second;
    float temperature, humidity, pressure;
    unsigned int light;
    float latitude, longitude;
};

enum Mode { STANDARD, ECONOMIQUE, MAINTENANCE, CONFIG };
enum Color { GREEN, RED, BLUE, YELLOW, ORANGE, WHITE, OFF };
enum Error { RTC, GPS, SENSOR, DATA, SD_FULL, SD_RW };

extern SoftwareSerial gps;
extern Color blinkColor1;
extern Color blinkColor2;
extern ChainableLED led;
extern Config config;
extern DS1307 clock;
extern BME280 sensor;
extern Button redButton;
extern Button greenButton;
extern Mode currentMode;
extern Mode previousMode;
extern bool longerBlink;
extern bool errors[6];

void checkUserInput();
void initBlinkInterrupt();
void stopBlinkInterrupt();
void greenButtonInterrupt();
void redButtonInterrupt();
void setMode(Mode mode);
void setLedColor(Color color);
void checkErrors();

#endif //MAIN_H
