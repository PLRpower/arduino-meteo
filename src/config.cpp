#include "main.h"

const char daysOfWeek[7][4] PROGMEM = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};  // Jours de la semaine

// Fonction pour mettre à jour un paramètre
void setParameter(Parameter &param, int newValue) {
    if (newValue >= param.minValue && newValue <= param.maxValue) {  // Si la nouvelle valeur est dans la plage autorisée
        param.value = newValue;  // Mise à jour du paramètre
        Serial.println(F("Paramètre mis à jour"));
    } else {
        Serial.println(F("Erreur: plage"));
    }
}

// Fonction pour appliquer une commande
void applyCommand(const char* command, int value) {
    static const struct {
        const char* name;  // Nom de la commande
        Parameter* param;  // Paramètre associé
        } commandMap[] = {  // Tableau associatif des commandes
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

    for (auto& cmd : commandMap) {  // Parcours du tableau des commandes
        if (!strcmp_P(command, cmd.name)) {  // Si la commande est trouvée
            setParameter(*cmd.param, value);  // Mise à jour du paramètre
            return;
        }
    }
    Serial.println(F("Erreur: COMMANDE"));  // Si la commande n'est pas trouvée
}

// Fonction appelée en permanence pendant le mode configuration pour vérifier les entrées utilisateur
void checkUserInput() {
    char input[32];  // Buffer pour stocker l'entrée utilisateur
    if (Serial.readBytesUntil('\n', input, sizeof(input)) > 0) {  // Si une entrée est disponible
        input[strcspn(input, "\r")] = 0;  // Suppression du caractère du retour à la ligne (pour éviter des problèmes)
        char* command = strtok(input, "=");  // Extraction de la commande
        char* value = strtok(NULL, "=");  // Extraction de la valeur

        if (!strcmp_P(command, PSTR("RESET"))) {  // Si la commande est RESET
            config = Config();  // Réinitialisation de la configuration
            Serial.println(F("Config réinitialisée"));

        } else if (!strcmp_P(command, PSTR("VERSION"))) {  // Si la commande est VERSION
            Serial.println(F("Version : v-final"));
            Serial.println(F("Lot : Groupe4"));

        } else if (!strcmp_P(command, PSTR("CLOCK")) && value) {  // Si la commande est CLOCK
            byte hours = atoi(value);  // Extraction des heures avec atoi (conversion ASCII -> entier)
            byte minutes = atoi(value + 3);  // Extraction des minutes (décalage de 3 caractères)
            byte seconds = atoi(value + 6);  // Extraction des secondes (décalage de 6 caractères)
            if (hours <= 23 && minutes <= 59 && seconds <= 59) {  // Si les valeurs sont valides
                clock.getTime();  // Récupération de l'heure actuelle
                clock.fillByHMS(hours, minutes, seconds);  // Mise à jour de l'heure
                clock.fillByYMD(clock.year + 2000, clock.month, clock.dayOfMonth);  // Récupération de la date actuelle
                clock.fillDayOfWeek(clock.dayOfWeek);  // Récupération du jour de la semaine actuel
                clock.setTime();  // Mise à jour de l'horloge
                Serial.println(F("Heure du système mise à jour."));
            } else {
                Serial.println(F("Erreur : DONNEES"));
            }

        } else if (!strcmp_P(command, PSTR("DATE")) && value) {  // Si la commande est DATE
            byte day = atoi(value);  // Extraction du jour avec atoi (conversion ASCII -> entier)
            byte month = atoi(value + 3);  // Extraction du mois (décalage de 3 caractères)
            int year = atoi(value + 6);  // Extraction de l'année (décalage de 6 caractères)

            if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2000 && year <= 2099) {  // Si les valeurs sont valides
                clock.getTime();  // Récupération de l'heure actuelle
                clock.fillByHMS(clock.hour, clock.minute, clock.second);  // Récupération de l'heure actuelle
                clock.fillByYMD(year, month, day);  // Mise à jour de la date
                clock.fillDayOfWeek(clock.dayOfWeek);  // Récupération du jour de la semaine actuel
                clock.setTime();  // Mise à jour de l'horloge
                Serial.println(F("Date du système mise à jour."));
            } else {
                Serial.println(F("Erreur : DONNEES"));
            }

        } else if (!strcmp_P(command, PSTR("DAY")) && value) {  // Si la commande est DAY
            int dayIndex = -1;  // Index du jour de la semaine
            for (int i = 0; i < 7; i++) {  // Parcours des jours de la semaine
                if (!strcmp(value, daysOfWeek[i])) {  // Si le jour est trouvé
                    dayIndex = i;  // Mise à jour de l'index
                    clock.getTime();  // Récupération de l'heure actuelle
                    clock.fillDayOfWeek(i + 1);  // Mise à jour du jour de la semaine
                    clock.fillByHMS(clock.hour, clock.minute, clock.second);  // Récupération de l'heure actuelle
                    clock.fillByYMD(clock.year + 2000, clock.month, clock.dayOfMonth);  // Récupération de la date actuelle
                    clock.setTime();  // Mise à jour de l'horloge
                    Serial.println(F("Jour de la semaine mis à jour."));
                    break;
                }
            }

            if(dayIndex == -1) {  // Si le jour n'est pas trouvé
                Serial.println(F("Erreur : DONNEES"));
            }
        } else {  // Si la commande est une commande de configuration
            applyCommand(command, value ? atoi(value) : 0);  // Application de la commande avec la fonction applyCommand
        }
        EEPROM.put(0, config);  // Enregistrement de la configuration dans l'EEPROM
    } else {  // Si la communication série est inactive (pas d'entrée depuis 30 minutes)
        Serial.println(F("Inactivité détectée"));
        setMode(STANDARD);  // Passage en mode standard
    }
}