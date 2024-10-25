#include <SD.h>
#include <Seeed_BME280.h>
#include <SoftwareSerial.h>
#include "variables.h"

String acquerirDonnees(Config& config, DS1307& clock, BME280& sensor, SoftwareSerial& gps, Mode& currentMode, unsigned long& startTimer, int& fileNumber) {
    int logIntervalMin = currentMode == MODE_ECONOMIQUE ? config.logInterval.value*2 : config.logInterval.value;
    if(millis() - startTimer >= logIntervalMin*60000) {
        clock.getTime();
        String donnees =
            String(clock.hour) + ":" + String(clock.minute) + ":" + String(clock.second) + ";" +
            String(sensor.getTemperature()) + ";" +
            String(sensor.getPressure()) + ";" +
            String(sensor.getHumidity()) + ";" +
            String(analogRead(A0)) + ";";

        if(currentMode != MODE_ECONOMIQUE) {
            if (gps.available()) {
                String nmeaSentence = gps.readStringUntil('\n');
                Serial.print(nmeaSentence);
                if (nmeaSentence.startsWith("$GNRMC")) {
                    char delimiter = ',';
                    String elements[20];

                    int index = 0;
                    for (int i = 0; i < nmeaSentence.length(); i++) {
                        if (nmeaSentence[i] == delimiter) {
                            index++;
                        } else {
                            elements[index] += nmeaSentence[i];
                        }
                    }

                    String latitude = elements[3];
                    String longitude = elements[5];
                    donnees += latitude + ";" + longitude + ";";
                    startTimer = millis();
                    return donnees;
                }
            }
        }

        startTimer = millis();
        if(currentMode == MODE_MAINTENANCE) {
            Serial.print(donnees);
        } else {
            sauvegarderDonnees(donnees, clock, config, fileNumber);
        }
        return donnees;
    }
    return "";
}