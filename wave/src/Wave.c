#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavetype.h"
#include "exception.h"

/////////////////////////////////////////////////////////
// 函数功能:
//  创建波形（创建波形头和数据空间）
//
// 参数：
//  (I)size(NO) - 所创建波形的大小，值应大于0
//  (I)cn(1) - 所创建波形的通道数量，取值应在[0,MA_MAX_WAVE_CN]之间
//
// 返回值：
//  所创建波形的指针
//
// 备注：
//  运行结束前需使用 maReleaseWave 释放波形
/////////////////////////////////////////////////////////
MAWave* maCreateWave(int size,int cn)
{
    MAWave *wave;
    int i;
    
    maException((size < 0),"invalid input",EXIT);
    
    if(cn == MA_DEFAULT) cn = 1;
    maException((cn < 1),"invalid input",EXIT);

    wave = (MAWave *)malloc(sizeof(MAWave));
    memset(wave,0,sizeof(MAWave));
    wave->memory = (float *)malloc(size*cn*sizeof(float));
    wave->data[0] = wave->memory;
    for(i=1;i<cn;i++)
        wave->data[i] = wave->data[i-1] + size;
    
    wave->size = size;
    wave->cn = cn;
    
    return wave;
}

/////////////////////////////////////////////////////////
// 函数功能:
//  创建波形头
//
// 参数：
//  (I)size(NO) - 所创建波形的大小
//  (I)cn(1) - 所创建波形的通道数量，取值应在[0,MA_MAX_WAVE_CN]之间
//
// 返回值：
//  所创建波形的指针
//
// 备注：
//  运行结束前需使用 maReleaseWaveHeader 函数释放波形头
/////////////////////////////////////////////////////////
MAWave* maCreateWaveHeader(int size,int cn)
{
    MAWave *wave;
    int data_byte;
    int i;
    
    maException((size < 0),"invalid input",EXIT);
    
    if(cn == MA_DEFAULT) cn = 1;
    maException((cn < 1),"invalid input",EXIT);
    
    wave = (MAWave *)malloc(sizeof(MAWave));
    memset(wave,0,sizeof(MAWave));
    
    for(i=0;i<MA_MAX_WAVE_CN;i++)
        wave->data[i] = NULL;
    
    wave->size = size;
    wave->cn = cn;
    
    return wave;
}

/////////////////////////////////////////////////////////
// 函数功能:
//  新建空波形头
//
// 参数：
//  无
//
// 返回值：
//  所新建波形的指针
//
// 备注：
//  运行结束前需使用 maReleaseWave 函数释放波形
/////////////////////////////////////////////////////////
MAWave* maNewWave()
{
    MAWave *wave;
    
    wave = (MAWave *)malloc(sizeof(MAWave));
    memset(wave,0,sizeof(MAWave));

    return wave;
}

/////////////////////////////////////////////////////////
// 函数功能:
//  为波形数据生成波形头
//
// 参数：
//  (I)size(NO) - 所生成波形的大小
//  (I)cn(1) - 所生成波形的通道数量
//  (I)data(NULL) - 波形数据，值为NULL时等同于 maCreateWave
//
// 返回值：
//  所创建波形的指针
//
// 备注：
//  运行结束前需使用 maReleaseWave 函数释放波形
/////////////////////////////////////////////////////////
MAWave* maGenerateWave(int size,int cn,float **data)
{
    MAWave *wave;
    int i;
    
    if((int)data <= 0)
        return maCreateWave(size,cn);
    
    maException((size < 0),"invalid input",EXIT);
    
    if(cn == MA_DEFAULT) cn = 1;
    maException((cn < 1),"invalid input",EXIT);
    
    wave = (MAWave *)malloc(sizeof(MAWave));
    memset(wave,0,sizeof(MAWave));

    wave->data[0] = data[0];
    for(i=1;i<cn;i++)
        wave->data[i] = data[i];
    
    wave->size = size;
    wave->cn = cn;
    
    return wave;
}

/////////////////////////////////////////////////////////
// 函数功能:
//  释放波形头和波形数据
//
// 参数：
//  (I)src(NO) - 所需要释放的波形
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maReleaseWave(MAWave *wave)
{
    int i;

    if((int)wave->memory > 0)
        free(wave->memory);
    
    free(wave);
    wave = NULL;
}

