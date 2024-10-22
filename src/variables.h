#ifndef MODES_H
#define MODES_H
#include <ChainableLED.h>

struct Parameter {
    int value;       // Valeur actuelle du paramètre
    int minValue;    // Valeur minimale
    int maxValue;    // Valeur maximale
};

struct Config {
    Parameter logInterval = {10, 1, 60};     // Intervalle de temps entre chaque log (1-60 minutes)
    Parameter fileMaxSize = {4096, 1024, 16384}; // Taille max fichier log (1k-16k)
    Parameter timeout = {30, 10, 120};          // Temps de capteur inactif avant extinction (10-120 secondes)
    Parameter lumin = {1, 0, 1};              // Activation capteur de luminosité (0 ou 1)
    Parameter luminLow = {255, 0, 1023};      // Luminosité faible (0-1023)
    Parameter luminHigh = {768, 0, 1023};     // Luminosité forte (0-1023)
    Parameter tempAir = {1, 0, 1};            // Activation capteur température air (0 ou 1)
    Parameter minTempAir = {-10, -40, 85};    // Température minimale (°C)
    Parameter maxTempAir = {60, -40, 85};     // Température maximale (°C)
    Parameter hygr = {1, 0, 1};               // Activation capteur hygrométrie (0 ou 1)
    Parameter hygrMinT = {0, -40, 85};     // Température min pour l'hygrométrie (°C)
    Parameter hygrMaxT = {50, -40, 85};    // Température max pour l'hygrométrie (°C)
    Parameter pressure = {1, 0, 1};           // Activation capteur pression (0 ou 1)
    Parameter pressureMin = {850, 300, 1100}; // Pression min (hPa)
    Parameter pressureMax = {1080, 300, 1100}; // Pression max (hPa)
};

struct Button {
    bool state;
    bool pressed5s;
    unsigned long pressStart;
    int pin;
};

enum Mode { MODE_STANDARD, MODE_ECONOMIQUE, MODE_MAINTENANCE, MODE_CONFIG };
enum LedColor { Rouge, Vert, Bleu, Jaune, Orange, Eteint };

void setLedColor(LedColor color, ChainableLED& led);
void changeMode(Button& redButton, Button& greenButton, ChainableLED& led, Mode& currentMode);
void handleButton(Button& button);
void resetButton(Button& button);
void setPreviousMode(ChainableLED& led, Mode& currentMode);
void setModeStandard(ChainableLED& led, Mode& currentMode);
void setModeEconomique(ChainableLED& led, Mode& currentMode);
void setModeMaintenance(ChainableLED& led, Mode& currentMode);
void setModeConfig(ChainableLED& led, Config& config);
bool demarrage(Button& redButton, ChainableLED& led);
void acquerirDonnees();
void sauvegarderDonnees(const Mode& currentMode);

#endif
