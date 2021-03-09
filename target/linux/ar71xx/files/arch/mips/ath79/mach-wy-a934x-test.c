https://github.com/test898qwerq/7628/tree/openwrt-18.06/target/linux/ar71xx

/*
 * Atheros WY_A934X_TEST reference board support
 *
 * Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
 * Copyright (c) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define WY_A934X_TEST_GPIO_LED_WLAN		13
#define WY_A934X_TEST_GPIO_LED_WPS		15
#define WY_A934X_TEST_GPIO_LED_SYSTEM		14

#define WY_A934X_TEST_GPIO_LED_WAN		18
#define WY_A934X_TEST_GPIO_LED_LAN1		19
#define WY_A934X_TEST_GPIO_LED_LAN2		20
#define WY_A934X_TEST_GPIO_LED_LAN3		21
#define WY_A934X_TEST_GPIO_LED_LAN4		12

#define WY_A934X_TEST_GPIO_BTN_WPS		17
#define WY_A934X_TEST_GPIO_BTN_RESET		17

#define WY_A934X_TEST_KEYS_POLL_INTERVAL	14	/* msecs */
#define WY_A934X_TEST_KEYS_DEBOUNCE_INTERVAL	(3 * WY_A934X_TEST_KEYS_POLL_INTERVAL)

#define WY_A934X_TEST_MAC0_OFFSET		0x0000
#define WY_A934X_TEST_MAC1_OFFSET		0x0006
#define WY_A934X_TEST_WMAC_CALDATA_OFFSET	0x000C
#define WY_A934X_TEST_CALDATA_OFFSET	0x1000
#define WY_A934X_TEST_PCIE_CALDATA_OFFSET	0x5000

static struct gpio_led a934x_leds_gpio[] __initdata = {
	{
		.name		= "a934x:green:system",
		.gpio		= WY_A934X_TEST_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
	{
		.name		= "a934x:green:wlan",
		.gpio		= WY_A934X_TEST_GPIO_LED_WLAN,
		.active_low	= 1,
	},
	{
		.name		= "a934x:green:lan1",
		.gpio		= WY_A934X_TEST_GPIO_LED_LAN1,
		.active_low	= 1,
	},
	{
		.name		= "a934x:green:lan2",
		.gpio		= WY_A934X_TEST_GPIO_LED_LAN2,
		.active_low	= 1,
	},
	{
		.name		= "a934x:green:lan3",
		.gpio		= WY_A934X_TEST_GPIO_LED_LAN3,
		.active_low	= 1,
	},
	{
		.name		= "a934x:green:lan4",
		.gpio		= WY_A934X_TEST_GPIO_LED_LAN4,
		.active_low	= 1,
	},
	{
		.name		= "a934x:green:WAN",
		.gpio		= WY_A934X_TEST_GPIO_LED_WAN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button a934x_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WY_A934X_TEST_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WY_A934X_TEST_GPIO_BTN_RESET,
		.active_low	= 1,
	},{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WY_A934X_TEST_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WY_A934X_TEST_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static void __init a934x_gpio_led_setup(void)
{
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_WLAN, true);
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_WAN, true);
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_WPS, true);
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_LAN1, true);
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_LAN2, true);
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_LAN3, true);
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_LAN4, true);

	gpio_set_value(WY_A934X_TEST_GPIO_LED_WLAN, 1);
	gpio_set_value(WY_A934X_TEST_GPIO_LED_WAN, 1);
	gpio_set_value(WY_A934X_TEST_GPIO_LED_WPS, 1);
	gpio_set_value(WY_A934X_TEST_GPIO_LED_LAN1, 1);
	gpio_set_value(WY_A934X_TEST_GPIO_LED_LAN2, 1);
	gpio_set_value(WY_A934X_TEST_GPIO_LED_LAN3, 1);
	gpio_set_value(WY_A934X_TEST_GPIO_LED_LAN4, 1);

	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_WLAN, 0);
	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_WAN, 0);
	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_WPS, 0);
	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_LAN1, 0);
	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_LAN2, 0);
	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_LAN3, 0);
	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_LAN4, 0);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(a934x_leds_gpio),
			a934x_leds_gpio);
}

static void __init a934x_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1f7f0000);

	ath79_register_m25p80(NULL);

	a934x_gpio_led_setup();

	ath79_register_usb();

	ath79_wmac_set_led_pin(WY_A934X_TEST_GPIO_LED_WLAN);
	ath79_register_wmac(art + WY_A934X_TEST_WMAC_CALDATA_OFFSET, NULL);

	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + WY_A934X_TEST_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + WY_A934X_TEST_MAC1_OFFSET, 0);

	/* WAN port */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	/* LAN ports */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_WY_A934X_TEST, "WY_A934X_TEST", "Qualcomm Atheros WY_A934X_TEST reference board",
	     a934x_setup);
