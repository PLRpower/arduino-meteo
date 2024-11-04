#include "main.h"

// Fonction pour sauvegarder les données sur la carte SD
void saveData() {
    clock.getTime();  // Récupération de l'heure actuelle
    char fileName[] = "20xxxx_0.LOG";  // Nom du fichier de log
    fileName[2] = '0' + clock.month / 10;  // Mise à jour du mois avec les dizaines
    fileName[3] = '0' + clock.month % 10;  // Mise à jour du mois avec les unités
    fileName[4] = '0' + clock.dayOfMonth / 10;   // Mise à jour du jour avec les dizaines
    fileName[5] = '0' + clock.dayOfMonth % 10;  // Mise à jour du jour avec les unités

    File32 file = SD.open(fileName, FILE_WRITE);  // Ouverture du fichier
    if (file) {  // Si le fichier est ouvert
        if(file.size() + sizeof(data) > config.fileMaxSize.value) {  // Si la taille du fichier dépasse la taille maximale
            int fileIndex = 1;  // Index du fichier
            char newFileName[] = "20xxxx_0.LOG";  // Nouveau nom de fichier
            do {  // Tant que le fichier existe
                newFileName[7] = '0' + fileIndex++;  // Mise à jour de l'index
            } while (SD.exists(newFileName));

            File newFile = SD.open(newFileName, FILE_WRITE);  // Ouverture du nouveau fichier
            if (newFile) {  // Si le fichier est ouvert
                char buffer[32];  // Buffer pour la copie des données
                while (file.available()) {  // Tant que le fichier contient des données
                    int bytesRead = file.readBytes(buffer, sizeof(buffer));  // Lecture des données
                    newFile.write(buffer, bytesRead);  // Écriture des données dans le nouveau fichier
                }
                newFile.close();  // Fermeture du nouveau fichier
            }
            file.close();  // Fermeture de l'ancien fichier
            file.remove();  // Suppression de l'ancien fichier
            file = SD.open(fileName, FILE_WRITE);  // Réouverture de l'ancien fichier
        }

        file.print(data.hour);
        file.print(':'); file.print(data.minute);
        file.print(':'); file.print(data.second);
        file.print(';'); file.print(data.temperature);
        file.print(';'); file.print(data.humidity);
        file.print(';'); file.print(data.pressure);
        file.print(';'); file.print(data.lumin); file.print(';');

        if (data.latitude != NULL && data.longitude != NULL) {  // Si les coordonnées GPS sont valides
            file.print(data.latitude); file.print(';'); file.println(data.longitude);  // Écriture des coordonnées GPS
        } else {
            file.println(F("NA;NA;"));  // Sinon, écriture de NA pour les coordonnées GPS
        }
        file.close();  // Fermeture du fichier
        errors[SD_RW] = false;  // Réinitialisation de l'erreur d'écriture
    }
    else {
        errors[SD_RW] = true;  // Si le fichier n'est pas ouvert, erreur d'écriture
    }
}