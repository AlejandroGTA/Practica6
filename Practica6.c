#include <18F4620.h>
#include <stdio.h>
#include <stdlib.h>
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, /*NOMCLR,*/ STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)



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

void main(void) {
   setup_oscillator(OSC_16MHZ);
   
while (1) {
   Start_signal();
   if(check_response()){                    // If there is response from sensor
      RH_byte1 = Read_Data();                 // read RH byte1
      RH_byte2 = Read_Data();                 // read RH byte2
      T_byte1 = Read_Data();                  // read T byte1
      T_byte2 = Read_Data();                  // read T byte2
      Checksum = Read_Data();                 // read checksum
      
      if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) )){ 
         output_e(0x02);
      }
   }
}
}
