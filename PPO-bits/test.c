#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Experimental code for processing the PPO data */

enum{
	NOT_INC = 0,
	UNMAPPED,
	INC,
	ERROR,
};

int ppo_elem_decode(unsigned short ppo)
{
	int popcount = __builtin_popcount((int)ppo);
	if (popcount <= 5)
		return INC;

	else if (popcount <= 11)
		return UNMAPPED;

	return NOT_INC;
}

#define PPO_UNMAPPED (-1)
#define PPO_BAD (-2)

int ppo_decode(unsigned short *ppo_data, int length /* in shorts */, int *offset, int *unmapped)
{
	int chop_start = 0;
	int chop_size = length;
	int chop_mid_point;
	int mid_point_ppo_type[2];
	int bad = 0;

	/* Binary chop to quickly find the offset */
	while (chop_size) {

		chop_mid_point = chop_start + chop_size/2;

		mid_point_ppo_type[0] = ppo_elem_decode(ppo_data[chop_mid_point]);

		/* if we are at the very end of he data buffer, avoid reading past the
		   end of the buffer and set the next type to NOT_INC */
		if (chop_mid_point == length-1) {
			mid_point_ppo_type[1] = NOT_INC;
			break;
		}

		mid_point_ppo_type[1] = ppo_elem_decode(ppo_data[chop_mid_point+1]);

		if (mid_point_ppo_type[0] != mid_point_ppo_type[1])
			break;

		if (mid_point_ppo_type[0] == UNMAPPED)
			/* There should not be consecutive UNMAPPED entries,
			   let the following bad check code deal with the error */
			break;

		/* On the next iteration, examine either the top or bottom half of the 
		   current chop, depending on the value half way through */
		if (mid_point_ppo_type[0] == INC)
			chop_start = chop_mid_point;

		/* Half the chop size on each iteration */
		chop_size >>= 1;
	}

	/* Work out the PPO offset and unmapped state */
	*unmapped = 0;
	*offset = 0;

	/* Sanity check the PPO elem preceding the midpoint is an INC type */
	if (chop_mid_point &&
			ppo_elem_decode(ppo_data[chop_mid_point-1]) != INC)

		bad = 1;
	else if (mid_point_ppo_type[0] == INC)

		if (mid_point_ppo_type[1] == INC)

			bad = 1;
		else {

			*offset = chop_mid_point + 1;
			if (mid_point_ppo_type[1] == UNMAPPED)
				*unmapped = 1;
		}

	else if (mid_point_ppo_type[0] == UNMAPPED)

		if (mid_point_ppo_type[1] == INC || 
				mid_point_ppo_type[1] == UNMAPPED)

			bad = 1;
		else {

			*unmapped = 1;
			*offset = chop_mid_point;
		}

	else
		*offset = 0;

	return bad;
}

int dump(unsigned short *data, int len)
{
	int i;

	for (i = 0; i < len; i++) {

		if (!(i % 8)) printf("\n");
		
		printf("0x%04x, ", data[i]);
	}
	printf("\n");
}

