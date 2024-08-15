#include "TimeBased.h"

void (*TimeBasedTasks[SERVICEBASEDETEMPS_NOMBRE_DE_PHASES])(void);

void serviceBaseDeTemps_neFaitRien()
{
}

void serviceBaseDeTemps_gere()
{
    unsigned char i;
    for (i = 0; i < SERVICEBASEDETEMPS_NOMBRE_DE_PHASES; i++)
    {
        TimeBasedTasks[i]();
    }
}
    

void serviceBaseDeTemps_initialise()
{
    unsigned char i;
    for (i = 0; i < SERVICEBASEDETEMPS_NOMBRE_DE_PHASES; i++)
    {
        TimeBasedTasks[i] = serviceBaseDeTemps_neFaitRien;
    }
}

void InitializeTimer()
{
    const esp_timer_create_args_t my_timer_args = {
        .callback = &serviceBaseDeTemps_gere,
        .name = "TimeBasedService"};
    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, TIMER_COOLDOWN_US));
}