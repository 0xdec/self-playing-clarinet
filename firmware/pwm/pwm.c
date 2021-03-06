#include "pwm.h"

// Initialize PWM interface with 16 bit timer
void PWM_init(uint16_t period) {
  // Select pin function CT16B0_MAT0 (sec 7.4.23)
  *iocon_register[PWM_PORT][PWM_PIN] &= ~BIT(0);
  *iocon_register[PWM_PORT][PWM_PIN] |=  BIT(1);
  // Select standard GPIO output mode (sec 7.4.23)
  *iocon_register[PWM_PORT][PWM_PIN] &= ~BIT(10);

  // Enable clock for 16-bit counter/timer 0 (sec 3.5.14)
  LPC_SYSCON->SYSAHBCLKCTRL |= BIT(7);

  // Prescale max value (sec 18.7.4)
  LPC_TMR16B0->PR = 48;
  // The TC will be reset if MR1 matches it (sec 18.7.6)
  LPC_TMR16B0->MCR = BIT(4);
  // Timer counter match value for period (sec 18.7.7)
  LPC_TMR16B0->MR1 = period;
  // Timer counter match value for pulse width (sec 18.7.7)
  LPC_TMR16B0->MR0 = period;
  // Set CT16B0_MAT0 to 1 on match (sec 18.7.10)
  LPC_TMR16B0->EMR |= BIT(5);
  // Select timer mode (sec 18.7.11)
  LPC_TMR16B0->CCR = 0;
  // PWM mode is enabled for CT16B0_MAT0 (sec 18.7.12)
  LPC_TMR16B0->PWMC = BIT(0);
}

// Resume PWM by enabling and resetting the timer counter
void PWM_enable(void) {
  // Enable and reset timer counter (sec 18.7.2)
  LPC_TMR16B0->TCR |= (BIT(1) | BIT(0));
  // Clear reset bit (sec 18.7.2)
  LPC_TMR16B0->TCR &= ~BIT(1);
}

// Pause PWM by disabling the timer counter
void PWM_disable(void) {
  // Disable timer counter (sec 18.7.2)
  LPC_TMR16B0->TCR &= ~BIT(0);
}

// Set the PWM pulse width
void PWM_width(uint16_t width) {
  PWM_disable();
  // Timer counter match value for pulse width (sec 18.7.7)
  LPC_TMR16B0->MR0 = LPC_TMR16B0->MR1 - width;
  PWM_enable();
}

// Set the PWM duty cycle
/* void setDuty(uint8_t duty) {
  PWM_width(pwmPeriod / (100 - duty));
} */
