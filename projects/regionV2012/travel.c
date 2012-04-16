#include "inc/hw_types.h"		// tBoolean
#include "RASLib/motor.h"
#include "ADS7830.h"
#include "utils/uartstdio.h"
#include "travel.h"


// Current location of robot
signed char currentCorner = TREE;
unsigned char currentFacing = false;
unsigned char lMotorSpeed = 0;
unsigned char rMotorSpeed = 0;

#define BACK_LEFT 0
#define FRONT_LEFT 1
#define FRONT 2
#define FRONT_RIGHT 3
#define BACK_RIGHT 4
#define BACK 5

#define LOWER 0
#define HIGHER 1

#define till(x,y) for(ADS7830_BurstRead(sensors);sensors[currentFacing?((x)+3)%6:(x)]<bounds[y][currentFacing?((x)+3)%6:(x)];ADS7830_BurstRead(sensors))
#define tillnot(x,y) for(ADS7830_BurstRead(sensors);sensors[currentFacing?((x)+3)%6:(x)]>bounds[y][currentFacing?((x)+3)%6:(x)];ADS7830_BurstRead(sensors))


// Array for reading sensors
unsigned char sensors[8];

//TODO experimentally determine these
const unsigned short bounds[2][8] = {
	{400,400,400,400,400,400,400,400},
	{600,600,600,600,600,600,600,600},
};


void travelInit(void) {
	//motors
	InitializeMotors(false,false);
	ADS7830_Init();
}

void stop(void) {
	SetMotorPowers(0,0);
}

void goForward(void) {
	SetMotorPowers(127,127);
}

void goBackward(void) {
	SetMotorPowers(-128,-128);
}

void goEngageCorner(void) {
	goWall();
}

void goWall(void) {
	SetMotorPowers(-128,-128);

}


//rotates clockwise by degrees
void goRotate(signed char degrees) {
	
}

//rotate to align with wall using IR sensors
void goAlignWall(char rightSensors, char goRight) {
	
}
						   
void gotoCorner(signed char dest,char flip) {
	signed char offdest;
	if (dest<0 || dest==currentCorner) return;

	if (currentCorner==TREE) {
		till(FRONT,LOWER) goForward();
		goAlignWall(false,true);
		till(FRONT_RIGHT,LOWER) goWall();
		goEngageCorner();
		currentCorner = ELECTRIC;
		currentFacing = false;
		if (dest != currentCorner) gotoCorner(dest,flip);
		return;
	}
	
	
	offdest = currentCorner - dest;
	if (offdest < 0) offdest = offdest+4;
	
	switch(offdest) {
		case 1:
			tillnot(FRONT_RIGHT,LOWER) goBackward();
			goRotate(flip?-135:45); 
			currentFacing ^= flip;
			till(FRONT,LOWER) goForward();
			goAlignWall(false,true);
			till(FRONT_RIGHT,LOWER) goForward();
			break;
		case 2:
			till(BACK,LOWER) goBackward();
			goRotate(45);
			goAlignWall(true,true);
			till(FRONT,LOWER) goForward();
			goRotate(flip?-45:45);
			goAlignWall(flip,!flip);
			currentFacing ^= flip;
			break;
		case 3:
			till(BACK,HIGHER) goBackward();
			goRotate(-45);
			till(BACK,LOWER) goBackward();
			goRotate(flip?90:0);
			goAlignWall(flip,flip);
			currentFacing ^= flip;
			break;
	}
	
	goEngageCorner();

	currentCorner = dest;
}

void testSensors(void) {
	ADS7830_BurstRead(sensors);
	UARTprintf("[%d %d %d %d %d %d %d %d]\n",sensors[0],sensors[1],sensors[2],sensors[3],sensors[4],sensors[5],sensors[6],sensors[7]);
}


