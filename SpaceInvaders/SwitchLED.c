//#include "tm4c123gh6pm.h"
//#include "SwitchLED.h"
//#include "Sound.h"
//unsigned int weapon1;
//unsigned int weapon2;
//void Delay10ms(void){unsigned long volatile time;
//  time = 14545;  // 10msec
//  while(time){
//		time--;
//  }
//}

//// Subroutine to initialize port D pins for the four external switches
//// enable PD3,PD2,PD1,PD0 with both edge interrupt enabled.
//// Inputs: None
//// Outputs: None
//unsigned char test;
//void PortE_Init(void){ unsigned long volatile delay;
//	SYSCTL_RCGC2_R |= 0x00000010; // (a) activate clock for port E
//  delay = SYSCTL_RCGC2_R;
//	GPIO_PORTE_CR_R = 0x03;
//	GPIO_PORTE_DIR_R &= ~0x03;    // (c) make PD3,2,1,0 input
//  GPIO_PORTE_AFSEL_R &= ~0x03;  // disable alt funct on PD3,2,1,0
//	GPIO_PORTE_AMSEL_R &= ~0x03;  //     disable analog functionality on PD3,2,1,0
//  GPIO_PORTE_DEN_R |= 0x03;     //     enable digital I/O on PD3,2,1,0
//  GPIO_PORTE_PCTL_R &= ~0x000000FF; //  configure PD3,2,1,0 as GPIO
//	GPIO_PORTE_PUR_R |= 0x03;     //     enable weak pull-up on PD3,2,1,0
//  GPIO_PORTE_IS_R &= ~0x03;     // (d) PD3,2,1,0 is edge-sensitive
//  GPIO_PORTE_IBE_R &= ~0x03;    //     PD3,2,1,0 is not both edges
//	GPIO_PORTE_IEV_R &= ~0x03;    //     PE1,0 is falling edge event
//  GPIO_PORTE_ICR_R = 0x03;      // (e) clear flags 3,2,1,0
//  GPIO_PORTE_IM_R |= 0x03;      // (f) arm interrupt on PD3,2,1,0
//  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x000000A0; // (g) bits:7-5 for PORTE, set priority to 5
//  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 3 in NVIC
//}


//void GPIOPortE_Handler(void){
//	unsigned char j;
//for(j=0;j<50;j++)
//		Delay10ms();
//	if(GPIO_PORTE_RIS_R&0x01){  // PE0 touched 
//    GPIO_PORTE_ICR_R = 0x01;  // acknowledge flag0
//		test++;
//		weapon1 =1;
//		Sound_Shoot();
//	}
//  if(GPIO_PORTE_RIS_R&0x02){  // PE1 touch
//    GPIO_PORTE_ICR_R = 0x02;  // acknowledge flag1
//		test--;
//		weapon2 =1;
//		Sound_Explosion();
//	}

//}
