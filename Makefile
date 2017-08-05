TITLE_ID = TSTKTINST
TARGET   = Testkit_Installer_Deluxe
OBJS     = main.o Archives.o TIDUtils.o

LIBS = -lvita2d -lSceDisplay_stub -lSceLibKernel_stub -lSceGxm_stub -lSceIofilemgr_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub -lScePvf_stub -lSceShellSvc_stub \
	-lSceCommonDialog_stub -lScePower_stub -lSceVshBridge_stub -lfreetype -lpng -ljpeg -lz -lm -lc

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -fno-lto
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "Testkit Installer Deluxe" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin -a pkg/Media/vs0.zip=Media/vs0.zip -a pkg/Media/vs0retail.zip=Media/vs0retail.zip \
	-a pkg/Media/kDump.skprx=Media/kDump.skprx -a pkg/Media/configs/boot_config_vita_orig.txt=Media/configs/boot_config_vita_orig.txt \
	-a pkg/Media/configs/boot_config_vita_mod.txt=Media/configs/boot_config_vita_mod.txt \
	-a pkg/icon0.png=sce_sys/icon0.png \
	-a pkg/pic0.png=sce_sys/pic0.png \
	-a pkg/bg0.png=sce_sys/livearea/contents/bg0.png \
	-a pkg/default_gate.png=sce_sys/livearea/contents/default_gate.png \
	-a pkg/template.xml=sce_sys/livearea/contents/template.xml $@

eboot.bin: $(TARGET).velf
	vita-make-fself -c $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^

clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."