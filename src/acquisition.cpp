#include "main.h"

// Fonction pour acquérir les données météo
void acquireGPS() {
    if (millis() - gpsTimer > config.timeout.value * 1000) {  // Si le timeout est atteint
        gpsState = FINISHED;  // Fin de l'acquisition
        data.latitude = NULL;  // Latitude nulle
        data.longitude = NULL;  // Longitude nulle
        errors[GPS] = true;  // Erreur GPS
        return;
    }

    if (gps.available()) {  // Si des données sont disponibles
        char gpsData[100];  // Buffer pour les données GPS
        gps.readBytesUntil('\n', gpsData, sizeof(gpsData) - 1);  // Lecture des données
        gpsData[sizeof(gpsData) - 1] = '\0';  // Ajout du caractère de fin de chaîne

        if (strncmp(gpsData, "$GNGGA", 6) == 0) {  // Si la trame est une trame GNGGA
            int fieldIndex = 0;  // Index du champ
            char* field = strtok(gpsData, ",");  // Extraction du premier champ

            while (field != NULL) {
                if (fieldIndex == 2 && strlen(field) > 0) {  // Si le champ est la latitude
                    data.latitude = atof(field) / 100.0;  // Conversion de la latitude
                } else if (fieldIndex == 4 && strlen(field) > 0) {  // Si le champ est la longitude
                    data.longitude = atof(field) / 100.0;  // Conversion de la longitude
                }

                fieldIndex++;  // Incrémentation de l'index
                field = strtok(NULL, ",");  // Extraction du champ suivant
            }

            if (data.latitude != NULL && data.longitude != NULL) {  // Si les coordonnées sont valides
                gpsState = FINISHED;  // Fin de l'acquisition
                errors[GPS] = false;  // Pas d'erreur GPS
            }
        }
    }
}

// Fonction pour acquérir les données météo
WeatherData acquireData() {  // Fonction pour acquérir les données météo
    clock.getTime();  // Récupération de l'heure actuelle
    data.hour = clock.hour;  // Récupération de l'heure
    data.minute = clock.minute;  // Récupération des minutes
    data.second = clock.second;  // Récupération des secondes

    errors[RTC] = (!data.hour || !data.minute || !data.second);  // Si l'heure n'est pas valide, erreur RTC

    if(config.lumin.value) {  // Si la luminosité est activée
        data.lumin = analogRead(A0);  // Lecture de la luminosité
        errors[SENSOR] = (data.lumin <= 0);  // Si la luminosité n'est pas valide, erreur de capteur
    }

    if(config.tempAir.value) {  // Si la température est activée
        data.temperature = sensor.getTemperature();  // Lecture de la température
        errors[DATA] = (data.temperature < config.minTempAir.value || data.temperature > config.maxTempAir.value);  // Si la température n'est pas valide, erreur de données
    }

    if(config.hygr.value) {  // Si l'humidité est activée
        data.humidity = sensor.getHumidity();  // Lecture de l'humidité
        errors[SENSOR] = (data.humidity <= 0);  // Si l'humidité n'est pas valide, erreur de capteur
    }

    if(config.pressure.value) {  // Si la pression est activée
        data.pressure = sensor.getPressure() / 100;  // Conversion de la pression en hPa
        errors[DATA] = (data.pressure < config.pressureMin.value || data.pressure > config.pressureMax.value);  // Si la pression n'est pas valide, erreur de données
    }

    gpsState = LOADING;  // Chargement des données GPS
    return data;  // Retour des données
}