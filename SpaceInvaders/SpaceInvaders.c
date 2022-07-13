// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Victor Minguela


// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5


// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "ST7735.h"
#include "Sound.h"
#include "PLL.h"
#include "Random.h"
#include "TExaS.h"
#include "Images.h"
#include "ADC.h"
#include "BSP.h"
#include "OS.h"
#include "CortexM.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler


unsigned long Convert(unsigned long sample);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void (*PeriodicTask0)(void);   // user function
void player_Init(void);
void new_Missle(void);
void new_Laser(void);
void missle_draw(void);
void missleCollision(void);
void enemy_move(void);
void enemylaser(void);
void laserplayercollision(void);
void checkEnemies(void);
void Move(void);
void lasermove(void);
void Init(void);
void Draw(void);


unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long pixel;
unsigned long currentX;
unsigned long currentY;
unsigned long weapon;
unsigned char end=0;
unsigned char count;
unsigned long score;
unsigned long Enemies=4;
int32_t Button;      // set on button touch
struct State {
  unsigned long x;      // x coordinate
  unsigned long y;      // y coordinate
  const unsigned short *image; // ptr->image
  long life;            // 0=dead, 1=alive
	long gone;
};          
typedef struct State STyp;
STyp Enemy[4];
STyp Player;
STyp Missle;
STyp Enemy_laser;
//main to test player movement, missle movement, and sound buttons
//ADC, DAC, switches
//int enemynum = 0;
int32_t task0, task1, task2;
int32_t TakeADCdata;
int32_t ADCmutex;
int32_t SwitchTouch;
//Measure ADC
void Task0(void){
	while(1){
		OS_Wait(&TakeADCdata); //signaled by OS
	//	OS_Wait(&ADCmutex);
		ADCdata = ADC0_In();
		Distance = Convert(ADCdata);
		pixel = (0.02976*Distance);
		task0++;
	//	OS_Signal(&ADCmutex);
	}
	
}
//Updates LCD 
int32_t LCDdata;
void Task1(void){
	while(Player.life != 0){
	//	OS_Wait(&LCDdata);
	//	OS_Sleep(12);
		currentX = Player.x;
		currentY = Missle.y;
		Draw();
		enemylaser();
		if(weapon==1){
			missle_draw();
		}
		OS_Sleep(15);
		enemy_move();
		lasermove();
		checkEnemies();
		if(Enemies==0){
			Init();
			Enemies=4;
			count = 0;
			end = 0;
		}
		if(weapon == 1){
			Move();
		}
		missleCollision();
		laserplayercollision();
		task1++;
	}
	ST7735_FillScreen(0x0000);            // set screen to black
	ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("FINAL SCORE:");
  ST7735_SetCursor(1, 3);
	ST7735_OutUDec(score);
}
//sound
int32_t Takesounddata;
void Task2(void){
	while(1){
		OS_Wait(&Takesounddata);
		Sound_Play();
		task2++;
	}
	
}

uint32_t task3;
void Task3(void){
	OS_InitSemaphore(&SwitchTouch,0); // signaled on touch button1
	OS_EdgeTrigger_Init(&SwitchTouch, 3);
	while(1){
		task3++;
		OS_Wait(&SwitchTouch); // OS signals on touch
    OS_Sleep(100);          // debounce the switches
		Sound_Shoot();
		new_Missle();
		weapon = 1;
		OS_EdgeTrigger_Restart();
	}
}

//---------------- Task7 dummy function ----------------
// *********Task7*********
// Main thread scheduled by OS round robin preemptive scheduler
// Task7 does nothing but never blocks or sleeps
// Inputs:  none
// Outputs: none
uint32_t Count7;
void Task7(void){
  Count7 = 0;
  while(1){
    Count7++;
    WaitForInterrupt();
  }
}

