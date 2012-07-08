#include <linux/module.h>		
#include <linux/kernel.h>
#include <linux/moduleparam.h>		
#include <linux/init.h>			
#include <linux/clk.h>		
#include <linux/irq.h>
#include <plat/gpio.h>
#include <linux/interrupt.h>
#include <asm/io.h>			
#include <plat/dmtimer.h>	
#include <linux/types.h>

// opaque pointer to timer object
static struct omap_dm_timer *timer_ptr;

// the IRQ # for our gp timer
static int32_t timer_irq;


// do some kernel module documentation
MODULE_AUTHOR("Justin Griggs <justin@bustedengineering.com>");
MODULE_DESCRIPTION("OMAP4460 and PWM GPIO generation Module");
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

 // print obnoxious text
  	printk("Meow Meow Meow %d\n", irq_counter ++);
	
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

static int __init pwm_start(void)
{
	int ret = 0;
  	struct clk *timer_fclk;
	uint32_t gt_rate;
	
	printk(KERN_INFO "Loading PWM Module... \n");

// request a timer (we are asking for ANY open timer, see dmtimer.c for details on how this works)
	timer_ptr = omap_dm_timer_request();
	if(timer_ptr == NULL){
		// no timers available
		printk("pwm module: No more gp timers available, bailing out\n");
		return -1;
	}

	// set the clock source to system clock
	omap_dm_timer_set_source(timer_ptr, OMAP_TIMER_SRC_32_KHZ);

	// set prescalar to 1:1
	omap_dm_timer_set_prescaler(timer_ptr, 0);		
	
	// figure out what IRQ our timer triggers
	timer_irq = omap_dm_timer_get_irq(timer_ptr);

	// install our IRQ handler for our timer
	ret = request_irq(timer_irq, timer_irq_handler, IRQF_DISABLED | IRQF_TIMER , "pwm", timer_irq_handler);
	if(ret){
		printk("pwm: request_irq failed (on irq %d), bailing out\n", timer_irq);
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
	printk("pwm: GP Timer initialized and started (%lu Hz, IRQ %d)\n", (long unsigned)gt_rate, timer_irq);

	// return sucsess
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
}

// entry and exit points
module_init(pwm_start);
module_exit(pwm_end);
