#include <EEPROM.h>
#include <Arduino.h>
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
            EEPROM.put(0, config);  // Sauvegarde de la configuration modifiée dans l'EEPROM
            return;
        }
    }

    Serial.println("Erreur, commande inconnue.");
}

void afficherParametres(Config& config) {
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
        Serial.println(commandNames[i] + "=" + String(params[i]->value));
    }
}

void setModeConfig(ChainableLED& led, Config& config) {
    unsigned long startMillis = millis();
    Serial.println("Début du mode configuration.");
    setLedColor(Jaune, led);
    EEPROM.get(0, config);  // Récupération des paramètres de configuration dans l'EEPROM
    while (millis() - startMillis < 30000) {
        if (Serial.available() > 0) {
            startMillis = millis();  // Réinitialisation du timer
            String input = Serial.readStringUntil('\n');
            input.trim(); // Supprimer les espaces inutiles

            if (input == "RESET") {
                EEPROM.put(0, Config()); // Sauvegarde des paramètres par défaut de la structure dans l'EEPROM
                config = Config(); // Réinitialisation des paramètres
                Serial.println("Configuration réinitialisée aux valeurs par défaut.");
            } else if (input == "VERSION") {
                Serial.println("Version: 1.0.0");
                Serial.println("Lot: Arduino-Groupe4");
            } else if (input == "DISPLAY") {
                afficherParametres(config);
            } else {
                int delimiterIndex = input.indexOf('=');
                if (delimiterIndex != -1) {
                    String command = input.substring(0, delimiterIndex);
                    String value = input.substring(delimiterIndex + 1);
                    applyCommand(command, value, config);
                } else {
                    Serial.println("Erreur, commande incorrecte.");
                }
            }
        }
    }
    Serial.println("Inactivité de 30 secondes détectée, démarrage du système en mode Standard.");
}
