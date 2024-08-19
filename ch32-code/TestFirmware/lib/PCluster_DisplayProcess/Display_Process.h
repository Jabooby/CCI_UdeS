/**
 * @file Display_Process.h
 * @author Renaud Gagnon
 * @brief Processus d'affichage des écrans et des aiguilles pour le PCluster
 * @version 0.1
 * @date 2024-03-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifdef __cplusplus
extern "C" {
#endif

// Include C library headers here
#include "../../include/main.h"
#include "LED_Animate.h"
#include "PCluster_Oled.h"
#include "PCluster_Motors.h"

#ifdef __cplusplus
}
#endif

#ifndef DISPLAY_PROCESS_H
#define DISPLAY_PROCESS_H

#define NEEDLE_BUFFERING_AMOUNT 20

extern PClusterMotor *Needles[4];

/**
 * @brief Initialize the display process
 * 
 */
void DisplayProcess_InitDisplayProcess(void);

/**
 * @brief Update needle and screens on the display
 * 
 */
void DisplayProcess_UpdateScreens(void);

/**
 * @brief Set the Needles object
 * 
 * @param needle ID of the needle to set
 * @param value Value to set to the needle of specified ID
 */
void DisplayProcess_SetNeedle(uint8_t needle, uint16_t value);

/**
 * @brief Set the value to display on screen of specified ID
 * 
 * @param screen 
 * @param value 
 */
void DisplayProcess_SetScreen(uint8_t screen, uint8_t value);

void DisplayProcess_PollMotors(void);
bool DisplayProcess_AllMotorsDone(void);
void DisplayProcess_SetLEDMode(u8 mode, u8 brightness);

#endif



