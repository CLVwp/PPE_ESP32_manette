#ifndef TASK_DISPLAY_H
#define TASK_DISPLAY_H

/** Tâche FreeRTOS : affichage grille 3x3 OLED + gestion mode (bouton). */
void taskDisplay(void* pvParameters);

/** Initialise I2C et OLED. À appeler depuis setup() avant de créer les tâches. Retourne true si OK. */
bool display_begin();

#endif
