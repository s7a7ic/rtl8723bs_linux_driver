# rtl8723bs linux driver

Realtek SDIO Wi-Fi driver patched for newer linux kernel versions (tested on 6.12 to 7.0).

Driver version: `v5.2.17.1_26955.20180307_COEX20180201-6f52`

I'm using this driver on the ASUS Tinker Board S (RK3288 - ARMv7) under LibreELEC.

Module name: 8723bs

List available parameters: `ls /sys/module/8723bs/parameters` or `modinfo 8723bs`

## Goal of this branch

1. Cleanup and remove as much unused code to still build a working driver (sledgehammer approach)
2. Get inspiration from linux staging driver for further cleanup
3. Compare to linux staging driver and patch some quirks of the staging driver

## Credits

Hopefully will soon be replaced by:

https://github.com/MocLG/rtw88-rtl8723bs

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
