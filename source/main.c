#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <3ds.h>

#include "LED.h"
#include "MCU.h"

//#define PATH_BOOTFIRM "boot.firm"
//#define PATH_LOG "TST/log.txt"
//#define PATH_FB3DSCFGSD "3DS/fastbootcfg.txt"
//#define PATH_FB3DSCFGCTR "/fastboot3ds/fastbootcfg.txt"
//#define FB3DS_FIRMBOOTSTR "RAM_FIRM_BOOT = Enabled"

#define FIRM_MAXSIZE (1024 * 1024 * 4 - 0x200)
#define FIRM_OFFSET (0x1000)
#define CONF_REG	0x17
#define PATH_SPLASH "romfs:/splash.bin"

#define PATH_PAYLOADSD1 PATH1 APP_TITLE ".firm"
#define PATH_PAYLOADSD2 PATH2 APP_TITLE ".firm"
#define PATH_PAYLOADSD3 PATH3 APP_TITLE ".firm"
#define PATH_PAYLOADSD4 PATH4 APP_TITLE ".firm"

#define PATH_PAYLOADRFS "romfs:/payload.firm"

void *buf = NULL;

void __attribute__((weak)) __appInit(void) 
{
    buf = linearAlloc(FIRM_OFFSET + FIRM_MAXSIZE);    
    
	// Initialize services
    srvInit();
    aptInit();
    acInit();
    hidInit();
    fsInit();
    archiveMountSdmc();
	romfsInit();
	
}

void __attribute__((weak)) __appExit(void) 
{
	// Exit services
	archiveUnmountAll();
	romfsExit();
    fsExit();
	gfxExit();
    hidExit();
	acExit();
    aptExit();
    srvExit();

    GSPGPU_FlushDataCache(buf, FIRM_MAXSIZE);
    linearFree(buf);
}

/*void wait_for_a()
{
	while(1)
		if(hidKeysDown() & KEY_A)
			break;
}*/

/*int RamBootFB3ds()
{
	size_t size;
	char config[25], *strout;
	FILE *cfg = fopen(PATH_FB3DSCFGSD, "r"), *log = fopen(PATH_LOG,"wb");

	if(cfg)
	{
		while(fgets(config, sizeof(config), cfg))
		{
			if(config == FB3DS_FIRMBOOTSTR)
			{
				return 1;
			}
			fprintf(log, "fgets read '%s'\n", config);
		}
		return 2;
	}
	fclose(log);
	return 0;
}*/
int main() 
{
	
    gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	
	u8 *contents, slot, annoy;
	FILE *payload, *splash, *log;
	size_t splash_size, payload_size;
	
	if (!buf)
	{
		printf("Error: out of memory\n");
    }
	mcuWriteRegister(CONF_REG,(u8*) 0, 1);
	mcuReadRegister(CONF_REG, &annoy, 1);
    printf("firm@%08lX\n", (u32) buf + FIRM_OFFSET);
    if ((u32) buf != 0x14000000) 
	{
		// must be at the start of FCRAM
        printf("Bad firm location\n");
    }
	
	splash = fopen(PATH_SPLASH, "rb");
	fseek(splash, 0, SEEK_END);
	splash_size = ftell(splash);
	rewind(splash);
	contents = malloc(splash_size);
	fread(contents, 1, splash_size, splash);
	memcpy(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), contents, splash_size);
	free(contents);
	gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();
	
	/*if(!fopen(PATH_DEFAULT, "r"))
	{
		printf("%s not found on the root of the SD\n", PATH_DEFAULT);
		gfxFlushBuffers();
		gfxSwapBuffers();
		fixcolor(255,0,0);
		for(int i=0; i<=300; i++) gspWaitForVBlank();
		errfInit();
		ERRF_ThrowResultWithMessage((Result) 'TST', "Read the README next time");
		errfExit();
	}*/

/*
	I know this is the worst thing ever but I'm not a programmer and I don't know how to make this prettier.
	Again sorry for the mess.

	This mess is used to check if PATH_PAYLOADSD1-4 isn't set to none and then tries to load it.
	If something is returned then it sets payload_found to 1 and skips the other checks.
	If nothing is returned then it checks the next path until it runs out of them.
	If no payloads have been found in the sd paths it loads the payload from ROMFS.
*/

	int payload_found = 0;

	if(PATH_PAYLOADSD1 != "none") {
		payload = fopen(PATH_PAYLOADSD1, "r");

		if (payload) {
			payload_found = 1;
			printf("Custom payload found on Path1 (SD)\n");
		}
	}

	if(PATH_PAYLOADSD2 != "none" && payload_found == 0) {
		payload = fopen(PATH_PAYLOADSD2, "r");

		if (payload) {
			payload_found = 1;
			printf("Custom payload found on Path2 (SD)\n");
		}
	}

	if(PATH_PAYLOADSD3 != "none" && payload_found == 0) {
		payload = fopen(PATH_PAYLOADSD3, "r");

		if (payload) {
			payload_found = 1;
			printf("Custom payload found on Path3 (SD)\n");
		}
	}

	if(PATH_PAYLOADSD4 != "none" && payload_found == 0) {
		payload = fopen(PATH_PAYLOADSD4, "r");

		if (payload) {
			payload_found = 1;
			printf("Custom payload found on Path4 (SD)\n");
		}
	}

	if(payload_found == 0) {
		payload = fopen(PATH_PAYLOADRFS, "r");

		if (payload) {
			printf("Using integrated payload (RomFS)\n");
		}
	}

	//log = fopen(PATH_LOG,"w");
	//fprintf(log, "RamBootFB3ds() returned %d", ret);
	//fclose(log);
	/*if(!ret && !annoy)
	{
		fixcolor(255, 0, 0);
		printf("FastBoot 3DS config not found, %s might not launch.\nPress (A) to get rid of this message\nPress (B) to ignore\n", APP_TITLE);
		while(1)
		{
			if(hidKeysDown() & KEY_A)
			{
				mcuWriteRegister(CONF_REG,(u8*) 1, 1);
				break;
			}
			if(hidKeysDown() & KEY_B)
			{
				break;
			}
		}
	}
	else if(ret == 2)
	{
		fixcolor(255, 165, 0);
		printf("FastBoot 3DS not properly configured.\nPlease enable 'FCRAM Boot' under the 'Boot Setup' menu\n");
		for(int i=0; i<=60*5; i++) gspWaitForVBlank();
	}*/
	
	fseek(payload, 0, SEEK_END);
	payload_size = ftell(payload);
	rewind(payload);
	if(fread(buf + 0x1000, 1, payload_size, payload) != payload_size)
		fixcolor(255, 0, 0);
	else
		fixcolor(0, 255, 0);
	
	fclose(payload);
	
	for(int i=0; i<=60; i++) gspWaitForVBlank();
	stfuled();
	APT_HardwareResetAsync();
	
    
    return 0;
}
