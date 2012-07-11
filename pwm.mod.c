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
	{ 0x306023f7, "module_layout" },
	{ 0xc1c0ff34, "omap_dm_timer_free" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x23446992, "omap_dm_timer_stop" },
	{ 0xd77fe604, "omap_dm_timer_start" },
	{ 0x7a12a827, "omap_dm_timer_set_int_enable" },
	{ 0x6686d03d, "omap_dm_timer_set_load" },
	{ 0x9bdb3dd, "clk_get_rate" },
	{ 0x6e5d69d8, "omap_dm_timer_get_fclk" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x1b08474e, "omap_dm_timer_get_irq" },
	{ 0xc88794aa, "omap_dm_timer_set_prescaler" },
	{ 0x5bf90808, "omap_dm_timer_set_source" },
	{ 0x3ba5a568, "omap_dm_timer_request" },
	{ 0x27e1a049, "printk" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xa77a15ed, "omap_dm_timer_read_status" },
	{ 0xe8f7df8f, "omap_dm_timer_write_status" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "1A3F15723C58C652018093D");
