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


// do some kernel module documentation
MODULE_AUTHOR("Justin Griggs <justin@bustedengineering.com>");
MODULE_DESCRIPTION("OMAP4460 PWM GPIO generation Module for robotics applications");
MODULE_LICENSE("GPL");

/* The interrupt handler.
This is pretty basic,  we only get an interrupt when the timer overflows,
We are just going to print a really obnoxious message each time
*/
static irqreturn_t timer_irq_handler(int irq, void *dev_id)
{
 	 // keep track of how many calls we had
  	static int32_t irq_counter = 0;

	// reset the timer interrupt status
	omap_dm_timer_write_status(timer_ptr, OMAP_TIMER_INT_OVERFLOW);
	omap_dm_timer_read_status(timer_ptr); // YES, you really need to do this 'wasteful' read

 	// set the pins to high, a reset
  	
	
	// tell the kernel it's handled
	return IRQ_HANDLED;
}

// set the pwm frequency
static int set_pwm_freq(int freq)
{
	
	return 0;
}

// set the pwm duty cycle
static int set_pwm_dutycycle(int dutycycle)
{
	return 0;
}

// setup a GPIO pin for use
static int pwm_setup_pin(int gpio_number)
{
	
	// see if that pin is available to use
	if (gpio_is_valid(gpio_number))
	{
		printk("pwm module: setting up gpio pin %i...\n",gpio_number);
		// allocate the GPIO pin
		err = gpio_request(gpio_number,"pwmIRQ");
		//error check
		if(err){
			printk('failed to request GPIO %i',gpio_number);
			return -1;
		}
		
		// set as output
		gpio_direction_ouput(gpio_number,0);
	}
	else
	{
		printk("pwm module: requested GPIO is not valid\n");
		// return failure
		return -1;	
	}
	
	// return success
	return 0;
}

static int __init pwm_start(void)
{
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

	// set the clock source to the 32kHz source
	omap_dm_timer_set_source(timer_ptr, OMAP_TIMER_SRC_32_KHZ);

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

	// set preload, and autoreload
	// we set it to the clock rate in order to get 1 overflow every 3 seconds
	omap_dm_timer_set_load(timer_ptr, 1, 0xFFFFFFFF - (3 * gt_rate));
	
	// setup timer to trigger our IRQ on the overflow event
	omap_dm_timer_set_int_enable(timer_ptr, OMAP_TIMER_INT_OVERFLOW);
	
	// start the timer!
	omap_dm_timer_start(timer_ptr);

	// done!		
	printk("pwm module: GP Timer initialized and started (%lu Hz, IRQ %d)\n", (long unsigned)gt_rate, timer_irq);

	
	// return success
	return 0;
}

static void __exit pwm_end(void)
{
	printk(KERN_INFO "Exiting PWM Module. \n");

	// stop the timer
	omap_dm_timer_stop(timer_ptr);
		
	// release the IRQ handler
	free_irq(timer_irq, timer_irq_handler);

 	// release the timer
  	omap_dm_timer_free(timer_ptr);

	// release GPIO
	
}

// entry and exit points
module_init(pwm_start);
module_exit(pwm_end);
