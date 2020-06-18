//timer file
/*Set up timer control register to count to 10ms
Set timer ocntrol register.
Set comparator ocntrol register.
Set value to math in timer with comparator.
Set flag.

*/

#include <htc.h>
#include <math.h>
//CONFIG PARAMETERS
__CONFIG(0x373A);


unsigned int result; //hold result
void timer_init(void){
	//GIE = 0; //Turn off all interrrupts
	T1CON  = 0x1; //Timer SET-UP ON - no need for T!SYNC<1>

}

void comparator_init(void){
	//GIE = 0;
	CCP1CON = 0x0B; //When reached limit, set comparator flag, GO ADC.
	//change to 2 to start AD ocnversion when it goes off
	//limit
	CCPR1H = 0xC3;
	CCPR1L = 0x50;
}

void periperals_init(void) {
	INTCON = 0x40; //peripherals enable PEIR = 1 and GIE = 0;
	PIE1 = 0x44; //AD and CCP on docu pg 22
	PIR1 = 0x0; //all flags off


}

void AD_init(void) {
	ADCON0 = 0x81;
	ADCON1 = 0xE;

}

void ports_init(void){
	//set up led output
	//analog converter
	TRISA = 0x1;
	PORTA = 0x0;
	TRISBbits.TRISB0 = 0; //led
	//Initilize LED
	PORTBbits.RB0 = 0;
}

//delay for argument
void delay(int x10) {
	int stop = x10/10; //stop at value ***might fail***
	int counter = 0;
	if (CCP1IF) {
		counter++;
		CCP1IF = 0;
	}
	if (counter == stop) {
		counter = 0;// reset counter
		RB0 ^= 1;
		//add iniits
		//GO = 1;
		//TX = 1;
	}

}

void interrupt isr(void) {
	delay(1);
	if(ADIF){
		result = ADRESH;
		result = (result*100) + ADRESL/(pow(10, 6));//good luck
		ADIF = 0;
	}

}


int main(void) {
	/*
	Initialization
	*/
	GIE = 0;
	timer_init();
	comparator_init();
	periperals_init();
	AD_init();
	ports_init();
	GIE = 1; //start interrrupts
	GO = 1; //start conversion


	while (1) {

	}

	return 0;
}
