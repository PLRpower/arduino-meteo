#include "variables.h"
#include <SoftwareSerial.h>

unsigned char buffer[64];
int count=0;
int sensorValue = 0;

void acquerirDonnees() {
    int lightData = analogRead(A0);
}

  // Acquisition données capteur

void clearBufferArray() {
    for (int i=0; i<count;i++) {
        buffer[i]=NULL;
    }
}

void acquisition_donnee_gps() {
    SoftwareSerial SoftSerial(2, 3);
    unsigned char buffer[64];
    int count=0;

    if (SoftSerial.available()) {
        while(SoftSerial.available()) {
            buffer[count++]=SoftSerial.read();
            if(count == 64)break;
        }
        Serial.write(reinterpret_cast<const char *>(buffer), count);
        clearBufferArray();
        count = 0;
    }

    if (Serial.available()) {
        SoftSerial.write(Serial.read());
    }
}

void acquisition_donnee_temperature() {

}