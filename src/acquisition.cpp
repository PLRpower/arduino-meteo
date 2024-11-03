#include "main.h"

GpsState gpsState = WAITING;

void acquerirGPS() {
    if (millis() - gpsTimer > config.timeout.value * 1000) {
        gpsState = FINISHED;
        data.latitude = NULL;
        data.longitude = NULL;
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

            if (data.latitude != NULL && data.longitude != NULL) {
                gpsState = FINISHED;
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
    };

    if(config.lumin.value) {
        data.light = analogRead(A0);
    }

    if(config.tempAir.value) {
        data.temperature = sensor.getTemperature();
    }

    if(config.hygr.value) {
        data.humidity = sensor.getHumidity();
    }

    if(config.pressure.value) {
        data.pressure = sensor.getPressure();
    }

    if(currentMode != ECONOMIQUE) {
        gpsState = LOADING;
    }

    return data;
}