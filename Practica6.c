#include <18F4620.h>
#include <stdio.h>
#include <stdlib.h>
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, /*NOMCLR,*/ STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)

int cambio=0;
int contador=1;
int bcdSin[10]={126, 48,109,121, 51, 91, 95,112,127,123};
int bcdCon[10]={254,176,237,249,179,219,223,240,255,251};
unsigned int8 dato[9] ={0} /*{0,3,5,0,2,0,0,0,5}*/;


#INT_TIMER0
void sti_TIMER0(){
   contador*=2;
   if(contador>=16){
      contador=1;
   } 
   set_timer0(120);
}

void main(void) {
   setup_oscillator(OSC_16MHZ);
   set_tris_d(0x00);
   set_tris_a(0x00);
   
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256 | RTCC_8_BIT);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   set_timer0(120);//194
while (1) {
   output_a(contador);
   output_d(bcdCon[contador]);
}
}
