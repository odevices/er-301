#include <string.h>

extern const char *ramdisk_path_array;
extern const char *ramdisk_file_data_array;
extern size_t ramdisk_file_size_array;
extern const unsigned int ramdisk_num;

const char *ramdisk_lookup(const char *path, size_t *nbytes)
{
	int i;
	for (i = 0; i < ramdisk_num; i++)
	{
		if (strcmp(path, (&ramdisk_path_array)[i]) == 0)
		{
			*nbytes = (&ramdisk_file_size_array)[i];
			return (&ramdisk_file_data_array)[i];
		}
	}

	return NULL;
}

const char *ramdisk_lookup_as_string(const char *path)
{
	int i;
	for (i = 0; i < ramdisk_num; i++)
	{
		if (strcmp(path, (&ramdisk_path_array)[i]) == 0)
		{
			return (&ramdisk_file_data_array)[i];
		}
	}

	return NULL;
}
