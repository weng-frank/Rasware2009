#include "inc/hw_types.h"		// tBoolean
#include "RASLib/motor.h"
#include "ADS7830.h"
#include "utils/uartstdio.h"
#include "driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "travel.h"

#include "RASLib/timer.h"



// Current location of robot
signed char currentCorner = TREE;
unsigned char currentFacing = false;
signed char motor_L = 0;
signed char motor_R = 0;

#define BACK_LEFT 0
#define FRONT_LEFT 1
#define FRONT 2
#define FRONT_RIGHT 3
#define BACK_RIGHT 4
#define BACK 5

#define LOWER 0
#define HIGHER 1

#define MOTOR_L_MAX 60
#define MOTOR_R_MAX 60

#define WALL_CONSTANT 10
#define TURN_CONSTANT 2

#define offset(x) (currentFacing?((x)+3)%6:(x))

#define till(t) for(ADS7830_Read();t;ADS7830_Read(),SetMotorPowers(motor_L,motor_R))
#define tripped(x,y) (ADS7830_Values[offset(x)]<bounds[y][offset(x)])

signed short piderror;	

signed char PID(unsigned char val,unsigned char goal,signed short Pk,signed short Dk) {
	signed char temp = piderror;
	piderror = (val-goal);
	return (signed char)((Pk*piderror + Dk*(piderror-temp))>>8);
}

//TODO experimentally determine these
const unsigned char bounds[2][6] = {
	{60,60,60,60,60,60},
	{80,80,80,80,80,80},
};


void travelInit(void) {
	//motors
	InitializeMotors(true,false);
	ADS7830_Init();
}

void stop(void) {
	motor_L = 0;
	motor_R = 0;
}

void goForward(void) {
	motor_L = MOTOR_L_MAX;
	motor_R = MOTOR_R_MAX;
	piderror = 0;
}

void goBackward(void) {
	motor_L = -MOTOR_L_MAX;
	motor_R = -MOTOR_R_MAX;
	piderror = 0;
}

void goEngageCorner(signed char sourcetype) {
	if (sourcetype==ELECTRIC||sourcetype==FLAG) {
		till(true) { //replace false with switch that needs to be switched
		//approach wall
		}
	} else {
		till(tripped(FRONT,LOWER)) { //replace false with switch that needs to be switched
		//approach wall
		}
	}
}

void goWall(void) {
	//temp for debug
	till(true) {
		//testSensors();
	 	if (tripped(FRONT_LEFT,LOWER)) {
			if (motor_L-WALL_CONSTANT > 0) motor_L -= WALL_CONSTANT;
		} else if (tripped(BACK_LEFT,LOWER)){
			if (motor_L+WALL_CONSTANT < MOTOR_L_MAX) motor_L += WALL_CONSTANT;
		}
	
	}


	/*signed char p = PID(ADS7830_Values[offset(FRONT_LEFT)],bounds[LOWER][offset(FRONT_LEFT)],100,0);
	if (motor_L+p > MOTOR_L_MAX) {
		motor_L = MOTOR_L_MAX;	
	} else if (motor_L+p < 0) {
		motor_L = 0;
	} else {
	   motor_L+=p;
	}*/
	//UARTprintf("diff : %d ",p);
	//UARTprintf("<%4d %4d>\n",motor_L,motor_R);
	//testSensors();
//	WaitUS(900000);
}


//rotates clockwise by degrees
void goRotate(signed char degrees) {
	SetMotorPowers(MOTOR_L_MAX,-MOTOR_R_MAX);
	WaitUS(degrees/TIME_CONSTANT);
	
}

//rotate to align with wall using IR sensors
void goAlignWall(char rightSensors, char goRight) {
	SetMotorPowers(MOTOR_L_MAX,-MOTOR_R_MAX);
	till(tripped(FRONT_LEFT,LOWER));
	till(tripped(BACK_LEFT ,LOWER));
}
						   
void gotoCorner(signed char dest,char flip) {
	signed char offdest;
	if (dest<0 || dest==currentCorner) return;
	if (currentCorner==TREE) {
		till(tripped(FRONT,LOWER)) goForward();
		goAlignWall(false,true);
		till(tripped(FRONT_RIGHT,LOWER)) goWall();
		goEngageCorner(ELECTRIC);
		currentCorner = ELECTRIC;
		currentFacing = false;
		if (dest != currentCorner) gotoCorner(dest,flip);
		return;
	}
	
	
	offdest = currentCorner - dest;
	if (offdest < 0) offdest = offdest+4
	switch(offdest) {
		case 1:
			till(!tripped(FRONT_RIGHT,LOWER)) goBackward();
			goRotate(flip?-135:45); 
			currentFacing ^= flip;
			till(tripped(FRONT,LOWER)) goForward();
			goAlignWall(false,true);
			till(tripped(FRONT_RIGHT,LOWER)) goForward();
			break;
		case 2:
			till(tripped(BACK,LOWER)) goBackward();
			goRotate(45);
			goAlignWall(true,true);
			till(tripped(FRONT,LOWER)) goForward();
			goRotate(flip?-45:45);
			goAlignWall(flip,!flip);
			currentFacing ^= flip;
			break;
		case 3:
			till(tripped(BACK,HIGHER)) goBackward();
			goRotate(-45);
			till(tripped(BACK,LOWER)) goBackward();
			goRotate(flip?90:0);
			goAlignWall(flip,flip);
			currentFacing ^= flip;
			break;
	}
	
	goEngageCorner(dest);

	currentCorner = dest;
}

void testSensors(void) {
	//SetMotorPowers(motor_L,motor_R);
	//ADS7830_Read();
	UARTprintf("[%3d %3d %3d %3d %3d %3d %3d %3d]\n",ADS7830_Values[0],ADS7830_Values[1],ADS7830_Values[2],ADS7830_Values[3],ADS7830_Values[4],ADS7830_Values[5],ADS7830_Values[6],ADS7830_Values[7]);
	WaitUS(100000);
}


