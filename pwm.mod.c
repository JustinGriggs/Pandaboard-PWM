#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xea9bdd96, "module_layout" },
	{ 0xfe990052, "gpio_free" },
	{ 0x4639a755, "omap_dm_timer_free" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x6a9edb83, "omap_dm_timer_stop" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x76ab6420, "omap_dm_timer_start" },
	{ 0xe065a362, "omap_dm_timer_set_int_enable" },
	{ 0xba4be137, "omap_dm_timer_set_load" },
	{ 0x201ead0e, "clk_get_rate" },
	{ 0x838d833a, "omap_dm_timer_get_fclk" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x37367a34, "omap_dm_timer_get_irq" },
	{ 0x5e07413e, "omap_dm_timer_set_prescaler" },
	{ 0x29af210d, "omap_dm_timer_set_source" },
	{ 0xe20df1cb, "omap_dm_timer_request" },
	{ 0x27e1a049, "printk" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x6c8d5ae8, "__gpio_get_value" },
	{ 0x510742d7, "omap_dm_timer_read_status" },
	{ 0x59031632, "omap_dm_timer_write_status" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "888F4533CD9EF443DD624B3");
