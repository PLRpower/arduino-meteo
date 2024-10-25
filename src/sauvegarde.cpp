#include <SD.h>
#include "variables.h"

void sauvegarderDonnees(String& data, DS1307& clock, Config& config, int& fileNumber) {
    clock.getTime();
    char fileName[14];
    sprintf(fileName, "%02d%02d%02d_0.LOG", 20, clock.month, clock.dayOfMonth);

    File file = SD.open(fileName, FILE_WRITE);
    if(file.size() + data.length() > config.fileMaxSize.value) {

    }
    Serial.print("test");
    file.println(data);
    file.close();

}