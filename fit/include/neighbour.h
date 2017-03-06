#ifndef _MA_NEIGHBOUR_H_
#define _MA_NEIGHBOUR_H_

#define neighbour_r0_caculate(psrc,pdst1,pdst2,func) {\
	int i,j;\
	int data[14];\
	\
	int base1 = 0;\
	int base2 = s32ImgWidth;\
	int base3 = base2 + s32ImgWidth;\
	int base4 = base3 + s32ImgWidth;\
	int base5 = base4 + s32ImgWidth;\
	int base6 = base5 + s32ImgWidth;\
	int base7 = base6 + s32ImgWidth;\
	\
	for(j=0;j<s32ImgHeight-6;j=j+6)\
	{\
		data[1] = psrc[base1];\
		data[3] = psrc[base2];\
		data[5] = psrc[base3];\
		data[7] = psrc[base4];\
		data[9] = psrc[base5];\
		data[11] = psrc[base6];\
		data[13] = psrc[base7];\
		\
		for(i=0;i<s32ImgWidth-1;i++)\
		{\
			int l = i+1;\
			\
			data[0] = data[1];\
			data[2] = data[3];\
			data[4] = data[5];\
			data[6] = data[7];\
			data[8] = data[9];\
			data[10] = data[11];\
			data[12] = data[13];\
			\
			data[1] = psrc[base1+l];\
			data[3] = psrc[base2+l];\
			data[5] = psrc[base3+l];\
			data[7] = psrc[base4+l];\
			data[9] = psrc[base5+l];\
			data[11] = psrc[base6+l];\
			data[13] = psrc[base7+l];\
			\
			func(data[0],data[1],data[2],data[3],pdst1[base1+i],pdst2[base1+i]);\
			func(data[2],data[3],data[4],data[5],pdst1[base2+i],pdst2[base2+i]);\
			func(data[4],data[5],data[6],data[7],pdst1[base3+i],pdst2[base3+i]);\
			func(data[6],data[7],data[8],data[9],pdst1[base4+i],pdst2[base4+i]);\
			func(data[8],data[9],data[10],data[11],pdst1[base5+i],pdst2[base5+i]);\
			func(data[10],data[11],data[12],data[13],pdst1[base6+i],pdst2[base6+i]);\
		}\
		base1 = base7;\
		base2 = base1 + s32ImgWidth;\
		base3 = base2 + s32ImgWidth;\
		base4 = base3 + s32ImgWidth;\
		base5 = base4 + s32ImgWidth;\
		base6 = base5 + s32ImgWidth;\
		base7 = base6 + s32ImgWidth;\
	}\
}

#define neighbour_r1_caculate(psrc,pdst1,pdst2,func) {\
	int i,j;\
	int data[24];\
	\
	int base1 = 0;\
	int base2 = s32ImgWidth;\
	int base3 = base2 + s32ImgWidth;\
	int base4 = base3 + s32ImgWidth;\
	int base5 = base4 + s32ImgWidth;\
	int base6 = base5 + s32ImgWidth;\
	int base7 = base6 + s32ImgWidth;\
	int base8 = base7 + s32ImgWidth;\
	\
	for(j=1;j<s32ImgHeight-6;j=j+6)\
	{\
		data[1] = psrc[base1];\
		data[2] = psrc[base1+1];\
		data[4] = psrc[base2];\
		data[5] = psrc[base2+1];\
		data[7] = psrc[base3];\
		data[8] = psrc[base3+1];\
		data[10] = psrc[base4];\
		data[11] = psrc[base4+1];\
		data[13] = psrc[base5];\
		data[14] = psrc[base5+1];\
		data[16] = psrc[base6];\
		data[17] = psrc[base6+1];\
		data[19] = psrc[base7];\
		data[20] = psrc[base7+1];\
		data[22] = psrc[base8];\
		data[23] = psrc[base8+1];\
		\
		for(i=1;i<s32ImgWidth-1;i++)\
		{\
			int l = i+1;\
			\
			data[0] = data[1];\
			data[3] = data[4];\
			data[6] = data[7];\
			data[9] = data[10];\
			data[12] = data[13];\
			data[15] = data[16];\
			data[18] = data[19];\
			data[21] = data[22];\
			data[1] = data[2];\
			data[4] = data[5];\
			data[7] = data[8];\
			data[10] = data[11];\
			data[13] = data[14];\
			data[16] = data[17];\
			data[19] = data[20];\
			data[22] = data[23];\
			data[2] = psrc[base1+l];\
			data[5] = psrc[base2+l];\
			data[8] = psrc[base3+l];\
			data[11] = psrc[base4+l];\
			data[14] = psrc[base5+l];\
			data[17] = psrc[base6+l];\
			data[20] = psrc[base7+l];\
			data[23] = psrc[base8+l];\
			\
			func(data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],pdst1[base2+i],pdst2[base2+i]);\
			func(data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],pdst1[base3+i],pdst2[base3+i]);\
			func(data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],pdst1[base4+i],pdst2[base4+i]);\
			func(data[9],data[10],data[11],data[12],data[13],data[14],data[15],data[16],data[17],pdst1[base5+i],pdst2[base5+i]);\
			func(data[12],data[13],data[14],data[15],data[16],data[17],data[18],data[19],data[20],pdst1[base6+i],pdst2[base6+i]);\
			func(data[15],data[16],data[17],data[18],data[19],data[20],data[21],data[22],data[23],pdst1[base7+i],pdst2[base7+i]);\
		}\
		base1 = base7;\
		base2 = base8;\
		base3 = base2 + s32ImgWidth;\
		base4 = base3 + s32ImgWidth;\
		base5 = base4 + s32ImgWidth;\
		base6 = base5 + s32ImgWidth;\
		base7 = base6 + s32ImgWidth;\
		base8 = base7 + s32ImgWidth;\
	}\
	\
	for(;j<s32ImgHeight-1;j++)\
	{\
		data[1] = psrc[base1];\
		data[2] = psrc[base1+1];\
		data[4] = psrc[base2];\
		data[5] = psrc[base2+1];\
		data[7] = psrc[base3];\
		data[8] = psrc[base3+1];\
		\
		for(i=1;i<s32ImgWidth-1;i++)\
		{\
			int l = i+1;\
			\
			data[0] = data[1];\
			data[1] = data[2];\
			data[2] = psrc[base1+l];\
			data[3] = data[4];\
			data[4] = data[5];\
			data[5] = psrc[base2+l];\
			data[6] = data[7];\
			data[7] = data[8];\
			data[8] = psrc[base3+l];\
			\
			func(data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],pdst1[base2+i],pdst2[base2+i]);\
		}\
		base1 = base2;\
		base2 = base3;\
		base3 = base2+s32ImgWidth;\
	}\
}

#endif