/////////////////////////////////////////////////////////
// 接口功能:
//  波形重定义
//
// 参数：
//  (I/O)src(NO) - 所需要重定义的波形
//  (I)size(src->size) - 重定义波形的大小
//  (I)cn(src->cn) - 所创建波形的通道数量
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maRedefineWave(MAWave *src,int size,int cn)
{
    int datasize;
    int srcsize;
    int cnsize;
    
    float *p;
    int i;
    
    maException(((int)src<=0),"invalid input",EXIT);
    
    if(size == MA_DEFAULT)    size = src->size;
    if(cn == MA_DEFAULT) cn = src->cn;
    
    if((src->size == size)&&(src->cn == cn))
        return;
    
    srcsize = src->size*src->cn;    
    datasize = size*cn;
    
    if(datasize > srcsize)
    {
        if((int)src->memory > 0)
            free(src->memory);
        src->memory = (float *)malloc(datasize * sizeof(float));
        src->data[0] = src->memory;
    }
    
    for(i=1;i<cn;i++)
        src->data[i] = src->data[i-1]+size;
    
    src->size = size;
    src->cn = cn;
}

void maWaveCopy(MAWave *src,MAWave *dst)
{
    int cn;
    maException(((int)src <= 0)||((int)dst == 0),"invalid input",EXIT);
    if(src == dst)
        return;
    
    maRedefineWave(dst,src->size,src->cn);
    for(cn=0;cn<src->cn;cn++)
        memcpy(dst->data[cn],src->data[cn],src->size*sizeof(float));
}
    
    
    

/////////////////////////////////////////////////////////
// 接口功能:
//  分通道求波形的平均值
//
// 参数：
//  (I)src(NO) - 所需计算的波形
//  (O)mean(NO) - 分通道波形的平均值
//  
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWavMean(MAWave *src,float *mean)
{
    int wav_size;
    int i,j;
    float sum;
        
    maException(((int)src <= 0)||((int)mean <= 0),"invalid input",EXIT);
    
    sum = 0.0;    
    wav_size = src->size;
    for(j=0;j<src->cn;j++)
    {
        for(i=0;i<wav_size;i++)
            sum = sum + src->data[j][i];
        mean[j] = sum/((float)wav_size);
    }
}

/////////////////////////////////////////////////////////
// 接口功能:
//  分通道求波形的平均绝对值
//
// 参数：
//  (I)src(NO) - 所需计算的波形
//  (O)mean(NO) - 分通道波形的平均绝对值
//  
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWavABSMean(MAWave *src,float *mean)
{
    int wav_size;
    int i,j;
    float sum;
    float **data;
    
    data = src->data;
    sum = 0.0;
    wav_size = src->size;
    for(j=0;j<src->cn;j++)
    {
        for(i=0;i<wav_size;i++)
            sum = (data[j][i]>0)?(sum+data[j][i]):(sum-data[j][i]);
        mean[j] = sum/((float)wav_size);
    }
}

/////////////////////////////////////////////////////////
// 接口功能:
//  分通道求波形值的平方的平均值
//
// 参数：
//  (I)src(NO) - 所需计算的波形
//  (O)mean(NO) - 分通道波形值的平方的平均值
//  
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWavSQMean(MAWave *src,float *mean)
{
    int wav_size;
    int i,j;
    float sum;
    float **data;
    
    sum = 0.0;
    data = src->data;
    wav_size = src->size;
    for(j=0;j<src->cn;j++)
    {
        for(i=0;i<wav_size;i++)
            sum = sum + data[j][i]*data[j][i];
        mean[j] = sum/((float)wav_size);
    }
}

// int maIsPSStable(MAWave *wavPS,float *power_hst)
// {
    // float mean;
    // int flag;
    
    // maWavABSMean(wavPS,&mean);
    
    // flag = (((power_hst[3] - mean)/mean)<0.4);
    
    // power_hst[3] = power_hst[2];
    // power_hst[2] = power_hst[1];
    // power_hst[1] = power_hst[0];
    // power_hst[0] = mean;
    
    // return flag;
// }

void maWaveAdd(MAWave *src1,MAWave *src2,MAWave *dst)
{
    int wav_size;    
    int cn,i;
        
    maException(((int)src1 <= 0)||((int)src2 <= 0),"invalid input",EXIT);
    wav_size = src1->size;
    maException((src2->size!= wav_size),"invalid input",EXIT);
    maException((src2->cn != src1->cn),"invalid input",EXIT);

    if((int)dst <= 0)
        dst = src1;
    else
        maRedefineWave(dst,wav_size,src1->cn);
    
    for(cn = 0;cn<src1->cn;cn++)
        for(i=0;i<wav_size;i++)
            dst->data[cn][i] = src1->data[cn][i] + src2->data[cn][i];
}
            
