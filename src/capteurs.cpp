#include "variables.h"
#include <SoftwareSerial.h>

unsigned char buffer[64];
int count=0;
const int lightPin = A0;
int lightSensorValue = 0;

void acquerirDonnees() {
  // Acquérir les données
}

  // Acquisition données capteur

void clearBufferArray()                     // function to clear buffer array
{
  for (int i=0; i<count;i++)
  {
    buffer[i]=NULL;
  }                      // clear all index of array with command NULL
}

void acquisition_donnee_gps() {
  SoftwareSerial SoftSerial(2, 3);
  unsigned char buffer[64];                   // buffer array for data receive over serial port
  int count=0;                                // counter for buffer array


  if (SoftSerial.available())                     // if date is coming from software serial port ==> data is coming from SoftSerial shield
  {
    while(SoftSerial.available())               // reading data into char array
    {
      buffer[count++]=SoftSerial.read();      // writing data into array
      if(count == 64)break;
    }
    Serial.write(reinterpret_cast<const char *>(buffer), count);                 // if no data transmission ends, write buffer to hardware serial port
    clearBufferArray();                         // call clearBufferArray function to clear the stored data from the array
    count = 0;                                  // set counter of while loop to zero
  }
  if (Serial.available())                 // if data is available on hardware serial port ==> data is coming from PC or notebook
  {
    SoftSerial.write(Serial.read());        // write it to the SoftSerial shield
  }
}

void acquisition_donnee_light() {
  analogRead(lightPin);
}

void acquisition_donnee_temperature() {

}




void sauvegarderDonnees(const Mode& currentMode) {
  if(currentMode != MODE_MAINTENANCE) {
    // Sauvegarde des données
  }
}