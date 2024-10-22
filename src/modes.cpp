#include "variables.h"

Mode previousMode;

void setModeStandard(ChainableLED& led, Mode& currentMode) {
    setLedColor(Vert, led);
    currentMode = MODE_STANDARD;
}

void setModeEconomique(ChainableLED& led, Mode& currentMode) {
    setLedColor(Bleu, led);
    currentMode = MODE_ECONOMIQUE;
}

void setModeMaintenance(ChainableLED& led, Mode& currentMode) {
    previousMode = currentMode;
    currentMode = MODE_MAINTENANCE;
    setLedColor(Orange, led);
}

void setPreviousMode(ChainableLED& led, Mode& currentMode) {
    if(previousMode == MODE_STANDARD) {
        setModeStandard(led, currentMode);
    } else {
        setModeEconomique(led, currentMode);
    }
}

bool demarrage(Button& redButton, ChainableLED& led) {
    unsigned long previousMillis = 0;
    bool color = false;
    int cycleCount = 0;

    while (cycleCount < 9) {
        const unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= 500) {
            previousMillis = currentMillis;
            setLedColor(color ? Eteint : Vert, led);
            color = !color;
            cycleCount++;
        }

        handleButton(redButton);
        if (redButton.state) {
            return true;
        }
    }

    return false;
}
