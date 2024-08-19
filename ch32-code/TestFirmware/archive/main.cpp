#ifdef __cplusplus
extern "C" {
#endif

// Include C library headers here
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_int.h"
#include "usb_mem.h"
#include "usb_sil.h"

#ifdef __cplusplus
}
#endif


#define USB_BUFFER_SIZE 64

void usb_init(void);
void usb_passthrough(void);

uint8_t usb_buffer[USB_BUFFER_SIZE];


void usb_init(void) {
    USB_Init();
    Set_USBConfig();
    USB_Interrupts_Config();
}

void usb_passthrough(void) {
    uint16_t bytes_received;

    // Check if data is received from the host
    bytes_received = USB_SIL_Read(ENDP1, usb_buffer);
    if (bytes_received > 0) {
        // Send the received data to the device
        USB_SIL_Write(ENDP2, usb_buffer, bytes_received);
        SetEPTxValid(ENDP2);
    }

    // Check if data is received from the device
    bytes_received = USB_SIL_Read(ENDP2, usb_buffer);
    if (bytes_received > 0) {
        // Send the received data to the host
        USB_SIL_Write(ENDP1, usb_buffer, bytes_received);
        SetEPTxValid(ENDP1);
    }
}

int main(void) {
    // System initialization
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//Cpu shit
    SystemCoreClockUpdate();    //More CPU shit
	Delay_Init();
    // USB initialization
    usb_init();

    // Main loop
    while (1) {
        // Implement the passthrough logic
        usb_passthrough();
    }

    return 0;
}
