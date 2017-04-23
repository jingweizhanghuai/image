#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavetype.h"
#include "exception.h"

#define MAX_PEAK_NUM 32

void maFindWavePeak(MAWave *wavSrc,int peak_width,int peak_height,int *peak_locate[],int peak_num[])
{
	int wav_size;
	
	float *data;
	
	int i,j,k;
	
	int n;
	int num;
	
	int flag;
	
	maException(((int)wavSrc <= 0),"invalid input",EXIT);
	
	wav_size = wavSrc->size;
	
	n = ((peak_width)>>1);
	
	num = 0;
	
	for(j=0;j<wavSrc->cn;j++)
	{
		// printf("bbbbbbbbbb\n");
		data = wavSrc->data[j];
		for(i=n+1;i<wav_size-n-1;i++)
		{
			if((data[i]-data[i-n-1]>peak_height)&&(data[i]-data[i+n+1]>peak_height))
			{
				flag = 1;
				for(k=i-n;k<=i+n;k++)
					if(data[k]>data[i])
						flag = 0;
				
				if(flag)
				{
					peak_locate[j][num] = i;
					num = num +1;
					if(num == MAX_PEAK_NUM)
						break;
				}
			}
		}
		peak_num[j] = num;
	}
}

void maFindMainWavePeak(MAWave *wavSrc,int peak_width,int peak_height,int *peak_locate)
{
	int *peaklocate[MA_MAX_WAVE_CN];
	int peak_num[MA_MAX_WAVE_CN];
	int value;
	int value_max;
	int cn;
	
	int i,j;
	
	cn = wavSrc->cn;
	
	for(i=0;i<cn;i++)	
		peaklocate[i] = (int *)malloc(MAX_PEAK_NUM*sizeof(int));
	
	maFindWavePeak(wavSrc,peak_width,peak_height,peaklocate,peak_num);
	
	for(i=0;i<cn;i++)
	{
		peak_locate[i] = -1;
		value_max = 0.0;
		for(j=0;j<peak_num[i];j++)
		{
			value = wavSrc->data[i][peaklocate[i][j]];
			if(value>value_max)
			{
				value_max = value;
				peak_locate[i] = peaklocate[i][j];
			}
		}
		
		free(peaklocate[i]);
	}
}

MAWave* maCreateWave(int size,int cn);
void maPSNormalize(MAWave *wavSrc,MAWave *wavDst,float norm_value);

void maFindMainWaveFrequency(MAWave *wavPS,int *peak_locate)
{
	MAWave *wavSrc;
	int wav_size;
	float *data;
	char *flag;
	int peak[32];
	int height[32];
	int height_max;
	
	int n;
	int i;
	int cn;
	int l;
	
	wav_size = wavPS->size;
	
	wavSrc = maCreateWave(wav_size,wavPS->cn);
	maPSNormalize(wavPS,wavSrc,((wavPS->size)<<4));
	
	flag = (char *)malloc(wav_size*sizeof(char));
	
	for(cn=0;cn<wavSrc->cn;cn++)
	{
		memset(flag,1,wav_size*sizeof(char));

		data = wavSrc->data[cn];
		n = 0;
		for(i=2;i<wav_size-2;i++)
		{
			// if(i==58)
				// printf("%f,%f,%f,%f,%f\n",data[i-2],data[i-1],data[i],data[i+1],data[i+2]);
			
			if((data[i]>data[i-1])&&(data[i]>data[i+1])&&(data[i]-data[i-2]>32)&&(data[i]-data[i+2]>32))
			{
				// printf("%d,",i);
				peak[n] = i;
				height[n] = data[i];
	
				for(l=i+i;l<wav_size-1;l=l+i)
				{
					flag[l] = 0;
					flag[l-1] = 0;
					flag[l+1] = 0;
				}
				
				if(flag[i])
				{
					n = n+1;
					if(n==32)
						break;
				}
			}
		}
					
		if(n!=1)//(n==0)
		{
			peak_locate[cn] = -1;
			continue;
		}
		
		
		peak_locate[cn] = peak[0];
		height_max = height[0];
		for(i=1;i<n;i++)
		{
			if(height[i]>height_max)
			{
				peak_locate[cn] = peak[i];
				height_max = height[i];
			}
		}
		if(height_max < 150)
			peak_locate[cn] = -1;	
		
	}
	
	free(flag);
	maReleaseWave(wavSrc);
}
			
				
				