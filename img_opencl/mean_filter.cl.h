char *kernel_source = 
"\n\
#define ELEMENT16_SHIFT_LEFT(vector,a) {vector.s1,vector.lo.lo.hi,vector.lo.hi,vector.hi,a}\n\
\n\
#define ELEMENT16_SHIFT_RIGHT(a,vector) {a,vector.lo,vector.hi.lo,vector.hi.hi.lo,vector.se}\n\
\n\
__kernel void mean_filter_r1(__global uchar *psrc, __global uchar *pdst,int width,int height,int r)\n\
{\n\
	int x=get_global_id(0);\n\
	int y=get_global_id(1);\n\
	int cn = get_global_id(2);\n\
	\n\
	int base = cn*width*height;\n\
	\n\
	int index0_min = base+y*width;\n\
	int index0_max = index0_min+width-1;\n\
	int index2_min = base+max(y-1,0)*width;\n\
	int index2_max = index2_min+width-1;\n\
	int index7_min = base+min(y+1,height-1)*width;\n\
	int index7_max = index7_min+width-1;\n\
	\n\
    int index0 = index0_min+(x<<4);\n\
	int index2 = index2_min+(x<<4);\n\
	int index7 = index7_min+(x<<4);\n\
	\n\
	__global uchar16 *p0;\n\
	p0 = (__global uchar16 *)(psrc+index0);\n\
	\n\
	__global uchar16 *p2;\n\
	p2 = (__global uchar16 *)(psrc+index2);\n\
	\n\
	__global uchar16 *p7;\n\
	p7 = (__global uchar16 *)(psrc+index7);\n\
	\n\
	uchar16 data0 = *p0;\n\
	uchar16 data4 = ELEMENT16_SHIFT_RIGHT(psrc[max(index0_min,index0-1)],data0);\n\
	uchar16 data5 = ELEMENT16_SHIFT_LEFT(data0,psrc[min(index0_max,index0+16)]);\n\
	\n\
	uchar16 data2 = *p2;\n\
	uchar16 data1 = ELEMENT16_SHIFT_RIGHT(psrc[max(index2_min,index2-1)],data2);\n\
	uchar16 data3 = ELEMENT16_SHIFT_LEFT(data2,psrc[min(index2_max,index2+16)]);\n\
	\n\
	uchar16 data7 = *p7;\n\
	uchar16 data6 = ELEMENT16_SHIFT_RIGHT(psrc[max(index7_min,index7-1)],data7);\n\
	uchar16 data8 = ELEMENT16_SHIFT_LEFT(data7,psrc[min(index7_max,index7+16)]);\n\
	\n\
	ushort16 data;\n\
	data = convert_ushort16(data0)+convert_ushort16(data1)+convert_ushort16(data2)+convert_ushort16(data3)+convert_ushort16(data4)+convert_ushort16(data5)+convert_ushort16(data6)+convert_ushort16(data7)+convert_ushort16(data8);\n\
	\n\
	ushort num = 9;\n\
	data = data/num;\n\
	\n\
	__global uchar16 *p_dst;\n\
	p_dst = (__global uchar16 *)(pdst+index0);\n\
	\n\
	*p_dst = convert_uchar16(data);\n\
}\n\
\n\
__kernel void mean_filter_r2(__global uchar *psrc, __global uchar *pdst,int width,int height,int r)\n\
{\n\
	int x=get_global_id(0);\n\
	int y=get_global_id(1);\n\
	int cn = get_global_id(2);\n\
	\n\
	int base = cn*width*height;\n\
	\n\
	int index1_min = base+max(y-2,0)*width;\n\
	int index1_max = index1_min+width-1;\n\
	int index2_min = base+max(y-1,0)*width;\n\
	int index2_max = index2_min+width-1;\n\
	int index3_min = base+y*width;\n\
	int index3_max = index3_min+width-1;\n\
	int index4_min = base+min(y+1,height-1)*width;\n\
	int index4_max = index4_min+width-1;\n\
	int index5_min = base+min(y+2,height-1)*width;\n\
	int index5_max = index5_min+width-1;\n\
	\n\
	int index1 = index1_min+(x<<4);\n\
	int index2 = index2_min+(x<<4);\n\
    int index3 = index3_min+(x<<4);\n\
	int index4 = index4_min+(x<<4);\n\
	int index5 = index5_min+(x<<4);\n\
	\n\
	__global uchar16 *p1;\n\
	p1 = (__global uchar16 *)(psrc+index1);\n\
	\n\
	__global uchar16 *p2;\n\
	p2 = (__global uchar16 *)(psrc+index2);\n\
	\n\
	__global uchar16 *p3;\n\
	p3 = (__global uchar16 *)(psrc+index3);\n\
	\n\
	__global uchar16 *p4;\n\
	p4 = (__global uchar16 *)(psrc+index4);\n\
	\n\
	__global uchar16 *p5;\n\
	p5 = (__global uchar16 *)(psrc+index5);\n\
	\n\
	uchar16 data13 = *p1;\n\
	uchar16 data12 = ELEMENT16_SHIFT_RIGHT(psrc[max(index1_min,index1-1)],data13);\n\
	uchar16 data11 = ELEMENT16_SHIFT_RIGHT(psrc[max(index1_min,index1-2)],data12);\n\
	uchar16 data14 = ELEMENT16_SHIFT_LEFT(data13,psrc[min(index1_max,index1+16)]);\n\
	uchar16 data15 = ELEMENT16_SHIFT_LEFT(data14,psrc[min(index1_max,index1+17)]);\n\
	\n\
	uchar16 data23 = *p2;\n\
	uchar16 data22 = ELEMENT16_SHIFT_RIGHT(psrc[max(index2_min,index2-1)],data23);\n\
	uchar16 data21 = ELEMENT16_SHIFT_RIGHT(psrc[max(index2_min,index2-2)],data22);\n\
	uchar16 data24 = ELEMENT16_SHIFT_LEFT(data23,psrc[min(index2_max,index2+16)]);\n\
	uchar16 data25 = ELEMENT16_SHIFT_LEFT(data24,psrc[min(index2_max,index2+17)]);\n\
	\n\
	uchar16 data33 = *p3;\n\
	uchar16 data32 = ELEMENT16_SHIFT_RIGHT(psrc[max(index3_min,index3-1)],data33);\n\
	uchar16 data31 = ELEMENT16_SHIFT_RIGHT(psrc[max(index3_min,index3-2)],data32);\n\
	uchar16 data34 = ELEMENT16_SHIFT_LEFT(data33,psrc[min(index3_max,index3+16)]);\n\
	uchar16 data35 = ELEMENT16_SHIFT_LEFT(data34,psrc[min(index3_max,index3+17)]);\n\
	\n\
	uchar16 data43 = *p4;\n\
	uchar16 data42 = ELEMENT16_SHIFT_RIGHT(psrc[max(index4_min,index4-1)],data43);\n\
	uchar16 data41 = ELEMENT16_SHIFT_RIGHT(psrc[max(index4_min,index4-2)],data42);\n\
	uchar16 data44 = ELEMENT16_SHIFT_LEFT(data43,psrc[min(index4_max,index4+16)]);\n\
	uchar16 data45 = ELEMENT16_SHIFT_LEFT(data44,psrc[min(index4_max,index4+17)]);\n\
	\n\
	uchar16 data53 = *p5;\n\
	uchar16 data52 = ELEMENT16_SHIFT_RIGHT(psrc[max(index5_min,index5-1)],data53);\n\
	uchar16 data51 = ELEMENT16_SHIFT_RIGHT(psrc[max(index5_min,index5-2)],data52);\n\
	uchar16 data54 = ELEMENT16_SHIFT_LEFT(data53,psrc[min(index5_max,index5+16)]);\n\
	uchar16 data55 = ELEMENT16_SHIFT_LEFT(data54,psrc[min(index5_max,index5+17)]);\n\
	\n\
	ushort16 data;\n\
	data = convert_ushort16(data11)+convert_ushort16(data12)+convert_ushort16(data13)+convert_ushort16(data14)+convert_ushort16(data15)+convert_ushort16(data21)+convert_ushort16(data22)+convert_ushort16(data23)+convert_ushort16(data24)+convert_ushort16(data25)+convert_ushort16(data31)+convert_ushort16(data32)+convert_ushort16(data33)+convert_ushort16(data34)+convert_ushort16(data35)+convert_ushort16(data41)+convert_ushort16(data42)+convert_ushort16(data43)+convert_ushort16(data44)+convert_ushort16(data45)+convert_ushort16(data51)+convert_ushort16(data52)+convert_ushort16(data53)+convert_ushort16(data54)+convert_ushort16(data55);\n\
	\n\
	ushort num = 25;\n\
	data = data/num;\n\
	\n\
	__global uchar16 *p_dst;\n\
	p_dst = (__global uchar16 *)(pdst+index3);\n\
	\n\
	*p_dst = convert_uchar16(data);\n\
}\n\
\n\
__kernel void mean_filter_r3(__global uchar *psrc,__global uchar *pdst,int width,int height,int r)\n\
{\n\
	int x=get_global_id(0);\n\
	int y=get_global_id(1);\n\
	int cn = get_global_id(2);\n\
	\n\
	int base = cn*width*height;\n\
	\n\
	int index0_min = base+max(y-3,0)*width;\n\
	int index0_max = index0_min+width-1;\n\
	int index1_min = base+max(y-2,0)*width;\n\
	int index1_max = index1_min+width-1;\n\
	int index2_min = base+max(y-1,0)*width;\n\
	int index2_max = index2_min+width-1;\n\
	int index3_min = base+y*width;\n\
	int index3_max = index3_min+width-1;\n\
	int index4_min = base+min(y+1,height-1)*width;\n\
	int index4_max = index4_min+width-1;\n\
	int index5_min = base+min(y+2,height-1)*width;\n\
	int index5_max = index5_min+width-1;\n\
	int index6_min = base+min(y+3,height-1)*width;\n\
	int index6_max = index6_min+width-1;\n\
	\n\
	int index0 = index0_min+(x<<4);\n\
	int index1 = index1_min+(x<<4);\n\
	int index2 = index2_min+(x<<4);\n\
    int index3 = index3_min+(x<<4);\n\
	int index4 = index4_min+(x<<4);\n\
	int index5 = index5_min+(x<<4);\n\
	int index6 = index6_min+(x<<4);\n\
	\n\
	__global uchar16 *p0;\n\
	p0 = (__global uchar16 *)(psrc+index0);\n\
	\n\
	__global uchar16 *p1;\n\
	p1 = (__global uchar16 *)(psrc+index1);\n\
	\n\
	__global uchar16 *p2;\n\
	p2 = (__global uchar16 *)(psrc+index2);\n\
	\n\
	__global uchar16 *p3;\n\
	p3 = (__global uchar16 *)(psrc+index3);\n\
	\n\
	__global uchar16 *p4;\n\
	p4 = (__global uchar16 *)(psrc+index4);\n\
	\n\
	__global uchar16 *p5;\n\
	p5 = (__global uchar16 *)(psrc+index5);\n\
	\n\
	__global uchar16 *p6;\n\
	p6 = (__global uchar16 *)(psrc+index6);\n\
	\n\
	uchar16 data03 = *p0;\n\
	uchar16 data02 = ELEMENT16_SHIFT_RIGHT(psrc[max(index0_min,index0-1)],data03);\n\
	uchar16 data01 = ELEMENT16_SHIFT_RIGHT(psrc[max(index0_min,index0-2)],data02);\n\
	uchar16 data00 = ELEMENT16_SHIFT_RIGHT(psrc[max(index0_min,index0-3)],data01);\n\
	uchar16 data04 = ELEMENT16_SHIFT_LEFT(data03,psrc[min(index0_max,index0+16)]);\n\
	uchar16 data05 = ELEMENT16_SHIFT_LEFT(data04,psrc[min(index0_max,index0+17)]);\n\
	uchar16 data06 = ELEMENT16_SHIFT_LEFT(data05,psrc[min(index0_max,index0+18)]);\n\
	\n\
	uchar16 data13 = *p1;\n\
	uchar16 data12 = ELEMENT16_SHIFT_RIGHT(psrc[max(index1_min,index1-1)],data13);\n\
	uchar16 data11 = ELEMENT16_SHIFT_RIGHT(psrc[max(index1_min,index1-2)],data12);\n\
	uchar16 data10 = ELEMENT16_SHIFT_RIGHT(psrc[max(index1_min,index1-3)],data11);\n\
	uchar16 data14 = ELEMENT16_SHIFT_LEFT(data13,psrc[min(index1_max,index1+16)]);\n\
	uchar16 data15 = ELEMENT16_SHIFT_LEFT(data14,psrc[min(index1_max,index1+17)]);\n\
	uchar16 data16 = ELEMENT16_SHIFT_LEFT(data15,psrc[min(index1_max,index1+18)]);\n\
	\n\
	uchar16 data23 = *p2;\n\
	uchar16 data22 = ELEMENT16_SHIFT_RIGHT(psrc[max(index2_min,index2-1)],data23);\n\
	uchar16 data21 = ELEMENT16_SHIFT_RIGHT(psrc[max(index2_min,index2-2)],data22);\n\
	uchar16 data20 = ELEMENT16_SHIFT_RIGHT(psrc[max(index2_min,index2-3)],data21);\n\
	uchar16 data24 = ELEMENT16_SHIFT_LEFT(data23,psrc[min(index2_max,index2+16)]);\n\
	uchar16 data25 = ELEMENT16_SHIFT_LEFT(data24,psrc[min(index2_max,index2+17)]);\n\
	uchar16 data26 = ELEMENT16_SHIFT_LEFT(data25,psrc[min(index2_max,index2+18)]);\n\
	\n\
	uchar16 data33 = *p3;\n\
	uchar16 data32 = ELEMENT16_SHIFT_RIGHT(psrc[max(index3_min,index3-1)],data33);\n\
	uchar16 data31 = ELEMENT16_SHIFT_RIGHT(psrc[max(index3_min,index3-2)],data32);\n\
	uchar16 data30 = ELEMENT16_SHIFT_RIGHT(psrc[max(index3_min,index3-3)],data31);\n\
	uchar16 data34 = ELEMENT16_SHIFT_LEFT(data33,psrc[min(index3_max,index3+16)]);\n\
	uchar16 data35 = ELEMENT16_SHIFT_LEFT(data34,psrc[min(index3_max,index3+17)]);\n\
	uchar16 data36 = ELEMENT16_SHIFT_LEFT(data35,psrc[min(index3_max,index3+18)]);\n\
	\n\
	uchar16 data43 = *p4;\n\
	uchar16 data42 = ELEMENT16_SHIFT_RIGHT(psrc[max(index4_min,index4-1)],data43);\n\
	uchar16 data41 = ELEMENT16_SHIFT_RIGHT(psrc[max(index4_min,index4-2)],data42);\n\
	uchar16 data40 = ELEMENT16_SHIFT_RIGHT(psrc[max(index4_min,index4-3)],data41);\n\
	uchar16 data44 = ELEMENT16_SHIFT_LEFT(data43,psrc[min(index4_max,index4+16)]);\n\
	uchar16 data45 = ELEMENT16_SHIFT_LEFT(data44,psrc[min(index4_max,index4+17)]);\n\
	uchar16 data46 = ELEMENT16_SHIFT_LEFT(data45,psrc[min(index4_max,index4+18)]);\n\
	\n\
	uchar16 data53 = *p5;\n\
	uchar16 data52 = ELEMENT16_SHIFT_RIGHT(psrc[max(index5_min,index5-1)],data53);\n\
	uchar16 data51 = ELEMENT16_SHIFT_RIGHT(psrc[max(index5_min,index5-2)],data52);\n\
	uchar16 data50 = ELEMENT16_SHIFT_RIGHT(psrc[max(index5_min,index5-3)],data51);\n\
	uchar16 data54 = ELEMENT16_SHIFT_LEFT(data53,psrc[min(index5_max,index5+16)]);\n\
	uchar16 data55 = ELEMENT16_SHIFT_LEFT(data54,psrc[min(index5_max,index5+17)]);\n\
	uchar16 data56 = ELEMENT16_SHIFT_LEFT(data55,psrc[min(index5_max,index5+18)]);\n\
	\n\
	uchar16 data63 = *p6;\n\
	uchar16 data62 = ELEMENT16_SHIFT_RIGHT(psrc[max(index6_min,index6-1)],data63);\n\
	uchar16 data61 = ELEMENT16_SHIFT_RIGHT(psrc[max(index6_min,index6-2)],data62);\n\
	uchar16 data60 = ELEMENT16_SHIFT_RIGHT(psrc[max(index6_min,index6-3)],data61);\n\
	uchar16 data64 = ELEMENT16_SHIFT_LEFT(data63,psrc[min(index6_max,index6+16)]);\n\
	uchar16 data65 = ELEMENT16_SHIFT_LEFT(data64,psrc[min(index6_max,index6+17)]);\n\
	uchar16 data66 = ELEMENT16_SHIFT_LEFT(data65,psrc[min(index6_max,index6+18)]);\n\
	\n\
	ushort16 data;\n\
	data = convert_ushort16(data00)+convert_ushort16(data01)+convert_ushort16(data02)+convert_ushort16(data03)+convert_ushort16(data04)+convert_ushort16(data05)+convert_ushort16(data06);\n\
	data = data+convert_ushort16(data10)+convert_ushort16(data11)+convert_ushort16(data12)+convert_ushort16(data13)+convert_ushort16(data14)+convert_ushort16(data15)+convert_ushort16(data16);\n\
	data = data+convert_ushort16(data20)+convert_ushort16(data21)+convert_ushort16(data22)+convert_ushort16(data23)+convert_ushort16(data24)+convert_ushort16(data25)+convert_ushort16(data26);\n\
	data = data+convert_ushort16(data30)+convert_ushort16(data31)+convert_ushort16(data32)+convert_ushort16(data33)+convert_ushort16(data34)+convert_ushort16(data35)+convert_ushort16(data36);\n\
	data = data+convert_ushort16(data40)+convert_ushort16(data41)+convert_ushort16(data42)+convert_ushort16(data43)+convert_ushort16(data44)+convert_ushort16(data45)+convert_ushort16(data46);\n\
	data = data+convert_ushort16(data50)+convert_ushort16(data51)+convert_ushort16(data52)+convert_ushort16(data53)+convert_ushort16(data54)+convert_ushort16(data55)+convert_ushort16(data56);\n\
	data = data+convert_ushort16(data60)+convert_ushort16(data61)+convert_ushort16(data62)+convert_ushort16(data63)+convert_ushort16(data64)+convert_ushort16(data65)+convert_ushort16(data66);\n\
	\n\
	ushort num = 49;\n\
	data = data/num;\n\
	\n\
	__global uchar16 *p_dst;\n\
	p_dst = (__global uchar16 *)(pdst+index3);\n\
	\n\
	*p_dst = convert_uchar16(data);\n\
}\n\
\n\
#define ELEMENT16_LEFT_SHIFT(vs,a,vd) {vd.s01234567 = vs.s12345678;vd.s89ab = vs.s9abc;vd.scd = vs.sde;vd.se = vs.sf;vd.sf = a;}\n\
\n\
#define ELEMENT16_RIGHT_SHIFT(a,vs,vd) {vd.s0 = a;vd.s1 = vs.s0;vd.s23 = vs.s12;vd.s4567 = vs.s3456;vd.s89abcdef = vs.s789abcde;}\n\
\n\
__kernel void mean_filter_rn(__global uchar *psrc,__global uchar *pdst,int width,int height,int r)\n\
{\n\
	int x=get_global_id(0);\n\
	int y=get_global_id(1);\n\
	int cn = get_global_id(2);\n\
	\n\
	int base = cn*width*height;\n\
	int index_min;\n\
	int index_max;\n\
	int index;\n\
	__global uchar16 *p;\n\
	uchar16 data0;\n\
	uchar16 data1,data3;\n\
	uchar16 data2,data4;\n\
	ushort16 sum;\n\
	\n\
	int i,j;\n\
	sum = 0;\n\
	for(i=-r;i<=r;i++)\n\
	{\n\
		index_min = base+min(max(y+i,0),height-1)*width;\n\
		index_max = index_min+width-1;\n\
		\n\
		index = index_min+(x<<4);\n\
		p = (__global uchar16 *)(psrc+index);\n\
		data0 = *p;\n\
		ELEMENT16_RIGHT_SHIFT(psrc[max(index_min,index-1)],data0,data1);\n\
		ELEMENT16_LEFT_SHIFT(data0,psrc[min(index_max,index+16)],data2);\n\
		sum = sum+convert_ushort16(data0)+convert_ushort16(data1)+convert_ushort16(data2);\n\
		for(j=2;j<=r;j++)\n\
		{\n\
			ELEMENT16_RIGHT_SHIFT(psrc[max(index_min,index-j)],data1,data3);\n\
			ELEMENT16_LEFT_SHIFT(data2,psrc[min(index_max,index+15+j)],data4);\n\
			data1 = data3;\n\
			data2 = data4;\n\
			sum = sum+convert_ushort16(data1)+convert_ushort16(data2);\n\
		}\n\
	}\n\
	\n\
	ushort num;\n\
	num = (r*2+1);\n\
	num = num*num;\n\
	sum = sum/num;\n\
	\n\
	__global uchar16 *p_dst;\n\
	p_dst = (__global uchar16 *)(pdst+base+y*width+(x<<4));\n\
	\n\
	*p_dst = convert_uchar16(sum);\n\
}\n\
";		
