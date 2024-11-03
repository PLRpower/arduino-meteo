#include "main.h"

// Déclaration des variables globales, utilisées dans plusieurs fonctions (setup, loop, ...)
Button redButton = {false, 0, 2};
Button greenButton = {false, 0, 3};
ChainableLED led(7, 8, 1);
Config config;
DS1307 clock;
BME280 sensor;
SoftwareSerial gps(5, 6);
Mode currentMode = STANDARD;
Mode previousMode;
GpsState gpsState = WAITING;
Color blinkColor1 = GREEN;
Color blinkColor2 = OFF;
SdFat32 SD;
WeatherData data;
unsigned long gpsTimer;
unsigned long startTimer = millis();
bool longerBlink = false;
bool errors[6] = {false};

void setup() {
    Serial.begin(9600);  // Initialisation de la communication série
    Serial.setTimeout(30000);  // Temps d'attente maximal pour la réception de données
    attachInterrupt(digitalPinToInterrupt(redButton.pin), redButtonInterrupt, CHANGE);  // Interruption sur le bouton rouge
    attachInterrupt(digitalPinToInterrupt(greenButton.pin), greenButtonInterrupt, CHANGE);  // Interruption sur le bouton vert

    if(starting()) {  // Si le bouton rouge est pressé pendant le démarrage
        setMode(CONFIG);  // On passe en mode configuration
    } else {
        setMode(STANDARD);  // On passe en mode standard
    }

    EEPROM.get(0, config);  // Récupération de la configuration enregistrée dans l'EEPROM
    gps.begin(9600);  // Initialisation du GPS
    sensor.init();  // Initialisation des capteurs de température, d'humidité et de pression
    clock.begin();  // Initialisation de l'horloge
    SD.begin(4);  // Initialisation de la carte SD, sur le pin 4
}

void loop() {
    checkErrors();  // Vérification et traitement des erreurs

    if (currentMode == CONFIG) {  // Si on est en mode configuration
        checkUserInput();  // Vérification des entrées utilisateur

    } else {  // Sinon
        handleButtonPresses();  // Traitement des pressions de boutons
        const unsigned long interval = config.logInterval.value * 60000 * (currentMode == ECONOMIQUE ? 2 : 1);  // Calcul de l'intervalle de temps entre chaque enregistrement
        if (millis() - startTimer >= interval) {  // Si l'intervalle est écoulé
            processData();  // Traitement des données (acquisition + enregistrement/affichage)
        }
    }
}