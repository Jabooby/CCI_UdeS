#ifndef TIMEBASE_H
#define TIMEBASE_H

#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define SERVICEBASEDETEMPS_NOMBRE_DE_PHASES 2
#define SERVICEBASEDETEMPS_NOMBRE_DE_PHASES_DANS_LOOP 3
#define TIMER_COOLDOWN_US 100

extern void (*TimeBasedTasks[SERVICEBASEDETEMPS_NOMBRE_DE_PHASES])(void);

void InitializeTimer();
void serviceBaseDeTemps_initialise();

#endif