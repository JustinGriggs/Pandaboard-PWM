#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <plat/dmtimer.h>
#include <linux/types.h>

#include "pwm.h"

//pointer to data struct
static struct pwm_data pwm_data_ptr;



// do some kernel module documentation
MODULE_AUTHOR("Justin Griggs <justin@bustedengineering.com>");
MODULE_DESCRIPTION("OMAP4460 PWM GPIO generation Module for robotics applications");
MODULE_LICENSE("GPL");

/* The interrupt handler.
This is pretty basic,  we only get an interrupt when the timer overflows,
We are just going to print a really obnoxious message each time
*/
static irqreturn_t timer_irq_handler(int irq, void *dev_id) {

	// reset the timer interrupt status
	omap_dm_timer_write_status(timer_ptr, OMAP_TIMER_INT_OVERFLOW);
	omap_dm_timer_read_status(timer_ptr); // YES, you really need to do this 'wasteful' read

 	// toggle pin
	if(gpio_get_value(pwm_data_ptr.pin)) {
		gpio_set_value(pwm_data_ptr.pin,1);
	}
	else {
		gpio_set_value(pwm_data_ptr.pin,0);
	}

	// tell the kernel it's handled
	return IRQ_HANDLED;
}

// set the pwm frequency
static int set_pwm_freq(int freq) {

	// set preload, and autoreload of the timer
	// the 32kHz source gives a 1ms tick rate, so we
	// set the load to 1/freq 
	uint32_t load = 0xFFFFFFFF - (1/freq * pwm_data_ptr.timer_rate);
	omap_dm_timer_set_load(timer_ptr, 1,load);
	
	return 0;
}

// set the pwm duty cycle
static int set_pwm_dutycycle(uint32_t pin,int dutycycle) {
	
	pwm_data_ptr.dutycycle = dutycycle;
	return 0;
}

// setup a GPIO pin for use
static int pwm_setup_pin(uint32_t gpio_number) {

	int err;

	// see if that pin is available to use
	if (gpio_is_valid(gpio_number)) {

		printk("pwm module: setting up gpio pin %i...",gpio_number);
		// allocate the GPIO pin
		err = gpio_request(gpio_number,"pwmIRQ");
		//error check
		if(err) {
			printk("pwm module: failed to request GPIO %i\n",gpio_number);
			return -1;
		}

		// set as output
		err = gpio_direction_output(gpio_number,0);

		//error check
		if(err) {
			printk("pwm module: failed to set GPIO to ouput\n");
			return -1;
		}

		//add gpio data to struct
		pwm_data_ptr.pin = gpio_number;
	}
	else
	{
		printk("pwm module: requested GPIO is not valid\n");
		// return failure
		return -1;
	}

	// return success
	printk("DONE\n");
	return 0;
}

static int __init pwm_start(void) {

	int ret = 0;
  	struct clk *timer_fclk;
	uint32_t gt_rate;


	printk(KERN_INFO "Loading PWM Module... \n");

	// request any timer
	timer_ptr = omap_dm_timer_request();
	if(timer_ptr == NULL){
		// no timers available
		printk("pwm module: No more gp timers available, bailing out\n");
		return -1;
	}

	// set the clock source to the system clock
	ret = omap_dm_timer_set_source(timer_ptr, OMAP_TIMER_SRC_32_KHZ);
	if(ret) {
		printk("pwm module: could not set source\n");
		return -1;
	}

	// set prescalar to 1:1
	omap_dm_timer_set_prescaler(timer_ptr, 0);

	// figure out what IRQ our timer triggers
	timer_irq = omap_dm_timer_get_irq(timer_ptr);

	// install our IRQ handler for our timer
	ret = request_irq(timer_irq, timer_irq_handler, IRQF_DISABLED | IRQF_TIMER , "pwm", timer_irq_handler);
	if(ret){
		printk("pwm module: request_irq failed (on irq %d), bailing out\n", timer_irq);
		return ret;
	}

	// get clock rate in Hz
	timer_fclk = omap_dm_timer_get_fclk(timer_ptr);
	gt_rate = clk_get_rate(timer_fclk);
	pwm_data_ptr.timer_rate = gt_rate;

	// set preload, and autoreload
	// we set it to a default of 1kHz
	omap_dm_timer_set_load(timer_ptr, 1, 0xFFFFFFFF- (1* gt_rate));

	// setup timer to trigger our IRQ on the overflow event
	omap_dm_timer_set_int_enable(timer_ptr, OMAP_TIMER_INT_OVERFLOW);

	// start the timer!
	omap_dm_timer_start(timer_ptr);

	// done!
	printk("pwm module: GP Timer initialized and started (%lu Hz, IRQ %d)\n", (long unsigned)gt_rate, timer_irq);


	
	// setup a GPIO
	ret = pwm_setup_pin(38);
	if (ret)
		pwm_data_ptr.pin = 38;
	

	// return success
	return 0;
}

static void __exit pwm_end(void) {

	printk(KERN_INFO "Exiting PWM Module. \n");

	// stop the timer
	omap_dm_timer_stop(timer_ptr);

	// release the IRQ handler
	free_irq(timer_irq, timer_irq_handler);

 	// release the timer
  	omap_dm_timer_free(timer_ptr);

	// release GPIO
	gpio_free(pwm_data_ptr.pin);
	
}

// entry and exit points
module_init(pwm_start);
module_exit(pwm_end);
