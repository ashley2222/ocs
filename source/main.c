#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <fcntl.h>
#include "kernel.h"
#include "asm.h"
#include "exploits.h"
#include "archive.h"
#include "log.h"
#include "httpc.h"
#include "fs.h"
#include "jsmn.h"

PrintConsole top, bottom;

#define result(str,ret,steps,step_count) print("Result for %s:",str); \
if(ret == 0) \
{	\
	progressbar("Total Progress:", step_count, steps, true);	\
	printf("\x1b[1;32m"); \
	print("Success\n");	\
	printf("\x1b[1;37m"); \
}	\
else	\
{	\
	printf("\x1b[1;31m"); \
	print("Fail: %08lX\n", ret); \
	printf("\x1b[1;37m"); \
}	

void downloadExtractStep1()
{
	progressbar("Total Progress:", 0, 5, true);
	print("Downloading safeb9sinstaller\n");
	Result ret = httpDownloadData(parseApi("https://api.github.com/repos/d0k3/SafeB9SInstaller/releases/latest", ".zip"));//safeb9sinstaller by d0k3
	result("Safeb9sinstaller Download", ret, 5, 1);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "SafeB9SInstaller.bin", "safehaxpayload.bin","/");
	httpFree();
	print("Downloading boot9strap\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/SciresM/boot9strap/releases/latest",".zip"));//b9s by scrisem
	result("b9s Download", ret, 5, 2);
	mkdir("/boot9strap",0777);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot9strap.firm", "boot9strap.firm", "/boot9strap/");
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot9strap.firm.sha", "boot9strap.firm.sha", "/boot9strap/");
	httpFree();
	print("Downloading luma\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/AuroraWright/Luma3DS/releases/latest", ".7z"));//luma by aurorawright
	result("Luma Download", ret, 5, 3);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot.firm", "boot.firm", "/");
	httpFree();
}

void ciaInstall(void *data, u32 size, int total, int step)
{
	Handle cia;
	Result ret = amInit();
	result("amInit", ret, total ,step);
	AM_InitializeExternalTitleDatabase(false);
	ret = AM_StartCiaInstall(MEDIATYPE_SD, &cia);
	result("Start_CiaInstall", ret, total ,step);
	ret = FSFILE_Write(cia, NULL, 0, data, size, 0);
	result("CIA write", ret, total ,step);
	ret = AM_FinishCiaInstall(cia);
	result("Finish Cia Install", ret, total ,step);
	amExit();
}

void doExploitsStep1()
{
	Result ret = 1;
	while(ret > 0)
	{
		ret = udsploit();
		result("Udsploit", ret, 5, 4);
		if(ret == 0)
			ret = hook_kernel();
		result("hook_kernel", ret, 5, 5);
	}
	safehax();
}

