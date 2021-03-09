/*
 *  WYLINK WY-A934X TEST board support
 *
 *  Copyright (C) 2014 Michel Stempin <michel.stempin@wanadoo.fr>
 *  Copyright (C) 2020 Wylink TEC.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
*/

#include <linux/gpio.h>
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define WY_A934X_TEST_GPIO_LED_WAN		18

#define A934X_MAC0_OFFSET	0x0000
#define A934X_MAC1_OFFSET	0x0006
#define A934X_WMAC_OFFSET	0x000C
#define A934X_CALDATA_OFFSET	0x1000
#define A934X_PCIE_CALDATA_OFFSET	0x5000

static struct gpio_led a934x_leds_gpio[] __initdata = {
	{
		.name		= "a934x:green:wan",
		.gpio		= WY_A934X_TEST_GPIO_LED_WAN,
		.active_low	= 1,
	},
};

static void __init a934x_gpio_led_setup(void)
{
	ath79_gpio_direction_select(WY_A934X_TEST_GPIO_LED_WAN, true);

	gpio_set_value(WY_A934X_TEST_GPIO_LED_WAN, 1);

	ath79_gpio_output_select(WY_A934X_TEST_GPIO_LED_WAN, 0);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(a934x_leds_gpio),
			a934x_leds_gpio);
}

static void __init a934x_setup(void)
{

	/* ART base address */
	u8 *art = (u8 *) KSEG1ADDR(0x1f7f0000);

	/* register flash. */
	ath79_register_m25p80(NULL);

	/* enable usb */
	ath79_register_usb();

	/* enable pci */
	ap91_pci_init(art + A934X_PCIE_CALDATA_OFFSET, NULL);

	/* MDIO Interface */
	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	// ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_ONLY_MODE);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(4);

	/* register eth0 as WAN, eth1 as LAN */
	/* WAN port */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_init_mac(ath79_eth0_data.mac_addr, art + A934X_MAC0_OFFSET, 0);
	ath79_register_eth(0);

	/* LAN port*/
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_init_mac(ath79_eth1_data.mac_addr, art + A934X_MAC1_OFFSET, 0);
	ath79_register_eth(1);

	/* register wireless mac with cal data */
	ath79_register_wmac(art + A934X_CALDATA_OFFSET, art + A934X_WMAC_OFFSET);
}

MIPS_MACHINE(ATH79_MACH_WY_A934X_TEST, "WY-A934X-TEST", "WYLINK WY-A934X-TEST", a934x_setup);
