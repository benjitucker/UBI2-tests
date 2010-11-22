#include <libubiio.h>
#include <libubiio_int.h>
#include <mtd/ubi-user.h>
#include <stdio.h>
#include <string.h>


static int usage(char **argv)
{
	fprintf(stderr, "Usage: %s ubi_volume\n"
			"Sample: %s /dev/ubi0_0\n",
			argv[0], argv[0]);
	return 1;
}

void dump(unsigned char *buf)
{
	int i;
	for (i = 0; i < 512; i++)
	{
		if (!(i % 16))
		{ 
			printf("\n%02x:", i);
		}
		printf(" %02x", buf[i]);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	struct ubi_volume_desc *desc;
	int   ubi_num;
	int   vol_id;
	unsigned char  buf[512];
	int   i;

	if (argc < 2)
		return usage(argv);

	sscanf(argv[1], "/dev/ubi%d_%d", &ubi_num, &vol_id);
	fprintf(stdout, "ubi_num = %d, vol_id = %d\n", ubi_num, vol_id);
	desc = ubi_open_volume(ubi_num, vol_id, UBI_READWRITE);
	if (desc == NULL)
	{
		perror("ubi_open_volume");
		fprintf(stderr, "Cannot open the volume\n");
		return 1;
	}

	printf("Min IO size %d\n", desc->di.min_io_size);

	printf("Write and read back zeros\n");
	
	memset(buf, 0, sizeof buf);

	if (ubi_leb_write(desc, 0, buf, 0, sizeof buf, UBI_LONGTERM))
	{
		perror("ubi_leb_write");
		fprintf(stderr, "Cannot write\n");
		return 1;
	}

	memset(buf, 0xbe, sizeof buf);

	if (ubi_leb_read(desc, 0, buf, 0, sizeof buf, 0) == -1)
	{
		perror("ubi_leb_read");
		fprintf(stderr, "Cannot read\n");
		return 1;
	}

	dump(buf);

	printf("Unmap the block\n");
	if (ubi_leb_unmap(desc, 0))
	{
		perror("ubi_leb_unmap");
		fprintf(stderr, "Cannot unmap\n");
		return 1;
	}

	printf("Write and read back pattern\n", desc->di.min_io_size);
	
	for (i = 0; i < 512; i++)
	{
		buf[i] = i;
	}

	if (ubi_leb_write(desc, 0, buf, 512, sizeof buf, UBI_LONGTERM))
	{
		perror("ubi_leb_write");
		fprintf(stderr, "Cannot write\n");
		return 1;
	}

	memset(buf, 0xbe, sizeof buf);

	if (ubi_leb_read(desc, 0, buf, 512, sizeof buf, 0) == -1)
	{
		perror("ubi_leb_read");
		fprintf(stderr, "Cannot read\n");
		return 1;
	}

	dump(buf);

	{
		struct ubi_device_info di;
		if (ubi_get_device_info(ubi_num, &di) == -1)
		{
			perror("ubi_get_dev_info");
			return 1;
		}
	}

	{
		fprintf(stdout, "Looking for the volume \"test\" on device %d\n", ubi_num);
		int tvol_id = ubi_get_vol_id_by_name(ubi_num, "test");
		if (tvol_id != vol_id)
		{
			fprintf(stderr, "Invalid vol id found for device \"test\" (%d) \n", tvol_id);
			return 1;
		}
	}

	ubi_close_volume(desc);
	fprintf(stdout, "Everything seems to be fine\n");
	return 0;
}

