#include "aes.h"

static const DWORD RoundTable[NUM_ROUND] = {
	0x01000000, 0x02000000, 0x04000000, 0x08000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x1b000000, 0x36000000,
};

static const DWORD T4[256] = {
	0x63636363, 0x7c7c7c7c, 0x77777777, 0x7b7b7b7b,
	0xf2f2f2f2, 0x6b6b6b6b, 0x6f6f6f6f, 0xc5c5c5c5,
	0x30303030, 0x01010101, 0x67676767, 0x2b2b2b2b,
	0xfefefefe, 0xd7d7d7d7, 0xabababab, 0x76767676,
	0xcacacaca, 0x82828282, 0xc9c9c9c9, 0x7d7d7d7d,
	0xfafafafa, 0x59595959, 0x47474747, 0xf0f0f0f0,
	0xadadadad, 0xd4d4d4d4, 0xa2a2a2a2, 0xafafafaf,
	0x9c9c9c9c, 0xa4a4a4a4, 0x72727272, 0xc0c0c0c0,
	0xb7b7b7b7, 0xfdfdfdfd, 0x93939393, 0x26262626,
	0x36363636, 0x3f3f3f3f, 0xf7f7f7f7, 0xcccccccc,
	0x34343434, 0xa5a5a5a5, 0xe5e5e5e5, 0xf1f1f1f1,
	0x71717171, 0xd8d8d8d8, 0x31313131, 0x15151515,
	0x04040404, 0xc7c7c7c7, 0x23232323, 0xc3c3c3c3,
	0x18181818, 0x96969696, 0x05050505, 0x9a9a9a9a,
	0x07070707, 0x12121212, 0x80808080, 0xe2e2e2e2,
	0xebebebeb, 0x27272727, 0xb2b2b2b2, 0x75757575,
	0x09090909, 0x83838383, 0x2c2c2c2c, 0x1a1a1a1a,
	0x1b1b1b1b, 0x6e6e6e6e, 0x5a5a5a5a, 0xa0a0a0a0,
	0x52525252, 0x3b3b3b3b, 0xd6d6d6d6, 0xb3b3b3b3,
	0x29292929, 0xe3e3e3e3, 0x2f2f2f2f, 0x84848484,
	0x53535353, 0xd1d1d1d1, 0x00000000, 0xedededed,
	0x20202020, 0xfcfcfcfc, 0xb1b1b1b1, 0x5b5b5b5b,
	0x6a6a6a6a, 0xcbcbcbcb, 0xbebebebe, 0x39393939,
	0x4a4a4a4a, 0x4c4c4c4c, 0x58585858, 0xcfcfcfcf,
	0xd0d0d0d0, 0xefefefef, 0xaaaaaaaa, 0xfbfbfbfb,
	0x43434343, 0x4d4d4d4d, 0x33333333, 0x85858585,
	0x45454545, 0xf9f9f9f9, 0x02020202, 0x7f7f7f7f,
	0x50505050, 0x3c3c3c3c, 0x9f9f9f9f, 0xa8a8a8a8,
	0x51515151, 0xa3a3a3a3, 0x40404040, 0x8f8f8f8f,
	0x92929292, 0x9d9d9d9d, 0x38383838, 0xf5f5f5f5,
	0xbcbcbcbc, 0xb6b6b6b6, 0xdadadada, 0x21212121,
	0x10101010, 0xffffffff, 0xf3f3f3f3, 0xd2d2d2d2,
	0xcdcdcdcd, 0x0c0c0c0c, 0x13131313, 0xecececec,
	0x5f5f5f5f, 0x97979797, 0x44444444, 0x17171717,
	0xc4c4c4c4, 0xa7a7a7a7, 0x7e7e7e7e, 0x3d3d3d3d,
	0x64646464, 0x5d5d5d5d, 0x19191919, 0x73737373,
	0x60606060, 0x81818181, 0x4f4f4f4f, 0xdcdcdcdc,
	0x22222222, 0x2a2a2a2a, 0x90909090, 0x88888888,
	0x46464646, 0xeeeeeeee, 0xb8b8b8b8, 0x14141414,
	0xdededede, 0x5e5e5e5e, 0x0b0b0b0b, 0xdbdbdbdb,
	0xe0e0e0e0, 0x32323232, 0x3a3a3a3a, 0x0a0a0a0a,
	0x49494949, 0x06060606, 0x24242424, 0x5c5c5c5c,
	0xc2c2c2c2, 0xd3d3d3d3, 0xacacacac, 0x62626262,
	0x91919191, 0x95959595, 0xe4e4e4e4, 0x79797979,
	0xe7e7e7e7, 0xc8c8c8c8, 0x37373737, 0x6d6d6d6d,
	0x8d8d8d8d, 0xd5d5d5d5, 0x4e4e4e4e, 0xa9a9a9a9,
	0x6c6c6c6c, 0x56565656, 0xf4f4f4f4, 0xeaeaeaea,
	0x65656565, 0x7a7a7a7a, 0xaeaeaeae, 0x08080808,
	0xbabababa, 0x78787878, 0x25252525, 0x2e2e2e2e,
	0x1c1c1c1c, 0xa6a6a6a6, 0xb4b4b4b4, 0xc6c6c6c6,
	0xe8e8e8e8, 0xdddddddd, 0x74747474, 0x1f1f1f1f,
	0x4b4b4b4b, 0xbdbdbdbd, 0x8b8b8b8b, 0x8a8a8a8a,
	0x70707070, 0x3e3e3e3e, 0xb5b5b5b5, 0x66666666,
	0x48484848, 0x03030303, 0xf6f6f6f6, 0x0e0e0e0e,
	0x61616161, 0x35353535, 0x57575757, 0xb9b9b9b9,
	0x86868686, 0xc1c1c1c1, 0x1d1d1d1d, 0x9e9e9e9e,
	0xe1e1e1e1, 0xf8f8f8f8, 0x98989898, 0x11111111,
	0x69696969, 0xd9d9d9d9, 0x8e8e8e8e, 0x94949494,
	0x9b9b9b9b, 0x1e1e1e1e, 0x87878787, 0xe9e9e9e9,
	0xcececece, 0x55555555, 0x28282828, 0xdfdfdfdf,
	0x8c8c8c8c, 0xa1a1a1a1, 0x89898989, 0x0d0d0d0d,
	0xbfbfbfbf, 0xe6e6e6e6, 0x42424242, 0x68686868,
	0x41414141, 0x99999999, 0x2d2d2d2d, 0x0f0f0f0f,
	0xb0b0b0b0, 0x54545454, 0xbbbbbbbb, 0x16161616
};

