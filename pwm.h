/*
 * pwm.h
 *
 */


#ifndef PWM_H
#define PWN_H

//pwm prescaler settings
#define TIMER_PRESC_1_1 -1
#define TIMER_PRESC_1_2 0
#define TIMER_PRESC_1_4 1
#define TIMER_PRESC_1_8 2
#define TIMER_PRESC_1_16 3
#define TIMER_PRESC_1_32 4
#define TIMER_PRESC_1_64 5
#define TIMER_PRESC_1_128 6
#define TIMER_PRESC_1_256 7 



// opaque pointer to timer object
static struct omap_dm_timer *timer_ptr;

// the IRQ # for our gp timer
static int32_t timer_irq;

// pointer gpio object

// struct for pwm object
struct pwm_data {
	uint32_t frequency;
	uint32_t dutycycle;
	uint32_t pin;
	uint32_t timer_rate;
	uint32_t value;
	uint32_t load;
};


static int set_pwm_freq(int freq);
static int set_pwm_dutycycle(uint32_t pin,int dutycycle);
static int pwm_setup_pin(uint32_t gpio_number);

#endif