void maWaveAverage(MAWave *src1,MAWave *src2,MAWave *dst)
{
    int wav_size;    
    int cn,i;
        
    maException(((int)src1 <= 0)||((int)src2 <= 0),"invalid input",EXIT);
    wav_size = src1->size;
    maException((src2->size!= wav_size),"invalid input",EXIT);
    maException((src2->cn != src1->cn),"invalid input",EXIT);

    if((int)dst <= 0)
        dst = src1;
    else
        maRedefineWave(dst,wav_size,src1->cn);
    
    for(cn = 0;cn<src1->cn;cn++)
        for(i=0;i<wav_size;i++)
            dst->data[cn][i] = (src1->data[cn][i] + src2->data[cn][i])/2.0;
}
    
void maWaveWeightedAverage(MAWave *src1,MAWave *src2,MAWave *dst,float weight1,float weight2)
{
    int wav_size;    
    int cn,i;
        
    maException(((int)src1 <= 0)||((int)src2 <= 0),"invalid input",EXIT);
    wav_size = src1->size;
    maException((src2->size!= wav_size),"invalid input",EXIT);
    maException((src2->cn != src1->cn),"invalid input",EXIT);

    if((weight1 == MA_DEFAULT)&&(weight2 == MA_DEFAULT))
    {
        maWaveAverage(src1,src2,dst);
        return;
    }
    else if((weight1 == MA_DEFAULT)&&(weight2 < 1.0)&&(weight2 > 0.0))
        weight1 = 1.0 - weight2;
    else if((weight2 == MA_DEFAULT)&&(weight1 < 1.0)&&(weight1 > 0.0))
        weight2 = 1.0 - weight1;
    else if((weight1 == MA_DEFAULT)||(weight2 == MA_DEFAULT))
        maException(1,"invalid input",EXIT);
    
    if((int)dst <= 0)
        dst = src1;
    else
        maRedefineWave(dst,wav_size,src1->cn);
    
    for(cn = 0;cn<src1->cn;cn++)
        for(i=0;i<wav_size;i++)
            dst->data[cn][i] = (src1->data[cn][i]*weight1 + src2->data[cn][i]*weight2)/(weight1+weight2);
}

void maWaveScale(MAWave *src,MAWave *dst,float k)
{
    int wav_size;
    int cn,i;
    
    maException(((int)src <= 0),"invalid input",EXIT);
    wav_size = src->size;
    
    if((int)dst <= 0)
        dst = src;
    else
        maRedefineWave(dst,wav_size,src->cn);
    
    for(cn = 0;cn<src->cn;cn++)
        for(i=0;i<wav_size;i++)
            dst->data[cn][i] = src->data[cn][i]*k;
}

void maWaveMul(MAWave *src1,MAWave *src2,MAWave *dst)
{
    int wav_size;    
    int cn,i;
    
    maException(((int)src1 <= 0)||((int)src2 <= 0),"invalid input",EXIT);
    wav_size = src1->size;
    maException((src2->size!= wav_size),"invalid input",EXIT);
    maException((src2->cn != src1->cn),"invalid input",EXIT);

    if((int)dst <= 0)
        dst = src1;
    else
        maRedefineWave(dst,wav_size,src1->cn);
    
    for(cn = 0;cn<src1->cn;cn++)
        for(i=0;i<wav_size;i++)
            dst->data[cn][i] = src1->data[cn][i] * src2->data[cn][i];
}

void maWaveDiv(MAWave *src1,MAWave *src2,MAWave *dst)
{
    int wav_size;
    int cn,i;
    
    maException(((int)src1 <= 0)||((int)src2 <= 0),"invalid input",EXIT);    
    wav_size = src1->size;
    maException((src2->size!= wav_size),"invalid input",EXIT);
    maException((src2->cn != src1->cn),"invalid input",EXIT);

    if((int)dst <= 0)
        dst = src1;
    else
        maRedefineWave(dst,wav_size,src1->cn);
    
    for(cn = 0;cn<src1->cn;cn++)
        for(i=0;i<wav_size;i++)
            dst->data[cn][i] = src1->data[cn][i] / src2->data[cn][i];
}




