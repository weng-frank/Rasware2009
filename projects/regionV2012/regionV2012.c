#include "inc/hw_types.h"		// tBoolean
#include "regionV2012.h"
#include "utils/uartstdio.h"	// input/output over UART
#include "driverlib/uart.h"		// input/output over UART
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "RASLib/init.h"
#include "travel.h"





// Order of sources
// The order indicates which one we want first
#define BEST_SOURCE ELECTRIC

//
// Goes to location
//


int charging(void){
	//returns 0 if not charging (source is not on) 
	//1 if charging
	//2 if fully charged
	
	
	//tesetsetsettest
	return 0;	
}


//
//Initialization method
//
void init(void) {
	//Necessary inits for chip
	LockoutProtection();
	InitializeMCU();
	
	//Various driver inits
	//initUART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);				
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	
	UARTStdioInit(0);
	
	travelInit();	
}


//
//Run method
//
void run(void) {
	/*signed char randoms = 3;

	//first when they are randomly on
	for (;randoms>=0;randoms--) {
		gotoCorner(nextSource());
			//TODO charge for some time?
		gotoCorner(FLAG);
			//TODO discharge?
	}

	//then when they are all on
	gotoCorner(BEST_SOURCE);
		//TODO charge
	gotoCorner(FLAG);
		//TODO discharge*/
		
		
		
	//timer thingy
	
/*	while(charging()!=2){
	
		while(time<=180s){//only one source on

			goToCorner(ELECTRIC);//go to the electric source, this is the default place to go
			while(charging()==1);//does nothing but charge so long as the source is on and not fully charged


			if(light source on){
				goToCorner(LIGHT);
				while(charging()==1);
			}

			else{
				goToCorner(FAN);
				while(charging())==1;
			}
		}
*/
		//3 minutes have passed, all sources on
		//want to go to the best
	
	gotoCorner(BEST_SOURCE);
	while(charging()!=2);//charge until fully charged
	
	
	//fully charged
	gotoCorner(FLAG);

}

//
//Don't put any important code in main
//Use main freely to test functions and stuff
//
//For competition main should simply call init and run
//
int main(void) {	
	init();
	for (;;) UARTprintf("HI");//run();
}


