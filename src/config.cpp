#include <EEPROM.h>
#include "main.h"

void setParameter(Parameter &param, int newValue) {
    if (newValue >= param.minValue && newValue <= param.maxValue) {
        param.value = newValue;
        Serial.println(F("Paramètre mis à jour"));
    } else {
        Serial.println(F("Erreur: plage"));
    }
}

void applyCommand(const char* command, int value) {
    static const struct {
        const char* name;
        Parameter* param;
        } commandMap[] = {
        {PSTR("LOG_INTERVAL"), &config.logInterval},
        {PSTR("FILE_MAX_SIZE"), &config.fileMaxSize},
        {PSTR("TIMEOUT"), &config.timeout},
        {PSTR("LUMIN"), &config.lumin},
        {PSTR("LUMIN_LOW"), &config.luminLow},
        {PSTR("LUMIN_HIGH"), &config.luminHigh},
        {PSTR("TEMP_AIR"), &config.tempAir},
        {PSTR("MIN_TEMP_AIR"), &config.minTempAir},
        {PSTR("MAX_TEMP_AIR"), &config.maxTempAir},
        {PSTR("HYGR"), &config.hygr},
        {PSTR("HYGR_MIN_T"), &config.hygrMinT},
        {PSTR("HYGR_MAX_T"), &config.hygrMaxT},
        {PSTR("PRESSURE"), &config.pressure},
        {PSTR("PRESSURE_MIN"), &config.pressureMin},
        {PSTR("PRESSURE_MAX"), &config.pressureMax},
    };

    for (auto& cmd : commandMap) {
        if (!strcmp_P(command, cmd.name)) {
            setParameter(*cmd.param, value);
            return;
        }
    }
    Serial.println(F("Erreur: COMMANDE"));
}

void checkUserInput() {
    char input[32];
    if (Serial.readBytesUntil('\n', input, sizeof(input)) > 0) {
        input[strcspn(input, "\r")] = 0;
        char* command = strtok(input, "=");
        char* value = strtok(NULL, "=");

        if (!strcmp_P(command, PSTR("RESET"))) {
            config = Config();
            Serial.println(F("Config réinitialisée"));
        } else if (!strcmp_P(command, PSTR("VERSION"))) {
            Serial.println(F("Version : v-final"));
            Serial.println(F("Lot : Groupe4"));
        } else if (!strcmp_P(command, PSTR("CLOCK")) && value) {
            byte hours = atoi(value);
            byte minutes = atoi(value + 3);
            byte seconds = atoi(value + 6);

            if (hours <= 23 && minutes <= 59 && seconds <= 59) {
                clock.getTime();
                clock.fillByHMS(hours, minutes, seconds);
                clock.fillByYMD(clock.year + 2000, clock.month, clock.dayOfMonth);
                clock.fillDayOfWeek(clock.dayOfWeek);
                clock.setTime();
                Serial.println(F("Heure du système mise à jour."));
            } else {
                Serial.println(F("Erreur : DONNEES"));
            }
        } else if (!strcmp_P(command, PSTR("DATE")) && value) {
            byte day = atoi(value);
            byte month = atoi(value + 3);
            int year = atoi(value + 6);

            if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2000 && year <= 2099) {
                clock.getTime();
                clock.fillByHMS(clock.hour, clock.minute, clock.second);
                clock.fillByYMD(year, month, day);
                clock.fillDayOfWeek(clock.dayOfWeek);
                clock.setTime();
                Serial.println(F("Date du système mise à jour."));
            } else {
                Serial.println(F("Erreur : DONNEES"));
            }
        } else if (!strcmp_P(command, PSTR("DAY")) && value) {
            const char daysOfWeek[7][4] PROGMEM = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
            int dayIndex = -1;

            for (int i = 0; i < 7; i++) {
                if (!strcmp(value, daysOfWeek[i])) {
                    dayIndex = i; // Trouvé, stocker l'index
                    clock.getTime();
                    clock.fillDayOfWeek(i + 1); // Remplir le jour de la semaine
                    clock.fillByHMS(clock.hour, clock.minute, clock.second);
                    clock.fillByYMD(clock.year + 2000, clock.month, clock.dayOfMonth);
                    clock.setTime();
                    Serial.println(F("Jour de la semaine mis à jour."));
                    break;
                }
            }

            if(dayIndex == -1) {
                Serial.println(F("Erreur : DONNEES"));
            }
        } else {
            applyCommand(command, value ? atoi(value) : 0);
        }
        EEPROM.put(0, config);
    } else {
        Serial.println(F("Inactivité détectée"));
        setMode(STANDARD);
    }
}