#include <Arduino.h>
#include <EEPROM.h>
#include "variables.h"

void setParameter(Parameter &param, const String& newValue, const String& command) {
    int value = newValue.toInt();
    if (value >= param.minValue && value <= param.maxValue) {
        param.value = value;
        Serial.println("Paramètre " + command + " mis à jour à " + value + ".");
    } else {
        Serial.println("Erreur, la valeur pour la commande " + command + " doit être comprise entre " + param.minValue + " et " + param.maxValue + ".");
    }
}

void applyCommand(const String &command, const String &value, Config &config) {
    Parameter* params[] = {
        &config.logInterval, &config.fileMaxSize, &config.timeout,
        &config.lumin, &config.luminLow, &config.luminHigh,
        &config.tempAir, &config.minTempAir, &config.maxTempAir,
        &config.hygr, &config.hygrMinT, &config.hygrMaxT,
        &config.pressure, &config.pressureMin, &config.pressureMax
    };

    const String commandNames[] = {
        "LOG_INTERVAL", "FILE_MAX_SIZE", "TIMEOUT",
        "LUMIN", "LUMIN_LOW", "LUMIN_HIGH",
        "TEMP_AIR", "MIN_TEMP_AIR", "MAX_TEMP_AIR",
        "HYGR", "HYGR_MINT", "HYGR_MAXT",
        "PRESSURE", "PRESSURE_MIN", "PRESSURE_MAX"
    };

    for (int i = 0; i < sizeof(commandNames) / sizeof(commandNames[0]); i++) {
        if (command == commandNames[i]) {
            setParameter(*params[i], value, command);
            EEPROM.put(0, config);
            return;
        }
    }

    Serial.println("Erreur, commande inconnue.");
}

void setModeConfig(ChainableLED& led, Config& config, DS1307& clock) {
}