void aes_gen_key(BYTE *Key, const BYTE *Seed, const BYTE *Rand) {
	int i, round;
	DWORD dw, dwTemp[8];
	BYTE buf[16];

	for (i = 0; i < 16; i++)
		buf[i] = Seed[i] ^ Rand[i];

	dwTemp[0] = ((((DWORD)buf[0]) << 24) ^ (((DWORD)buf[1]) << 16) ^ (((DWORD)buf[2]) << 8) ^ ((DWORD)buf[3]));
	dwTemp[1] = ((((DWORD)buf[4]) << 24) ^ (((DWORD)buf[5]) << 16) ^ (((DWORD)buf[6]) << 8) ^ ((DWORD)buf[7]));
	dwTemp[2] = ((((DWORD)buf[8]) << 24) ^ (((DWORD)buf[9]) << 16) ^ (((DWORD)buf[10]) << 8) ^ ((DWORD)buf[11]));
	dwTemp[3] = ((((DWORD)buf[12]) << 24) ^ (((DWORD)buf[13]) << 16) ^ (((DWORD)buf[14]) << 8) ^ ((DWORD)buf[15]));

	for (round = 0; round < NUM_ROUND; round++) {
		dw = dwTemp[3];
		dwTemp[4] = dwTemp[0] ^
			(T4[dw & 0xff] & 0x0000ff00) ^
			(T4[(dw >> 8) & 0xff] & 0x00ff0000) ^
			(T4[(dw >> 16) & 0xff] & 0xff000000) ^
			(T4[(dw >> 24)] & 0x000000ff) ^
			RoundTable[round];
		dwTemp[5] = dwTemp[1] ^ dwTemp[4];
		dwTemp[6] = dwTemp[2] ^ dwTemp[5];
		dwTemp[7] = dwTemp[3] ^ dwTemp[6];

		for (i = 0; i < 4; i++)
			dwTemp[i] = dwTemp[i + 4];
	}
	CopyMemory(Key, dwTemp, 16);
	return;
}

void aes_gen_key2(BYTE *Key, const BYTE *Seed, const BYTE *Rand) {
	int i, round;
	DWORD dw, dwTemp[8];
	BYTE buf[16];

	for (i = 0; i < 16; i++)
		buf[i] = Seed[i] ^ Rand[i];

	dwTemp[0] = ((((DWORD)buf[0]) << 24) ^ (((DWORD)buf[1]) << 16) ^ (((DWORD)buf[2]) << 8) ^ ((DWORD)buf[3]));
	dwTemp[1] = ((((DWORD)buf[3]) << 24) ^ (((DWORD)buf[4]) << 16) ^ (((DWORD)buf[5]) << 8) ^ ((DWORD)buf[6]));
	dwTemp[2] = ((((DWORD)buf[7]) << 24) ^ (((DWORD)buf[8]) << 16) ^ (((DWORD)buf[9]) << 8) ^ ((DWORD)buf[10]));
	dwTemp[3] = ((((DWORD)buf[11]) << 24) ^ (((DWORD)buf[12]) << 16) ^ (((DWORD)buf[13]) << 8) ^ ((DWORD)buf[14]));

	for (round = 0; round < NUM_ROUND; round++) {
		dw = dwTemp[3];
		dwTemp[4] = dwTemp[0] ^
			(T4[dw & 0xff] & 0x0000ff00) ^
			(T4[(dw >> 5) & 0xff] & 0x00ff0000) ^
			(T4[(dw >> 13) & 0xff] & 0xff000000) ^
			(T4[(dw >> 19) & 0xff] & 0x000000ff) ^
			T4[round];
		dwTemp[5] = dwTemp[1] ^ dwTemp[4];
		dwTemp[6] = dwTemp[2] ^ dwTemp[5];
		dwTemp[7] = dwTemp[3] ^ dwTemp[6];

		for (i = 0; i < 4; i++)
			dwTemp[i] = dwTemp[i + 4];
	}
	CopyMemory(Key, dwTemp, 16);
	return;
}
 