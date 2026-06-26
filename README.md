# rtl8723bs v5.2.17.1_26955.20180307_COEX20180201-6f52

Patched driver for newer linux kernel versions (tested on 6.12 to 7.0).

I'm using this driver on the ASUS Tinker Board S (RK3288 - ARMv7) under LibreELEC.

Module name: 8723bs

List available parameters: `ls /sys/module/8723bs/parameters` or `modinfo 8723bs`

## Credits

Inspired by:

https://github.com/RinCat/RTL88x2BU-Linux-Driver

Forked from:

https://github.com/Lebensgefahr/rockchip_wlan

Original source from:

https://github.com/rockchip-linux/kernel/tree/release-4.4/drivers/net/wireless/rockchip_wlan/rtl8723bs

## Why use this driver instead of the staging driver from the kernel?

The staging driver often does not show all SSIDs and has the "invalid_key" error.
If you manage to connect, it often randomly disconnects or won't automatically connect at all.
This old driver is very stable in this regard but has no WPA3 support. Use at your own risk.