main()
{
	static unsigned char ppo_table[256];
	static char ppo_str[5][20] = {"NOT_INC", "UNMAPPED", "INC", "ERROR"};

	int i, j, k, l, ii, kk;
	int first_nibble_bits;
	int second_nibble_bits;
	int test_ppo;
	int ppo_type, decoded_ppo_type;

	int offset, unmapped, bad;

	struct ppot {
		unsigned short ppo_data[1024];
		int length;
		int exptd_offset;
		int exptd_unmapped;
		int exptd_bad;
	} ppo_tests[] = {
		{
			{
				0x0000, 0x0000, 0x0000, 0x0000, 0x00ff, 0xffff, 0xffff, 0xffff
			},
			8, 4, 1, 0,
		},
		{
			{
				0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff
			},
			8, 5, 0, 0,
		},
		{
			{
				0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
			},
			8, 0, 0, 0,
		},
		{
			{
				0x00ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
			},
			8, 0, 1, 0,
		},
		{
			{
				0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
			},
			8, 8, 0, 0,
		},
		{
			{
				0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00ff
			},
			8, 7, 1, 0,
		},
		{
			{
				0x00ff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
			},
			8, 0, 1, 1,
		},
		{
			{
				0x0000, 0x0000, 0x0000, 0x0000, 0x00ff, 0x0000, 0x0000, 0x00ff
			},
			8, 7, 1, 1,
		},
		{
			{
				0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00ff, 0xffff
			},
			9, 7, 1, 0,
		},
		{
			{
				0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
			},
			9, 1, 0, 0,
		},
	};

	for (ii = 0; ii < 1000000; ii++)
	for (i = 0; i < (sizeof(ppo_tests)/sizeof(struct ppot)); i++) {

		unsigned short ppo_data[1024];

		/* Add some random errors */
		memcpy(ppo_data, ppo_tests[i].ppo_data, ppo_tests[i].length*2);

		k = rand() % 3;
		for (j = 0; j < k; j++) {

			ppo_data[rand() % ppo_tests[i].length] ^= (1 << (rand() % 16));
		}

		bad = ppo_decode(
				ppo_data, 
				ppo_tests[i].length, 
				&offset, &unmapped);

		if (ppo_tests[i].exptd_bad) {

			if (!bad) {
				printf("test %d failed\n", i);
				dump(ppo_data, ppo_tests[i].length);
			}

			//			else
			//				printf("test %d passed\n", i);
		}

		else if (offset != ppo_tests[i].exptd_offset ||
				unmapped != ppo_tests[i].exptd_unmapped) {

			printf("test %d failed\n", i);
			dump(ppo_data, ppo_tests[i].length);
		}
		//		else
		//			printf("test %d passed\n", i);
	}




#if 0
	/* Build the PPO table */
	for (i = 0; i < 256; i++) {

		/* decide if the value is NOT_INC, UNMAPPED and INC */

#if 0
		/* Test for inc first, bits 0 to 6 clear, bits set */
		first_nibble_bits = __builtin_popcount(i >> 4);
		second_nibble_bits = __builtin_popcount(i & 0x0f);

		if (first_nibble_bits <= 1 && second_nibble_bits >= 3)
			ppo_table[i] = UNMAPPED;

		else if (first_nibble_bits >= 2 && second_nibble_bits >= 2)
				ppo_table[i] = NOT_INC;

		else if (first_nibble_bits <= 2 && second_nibble_bits <= 2)
				ppo_table[i] = INC;

		else {
			printf("PPO %02x considered error\n", i);
			ppo_table[i] = ERROR;
		}
#endif

		if (__builtin_popcount(i) <= 3)
			ppo_table[i] = INC;

		else if (__builtin_popcount(i) <= 5)
			ppo_table[i] = UNMAPPED;

		else
			ppo_table[i] = NOT_INC;
	}
#endif


#if 0
	/* bit error test code */
	for (ppo_type = NOT_INC; ppo_type < ERROR; ppo_type++) {

		printf("Test %s with errors:\n", ppo_str[ppo_type]);

		/* Test NOT_INC with up to two errors */
		for (i = 0; i < 8; i++) {
		for (ii = 0; ii < 8; ii++) {

			for (j = 0; j < 8; j++) {

				for (k = 8; k < 16; k++) {
				for (kk = 8; kk < 16; kk++) {

					for (l = 8; l < 16; l++) {

						switch (ppo_type) {
							case NOT_INC:
								test_ppo = 0xffff;		/* All bits set - not inc */
								break;

							case UNMAPPED:
								test_ppo = 0x00ff;		/* half bits set - unmapped */
								break;

							case INC:
								test_ppo = 0x0000;		/* All bits clear - inc */
								break;
						}


						/* flip some bits */
						test_ppo ^= ((1 << i) | (1 << j) | (1 << k) | (1 << l) | (1 << ii) | (1 << kk));

						printf("Testing i=%04x ii=%04x j=%04x k=%04x kk=%04x l=%04x 0x%04x", 
								(1 << i), (1 << ii), (1 << j), (1 << k), (1 << kk), (1 << l), test_ppo);

						/* check the ppo table */
						decoded_ppo_type = ppo_elem_decode(test_ppo);
						if (decoded_ppo_type != ppo_type) {
							printf(" should have been %s but instead it was %s\n", 
								ppo_str[ppo_type], ppo_str[decoded_ppo_type]);
						}
						else {
							printf("\n");
						}
					}
				}
			}
				}
			}
		}
	}
#endif

	return 0;
}




