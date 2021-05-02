# This-Shortcut-Thingy

A fork of [That-Shortcut-Thingy](https://github.com/Ordim3n/That-Shortcut-Thingy) by [Ordim3n](https://github.com/Ordim3n)

## Usage instructions:
  Needs [FastBoot3ds](https://github.com/derrekr/fastboot3DS) or the discontinued [GodMode9 bootloader](https://github.com/d0k3/GodMode9/releases/tag/v1.9.2pre1) as Firm0 instead of B9S
  
  1. Install the CIA
  2. Launch the shortcut
 
The app will search for a payload in the folders specified in makefile and if it finds something it will boot that, otherwise it will boot the included payload.
**If you are using FastBoot3ds, FCRAM Boot needs to be enabled. It is located under Boot Setup.**

## Building instructions:
  1. Setup devkitPro and install 3ds-dev
  2. Download and add bannertool and makerom to path
  3. Put the icon, banner image and banner sound in the assets folder
  4. Change `APP_TITLE`, `APP_DESCRIPTION`, `APP_AUTHOR`, `APP_PRODUCT_CODE`, `APP_UNIQUE_ID`, `PATH1`, `PATH2`, `PATH3` and `PATH4` to your needs 
  5. Drop the payload and the splash screen (using the same method as Luma3ds splashes) in the romfs folder
  6. run `make`
  7. have fun!

**Note: You need to install bannertool and makerom