void downloadExtractStep2()
{
	progressbar("Total Progress:", 0, 9, true);
	print("Downloading and Installing lumaupdater\n");
	Result ret = httpDownloadData(parseApi("https://api.github.com/repos/KunoichiZ/lumaupdate/releases/latest", ".cia")); //lumaupdater by hamcha & KunoichiZ
	result("Download", ret, 9, 1);
	ciaInstall(httpRetrieveData(), httpBufSize(), 9, 1);
	httpFree();
	print("Downloading and Installing DSP1\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/zoogie/DSP1/releases/latest", ".cia"));//DSP1 by zoogie
	result("Download", ret, 9, 2);
	ciaInstall(httpRetrieveData(), httpBufSize(), 9, 2);
	httpFree();
	print("Downloading and Installing Anemone3DS\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/astronautlevel2/Anemone3DS/releases/latest", ".cia"));//Anemone3ds by AstronaultLevel2
	result("Download", ret, 9, 3);
	ciaInstall(httpRetrieveData(), httpBufSize(), 9, 3);
	httpFree();
	print("Downloading and Installing FBI\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/steveice10/FBI/releases/latest", ".cia"));//FBI by steveice10
	result("Download", ret, 9, 4);
	ciaInstall(httpRetrieveData(), httpBufSize(), 9, 4);
	httpFree();
	print("Downloading boot.3dsx\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/fincs/new-hbmenu/releases/latest", ".3dsx"));// By smealum & others
	result("Download", ret, 9, 5);
	fsOpenAndWrite("/boot.3dsx",httpRetrieveData(), httpBufSize());
	httpFree();
	print("Downloading godmode9\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/d0k3/GodMode9/releases/latest", ".zip"));// By d0k3
	result("Download", ret, 9, 6);
	mkdir("/luma/payloads", 0777);
	mkdir("/gm9",0777);
	mkdir("/gm9/scripts", 0777);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "__ALL__", "__NOTUSED__", "__NOTUSED__");
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "GodMode9.firm", "GodMode9.firm", "/luma/payloads/"); 
	print("Downloading godmode9 sd card cleaup script\n");
	ret = httpDownloadData("http://3ds.guide/gm9_scripts/cleanup_sd_card.gm9"); //By d0k3
	result("Download", ret, 9, 7);
	fsOpenAndWrite("/gm9/scripts/cleanup_sd_card.gm9", httpRetrieveData(), httpBufSize());
	print("Downloading godmode9 ctr-nand luma script\n");
	ret = httpDownloadData("http://3ds.guide/gm9_scripts/setup_ctrnand_luma3ds.gm9"); //By d0k3
	result("Download", ret, 9, 8);
	fsOpenAndWrite("/gm9/scripts/setup_ctrnand_luma3ds.gm9", httpRetrieveData(), httpBufSize());
	httpFree();
	//Best time to install hblauncher_loader
	print("Downloading hblauncher_loader\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/yellows8/hblauncher_loader/releases/latest", ".zip"));//hblauncher_loader by yellows8
	result("Download", ret, 9, 9);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "hblauncher_loader.cia", "hblauncher_loader.cia", "/");
	httpFree();
	u32 size;
	u8 *data = fsOpenAndRead("/hblauncher_loader.cia", &size);
	printf("Trying to install hblauncher_loader.cia\n");
	ciaInstall(data, size, 9, 9);
	free(data);
}

int main()
{
	//preliminary stuff
	gfxInitDefault();
	logInit();
	httpcInit(0);
	consoleInit(GFX_TOP, &top);
	consoleInit(GFX_BOTTOM, &bottom);
	consoleSelect(&bottom);
	printf("\x1b[1;37m");
	printf("Welcome to \x1b[1mOCS!!\x1b[0m\nMade by:- \x1b[1;32mKartik\x1b[1;37m\nSpecial Thanks to :-\n\x1b[1;33mChromaryu\x1b[1;37m:- For Testing\n\x1b[1;35mSmealum\x1b[1;37m and \x1b[1;33myellows8\x1b[1;37m:- For udsploit\n\x1b[1;36mTinivi\x1b[1;37m for safehax");
	consoleSelect(&top);
	printf("\x1b[1;37m");
	bool cfwflag = false;
	u32 status;
	ACU_GetWifiStatus(&status);
	if(status == 0)
	{
		printf("You're not connected to the internet, please connect to a internet and open this app again. Press START to exit.");
	}
	else
	{	
		printf("Press A to begin\n");
		while(aptMainLoop())
		{
			hidScanInput();
			if(hidKeysDown() & KEY_A)
			break;
		}
	
		printf("Checking if cfw is installed\n");
		lumainfo version;
		Result ret = checkLumaVersion(&version);
		printf("Ret %08lX", ret);
		(ret == 0xF8C007F4) ? (cfwflag = false) : (cfwflag = true);
		if(cfwflag == false)
		{
			printf("Not running cfw\n");
			if(checkFileExists("/safehaxpayload.bin") == 0) //check if files already exsist for step 1.
			{	
				print("Downloading files for CFW installation\n");
				downloadExtractStep1();
			}	
			printf("Running exploits\n");
			doExploitsStep1();
		}
		else
		{
			//User is running luma cfw
			printf("Running cfw\n");
			if(ret == 0)
			{
				double ver = version.versionMajor + (double)(version.versionMinor/10);
				if(ver < 7.1)
				{
					printf("A9LH has been detected. Press start to exit.");
				}
			}
			print("Downloading files for Step 2...\n");
			//parseApi("https://api.github.com/repos/pirater12/ocs/releases/latest");
			downloadExtractStep2();
			printf("Proccess Finished. Press Start to exit and enjoy\n");
		}
	}
	
	while(aptMainLoop())
	{
		hidScanInput();

		if(hidKeysDown() & KEY_START)
			break;

	}
	httpcExit();
	logExit();
	gfxExit();
	return 0;
}