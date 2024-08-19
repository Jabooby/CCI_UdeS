/**
 * @file stepper.c
 * @author Renaud Gagnon
 * @brief This file handles the movements of the stepper 
 * 			motor drivers on the PCluster device
 * @version 0.1
 * @date 2023-06-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "PCluster_Motors.h"
#include <stdio.h>
#include "../../include/main.h"
#include <debug.h>
#include <functional>

//Public defines (referenced with "extern")
void (*stepperStateMachine) ();

//Private defines


//Private variable defines

//Private function defines
void forward1();
void backward1();
void forward2();
void backward2();
void forward3();
void backward3();
void forward4();
void backward4();




void piloteAiguilles_init ()
{
	portEnable(port_A);
	portEnable(port_B);
	portEnable(port_C);

	//INIT GPIO PINS
	pinMode(PIN_MOT_RST, pinMode_O_pushPull);
	digitalWrite(PIN_MOT_RST, 1);
	pinMode(PIN_SCX_MOTOR1, pinMode_O_pushPull);
	pinMode(PIN_SCX_MOTOR2, pinMode_O_pushPull);
	pinMode(PIN_SCX_MOTOR3, pinMode_O_pushPull);
	pinMode(PIN_SCX_MOTOR4, pinMode_O_pushPull);
	pinMode(PIN_CW_MOTOR1, pinMode_O_pushPull);
	pinMode(PIN_CW_MOTOR2, pinMode_O_pushPull);
	pinMode(PIN_CW_MOTOR3, pinMode_O_pushPull);
	pinMode(PIN_CW_MOTOR4, pinMode_O_pushPull);
}

PClusterMotor::PClusterMotor(int motnum)
{
	//Set the motor number
	motorNum = motnum;
	//Set the state machine to the init state
	//Set the init done flag to false
	flagInitDone = false;
	//Set the init sequence to 0
	initSequence = 0;
	initCounter = 0;
	//Set the motor pins
	switch (motorNum)
	{
	case 1:
		scxPin = PIN_SCX_MOTOR1;
		cwPin = PIN_CW_MOTOR1;
		stepper = new AccelStepper(forward1, backward1);
		forward = forward1;
		backward = backward1;
		break;
	case 2:
		scxPin = PIN_SCX_MOTOR2;
		cwPin = PIN_CW_MOTOR2;
		stepper = new AccelStepper(forward2, backward2);
		forward = forward2;
		backward = backward2;
		break;
	case 3:
		scxPin = PIN_SCX_MOTOR3;
		cwPin = PIN_CW_MOTOR3;
		stepper = new AccelStepper(forward3, backward3);
		forward = forward3;
		backward = backward3;
		break;
	case 4:
		scxPin = PIN_SCX_MOTOR4;
		cwPin = PIN_CW_MOTOR4;
		stepper = new AccelStepper(forward4, backward4);
		forward = forward4;
		backward = backward4;
		break;
	default:
		break;
	}
	piloteAiguilles_init();
	stepper->setMaxSpeed(STEPPER_MAX_SPEED);
	stepper->setAcceleration(STEPPER_MAX_ACCEL);
	stepper->setSpeed(0);
	stepper->setCurrentPosition(0);
}

void forward1()
{
	digitalWrite(PIN_SCX_MOTOR1, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR1, CW_PINSTATE_CW);
	digitalWrite(PIN_SCX_MOTOR1, SCX_PINSTATE_HIGH);
}

void backward1()
{
	digitalWrite(PIN_SCX_MOTOR1, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR1, CW_PINSTATE_CCW);
	digitalWrite(PIN_SCX_MOTOR1, SCX_PINSTATE_HIGH);
}

void forward2()
{
	digitalWrite(PIN_SCX_MOTOR2, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR2, CW_PINSTATE_CW);
	digitalWrite(PIN_SCX_MOTOR2, SCX_PINSTATE_HIGH);
}

void backward2()
{
	digitalWrite(PIN_SCX_MOTOR2, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR2, CW_PINSTATE_CCW);
	digitalWrite(PIN_SCX_MOTOR2, SCX_PINSTATE_HIGH);
}

void forward3()
{
	digitalWrite(PIN_SCX_MOTOR3, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR3, CW_PINSTATE_CW);
	digitalWrite(PIN_SCX_MOTOR3, SCX_PINSTATE_HIGH);
}	

void backward3()
{
	digitalWrite(PIN_SCX_MOTOR3, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR3, CW_PINSTATE_CCW);
	digitalWrite(PIN_SCX_MOTOR3, SCX_PINSTATE_HIGH);
}	

void forward4()
{
	digitalWrite(PIN_SCX_MOTOR4, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR4, CW_PINSTATE_CW);
	digitalWrite(PIN_SCX_MOTOR4, SCX_PINSTATE_HIGH);
}

void backward4()
{
	digitalWrite(PIN_SCX_MOTOR4, SCX_PINSTATE_LOW);
	digitalWrite(PIN_CW_MOTOR4, CW_PINSTATE_CCW);
	digitalWrite(PIN_SCX_MOTOR4, SCX_PINSTATE_HIGH);
}




void PClusterMotor::poll()
{
	switch (initSequence)
	{
		case 0:
			forward();
			initCounter++;
			if (initCounter >= MOTORS_INIT_MAXING_STEPS)
			{
				initCounter = 0;
				initSequence++;
			}
			break;
		case 1:
			backward();
			initCounter++;
			if (initCounter >= MOTORS_INIT_ZEROING_STEPS)
			{
				initCounter = 0;
				flagInitDone = true;
				initSequence++;
				stepper->setCurrentPosition(0);
			}
			break;
		case 2:
			stepper->run();
			if(flagInitDone == false)
			{
				initSequence = 0;
			}
			break;
	}
}

void PClusterMotor::Reset()
{
	flagInitDone = false;
}

void PClusterMotor::SetPercent(u16 percent)
{
	stepper->moveTo((percent * MAX_MOTOR_VALUE) / 100);
}

void PClusterMotor::SetPercent(float percent)
{
	stepper->moveTo((long)((percent * MAX_MOTOR_VALUE) / 100));
}

bool PClusterMotor::InitIsDone()
{
	return flagInitDone;
}

