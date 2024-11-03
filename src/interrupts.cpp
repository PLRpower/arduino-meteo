#include "main.h"

bool useColor1 = true;  // Couleur utilisée pour le clignotement
bool longerState = false;  // Indique si la couleur 1 doit rester plus longtemps
bool alreadyInit = false;  // Indique si l'interruption est déjà initialisée

// Fonction permettant d'initialiser l'interruption pour le clignotement
void initBlinkInterrupt() {
    if(!alreadyInit) {  // Si l'interruption n'est pas déjà initialisée
        noInterrupts();  // Désactivation des autres interruptions pour éviter les problèmes
        longerState = false;  // Initialisation de l'état de clignotement
        TCCR1A = 0;  // Réinitialisation des registres
        TCCR1B = 0;  // Réinitialisation des registres
        TCNT1  = 0;  // Réinitialisation du compteur
        OCR1A = (62500 * 0.5) - 1;  // Valeur de comparaison pour une interruption toutes les 0.5 secondes
        TCCR1B |= (1 << WGM12) | (1 << CS12);  // Configuration du timer en mode CTC avec un prédiviseur de 256
        TIMSK1 |= (1 << OCIE1A);  // Activation de l'interruption
        alreadyInit = true;  // Indication que l'interruption est initialisée
        interrupts();  // Réactivation des autres interruptions
    }
}

// Fonction permettant d'arrêter l'interruption pour le clignotement
void stopBlinkInterrupt() {
    TIMSK1 &= ~(1 << OCIE1A);  // Désactivation de l'interruption
    setMode(currentMode);  // Mise à jour de la couleur de la LED
    alreadyInit = false;  // Indication que l'interruption n'est plus initialisée
}

// Interruption pour le clignotement
ISR(TIMER1_COMPA_vect) {
    setLedColor(useColor1 ? blinkColor1 : blinkColor2);  // Mise à jour de la couleur de la LED
    if(useColor1 && longerBlink && !longerState) {  // Si la couleur 1 doit rester plus longtemps
        longerState = true;  // Mise à jour de l'état
    } else {  // Sinon
        useColor1 = !useColor1;  // Changement de couleur
        longerState = false;  // Réinitialisation de l'état
    }
}

// Fonction appelée lors des interruptions sur les boutons
void buttonInterrupt(Button& button) {
    bool isPressed = digitalRead(button.pin) == LOW;  // Lecture de l'état du bouton

    if (isPressed && !button.state) {  // Si le bouton vient d'être appuyé
        button.pressStart = millis();  // Mise à jour de l'heure de début d'appui
        button.state = true;  // Mise à jour de l'état du bouton
    } else if (!isPressed) {  // Si le bouton est relâché
        button.state = false;  // Mise à jour de l'état du bouton
    }
}

void redButtonInterrupt() {  // Interruption sur le bouton rouge
    buttonInterrupt(redButton);
}

void greenButtonInterrupt() {  // Interruption sur le bouton vert
    buttonInterrupt(greenButton);
}