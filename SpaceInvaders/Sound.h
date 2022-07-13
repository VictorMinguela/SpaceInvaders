// Sound.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014

void Sound_Play(void);
void Sound_Out(const unsigned char *pt, unsigned long count);
void Sound_Shoot(void);
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also initializes DAC
// Input: none
// Output: none
void Sound_Init(void);

void Sound_Killed(void);

void Sound_Explosion(void);

void Sound_Fastinvader1(void);
void Sound_Fastinvader2(void);

void Sound_Fastinvader3(void);
void Sound_Fastinvader4(void);

void Sound_Highpitch(void);
