#include "Mouse.h"

#define APP_BUTTON (GPIO_NUM_0) // Use BOOT signal by default
#define JOY_X ADC1_CHANNEL_3
#define JOY_Y ADC1_CHANNEL_4
#define RIGHT_MOUSE_CLICK GPIO_NUM_6
#define LEFT_MOUSE_CLICK GPIO_NUM_7

#define MOUSE_SCALE_FACTOR -0.01
#define MOUSE_DEAD_ZONE 100
#define MOUSE_MIDDLE 1925

int8_t Delta_x;
int8_t Delta_y;
int8_t MouseClickValue;

int8_t Compteur = 0;

void MouseControl()
{
    static int oldMouseClick = 0;

    MouseDelta(&Delta_x, &Delta_y);
    MouseClick(&MouseClickValue);

    if(0 != Delta_x || 0 != Delta_y || oldMouseClick != MouseClickValue)
    {
        tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE, MouseClickValue, Delta_x, Delta_y, 0, 0);
        oldMouseClick = MouseClickValue;
    }
}

void MouseDelta(int8_t *delta_x_ret, int8_t *delta_y_ret)
{
    int joyX = adc1_get_raw(JOY_X);
    int joyY = adc1_get_raw(JOY_Y);

    if(joyX < MOUSE_MIDDLE-MOUSE_DEAD_ZONE)
    {
        *delta_x_ret = (joyX-MOUSE_MIDDLE-MOUSE_DEAD_ZONE)*-MOUSE_SCALE_FACTOR;
    }
    else if(joyX > MOUSE_MIDDLE+MOUSE_DEAD_ZONE)
    {
        *delta_x_ret = (joyX-MOUSE_MIDDLE-MOUSE_DEAD_ZONE)*-MOUSE_SCALE_FACTOR;
    }
    else
    {
        *delta_x_ret = 0;
    }

    if(joyY < MOUSE_MIDDLE-MOUSE_DEAD_ZONE)
    {
        *delta_y_ret = (joyY-MOUSE_MIDDLE-MOUSE_DEAD_ZONE)*MOUSE_SCALE_FACTOR;
    }
    else if(joyY > MOUSE_MIDDLE+MOUSE_DEAD_ZONE)
    {
        *delta_y_ret = (joyY-MOUSE_MIDDLE-MOUSE_DEAD_ZONE)*MOUSE_SCALE_FACTOR;
    }
    else
    {
        *delta_y_ret = 0;
    }
}

void MouseClick(int8_t *Click)
{
    static bool leftActive = false;
    static bool rightActive = false;

    if(gpio_get_level(LEFT_MOUSE_CLICK) == 1 && !leftActive)
    {
        leftActive = true;
        *Click = 0x01;
        return;
    }

    if(gpio_get_level(LEFT_MOUSE_CLICK) == 0 && leftActive)
    {
        leftActive = false;
        *Click = 0x00;
        return;
    }
    
    if(gpio_get_level(RIGHT_MOUSE_CLICK) == 1 && !rightActive)
    {
        rightActive = true;
        *Click = 0x02;
        return;
    }

    if(gpio_get_level(RIGHT_MOUSE_CLICK) == 0 && rightActive)
    {
        rightActive = false;
        *Click = 0x00;
        return;
    }
}

void SetupGPIOMouse()
{
    adc1_config_width(ADC_WIDTH_BIT_12); // 12-bit width
    adc1_config_channel_atten(JOY_X, ADC_ATTEN_DB_12); 
    adc1_config_channel_atten(JOY_Y, ADC_ATTEN_DB_12); 

    // Configure the GPIO pin as input
    gpio_config_t leftClick;
    leftClick.intr_type = GPIO_INTR_DISABLE;  // Disable interrupts for this pin
    leftClick.mode = GPIO_MODE_INPUT;         // Set as input mode
    leftClick.pin_bit_mask = (1ULL << LEFT_MOUSE_CLICK); // Bit mask for the pin
    leftClick.pull_down_en = GPIO_PULLDOWN_DISABLE; // No pull-down resistor
    leftClick.pull_up_en = GPIO_PULLUP_DISABLE; // No pull-up resistor
    gpio_config(&leftClick); // Apply the configuration

    // Configure the GPIO pin as input
    gpio_config_t rightClick;
    rightClick.intr_type = GPIO_INTR_DISABLE;  // Disable interrupts for this pin
    rightClick.mode = GPIO_MODE_INPUT;         // Set as input mode
    rightClick.pin_bit_mask = (1ULL << RIGHT_MOUSE_CLICK); // Bit mask for the pin
    rightClick.pull_down_en = GPIO_PULLDOWN_DISABLE; // No pull-down resistor
    rightClick.pull_up_en = GPIO_PULLUP_DISABLE; // No pull-up resistor
    gpio_config(&rightClick); // Apply the configuration
}