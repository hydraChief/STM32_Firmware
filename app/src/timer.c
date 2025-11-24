#include "timer.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>

#define PRESCALER (83) // 84MHz / (83 + 1) = 1MHz timer clock : this is frequency of counter increment
#define ARR (9999)   // 1MHz / (9999 + 1) = 100Hz PWM frequency


void timer_setup(void){
    rcc_periph_clock_enable(RCC_TIM2);

    // High level timer config
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

    //setupr PWM mode
    timer_set_oc_mode(TIM2, TIM_OC1,TIM_OCM_PWM1);

    //enable PWM output
    timer_enable_counter(TIM2);
    timer_enable_oc_output(TIM2, TIM_OC1);

    //set prescaler and period
    timer_set_prescaler(TIM2, PRESCALER);
    timer_set_period(TIM2, ARR);
}

void timer_pwm_set_duty_cycle(float duty_cycle){
    const float value= (float)ARR*(duty_cycle/100.0f);
    timer_set_oc_value(TIM2, TIM_OC1, (uint32_t)value);
}