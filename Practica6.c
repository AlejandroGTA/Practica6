#include <18F4620.h>
#include <stdio.h>
#include <stdlib.h>
#use fast_io(a)
#use fast_io(d)
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)

//contador de anillo respecto a los activadores de los displays para el control del uso y mostrado de los displays
//timer0 con el despliegue de la memoria de video(displays)

unsigned char Check;

#BIT DataDht = 0xF82.4             //pin de dato de entrada del DHT11 portc
#BIT Data_Pin_Direction = 0xF94.4  //pin direccion trsic
unsigned int8 T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum ;
short Time_out;
void Start_signal(){
   Data_Pin_Direction = 0;  //configuracion del pin C4 como salida
   DataDht = 0;       //se encia un 0 al sensor
   delay_ms(18);
   DataDht = 1;
   delay_us(30);
   Data_Pin_Direction = 1;  //configuracion de pin C4 como entrada
}

short check_response(){
   delay_us(40);
   if(!DataDht){                     // Read and test if connection pin is low
      delay_us(80);
      if(DataDht){                    // Read and test if connection pin is high
         delay_us(50);
         return 1;}
   }
}

unsigned int8 Read_Data(){
   unsigned int8 i, k, data = 0;     // k is used to count 1 bit reading duration
   if(Time_out)
      break;
   for(i = 0; i < 8; i++){
      k = 0;
      while(!DataDht){                          // Wait until pin goes high
         k++;
         if (k > 100) {
            Time_out = 1; break;
         }
         delay_us(1);
      }
      delay_us(30);
      if(!DataDht)
         bit_clear(data, (7 - i));               // Clear bit (7 - i)
      else{
         bit_set(data, (7 - i));                 // Set bit (7 - i)
         while(DataDht){                         // Wait until pin goes low
            k++;
            if (k > 100) {Time_out = 1; break;}
               delay_us(1);}
      }
   }
   return data;
}
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

#INT_RB
void isr_RB(){
   if(1==input(PIN_B4)){
      cambio++;
      if(cambio==2){
         cambio=0;
      }
   }
}

void main(void) {
   set_tris_d(0x00);
   set_tris_a(0x00);
   set_tris_e(0x00);   
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256 | RTCC_8_BIT);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   set_timer0(120);//194
   
   while (1) { 
      Start_signal();
      if(check_response()){                    
         Rh_byte1 = Read_Data();          
         Rh_byte2 = Read_Data();                 
         T_byte1 = Read_Data();                   
         T_byte2 = Read_Data();                     
         CheckSum = Read_Data();                       
         if(CheckSum == ((Rh_Byte1 + Rh_Byte2 + T_Byte1 + T_Byte2) )){
            switch(cambio){
               case 0:
                  output_e(0x02);
                  dato[1]=(int)T_Byte1/10;
                  dato[2]=(int)T_Byte1%10;
                  dato[4]=(int)T_Byte2/10;
                  dato[8]=(int)T_Byte2%10;
               break;
               case 1:
                  output_e(0x01);
                  dato[1]=(int)Rh_Byte1/10;
                  dato[2]=(int)Rh_Byte1%10;
                  dato[4]=(int)Rh_Byte2/10;
                  dato[8]=(int)Rh_Byte2%10;
               break;
            }
         }
      }
      if(contador==2){
         output_a(contador);
         output_d(bcdCon[dato[contador]]);
      }
      else{
         output_a(contador);
         output_d(bcdSin[dato[contador]]);
      }   
   }
}