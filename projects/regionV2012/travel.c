#include "inc/hw_types.h"		// tBoolean
#include "RASLib/motor.h"
#include "travel.h"


// Current location of robot
signed char currentCorner = TREE;
bool backwards = 0;
unsigned char lMotorSpeed = 0;
unsigned char rMotorSpeed = 0;

#define BACK_LEFT 0
#define FRONT_LEFT 1
#define FRONT 2
#define FRONT_LEFT 3
#define BACK_RIGHT 4
#define BACK 5

#define LOWER 0
#define HIGHER 1

#define BACK_OFFSET (backwards?3:0)
#define till(x,y) for(ADS7830_BurstRead(sensors);sensors[backwards?((x)+3)%6:(x)]<bounds[y][backwards?((x)+3)%6:(x)];ADS7830_BurstRead(sensors))
#define tillnot(x,y) for(ADS7830_BurstRead(sensors);sensors[backwards?((x)+3)%6:(x)]>bounds[y][backwards?((x)+3)%6:(x)];ADS7830_BurstRead(sensors))
#define tillsource() for(ADS7830_BurstRead(sensors);true;ADS7830_BurstRead(sensors)) //TODO


// Array for reading sensors
unsigned char[8] sensors;

//TODO experimentally determine these
const unsigned char[2][8] bounds = {
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

void goCorner(void) {
	goWall();
}

void goWall(void) {
	SetMotorPowers(-128,-128);

}

void gotoCorner(char dest,bool flip) {
	char offdest;
	if (dest<0 || dest==currentCorner) return;

	if (currentCorner==TREE) {
		till(FRONT,LOWER) goForward();
		//rotate right
		till(FRONT_RIGHT,LOWER) goWall();
		tillsource() goCorner();
		currentCorner = ELECTRIC;
		if (dest != currentCorner) gotoCorner(dest);
		return;
	}
	
	
	offdest = currentCorner - dest;
	if (offdest < 0) offdest = offdest+4;
	
	switch(offdest) {
		case 1:
			tillnot(FRONT_RIGHT,LOWER) goBackwards();
			//rotate right or flip
			//asymptote drive?
			till(FRONT,LOWER) goForwards();
			break;
		case 2:
			till(BACK,LOWER) goBackwards();
			//rotate right
			till(FRONT,LOWER) goForwards();
			//rotate right and flip
			break;
		case 3:
			till(BACK,HIGHER) goBackwards();
			//rotate left
			till(BACK,LOWER) goBackwards();
			//rotate left or flip
			break;
	}
	
	tillsource() goCorner();	

	currentCorner = dest;
}

void testSensors(void) {
	ADS7830_BurstRead(sensors);
	UARTprintf("[%d %d %d %d %d %d %d %d]",sensors[0],sensors[1],sensors[2],sensors[3],sensors[4],sensors[5],sensors[6],sensors[7]);
}


