#include "main.h"

// Fonction pour mettre la couleur de la LED
void setLedColor(Color color) {
    switch (color) {
        case GREEN: led.setColorRGB(0, 0, 255, 0); break;
        case RED: led.setColorRGB(0, 255, 0, 0); break;
        case BLUE: led.setColorRGB(0, 0, 0, 255); break;
        case YELLOW: led.setColorRGB(0, 255, 255, 0); break;
        case ORANGE: led.setColorRGB(0, 255, 100, 0); break;
        case WHITE: led.setColorRGB(0, 255, 255, 255); break;
        case OFF: led.setColorRGB(0, 0, 0, 0); break;
    }
}

// Fonction pour mettre le mode
void setMode(Mode mode) {
    const Color modeColors[4] = {GREEN, BLUE, ORANGE, YELLOW};
    setLedColor(modeColors[mode]);  // Mise à jour de la couleur de la LED en fonction du mode
    currentMode = mode;  // Mise à jour du mode actuel
}

// Fonction pour vérifier et traiter les erreurs
void checkErrors() {
    const Color errorColors[6] = {BLUE, YELLOW, GREEN, GREEN, WHITE, WHITE};  // Couleurs pour les erreurs
    for (int i = 0; i < 6; ++i) {  // Parcours des erreurs
        if (errors[i]) {  // Si l'erreur est détectée
            blinkColor1 = RED;  // Couleur 1 pour le clignotement
            blinkColor2 = errorColors[i];  // Couleur 2 pour le clignotement
            longerBlink = i == 3 || i == 5;  // Indique si la couleur 2 doit rester plus longtemps
            initBlinkInterrupt();  // Initialisation de l'interruption pour le clignotement
            return;
        }
    }

    stopBlinkInterrupt();  // Arrêt de l'interruption pour le clignotement si aucune erreur n'est détectée
}

// Fonction pour démarrer le système (faire clignoter la LED et vérifier si le bouton rouge est pressé)
bool starting() {
    initBlinkInterrupt();  // Initialisation de l'interruption pour le clignotement
    unsigned long startTimer = millis();  // Timer pour le démarrage
    while (millis() - startTimer < 5000) {  // Tant que le timer n'est pas écoulé (5 secondes)
        if(redButton.state) {  // Si le bouton rouge est pressé
            stopBlinkInterrupt();  // Arrêt de l'interruption pour le clignotement
            return true;  // Retourne vrai (le bouton rouge est pressé)
        }
    }
    stopBlinkInterrupt();  // Arrêt de l'interruption pour le clignotement
    return false;  // Retourne faux (le bouton rouge n'est pas pressé)
}

// Fonction pour vérifier les entrées utilisateur
void handleButtonPresses() {
    if (redButton.state && millis() - redButton.pressStart > 5000) {  // Si le bouton rouge est pressé pendant plus de 5 secondes
        if (currentMode == MAINTENANCE) {  // Si on est en mode maintenance
            SD.begin(4);  // Redémarrage de la carte SD
            setMode(previousMode);  // Retour au mode précédent
        } else {
            SD.end();  // Arrêt de la carte SD
            previousMode = currentMode;  // Sauvegarde du mode actuel
            setMode(MAINTENANCE);  // Passage en mode maintenance
        }
        redButton.state = false;  // Réinitialisation de l'état du bouton
    } else if (greenButton.state && millis() - greenButton.pressStart > 5000) {  // Si le bouton vert est pressé pendant plus de 5 secondes
        if (currentMode != MAINTENANCE) {  // Si on n'est pas en mode maintenance
            setMode(currentMode == ECONOMIQUE ? STANDARD : ECONOMIQUE);  // Passage en mode économique ou standard
        }
        greenButton.state = false;  // Réinitialisation de l'état du bouton
    }
}

// Fonction pour acquérir les données météo
void processData() {
    if (gpsState == WAITING) {  // Si le GPS est en attente
        acquireData();  // Acquisition des données météo
        gpsTimer = millis();  // Mise à jour du timer GPS
    } else if (gpsState == LOADING) {  // Si le GPS est en chargement
        acquireGPS();  // Vérifier si le GPS a reçu des données
    } else {  // Si le GPS a fini de charger
        gpsState = WAITING;  // Retour à l'état d'attente
        startTimer = millis();  // Mise à jour du timer
        if (currentMode == MAINTENANCE) {  // Si on est en mode maintenance
            displayData();  // Affichage des données
        } else {
            saveData();  // Sauvegarde des données
        }
    }
}

// Fonction pour afficher toutes les données météo dans le moniteur série
void displayData() {
    Serial.print(F("Heure : ")); Serial.print(data.hour);
    Serial.print(':'); Serial.print(data.minute);
    Serial.print(':'); Serial.print(data.second);
    Serial.print(F(" | Température : ")); Serial.print(data.temperature);
    Serial.print(F("°C | Humidité : ")); Serial.print(data.humidity);
    Serial.print(F("% | Pression : ")); Serial.print(data.pressure);
    Serial.print(F("hPa | Luminosité : ")); Serial.print(data.lumin);
    if (data.latitude == NULL || data.longitude == NULL) {  // Si les coordonnées GPS ne sont pas valides
        Serial.println(F(" | Latitude : NA | Longitude : NA"));  // Affichage de NA
    } else {
        Serial.print(F(" | Latitude : ")); Serial.print(data.latitude);
        Serial.print(F(" | Longitude : ")); Serial.println(data.longitude);
    }
}
