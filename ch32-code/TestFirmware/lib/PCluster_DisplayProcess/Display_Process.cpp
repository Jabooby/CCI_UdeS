/**
 * @file Display_Process.c
 * @author Renaud Gagnon
 * @brief Processus d'affichage des écrans et des aiguilles pour le PCluster
 * 			permet d'organiser en un seul endroit les fonctions de gestion des 
 * 			aiguilles et des écrans du PCluster.
 * @version 0.1
 * @date 2024-03-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Display_Process.h"
#include "PCluster_Oled.h"

u8 screenValues[4] = {0};
u8 oldScreenValues[4] = {0};
//PClusterMotor stepper1(STEPPER1);
//PClusterMotor stepper2(STEPPER2);
//PClusterMotor stepper3(STEPPER3);
//PClusterMotor stepper4(STEPPER4);
PClusterMotor *Needles [4];
u8 LEDBrightness = 100;
u8 LEDMode = LEDMODE_RAINBOW;
u8 needleValues[4] = {0};


void InitScreens (void);		//Private function to initialize the screens

/**
 * @brief Initialize the display process
 * 
 */
void DisplayProcess_InitDisplayProcess(void)
{
	Needles[0] = new PClusterMotor(STEPPER1);
	Needles[1] = new PClusterMotor(STEPPER2);
	Needles[2] = new PClusterMotor(STEPPER3);
	Needles[3] = new PClusterMotor(STEPPER4);
	InitScreens();
	LEDAnimate_init();
	LEDAnimate_update(LEDMODE_RAINBOW, 10, screenValues);

	while(!Needles[0]->InitIsDone()) 
    {
        Delay_Us(250);
        DisplayProcess_PollMotors();
    }
    
	
}

/**
 * @brief Update needle and screens on the display
 * 
 */
void DisplayProcess_UpdateScreens(void)
{
	//Update Screens
	for (int i = 0; i <= 3; i++)
	{
		if (screenValues[i] != oldScreenValues[i])
		{
			oldScreenValues[i] = screenValues[i];

			I2C_SelectChannel(i+1);
    		oled_setbuf(0);
    		if (screenValues[i] == 1) oled_drawstr_sz(24, 18, "CPU %", 1, fontsize_16x16);
    		else if (screenValues[i] == 2) oled_drawstr_sz(24, 18, "CPU Temp", 1, fontsize_16x16);
    		else if (screenValues[i] == 3) oled_drawstr_sz(24, 18, "MEM %", 1, fontsize_16x16);
    		else if (screenValues[i] == 4) oled_drawstr_sz(24, 18, "GPU%", 1, fontsize_16x16);
            else if (screenValues[i] == 4) oled_drawstr_sz(24, 18, "GPU Temp", 1, fontsize_16x16);
    		oled_refresh();
		}
	}    
}

/**g
 * @brief Set the Needles object. It also buffers the value 
 * 			to smooth the needle movement.
 * 
 * @param needle ID of the needle to set (1 to 4)
 * @param value Value to set to the needle of specified ID
 */
void DisplayProcess_SetNeedle(uint8_t needle, uint16_t value)
{
	
	static uint16_t buffer[4] [NEEDLE_BUFFERING_AMOUNT] = {0};
	static u16 index[4] = {0};
	float bufferedValue = 0;
	u16 temp = 0;
	buffer[needle-1][index[needle-1]] = value;
	for (int i = 0; i < NEEDLE_BUFFERING_AMOUNT; i++)
	{
		temp += buffer[needle-1][i];
	}
	bufferedValue = temp/NEEDLE_BUFFERING_AMOUNT;
	Needles[needle-1]->SetPercent(bufferedValue);
	needleValues[needle-1] = bufferedValue;
	if (index[needle-1]<NEEDLE_BUFFERING_AMOUNT-1)index[needle-1]++;
	else index[needle-1] = 0;
}

/**
 * @brief Set the Screen object
 * 
 * @param screen Id of the screen to set (1 to 4)
 * @param value Value to set to the screen of specified ID
 */
void DisplayProcess_SetScreen(uint8_t screen, uint8_t value)
{
    
    if (screen < 1 || screen > 4) return;
	screenValues[screen-1] = value;
}

/**
 * @brief Set the LED mode and brightness
 * 
 * @param mode 
 * @param brightness 0 to 100
 */
void DisplayProcess_SetLEDMode(u8 mode, u8 brightness)
{
	LEDMode = mode;
	LEDBrightness = brightness;
}

/**
 * @brief Polls the motors for updating their position
 * 
 */
void DisplayProcess_PollMotors(void)
{
	static u16 ledAnimateCounter = 0;
	static u16 needlePositions[4] = {0};
	static u8 needleValues[4] = {0};
	for(int i=0 ; i<=3 ; i++) Needles[i]->poll();
	ledAnimateCounter++;
	if(ledAnimateCounter++ >= 5)
	{
		for(int i=0 ; i<=3 ; i++) 
		{
			needlePositions[i] = Needles[i]->stepper->currentPosition();
			needleValues[i] = ((needlePositions[i])*100)/MAX_MOTOR_VALUE;
		}
		LEDAnimate_update(LEDMode, LEDBrightness, needleValues);
		ledAnimateCounter = 0;
	};
}

bool DisplayProcess_AllMotorsDone(void)
{
	return (Needles[0]->stepper->distanceToGo()==0 && Needles[1]->stepper->distanceToGo()==0 && Needles[2]->stepper->distanceToGo()==0 && Needles[3]->stepper->distanceToGo()==0);
}

/**
 * @brief Private function to initialize the screens
 * 
 */
void InitScreens (void)
{
    oled_init();
    for(uint8_t i = 1 ; i<= 4 ; i++)
    {
        I2C_SelectChannel(i);
        oled_sendInitCommands();
        oled_setbuf(0);
        oled_refresh();
    }
}

void InitLEDs (u8* values)
{
    LEDAnimate_init();
    //Set needle leds values
    LED_Interface_Set(0, 0, 0x0F0000);
    LED_Interface_Set(0, 12, 0xFFFFFF);
    LED_Interface_Refresh();

    //Set the led ring to rainbow mode
    LEDAnimate_update(LEDMODE_RAINBOW, 10, values);
}