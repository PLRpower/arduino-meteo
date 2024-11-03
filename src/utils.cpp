#include "main.h"

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

void setMode(Mode mode) {
    const Color modeColors[4] = {GREEN, BLUE, ORANGE, YELLOW};
    setLedColor(modeColors[mode]);
    currentMode = mode;
}

void checkErrors() {
    const Color errorColors[6] = {BLUE, YELLOW, GREEN, GREEN, WHITE, WHITE};

    for (int i = 0; i < 6; ++i) {
        if (errors[i]) {
            blinkColor1 = RED;
            blinkColor2 = errorColors[i];
            longerBlink = i == 3 || i == 5;
            initBlinkInterrupt();
            return;
        }
    }

    stopBlinkInterrupt();
}