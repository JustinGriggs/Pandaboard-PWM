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

#define TIMER_MAX 0xFFFFFFFF


// do some kernel module documentation
MODULE_AUTHOR("Justin Griggs <justin@bustedengineering.com>");
MODULE_DESCRIPTION("OMAP PWM GPIO generation Module for robotics applications");
MODULE_LICENSE("GPL");


static void timer_handler(void) {

	// reset the timer interrupt status
	omap_dm_timer_write_status(timer_ptr,OMAP_TIMER_INT_OVERFLOW);
	omap_dm_timer_read_status(timer_ptr); //you need to do this read
	//omap_dm_timer_write_counter(timer_ptr,0);	
	//printk("pwm module: Interrupt ");

 	// toggle pin
	if(gpio_get_value(pwm_data_ptr.pin) == 0 ) {
		gpio_set_value(pwm_data_ptr.pin,1);
		//printk("high \n");
	}
	else {
		gpio_set_value(pwm_data_ptr.pin,0);
		//printk("low \n");
	}

}

 

//the interrupt handler
static irqreturn_t timer_irq_handler(int irq, void *dev_id) {

	timer_handler();
	// tell the kernel it's handled
	return IRQ_HANDLED;
}

// set the pwm frequency
static int set_pwm_freq(int freq) {

	// set preload, and autoreload of the timer
	//
	uint32_t period = pwm_data_ptr.timer_rate / (4*freq);
	uint32_t load = TIMER_MAX+1 - period;
	omap_dm_timer_set_load(timer_ptr, 1,load);
	//store the new frequency
	pwm_data_ptr.frequency = freq;
	pwm_data_ptr.load = load;
	
	return 0;
}

// set the pwm duty cycle
static int set_pwm_dutycycle(uint32_t pin,int dutycycle) {
	
	//uint32_t val = TIMER_MAX+1 - (256*dutycycle/pwm_data_ptr.frequency);
	uint32_t val = 	TIMER_MAX+1 - 2*pwm_data_ptr.load;
	omap_dm_timer_set_match(timer_ptr,1,pwm_data_ptr.load-0x100);
	pwm_data_ptr.dutycycle = dutycycle;

	return 0;
}

// setup a GPIO pin for use
static int pwm_setup_pin(uint32_t gpio_number) {

	int err;

	// see if that pin is available to use
	if (gpio_is_valid(gpio_number)) {

		printk(KERN_INFO "pwm module: setting up gpio pin %i...",gpio_number);
		// allocate the GPIO pin
		err = gpio_request(gpio_number,"pwmIRQ");
		//error check
		if(err) {
			printk(KERN_WARNING "pwm module: failed to request GPIO %i\n",gpio_number);
			return -1;
		}

		// set as output
		err = gpio_direction_output(gpio_number,0);

		//error check
		if(err) {
			printk(KERN_WARNING "pwm module: failed to set GPIO to ouput\n");
			return -1;
		}

		//add gpio data to struct
		pwm_data_ptr.pin = gpio_number;
	}
	else
	{
		printk(KERN_DEBUG "pwm module: requested GPIO is not valid\n");
		// return failure
		return -1;
	}

	// return success
	printk(KERN_INFO "pwm module: setup DONE\n");
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
		printk(KERN_ERR "pwm module: No more gp timers available, bailing out\n");
		return -1;
	}

	// set the clock source to the system clock
	ret = omap_dm_timer_set_source(timer_ptr, OMAP_TIMER_SRC_SYS_CLK);
	if(ret) {
		printk(KERN_ERR "pwm module: could not set source\n");
		return -1;
	}

	// set prescalar to 1:1
	omap_dm_timer_set_prescaler(timer_ptr, 0);

	// figure out what IRQ our timer triggers
	timer_irq = omap_dm_timer_get_irq(timer_ptr);

	// install our IRQ handler for our timer
	ret = request_irq(timer_irq, timer_irq_handler, IRQF_DISABLED | IRQF_TIMER , "pwm", timer_irq_handler);
	if(ret){
		printk(KERN_WARNING "pwm module: request_irq failed (on irq %d), bailing out\n", timer_irq);
		return ret;
	}

	// get clock rate in Hz and add it to struct
	timer_fclk = omap_dm_timer_get_fclk(timer_ptr);
	gt_rate = clk_get_rate(timer_fclk);
	pwm_data_ptr.timer_rate = gt_rate;

	// set preload, and autoreload
	// we set it to a default of 1kHz
	set_pwm_freq(1000);

	// setup timer to trigger IRQ on the overflow
	omap_dm_timer_set_int_enable(timer_ptr, OMAP_TIMER_INT_OVERFLOW);
	
	// start the timer
	omap_dm_timer_start(timer_ptr);

	// done!
	printk(KERN_INFO "pwm module: GP Timer initialized and started (%lu Hz, IRQ %d)\n",
	(long unsigned)gt_rate, timer_irq);


	
	// setup a GPIO
	pwm_setup_pin(38);
	
	pwm_data_ptr.pin = 38;

	set_pwm_dutycycle(1,150);
	

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
