
#include "includes.h"
#include "RN5T618.h" 

unsigned char tempDevice = 0; //0 : MAX    1: NTC



int main(void)
{
	InitMPhaseBsp();

	//PrintMsg("System start()!\r\n");
//    LED_LOW;
	delay_ms(50);
//    LED_HI;
  PowerRN5T618_init();
	AppProgramme();

	return 0;
}



