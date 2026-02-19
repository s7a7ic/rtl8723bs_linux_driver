# rtl8723bs v5.2.17.1_26955.20180307_COEX20180201-6f52

Module name: 8723bs

List available parameters: `ls /sys/module/8723bs/parameters` or `modinfo 8723bs`

## Goal of this branch

1. Cleanup and remove as much unused code to still build a working driver
2. Get inspiration from linux staging driver for further cleanup
3. Compare to linux staging driver and patch some quirks of the staging driver

## Credits

Inspired by:

https://github.com/RinCat/RTL88x2BU-Linux-Driver

Forked from:

https://github.com/Lebensgefahr/rockchip_wlan

Original source from:

https://github.com/rockchip-linux/kernel/tree/release-4.4/drivers/net/wireless/rockchip_wlan/rtl8723bs
