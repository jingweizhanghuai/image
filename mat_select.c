#include <stdio.h>
#include "type.h"
#include "err.h"

void select_data(void *p_src1,void *p_src2,void *p_sel,void *p_dst,int n)
{	
	if(n == 0)
		return;
	
	#ifdef DEBUG
	if((p_src1 == NULL)||(p_src2 == NULL)||(p_sel == NULL)||(p_dst == NULL))
	{
		printf("IMG Error:\n\tin imgMatSelect.\n");
		exit(0);
	}		
	#endif
	
	int *src1_data;
	src1_data = (int *)p_src1;
	
	int *src2_data;
	src2_data = (int *)p_src2;
	
	int *sel_data;
	sel_data = (int *)p_sel;
	
	int *dst_data;
	dst_data = (int *)p_dst;
	
	int size;
	size = (n>>6);
	
	int i;
	for(i=0;i<size;i++)
	{
		dst_data[0] = (src1_data[0]&(!sel_data[0]))+(src2_data[0]&sel_data[0]);
		dst_data[1] = (src1_data[1]&(!sel_data[1]))+(src2_data[1]&sel_data[1]);
		dst_data[2] = (src1_data[2]&(!sel_data[2]))+(src2_data[2]&sel_data[2]);
		dst_data[3] = (src1_data[3]&(!sel_data[3]))+(src2_data[3]&sel_data[3]);
		dst_data[4] = (src1_data[4]&(!sel_data[4]))+(src2_data[4]&sel_data[4]);
		dst_data[5] = (src1_data[5]&(!sel_data[5]))+(src2_data[5]&sel_data[5]);
		dst_data[6] = (src1_data[6]&(!sel_data[6]))+(src2_data[6]&sel_data[6]);
		dst_data[7] = (src1_data[7]&(!sel_data[7]))+(src2_data[7]&sel_data[7]);
		dst_data[8] = (src1_data[8]&(!sel_data[8]))+(src2_data[8]&sel_data[8]);
		dst_data[9] = (src1_data[9]&(!sel_data[9]))+(src2_data[9]&sel_data[9]);
		dst_data[10] = (src1_data[10]&(!sel_data[10]))+(src2_data[10]&sel_data[10]);
		dst_data[11] = (src1_data[11]&(!sel_data[11]))+(src2_data[11]&sel_data[11]);
		dst_data[12] = (src1_data[12]&(!sel_data[12]))+(src2_data[12]&sel_data[12]);
		dst_data[13] = (src1_data[13]&(!sel_data[13]))+(src2_data[13]&sel_data[13]);
		dst_data[14] = (src1_data[14]&(!sel_data[14]))+(src2_data[14]&sel_data[14]);
		dst_data[15] = (src1_data[15]&(!sel_data[15]))+(src2_data[15]&sel_data[15]);
		
		src1_data = src1_data+16;
		src2_data = src2_data+16;
		dst_data = dst_data+16;
		sel_data = sel_data+16;
	}
	
	size = ((n>>2)-size*16);
	
	if(size !=0)
	{
		for(i=0;i<size;i++)
		{
			dst_data[0] = (src1_data[0]&(!sel_data[0]))+(src2_data[0]&sel_data[0]);
			
			src1_data = src1_data+1;
			src2_data = src2_data+1;
			dst_data = dst_data+1;
			sel_data = sel_data+1;
		}
	}
	
	size = n&0x03;
	if(size !=0)
	{
		char *psrc1;
		psrc1 = (char *)src1_data;
		char *psrc2;
		psrc2 = (char *)src2_data;
		char *psel;
		psel = (char *)sel_data;
		char *pdst;
		pdst = (char *)dst_data;
		
		for(i=0;i<size;i++)
		{
			pdst[0] = (psrc1[0]&(!psel[0]))+(psrc2[0]&psel[0]);
			psrc1++;
			psrc2++;
			psel++;
			pdst++;
		}
	}
}

void imgMatSelect(ImgMat *src1,ImgMat *src2,ImgMat *sel,ImgMat *dst)
{
	int size;
	size = 1<<((src1->type)&0x06);
	
	int cn;
	cn = ((src1->type)>>3)+1;
	
	int n;
	n = (src1->size)*size*cn;
	
	unsigned char *p_src1;
	p_src1 = src1->data.ptr;
	
	unsigned char *p_src2;
	p_src2 = src2->data.ptr;
	
	unsigned char *p_dst;
	p_dst = dst->data.ptr;
	
	unsigned char *p_sel;
	p_sel = sel->data.ptr;
	
	select_data(p_src1,p_src2,p_sel,p_dst,n);
}
