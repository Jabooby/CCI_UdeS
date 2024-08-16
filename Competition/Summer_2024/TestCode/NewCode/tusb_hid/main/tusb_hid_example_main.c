/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_random.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "esp_check.h"
#include "tusb_msc_storage.h"
#include "tusb_cdc_acm.h"
#include "tinyusb.h"
#include "class/hid/hid_device.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "TimeBased.h"
#include "Macro.h"
#include "Storage.h"

#define VENDOR_ID 0x303A
#define PRODUCT_ID 0x4004
#define BLINK_GPIO 33
#define BASE_PATH "" // base path to mount the partition

static const char *TAG = "example";
uint8_t ReceivedValue = 0;
bool LEDValue = false;
bool HackingEnabled = false;

const char *filename = BASE_PATH "/Hack/Mystery1.bat\0";
const char *filename1 = BASE_PATH "/Hack/Mystery2.bat\0";
const char *filename2 = BASE_PATH "/Hack/Mystery3.bat\0";
const char *filename3 = BASE_PATH "/Hack/Mystery4.bat\0";
const char *filename4 = "/ESP/TEST.TXT\0";

void MouseStuff();
void OpenFileHack();
/************* TinyUSB descriptors ****************/

#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN + TUD_MSC_DESC_LEN)

/**
 * @brief HID report descriptor
 *
 * In this example we implement Keyboard + Mouse HID device,
 * so we must define both report descriptors
 */
const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE))
};

const char* hid_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "TinyUSB",             // 1: Manufacturer
    "TinyUSB Device",      // 2: Product
    "123456",              // 3: Serials, should use chip ID
    "Example HID interface",  // 4: HID
};

/**
 * @brief Configuration descriptor
 *
 * This is a simple configuration descriptor that defines 1 configuration and 1 HID interface
 */
static const uint8_t hid_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),

    // MSC Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(1, 5, 0x02, 0x82, 64),
};

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
    return hid_report_descriptor;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    ReceivedValue = buffer[0];
    LEDValue = !LEDValue;
    gpio_set_level(BLINK_GPIO, LEDValue);
    
}


static tusb_desc_device_t descriptor_config = {
    .bLength = sizeof(descriptor_config),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x303A, // This is Espressif VID. This needs to be changed according to Users / Customers
    .idProduct = 0x4002,
    .bcdDevice = 0x100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

void app_main(void)
{
    SetupGPIOMouse();
    SetupGPIOKeyboard();   
    serviceBaseDeTemps_initialise();
    TimeBasedTasks[0] = MouseStuff;
    TimeBasedTasks[1] = OpenFileHack;
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    InitializeStorage();

    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = &descriptor_config,
        .string_descriptor = hid_string_descriptor,
        .string_descriptor_count = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]),
        .external_phy = false,
        .configuration_descriptor = hid_configuration_descriptor,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");

    InitializeTimer();
    
    //fopen(filename4, "r");
    
    while (1) {
        
    }
}

void MouseStuff()
{
    static bool active = false;
    static uint32_t counter = 0;
    
    counter++;
    if(counter < 250)
        return;
    counter = 0;

    if(gpio_get_level(GPIO_NUM_0) == 0 && !active)
    {
        active = true;
        HackingEnabled = !HackingEnabled;
        OpenDocument(filename);
        GetRickRolled();
    }
    if(gpio_get_level(GPIO_NUM_0) == 1 && active)
    {
        active = false;
    }

    MouseControl();
}

void OpenFileHack()
{
    static uint32_t counter = 0;
    counter++;
    if(counter < 100)
        return;
    counter = 0;
    //OpenDocument(filename2);

    if(!HackingEnabled)
    {
        return;
    }

    uint32_t random_value = esp_random() % 101;

    switch (random_value)
    {
    case 21:
        OpenDocument(filename);
        break;

    case 73:
        OpenDocument(filename1);
        break;

    case 54:
        OpenDocument(filename2);
        break;

    case 32:
        OpenDocument(filename3);
        break;
    
    default:
        break;
    }
}