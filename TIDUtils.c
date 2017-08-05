#include "TIDUtils.h"

// For writing to vs0
int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int exists(const char *fname) {
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

int hasEndSlash(const char *path) {
	return path[strlen(path) - 1] == '/';
}

int removePath(const char *path) {
	SceUID dfd = sceIoDopen(path);
	if (dfd >= 0) {
		int res = 0;
		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));

			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
				snprintf(new_path, 1024, "%s%s%s", path, hasEndSlash(path) ? "" : "/", dir.d_name);

				if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
					int ret = removePath(new_path);
					if (ret <= 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				} else {
					int ret = sceIoRemove(new_path);
					if (ret < 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				}

				free(new_path);
			}
		} while (res > 0);

		sceIoDclose(dfd);

		int ret = sceIoRmdir(path);
		if (ret < 0)
			return ret;
	} else {
		int ret = sceIoRemove(path);
		if (ret < 0)
			return ret;
	}

	return 1;
}

int copykDump() {
	long plugin_size;
	FILE *fp = fopen("app0:Media/kDump.skprx","rb");

	fseek(fp, 0, SEEK_END);
	plugin_size = ftell(fp);
	rewind(fp);

	char* plugin_buffer = (char*) malloc(sizeof(char) * plugin_size);
	fread(plugin_buffer, sizeof(char), (size_t)plugin_size, fp);

	FILE *pFile = fopen("ur0:tai/kDump.skprx", "wb");
	
	for (int i = 0; i < plugin_size; ++i) {
			fputc(plugin_buffer[i], pFile);
	}
   
	fclose(fp);
	fclose(pFile);
	return 1;
}

int appendConfig(int type) {
	long plugin_size;
	FILE *fp;
	
	if (type == 0) // Install
		fp = fopen("app0:Media/configs/boot_config_vita_mod.txt","rb");
	else if (type == 1) // Uninstall
		fp = fopen("app0:Media/configs/boot_config_vita_orig.txt","rb");

	fseek(fp, 0, SEEK_END);
	plugin_size = ftell(fp);
	rewind(fp);

	char* plugin_buffer = (char*) malloc(sizeof(char) * plugin_size);
	fread(plugin_buffer, sizeof(char), (size_t)plugin_size, fp);

	FILE *pFile = fopen("ur0:tai/boot_config.txt", "wb");
	
	for (int i = 0; i < plugin_size; ++i) {
			fputc(plugin_buffer[i], pFile);
	}
   
	fclose(fp);
	fclose(pFile);
	return 1;
}

