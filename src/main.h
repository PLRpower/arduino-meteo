#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>  // Librairie Arduino pour les fonctions de base
#include <EEPROM.h>  // Librairie pour l'écriture et la lecture dans l'EEPROM
#include <DS1307.h>  // Librairie pour l'horloge RTC
#include <ChainableLED.h>  // Librairie pour la LED RGB
#include <Seeed_BME280.h>  // Librairie pour le capteur BME280 (température, humidité, pression)
#include <SoftwareSerial.h>  // Librairie pour la communication série logicielle (GPS)
#include <SdFat.h>  // Librairie pour la carte SD (optimisée et plus légère que la librairie SD.h)

// Structure pour les paramètres de configuration
struct Parameter {
    int value;
    int minValue;
    int maxValue;
};

// Structure pour la configuration
struct Config {
    Parameter logInterval = {10, 1, 60};  // Interval de log en minutes
    Parameter fileMaxSize = {2048, 1024, 16384};  // Taille maximale d'un fichier en octets
    Parameter timeout = {30, 10, 120};  // Timeout en secondes pour le GPS
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

// Structure pour un bouton
struct Button {
    bool state;
    unsigned long pressStart;
    byte pin;
};

// Structure pour les données météo
struct WeatherData {
    byte hour, minute, second;
    float temperature, humidity, pressure;
    unsigned int lumin;
    float latitude, longitude;
};

// Enumérations pour les différents états, modes, couleurs et erreurs
enum GpsState { WAITING, LOADING, FINISHED };
enum Mode { STANDARD, ECONOMIQUE, MAINTENANCE, CONFIG };
enum Color { GREEN, RED, BLUE, YELLOW, ORANGE, WHITE, OFF };
enum Error { RTC, GPS, SENSOR, DATA, SD_FULL, SD_RW };


// Déclaration des variables globales, utilisées dans plusieurs fonctions (setup, loop, ...)
extern SoftwareSerial gps;
extern ChainableLED led;
extern DS1307 clock;
extern BME280 sensor;
extern SdFat32 SD;
extern Color blinkColor1;  // Couleur 1 pour le clignotement
extern Color blinkColor2;  // Couleur 2 pour le clignotement
extern Config config;  // Configuration, avec tous les paramètres
extern Button redButton;  // Bouton rouge
extern Button greenButton;  // Bouton vert
extern Mode currentMode;  // Mode actuel
extern Mode previousMode;  // Mode précédent (pour la maintenance)
extern WeatherData data;  // Données météo
extern GpsState gpsState;  // Etat du GPS
extern bool longerBlink;  // Indique si la couleur 1 doit rester plus longtemps
extern bool errors[6];  // Tableau des erreurs
extern unsigned long gpsTimer;  // Timer pour le GPS
extern unsigned long startTimer;  // Timer pour les intervalles de log

// Déclaration des fonctions utilisées dans le programme
void checkUserInput();
void initBlinkInterrupt();
void stopBlinkInterrupt();
void greenButtonInterrupt();
void redButtonInterrupt();
void checkErrors();
void acquireGPS();
bool starting();
void saveData();
void displayData();
void handleButtonPresses();
void processData();
void setMode(Mode mode);
void setLedColor(Color color);
WeatherData acquireData();

#endif
