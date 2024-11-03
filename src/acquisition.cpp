#include "main.h"

enum AcquisitionEtat { EN_ATTENTE, EN_COURS, TERMINE };
AcquisitionEtat etatGPS = EN_ATTENTE;

void acquerirGPS(MeteoData &data) {
    if (etatGPS == EN_COURS) {
        if (millis() - startTimer > config.timeout.value * 1000) {
            // Timeout atteint, arrêt de l'acquisition
            etatGPS = TERMINE;
            return;
        }

        if (gps.available()) {
            char gpsData[100];
            gps.readBytesUntil('\n', gpsData, sizeof(gpsData) - 1);
            gpsData[sizeof(gpsData) - 1] = '\0';

            if (strncmp(gpsData, "$GNGGA", 6) == 0) {
                int fieldIndex = 0;
                char* field = strtok(gpsData, ",");

                while (field != NULL) {
                    if (fieldIndex == 2 && strlen(field) > 0) {
                        data.latitude = atof(field) / 100.0;
                    } else if (fieldIndex == 4 && strlen(field) > 0) {
                        data.longitude = atof(field) / 100.0;
                    }

                    fieldIndex++;
                    field = strtok(NULL, ",");
                }

                if (data.latitude != 0.0 && data.longitude != 0.0) {
                    etatGPS = TERMINE;
                }
            }
        }
    }
}

MeteoData acquerirDonnees() {
    clock.getTime();
    MeteoData data {
        data.hour = clock.hour,
        data.minute = clock.minute,
        data.second = clock.second,
        data.temperature = sensor.getTemperature(),
        data.humidity = sensor.getHumidity(),
        data.pressure = sensor.getPressure(),
        data.light = analogRead(A0)
    };

    if(currentMode != ECONOMIQUE) {
        startTimer = millis();
        data.latitude = NAN;
        data.longitude = NAN;
        while(data.latitude != NAN && data.longitude != NAN && millis() - startTimer < config.timeout.value * 1000) {
            if(gps.available()) {
                char gpsData[100]; // Assurez-vous que ce tableau est suffisamment grand pour contenir la ligne NMEA
                gps.readBytesUntil('\n', gpsData, sizeof(gpsData) - 1);
                gpsData[sizeof(gpsData) - 1] = '\0'; // Null-terminer le tableau

                if (strncmp(gpsData, "$GNGGA", 6) == 0) {
                    int fieldIndex = 0;
                    char* field = strtok(gpsData, ",");

                    // Découper la trame NMEA en champs
                    while (field != NULL) {
                        // Extraire la latitude et la longitude aux positions spécifiques
                        if (fieldIndex == 2 && strlen(field) > 0) {
                            data.latitude = atof(field) / 100.0; // Convertir la latitude en float
                        } else if (fieldIndex == 4 && strlen(field) > 0) {
                            data.longitude = atof(field) / 100.0; // Convertir la longitude en float
                        }

                        fieldIndex++;
                        field = strtok(NULL, ",");
                    }
                }
            }
        }
    }

    return data;
}