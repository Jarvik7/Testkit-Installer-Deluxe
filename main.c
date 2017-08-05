#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/io/dirent.h>
#include <psp2/shellutil.h>
#include <psp2/power.h>
#include <psp2/kernel/processmgr.h>
#include <vita2d.h>

#include "Archives.h"
#include "TIDUtils.h"

int main() {
	SceCtrlData pad, old_pad;
	unsigned int keys_down;
	old_pad.buttons = 0;
	
	void *buf = malloc(0x100);

	vita2d_pgf *font;
	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
	font = vita2d_load_default_pgf();
	memset(&pad, 0, sizeof(pad));
	
	const char Disclaimer[512] = "This application is provided without warranty. The authors\ncannot be held accountable for any damage or data loss to\nyour Vita system. Although we have ensured that all bugs\nhave been ironed out, and have conducted rigorous testing,\nstill use this installer with caution.\n\nIf you have paid for this software, you have been scammed\nand should demand your money back immediately.\n\nThe full source code can be found at:\nhttps://github.com/Voxel9/Testkit-Installer-Deluxe";
	const char NotContinue[256] = "This installer can NOT continue.\n\nur0:tai/boot_config.txt does not exist. Make sure you have\nHENkaku Enso installed, or reinstall the config if it is inactive.\n\nPress X to exit...";
	const char PSTVNotContinue[256] = "This installer can NOT continue.\n\nThis testkit firmware is currently unsupported on PSTV.\nOnly PS Vita handheld models are supported by this app.\n\nPress X to exit...";
	const char Credits[512] = "CREDITS:\n\nTestkit Installer Deluxe - Voxel\nTestkit Spoofer plugin (kDump.skprx) - Zecoxao\nActivation Spoofing - PSVitaDevTestKit95\nFull Testkit Enabler trick - SKGleba\nSupport and pointers - CelesteBlue and Modz2014\nlpp-vita ZIP extraction code - Rinnegatamante\nHENkaku Enso - Team Molecule";
	const char MainMenuOpts[4][64] = {"Install Testkit Firmware", "Uninstall Testkit Firmware", "Credits", "Exit"};
	const char YesNo[2][4] = {"No", "Yes"};
	
	int config_exists = exists("ur0:tai/boot_config.txt");
	int UIScreen = 0;
	int selected = 0;
	int doStuff = 0;
	
	while(1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);
		keys_down = pad.buttons & ~old_pad.buttons;
		
		vita2d_start_drawing();
		vita2d_clear_screen();
		
		vita2d_pgf_draw_text(font, 215, 45, RGBA8(255,255,255,255), 1.5f, "Testkit Installer Deluxe v0.1a by Voxel");
		
		switch(UIScreen) {
			case 0: // Disclaimer Message
				if (config_exists == 1 && !vshSblAimgrIsGenuineDolce()) vita2d_pgf_draw_text(font, 35, 100, RGBA8(255,255,255,255), 1.5f, Disclaimer);
				else if (config_exists == 0) vita2d_pgf_draw_text(font, 35, 100, RGBA8(255,255,255,255), 1.5f, NotContinue);
				else if (vshSblAimgrIsGenuineDolce()) vita2d_pgf_draw_text(font, 35, 100, RGBA8(255,255,255,255), 1.5f, PSTVNotContinue);
				vita2d_draw_rectangle(402, 425, 145, 70, RGBA8(226,92,92,255));
				vita2d_pgf_draw_text(font, 410, 470, RGBA8(0,0,0,255), 1.5f, "Continue");
				break;
			case 1: // Main Menu
				if (selected < 0) selected = 0;
				if (selected > 3) selected = 3;
				for (int i = 0; i < 4; i++) {
					if (i == selected) {
						vita2d_draw_rectangle(70, (80*i) + 85, 820, 70, RGBA8(226,92,92,255));
						vita2d_pgf_draw_text(font, 85, (80*i) + 130, RGBA8(0,0,0,255), 1.5f, MainMenuOpts[i]);
					} else vita2d_pgf_draw_text(font, 85, (80*i) + 130, RGBA8(255,255,255,255), 1.5f, MainMenuOpts[i]);
				}
				break;
			case 2: // Credits Menu
				vita2d_pgf_draw_text(font, 35, 100, RGBA8(255,255,255,255), 1.5f, Credits);
				vita2d_draw_rectangle(402, 425, 145, 70, RGBA8(226,92,92,255));
				vita2d_pgf_draw_text(font, 410, 470, RGBA8(0,0,0,255), 1.5f, "Continue");
				break;
			case 3: // Install Confirmation
				vita2d_pgf_draw_text(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Are you sure you want to install testkit firmware?\nThe system will restart after the installation.");
				if (selected < 0) selected = 0;
				if (selected > 1) selected = 1;
				for (int i = 0; i < 2; i++) {
					if (i == selected) {
						vita2d_draw_rectangle((200*i) + 302, 328, 145, 70, RGBA8(226,92,92,255));
						vita2d_pgf_draw_text(font, (200*i) + 310, 373, RGBA8(0,0,0,255), 1.5f, YesNo[i]);
					} else vita2d_pgf_draw_text(font, (200*i) + 310, 373, RGBA8(255,255,255,255), 1.5f, YesNo[i]);
				}
				break;
			case 4: // Install Ongoing
				if (doStuff == 0) vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Remounting vs0 as writable...");
				if (doStuff == 1) {
					vshIoUmount(0x300, 0, 0, 0);
					_vshIoMount(0x300, 0, 2, buf);
					vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Wiping vs0 ready for extraction...\nDo not turn off your Vita or exit the app!");
				} else if (doStuff == 2) {
					removePath("vs0:app/NPXS10003");
					removePath("vs0:app/NPXS10008");
					removePath("vs0:app/NPXS10013");
					removePath("vs0:app/NPXS10015");
					removePath("vs0:app/NPXS10026");
					removePath("vs0:app/NPXS10030");
					removePath("vs0:app/NPXS10031");
					removePath("vs0:app/NPXS10033");
					removePath("vs0:app/NPXS10034");
					removePath("vs0:app/NPXS10068");
					removePath("vs0:app/NPXS10103");
					removePath("vs0:app/NPXS10104");
					removePath("vs0:app/NPXS10998");
					removePath("vs0:sys");
					removePath("vs0:vsh");
					vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Installing testkit firmware files...\nDo not turn off your Vita or exit the app!");
				} else if (doStuff == 3) {
					Zip *handle = ZipOpen("app0:/Media/vs0.zip");
					ZipExtract(handle, NULL, "vs0:");
					ZipClose(handle);
					vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Copying testkit spoofer and copying boot_config...");
				} else if (doStuff == 4) {
					copykDump();
					appendConfig(0);
				} else if (doStuff == 5) UIScreen = 5;
				doStuff++;
				break;
			case 5: // Install Complete
				vita2d_pgf_draw_text(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Installation complete. Press X to restart the system.");
				vita2d_draw_rectangle(402, 425, 145, 70, RGBA8(226,92,92,255));
				vita2d_pgf_draw_text(font, 410, 470, RGBA8(0,0,0,255), 1.5f, "Continue");
				break;
			case 6: // Uninstall confirmation
				vita2d_pgf_draw_text(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Are you sure you want to uninstall testkit firmware?\nThe system will restart after the installation.");
				if (selected < 0) selected = 0;
				if (selected > 1) selected = 1;
				for (int i = 0; i < 2; i++) {
					if (i == selected) {
						vita2d_draw_rectangle((200*i) + 302, 328, 145, 70, RGBA8(226,92,92,255));
						vita2d_pgf_draw_text(font, (200*i) + 310, 373, RGBA8(0,0,0,255), 1.5f, YesNo[i]);
					} else vita2d_pgf_draw_text(font, (200*i) + 310, 373, RGBA8(255,255,255,255), 1.5f, YesNo[i]);
				}
				break;
			case 7: // Uninstall Ongoing
				if (doStuff == 0) vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Remounting vs0 as writable...");
				if (doStuff == 1) {
					vshIoUmount(0x300, 0, 0, 0);
					_vshIoMount(0x300, 0, 2, buf);
					vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Wiping vs0 ready for extraction...\nDo not turn off your Vita or exit the app!");
				} else if (doStuff == 2) {
					removePath("vs0:app/NPXS10003");
					removePath("vs0:app/NPXS10008");
					removePath("vs0:app/NPXS10013");
					removePath("vs0:app/NPXS10015");
					removePath("vs0:app/NPXS10026");
					removePath("vs0:app/NPXS10030");
					removePath("vs0:app/NPXS10031");
					removePath("vs0:app/NPXS10033");
					removePath("vs0:app/NPXS10034");
					removePath("vs0:app/NPXS10068");
					removePath("vs0:app/NPXS10103");
					removePath("vs0:app/NPXS10104");
					removePath("vs0:app/NPXS10998");
					removePath("vs0:sys");
					removePath("vs0:vsh");
					vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Reinstalling retail firmware files...\nDo not turn off your Vita or exit the app!");
				} else if (doStuff == 3) {
					Zip *handle = ZipOpen("app0:/Media/vs0retail.zip");
					ZipExtract(handle, NULL, "vs0:");
					ZipClose(handle);
					vita2d_pgf_draw_textf(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Removing testkit spoofer and copying boot_config...");
				} else if (doStuff == 4) {
					sceIoRemove("ur0:tai/kDump.skprx");
					appendConfig(1);
				} else if (doStuff == 5) UIScreen = 8;
				doStuff++;
				break;
			case 8:
				vita2d_pgf_draw_text(font, 35, 115, RGBA8(255,255,255,255), 1.5f, "Uninstallation complete. Press X to restart the system.");
				vita2d_draw_rectangle(402, 425, 145, 70, RGBA8(226,92,92,255));
				vita2d_pgf_draw_text(font, 410, 470, RGBA8(0,0,0,255), 1.5f, "Continue");
				break;
		}
		
		if (keys_down & SCE_CTRL_CROSS) {
			switch(UIScreen) {
				case 0:
					if (config_exists == 1 && !vshSblAimgrIsGenuineDolce()) UIScreen = 1;
					else sceKernelExitProcess(0);
					break;
				case 1:
					if (selected == 0) UIScreen = 3;
					if (selected == 1) UIScreen = 6;
					if (selected == 2) UIScreen = 2;
					if (selected == 3) sceKernelExitProcess(0);
					break;
				case 2:
					UIScreen = 1;
					break;
				case 3:
					if (selected == 0) UIScreen = 1;
					if (selected == 1) UIScreen = 4;
					break;
				case 5:
					scePowerRequestColdReset();
				case 6:
					if (selected == 0) UIScreen = 1;
					if (selected == 1) UIScreen = 7;
					break;
				case 8:
					scePowerRequestColdReset();
					break;
			}
		}
		
		if (keys_down & SCE_CTRL_UP) {
			switch(UIScreen) {
				case 1:
					selected -= 1;
					break;
			}
		} if (keys_down & SCE_CTRL_DOWN) {
			switch(UIScreen) {
				case 1:
					selected += 1;
					break;
			}
		} if (keys_down & SCE_CTRL_RIGHT) {
			switch(UIScreen) {
				case 3:
					selected += 1;
					break;
				case 6:
					selected += 1;
					break;
			}
		} if (keys_down & SCE_CTRL_LEFT) {
			switch(UIScreen) {
				case 3:
					selected -= 1;
					break;
				case 6:
					selected -= 1;
					break;
			}
		}
		
		old_pad = pad;
		
		vita2d_end_drawing();
		vita2d_swap_buffers();
	}
	vita2d_fini();
	vita2d_free_pgf(font);
	
	sceKernelExitProcess(0);
	return 0;
}