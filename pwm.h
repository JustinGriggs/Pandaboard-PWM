/*
 * pwm.h
 *
 */


#ifndef PWM_H
#define PWN_H

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
};


static int set_pwm_freq(int freq);
static int set_pwm_dutycycle(int dutycycle);
static int pwm_setup_pin(int gpio_number);

#endif
