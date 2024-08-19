/**
 * @file stepper.h
 * @author Renaud Gagnon
 * @brief This file handles the movements of the stepper 
 * 			motor drivers on the PCluster device
 * @version 0.1
 * @date 2023-06-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PCLUSTER_MOTORS_H
#define PCLUSTER_MOTORS_H

#ifdef __cplusplus
extern "C" {
#endif

// Include C library headers here
#include "wiring.h"
#include "AccelStepper.h"
#include "../../include/main.h"


#ifdef __cplusplus
}
#endif


#define STEPPER1 3
#define STEPPER2 2
#define STEPPER3 4
#define STEPPER4 1

#define PIN_MOT_RST   pin_A5
#define PIN_SCX_MOTOR1 pin_B13
#define PIN_SCX_MOTOR2 pin_B15
#define PIN_SCX_MOTOR3 pin_A9
#define PIN_SCX_MOTOR4 pin_B11
#define PIN_CW_MOTOR1 pin_B12
#define PIN_CW_MOTOR2 pin_B14
#define PIN_CW_MOTOR3 pin_A8
#define PIN_CW_MOTOR4 pin_B10
#define PIN_CW pin_A8

#define SCX_PINSTATE_HIGH 	(uint8_t)1
#define SCX_PINSTATE_LOW	(uint8_t)0

#define CW_PINSTATE_CW	1
#define CW_PINSTATE_CCW	0

//Steps values
#define MOTORS_INIT_MAXING_STEPS 3900
#define MOTORS_INIT_ZEROING_STEPS 2890
#define MAX_MOTOR_VALUE 2880
#define MIN_MOTOR_VALUE 0

#define STEPPER_MAX_SPEED 100000
#define STEPPER_MAX_ACCEL 5000

#define INIT_COUNTDIVIDER 1


class PClusterMotor
{
    public:
        AccelStepper *stepper;
        PClusterMotor(int motNum);        //Constructor
        void poll();            //Polls the stepper motor (calls the state machine function pointer)
        bool InitIsDone();      //Returns true if the motor has been homed since reset
        void (*forward)();
        void (*backward)();
        void SetPercent (u16 percent);
        void SetPercent (float percent);
    private:
        void (*stepperStateMachine) ();
        bool flagInitDone;
        GPIOpins scxPin;
        GPIOpins cwPin;
        GPIOpins rstPin;
        int motorNum;
        
        int initSequence;
	    int initCounter;
        void Reset();
        bool (*setMotorPin)();
        
        
};

#endif
