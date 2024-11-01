#include "main.h"

Mode currentMode = STANDARD;
Mode previousMode;

void setLedColor(Color color) {
    switch (color) {
        case GREEN: led.setColorRGB(0, 0, 255, 0); break;
        case RED: led.setColorRGB(0, 255, 0, 0); break;
        case BLUE: led.setColorRGB(0, 0, 0, 255); break;
        case YELLOW: led.setColorRGB(0, 255, 255, 0); break;
        case ORANGE: led.setColorRGB(0, 255, 165, 0); break;
        case WHITE: led.setColorRGB(0, 255, 255, 255); break;
        case OFF: led.setColorRGB(0, 0, 0, 0); break;
    }
}

void setMode(Mode mode) {
    previousMode = currentMode;
    currentMode = mode;
    switch (mode) {
        case STANDARD: led.setColorRGB(0, 0, 255, 0); break;
        case ECONOMIQUE: led.setColorRGB(0, 0, 0, 255); break;
        case MAINTENANCE: led.setColorRGB(0, 255, 100, 0); break;
        case CONFIG: led.setColorRGB(0, 255, 255, 0); break;
    }
}