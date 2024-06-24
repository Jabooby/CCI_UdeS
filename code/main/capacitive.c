#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "capacitive.h"

static void capacitive_read_task()
{
    uint32_t touch_value;

    /* Wait touch sensor init done */
    vTaskDelay(100 / portTICK_PERIOD_MS);
    printf("Touch Sensor read, the output format is: \nTouchpad num:[raw data]\n\n");

    while (1)
    {
        int i = TOUCH_BUTTON_NUM;
        touch_pad_read_raw_data(TOUCH_BUTTON_NUM, &touch_value); // read raw data.
        printf("T%d: [%4" PRIu32 "] ", TOUCH_BUTTON_NUM, touch_value);

        printf("\n");
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void capacitive_init()
{
    /* Initialize touch pad peripheral. */
    touch_pad_init();

    touch_pad_config(TOUCH_BUTTON_NUM);

    /* Initialize touch pad peripheral. */
    touch_pad_init();
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++)
    {
        touch_pad_config(button[i]);
    }

    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };

    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();
    ESP_LOGI(TAG, "Denoise function init");

    /* Enable touch sensor clock. Work mode is "timer trigger". */
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();
}