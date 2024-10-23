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

void setModeConfig(ChainableLED& led, Config& config, DS1307& clock) {
    unsigned long startMillis = millis();
    Serial.println("Début du mode configuration.");
    setLedColor(Jaune, led);
    EEPROM.get(0, config);  // Récupération des paramètres de configuration dans l'EEPROM
    while (millis() - startMillis < 30000) {
        if (Serial.available() > 0) {
            startMillis = millis();  // Réinitialisation du timer
            String input = Serial.readStringUntil('\n');
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
                    if (command == "CLOCK") {
                        int hours = value.substring(0, value.indexOf(':')).toInt();
                        int minutes = value.substring(value.indexOf(':') + 1, value.lastIndexOf(':')).toInt();
                        int seconds = value.substring(value.lastIndexOf(':') + 1).toInt();
                        Serial.println(hours);
                        Serial.println(minutes);
                        Serial.println(seconds);
                        if(hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59) {
                            clock.fillByHMS(hours, minutes, seconds);
                            clock.setTime();
                            Serial.println("Heure du système mise à jour.");
                        } else {
                            Serial.println("Erreur, veuillez saisir des valeurs entre 0 et 23 pour les heures, entre 0 et 59 pour les minutes et les secondes, au format hh:mm:ss.");
                        }
                    } else if (command == "DATE") {
                        int day = value.substring(0, value.indexOf('/')).toInt();
                        int month = value.substring(value.indexOf('/') + 1, value.lastIndexOf(':')).toInt();
                        int year = value.substring(value.lastIndexOf('/') + 1).toInt();
                        if(day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2000 && year <= 2099) {
                            clock.fillByYMD(year, month, day);
                            clock.setTime();
                            Serial.println("Date du système mise à jour.");
                        } else {
                            Serial.println("Erreur, veuillez saisir des valeurs entre 1 et 31 pour le jour, entre 1 et 12 pour le mois et entre 2000 et 2099 pour l'année, au format jj/mm/aaaa.");
                        }
                        clock.fillByYMD(2017,1,19);//Jan 19,2013
                        clock.setTime();
                    } else if (command == "DAY") {
                        if (value == "MON") {
                            clock.fillDayOfWeek(MON);
                        } else if (value == "TUE") {
                            clock.fillDayOfWeek(TUE);
                        } else if (value == "WED") {
                            clock.fillDayOfWeek(WED);
                        } else if (value == "THU") {
                            clock.fillDayOfWeek(THU);
                        } else if (value == "FRI") {
                            clock.fillDayOfWeek(FRI);
                        } else if (value == "SAT") {
                            clock.fillDayOfWeek(SAT);
                        } else if (value == "SUN") {
                            clock.fillDayOfWeek(SUN);
                        } else {
                            Serial.println("Erreur, veuillez saisir un jour de la semaine valide (MON, TUE, WED, THU, FRI, SAT, SUN)");
                        }
                        clock.setTime();
                    } else {
                        applyCommand(command, value, config);
                    }
                } else {
                    Serial.println("Erreur, commande incorrecte.");
                }
            }
        }
    }
    Serial.println("Inactivité de 30 secondes détectée, démarrage du système en mode Standard.");
}
