/**
 * @file i2c_custom.h
 * @author Renaud Gagnon
 * @brief This file is a driver for the ch32v003. 
 *          It enables multiplexing on 4 different scl lines but one sda.
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "i2c_custom.h"
#include <wiring.h>
#include "../../include/main.h"

u8 customI2CPins[] = {PIN_SDA, PIN_SCL1, PIN_SCL2 , PIN_SCL3, PIN_SCL4};
u8 channel = 1;

//Private function prototypes
unsigned char ucLire8BitsI2C (u8 bAckValue);
void vEcrire8BitsI2C (unsigned char ucTxData);
void i2cDelay (void);
void I2CWriteBit (u8 bInput);
u8 I2CReadBit ();
 void I2CStartBit();
void I2CStopBit();


void i2c_init()
{
	digitalWrite(PIN_SDA, 1);
	digitalWrite(PIN_SCL1, 1);
	digitalWrite(PIN_SCL2, 1);
	digitalWrite(PIN_SCL3, 1);
	digitalWrite(PIN_SCL4, 1);
    pinMode(PIN_SDA, pinMode_O_openDrain);
    pinMode(PIN_SCL1, pinMode_O_openDrain);
    pinMode(PIN_SCL2, pinMode_O_openDrain);
    pinMode(PIN_SCL3, pinMode_O_openDrain);
    pinMode(PIN_SCL4, pinMode_O_openDrain);
	//Delay_Ms(1);
}

void I2C_SelectChannel (u8 selection)
{
	digitalWrite(customI2CPins[channel], 0); 				//Set previous channel to low
    if (selection>=1 && selection<=4)channel = selection; 	//Set new channel
}

u8 i2c_send(uint8_t addr, uint8_t *data, uint8_t sz)
{
    I2CStartBit();
    int i = 0;
    vEcrire8BitsI2C(addr<<1);
    for (i = 0 ; i<=(sz-1) ; i++) vEcrire8BitsI2C(data[i]);
    I2CStopBit();
    return 0;
}
// *************************************************************************************************
//  Auteur: Renaud Gagnon
//  Date de cr�ation :  09-09-2021			      
//	Description : 	fonction de r�ception de 8 bits de donn�e 
//			provenant d'une communication I2C.
//
//	Param�tres d'entr�e : 	Valeur du ack voulu (0x00 = 0 et 0xFF = 1).		
//	Param�tres de sortie : 	Byte re�u.		
//
unsigned char ucLire8BitsI2C (u8 bAckValue)
{
	u8 ucOutput = 0;
	u8 i;
	for (i = 0 ; i <= 7 ; i++)
	{
		ucOutput = (ucOutput << 1) | I2CReadBit();		
	}
	I2CWriteBit (bAckValue);
	return ucOutput;
}
// *************************************************************************************************



// *************************************************************************************************
//  Auteur: Renaud Gagnon
//  Date de cr�ation :  09-09-2021
//
//  Description: Routine d'envoie de 8 bits de donn�es en I2C.
//  Param�tres d'entr�es : Octet que l'on veut envoyer
//  Param�tres de sortie : �tat du ack venant du slave
//  Notes     		 			 : Aucune
//
void vEcrire8BitsI2C (unsigned char ucTxData)
{
	u8 i;
	for (i = 0 ; i < 8 ; i++)
	{
		if ((ucTxData & 0x80) == 0x80)//(ucTxData & (0x80 >> i))
		{
		I2CWriteBit (1);
		}
		else I2CWriteBit (0);
		ucTxData = ucTxData << 1;
	}
	digitalWrite(PIN_SDA, 1);
	I2CReadBit();
	
}
// *************************************************************************************************
void i2cDelay (void)
{
 u8 i;
 for (i = 0 ; i < 1 ; i++){}
}

// *************************************************************************************************
//  Auteur: Renaud Gagnon
//  Date de cr�ation :  09-09-2021
//
//  Description: �crit un bit I2C sur les lignes SDA et SCL
//  Param�tres d'entr�es : bit � �crire
//  Param�tres de sortie : Aucun
//  Notes     		 	 : Aucune
//
 void I2CWriteBit (u8 bInput)
 {
	 digitalWrite(customI2CPins[channel], 0);
	 i2cDelay();
     digitalWrite(PIN_SDA, bInput);
	 i2cDelay();
	 digitalWrite(customI2CPins[channel], 1);
	 i2cDelay();
	 digitalWrite(customI2CPins[channel], 0);
	 i2cDelay();
 }
 
 // *************************************************************************************************
//  Auteur: Renaud Gagnon
//  Date de cr�ation :  09-09-2021
//
//  Description: Lis un bit I2C sur les lignes SDA et SCL
//  Param�tres d'entr�es : Aucun
//  Param�tres de sortie : bit lu
//  Notes     		 	 : Aucune
//
 u8 I2CReadBit ()
 {
	 u8 output;
	 pinMode(PIN_SDA, pinMode_I_floating);
	 digitalWrite(customI2CPins[channel], 0);
	 i2cDelay();
	 digitalWrite(customI2CPins[channel], 1);
	 output = digitalRead(PIN_SDA);
	 i2cDelay();
	 digitalWrite(customI2CPins[channel], 0);
	 i2cDelay();
     pinMode(PIN_SDA, pinMode_O_openDrain);
	 return output;
 }

  
// *************************************************************************************************
//  Auteur: Renaud Gagnon
//  Date de cr�ation :  09-09-2021
//
//  Description: provoque un "start-condition" I2C sur les lignes SDA et SCL
//  Param�tres d'entr�es : Aucun
//  Param�tres de sortie : Aucun
//  Notes     		 	 : Aucune
//
void I2CStartBit()
{
	digitalWrite(customI2CPins[channel], 1);
	digitalWrite(PIN_SDA, 1);
	i2cDelay();
	digitalWrite(PIN_SDA, 0);
	i2cDelay();
	digitalWrite(customI2CPins[channel], 0);
	i2cDelay();
}
// *************************************************************************************************

// *************************************************************************************************
//  Auteur: Renaud Gagnon
//  Date de cr�ation :  09-09-2021
//
//  Description: provoque un "stop-condition" I2C sur les lignes SDA et SCL
//  Param�tres d'entr�es : Aucun
//  Param�tres de sortie : Aucun
//  Notes     		 : Aucune
//
void I2CStopBit()
{
	digitalWrite(customI2CPins[channel], 0);
	digitalWrite(PIN_SDA, 0);
	i2cDelay();
	digitalWrite(customI2CPins[channel], 1);
	i2cDelay();
	digitalWrite(PIN_SDA, 1);
	i2cDelay();
}