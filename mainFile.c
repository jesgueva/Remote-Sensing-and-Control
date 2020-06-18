/*
Final project. Jesus Esgueva, Daniel Arpide, Marc Ghannam
This progam reads an analog voltage using the ADC and sends it over to another board and receive its value using
USART technology and displaying the decimal value V.VVV in the seven-segment display



*/


#include <htc.h>
//CONFIG PARAMETERS
__CONFIG(0x373A);

//Fuction declarations
int ADC2INT(); //Convert the result of the ADC in to a integer.
void TX(); //Transmits ADRESH and ADREL back to back
void RX(); //Receives two bytes
void delay(); //timer delay, 10ms
void timer_init(); //timer inilization
void comparator_init(); //Comparator inialization
void periperals_init(); //Interrupt periperals inialization
void AD_init(); //ADC inialization
void USART_init(); //USART inialization
void ports_init(); //PORTS set-up

//Global variables
int counter; //timer counter

unsigned int a, b, c, d; //Values for each 7=segment dipslay
//Array with values [0.] to [9.]
unsigned int seg_dot[] = {0x40, 0X79, 0X24, 0X30, 0X19,0X12, 0X02, 0X78,0X00, 0X10};
 //Array with values[0] to [9]
unsigned int seg[] = {0XC0,0XF9,0XA4,0XB0,0X99, 0X92, 0X82, 0XF8, 0X80, 0X90};

unsigned char vals_RX[2]; //Holds vlaues received



int main(void) {
	/*
	Initialization
	*/
	GIE = 0; //make sure all interrupts are off
	timer_init();
	comparator_init();
	periperals_init();
	AD_init();
	ports_init();
	USART_init();
	GIE = 1; //start interrrupts
	GO = 1; //start first conversion
	TXEN = 1; //Enable trasmission
	counter = 0; //initilizes counter
	a, b, c, d = 0; //Initializes values for 7 segment display



	//Infinite loop
	while (1) {
		PORTD = 0xFF; //Clear residual bits
		//4th digit
		RC0 = 0;
		PORTD = seg[a];
		delay();
		RC0 = 1;

		//3rd digit
        PORTD = seg[b];
        RC1 =0;
        delay();
        RC1 = 1;

        //2nd digit
        PORTD = seg[c];
        RC2 = 0;
        delay();
        RC2 = 1;

        //1st digit
        PORTD = seg_dot[d];
        RC3 = 0;
        delay();
        RC3 = 1;

	}

	return 0;
}

void interrupt isr(void) {
	delay(); //5 seconds

	if(ADIF){
		TX();
		RX();
		unsigned int result = ADC2INT();
		a = result % 10;//4th digit is saved here

    b = (result/10) % 10;//3rd digit is saved here

    c = (result/100) % 10; //2nd digit is saved here

		d = (result/1000) % 10; //1st digit is saved here

		ADIF = 0; //reset flag
		//delay(500);

	}

}

//delay
void delay() {
	if (CCP1IF) {
		counter++; //Increase counter
		CCP1IF = 0;
		//add 7 segment display digits

	}
	//5 second delay
	if (counter == 500) {
		counter = 0;// reset counter
		RB0 ^= 1; //debug signal
		GO = 1; //starts ADC
	}

}

void timer_init(void){
	//GIE = 0; //Turn off all interrrupts
	T1CON  = 0x1; //Timer SET-UP ON - no need for T1SYNC<1>

}

void comparator_init(void){
	//GIE = 0;
	CCP1CON = 0x0B; //When reached limit, set comparator flag, GO ADC.

	//limit
	CCPR1H = 0xC3;
	CCPR1L = 0x50;
}

void periperals_init(void) {
	INTCON = 0x40; //peripherals enable PEIR = 1 and GIE = 0;
	PIE1 = 0x44; //AD and CCP , TX and RX on docu pg 22
	PIR1 = 0x0; //all flags off


}

void AD_init(void) {
	ADCON0 = 0x81;
	ADCON1 = 0xE;

}

void ports_init(void){

	//analog converter
	TRISA = 0x1; //Input ADC
	TRISB = 0x0; //debug timer
	TRISC = 0x80;//Output display driver and USART<6,7>
	TRISD = 0x0; //Output 7 segment

	//Clear registers
	PORTA = 0x0;
	PORTB = 0x0;
	PORTC = 0x0; //clear PORTC
	PORTD = 0xFF; //Turn off all segments


}

void USART_init(){
	TXSTA = 0x6;//TX enable 0
	RCSTA = 0x90;
	//Baud_Rate = 9600;
	//Freq = 20000000;

	SPBRG = 129; //data speed

}



/*
ADRESH multiplied by 2^3 to obtain te quivalent number displaced 2 bits.
Add ADRESL 2 most significant bits. Shiffted to right.
Convert to decimal value multiplaying 5 dividing by 1024
Multiply by 1000 to displace 3 decimal figure to the left and cast as INT
*/

int ADC2INT(){

	unsigned int result = (vals_RX[1] * 4) + (vals_RX[0] >> 6); //(ADRESH * 4) + (ADRESL >> 6); Convert to an int
	int seg = (int)(result * 5 * 1.0 / 1024 * 1000); //conver to a4 digit value
	return seg;

}

//Transmits ADRESH and ADREL back to back
void TX(){
	//feed TXREG with the values from the ADC
	if(TXIF){ //TSR empty
		TXREG = ADRESH; //FIRST word
		while(!TXIF); //Wait until TRMT is loaded
		TXREG = ADRESL; //Second word
	}
}

//Receives two bytes
void RX(){

	if(RCIF){
		for(int i = 1; i >= 0 ; i--){
			vals_RX[i] = RCREG;//Store values to array
		}
		if(OERR){ //Check for overrun error
			CREN = 0; //reset receiver OFF
			for(int i = 1; i >= 0 ; i--){
				vals_RX[i] = 0;
			}
			CREN = 1; //reset receiver ON
		}
	}
}
