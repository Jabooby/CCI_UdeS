#include "Keyboard.h"


#define APP_BUTTON (GPIO_NUM_0) // Use BOOT signal by default


void SetupGPIOKeyboard()
{
    // Initialize button that will trigger HID reports
    const gpio_config_t boot_button_config = {
        .pin_bit_mask = BIT64(APP_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = false,
        .pull_down_en = false,
    };
    ESP_ERROR_CHECK(gpio_config(&boot_button_config));
}

void WriteASCII(uint8_t chr)
{
    uint8_t const conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };

    uint8_t keycode[6] = { 0 };
    uint8_t modifier   = 0;

    if ( conv_table[chr][0] ) 
        modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
    
    keycode[0] = conv_table[chr][1];
    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, modifier, keycode);

    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, NULL, NULL);
}

void WriteWordsASCII(uint8_t *chars, uint8_t size)
{
    for(int i = 0; i < size; i++)
    {
        WriteASCII(chars[i]);
    }
}

void WriteCharacter(unsigned long chr)
{
    uint8_t keycode[6] = { 0 };
    keycode[0] = chr &0x0000FF;
    keycode[1] = chr &0x00FF00;
    keycode[2] = chr &0xFF0000;
    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, NULL, keycode);

    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, NULL, NULL);
}

void WriteCombinationOfCharacterASCII(unsigned char chr)
{
    uint8_t const conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };

    uint8_t keycode[6] = { 0 };
    uint8_t modifier   = 0;

    if ( conv_table[chr][0] ) 
        modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
    
    keycode[0] = conv_table[chr][1];
    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, modifier, keycode);
}

void WriteCombinationOfCharacter(unsigned long chr)
{
    uint8_t keycode[6] = { 0 };
    keycode[0] = chr &0x0000FF;
    keycode[1] = chr &0x00FF00;
    keycode[2] = chr &0xFF0000;
    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, NULL, keycode);
}

void WriteEndCombination()
{
    while (!tud_hid_ready());
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, NULL, NULL);
}