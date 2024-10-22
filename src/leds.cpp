#include <ChainableLED.h>
#include "variables.h"

void setLedColor(const LedColor color, ChainableLED& led) {
    switch (color) {
        case Rouge:
            led.setColorRGB(0, 255, 0, 0);
        break;
        case Vert:
            led.setColorRGB(0, 0, 255, 0);
        break;
        case Bleu:
            led.setColorRGB(0, 0, 0, 255);
        break;
        case Jaune:
            led.setColorRGB(0, 255, 255, 0);
        break;
        case Orange:
            led.setColorRGB(0, 255, 165, 0);
        break;
        default:
            led.setColorRGB(0, 0, 0, 0);
        break;
    }
}