int main(void){ 
	OS_Init();
	DisableInterrupts();
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
	player_Init();
	Init();
	new_Laser();
	Sound_Init(); // initialize SysTick timer and DAC
	Output_Init();
	OS_InitSemaphore(&TakeADCdata,0);
  OS_InitSemaphore(&Takesounddata,0);

	OS_AddThread(&Task0,0); //ADC
	OS_AddThread(&Task1,0); //LCD
	OS_AddThread(&Task3,1); //Button
	OS_AddThread(&Task2,2); //Sound
	OS_AddThread(&Task7,3); //Dummy
	OS_PeriodTrigger0_Init(&TakeADCdata, 1);
	OS_PeriodTrigger1_Init(&Takesounddata, 1);
	 OS_Launch(BSP_Clock_GetFreq()/THREADFREQ); // doesn't return, interrupts enabled in here

}
void player_Init(void){
	Player.x = 60;
	Player.y = 120;
	Player.image = PlayerShip;
	Player.life = 1;
	Player.gone = 0;
	
}
void Init(void){ int i;
  for(i=0;i<4;i++){
    Enemy[i].x = 20*i;
    Enemy[i].y = 50;
    Enemy[i].image = SmallEnemy30pointA;
    Enemy[i].life = 1;
		Enemy[i].gone = 0;
   }
	//new_Laser();
}
void new_Missle(void){
	if(weapon==0){
		Missle.x = Player.x + 2;
		Missle.y = 110;
		Missle.image = missle1;
		Missle.life = 1;
		Missle.gone = 0;
	}
}
unsigned long num;
void new_Laser(void){
	num = Random32() % 4;
	if(Enemy[num].life == 1){
		Enemy_laser.x = Enemy[num].x + 7;
		Enemy_laser.y = 60;
		Enemy_laser.image = elaser;
		Enemy_laser.life = 1;
		Enemy_laser.gone = 0;
	}
}
void Draw(void){ int i;
		Player.x = pixel;
    if(Player.life > 0){
		ST7735_FillRect(currentX, Player.y-7, 20, 8, ST7735_BLACK);
     ST7735_DrawBitmap(Player.x, Player.y, Player.image, 18, 8);
    }else{
		//	if(Player.gone == 0){
				ST7735_FillRect(Player.x, Player.y-7, 20, 8, ST7735_BLACK);
				Player.gone = 1;
		//	}
		}
  for(i=0;i<4;i++){
    if(Enemy[i].life > 0){
     ST7735_DrawBitmap(Enemy[i].x, Enemy[i].y, Enemy[i].image, 16, 10);
    }else{
			if(Enemy[i].gone == 0){
				ST7735_FillRect(Enemy[i].x, Enemy[i].y-10, 16, 10, ST7735_BLACK);
				Sound_Killed();
				Enemy[i].gone = 1;
			}
		}
  }

}
void enemylaser(void){
	//lasermove();
		if((Enemy_laser.life > 0)){
		ST7735_FillRect(Enemy_laser.x, Enemy_laser.y - 10, 2, 9, ST7735_BLACK);
     ST7735_DrawBitmap(Enemy_laser.x, Enemy_laser.y, Enemy_laser.image, 2, 9);
    }else{
				ST7735_FillRect(Enemy_laser.x, Enemy_laser.y -8, 2, 9, ST7735_BLACK);
				new_Laser();
		}
}

void missle_draw(void){
	if((Missle.life > 0)){
		ST7735_FillRect(Missle.x, currentY, 4, 9, ST7735_BLACK);
     ST7735_DrawBitmap(Missle.x, Missle.y, Missle.image, 4, 9);
    }else{
				ST7735_FillRect(Missle.x, Missle.y -6, 4, 9, ST7735_BLACK);
			  weapon = 0;
		}
}
void lasermove(void){
		if(Enemy_laser.y < 154){
			Enemy_laser.y += 2;
	}else{
		Enemy_laser.life = 0;
	}
}
void Move(void){ 
	if(Missle.y > 5){
		Missle.y -= 2;
	}else{
		Missle.life = 0;
	}
}

void enemy_move(void){
	int i;
	if((count <= 200)&&(end==0)){
		for(i=0;i<4;i++){
			count++;
			if(Enemy[i].life){
				Enemy[i].x += 1;
				
			}
		}
		if(count > 200) end = 1;
}
	else{
		for(i=0;i<4;i++){
			count--;
			if(Enemy[i].life){
				Enemy[i].x -= 1;
			}
		}
		if(count<=0) end = 0;
	}

	
}

void missleCollision(void){int i;
	if(Missle.life > 0){
		for(i=0;i<4;i++){
			if(Enemy[i].life){
				if(((Missle.x > Enemy[i].x) &&(Missle.x < Enemy[i].x + 17)) && (Missle.y == Enemy[i].y)){
					Missle.life = 0;
					Enemy[i].life = 0;
					score += 10;
				}
			}
		}	
	}
	
}
void laserplayercollision(void){
	if(Enemy_laser.life>0){
		if(((Enemy_laser.x > Player.x) &&(Enemy_laser.x < Player.x + 15)) && (Enemy_laser.y == Player.y)){
			Enemy_laser.life = 0;
			Player.life = 0;
		}
	}
}

void checkEnemies(void){int i;
	for(i=0;i<4;i++){
		if(Enemy[i].life==0){
			Enemies--;
		}
	}
	if(Enemies!=0){
		Enemies=4;
	}
}

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
	double val = (0.00105*sample);
	unsigned long dis = val * 1000;
	/////for lcd: 128/4300 = 0.029767441
  return dis;
}



