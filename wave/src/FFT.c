//////////////////////////////////////////////////////////
// Copyright (c) Beijing IMPower Technologies Co. Ltd.
// All rights reserved.
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "wavetype.h"
#include "exception.h"

MAWave* maCreateWave(int size,int cn);

#define MA_PI 3.14159265358979323846

#define COMPLEXADD(re0,im0,re1,im1,re_out,im_out) {\
    re_out = re0+re1;\
    im_out = im0+im1;\
}

#define COMPLEXSUB(re0,im0,re1,im1,re_out,im_out) {\
    re_out = re0-re1;\
    im_out = im0-im1;\
}

#define COMPLEXMUL(re0,im0,re1,im1,re_out,im_out) {\
    re_out = re0*re1-im0*im1;\
    im_out = im0*re1+re0*im1;\
}

#define FFTCACL(re0,im0,re1,im1) {\
    float re_mul,im_mul;\
    COMPLEXMUL(Wre[k],Wim[k],re1,im1,re_mul,im_mul);\
    COMPLEXSUB(re0,im0,re_mul,im_mul,re1,im1);\
    COMPLEXADD(re0,im0,re_mul,im_mul,re0,im0);\
}

/////////////////////////////////////////////////////////
// 接口功能:
//  快速傅里叶变换
//
// 参数：
//  (I)wavSrc(NO) - 需进行傅里叶变换的时域波形
//  (O)wavFFT(wavSrc) - 傅里叶变换后的频域波形
//
// 返回值：
//  无
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.21
/////////////////////////////////////////////////////////
void maWaveFFT(MAWave *wavSrc,MAWave *wavFFT)
{    
    int i,j,k,n;
    
    int cn;
    
    int WavSize;
    
    float *InData;
    float *FFTDataRe;
    float *FFTDataIm;
    
    int N;
    
    float *Wre,*Wim;
    
    int out_valid;
    
    maException(((int)wavSrc <= 0),"invalid input",EXIT);
    WavSize = wavSrc->size;

    n=1;
    while(WavSize>(2<<n))
        n=n+1;
    
    N = (2<<n);
    
    if(((int)wavFFT <= 0)||(wavFFT == wavSrc))
    {
        out_valid = 0;
        wavFFT = maCreateWave(N,((wavSrc->cn)<<1));
    }
    else
    {
        out_valid = 1;
        maRedefineWave(wavFFT,N,((wavSrc->cn)<<1));
    }
    
    N=(N>>1);
    
    Wre = (float *)malloc(N*sizeof(float));
    Wim = (float *)malloc(N*sizeof(float));
    
    for(k=0;k<N;k++)
    {
        Wre[k] = (float)cos((((double)(k))/((double)(N)))*MA_PI);
        Wim[k] = 0.0- (float)sin((((double)(k))/((double)(N)))*MA_PI);
    }
    
    for(cn=0;cn<wavSrc->cn;cn++)
    {
        InData = wavSrc->data[cn];
        FFTDataRe = wavFFT->data[(cn<<1)];
        FFTDataIm = wavFFT->data[(cn<<1)+1];
        
        FFTDataRe[0] = InData[0];
        FFTDataRe[N] = InData[1];
        FFTDataIm[0] = 0.0;
        FFTDataIm[N] = 0.0;
        for(i=1,j=N;i<N;i++)  
        {
            if(j>=WavSize)
            {
                FFTDataRe[i] = 0;
                FFTDataRe[i+N] = 0;
            }
            else
            {        
                FFTDataRe[i] = InData[j];
                FFTDataRe[i+N] = InData[j+1];
            }
            
            FFTDataIm[i] = 0.0;
            FFTDataIm[i+N] = 0.0;
            
            k=N;  
            while(k<=j)  
            {  
                j=j-k;  
                k=k/2;  
            }
            j=j+k;  
        }
        
        for(n=1;n<=N;n=(n<<1))    
            for(j=0;j<(N<<1);j=j+(n<<1))
                for(i=0,k=0;i<n;i++,k=k+N/n)
                {
                    // printf("n is %d\tj is %d\tk is %d\tN is %d\n",n,j,k,N);
                    FFTCACL(FFTDataRe[j+i],FFTDataIm[j+i],FFTDataRe[j+i+n],FFTDataIm[j+i+n]);
                }
    }
    
    free(Wre);
    free(Wim);
    
    if(!out_valid)
    {
        maReleaseWave(wavSrc);
        wavSrc = wavFFT;
    }    
}

#define MA_SQUAR_POWERS    1
#define MA_POWERS          2
#define MA_LOG_POWERS      3

float maMathSqrt(float x);

/////////////////////////////////////////////////////////
// 接口功能:
//  计算功率谱
//
// 参数：
//  (I)wavFFT(NO) - 傅里叶变换后的频域波形
//  (O)wavPS(wavFFT) - 计算得到的功率谱
//  (I)mode(MA_SQUAR_POWERS) - 功率谱的计算方式，可选MA_SQUAR_POWERS（平方）、MA_POWERS（开方）、MA_LOG_POWERS（对数）
//
// 返回值：
//  无
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.21
/////////////////////////////////////////////////////////
void maPowerSpectrum(MAWave *wavFFT,MAWave *wavPS,int mode)
{
    int wav_size;
    float *re,*im;
    float *ps;
    int i,j;
    
    if(mode == MA_DEFAULT)
        mode = MA_SQUAR_POWERS;
    maException(((mode<1)||(mode>3)),"invalid input",EXIT);
    
    wav_size = (wavFFT->size)>>1;
    
    maException((wavFFT == NULL),"invalid input",EXIT);
    
    if((wavPS == NULL)||((int)wavPS == MA_DEFAULT))
        wavPS = wavFFT;
    
    maRedefineWave(wavPS,wav_size,((wavFFT->cn)>>1));
    
    if(mode == MA_SQUAR_POWERS)
    {
        for(j=0;j<wavPS->cn;j++)
        {
            re = wavFFT->data[(j<<1)];
            im = wavFFT->data[(j<<1)+1];
            ps = wavPS->data[j];
            for(i=0;i<wav_size;i++)
                ps[i] = re[i]*re[i] + im[i]*im[i];
        }
    }
    else if(mode == MA_POWERS)
    {
        for(j=0;j<wavPS->cn;j++)
        {
            re = wavFFT->data[(j<<1)];
            im = wavFFT->data[(j<<1)+1];
            ps = wavPS->data[j];
            for(i=0;i<wav_size;i++)
                ps[i] = maMathSqrt(re[i]*re[i] + im[i]*im[i]);
        }
    }
    else if(mode = MA_LOG_POWERS)
    {
        for(j=0;j<wavPS->cn;j++)
        {
            re = wavFFT->data[(j<<1)];
            im = wavFFT->data[(j<<1)+1];
            ps = wavPS->data[j];
            for(i=0;i<wav_size;i++)
                ps[i] = (log10((double)(re[i]*re[i] + im[i]*im[i])))/2.0;
        }
    }    
}


