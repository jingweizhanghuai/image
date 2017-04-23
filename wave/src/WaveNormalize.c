#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavetype.h"
#include "exception.h"

#define ABS(x) (((x)>0)?(x):(0-(x)))

/////////////////////////////////////////////////////////
// 接口功能:
//  功率谱的归一化
//
// 参数：
//  (I)src(NO) - 待归一化的功率谱波形
//  (O)dst(src) - 归一化后的功率谱波形
//  (I)norm_value(src->size) - 归一化系数
//
// 返回值：
//  无
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.4.5
/////////////////////////////////////////////////////////
void maPSNormalize(MAWave *src,MAWave *dst,float norm_value)
{
    int wav_size;
    float **data_src,**data_dst;
    float sum;
    float k;
    int i,cn;
    
    maException((src == NULL),"invalid input",EXIT);
    maException((src->info.wave_type != MA_WAVE_PS),"invalid input",EXIT);
    
    wav_size = src->size;
    if(norm_value == MA_DEFAULT)
        norm_value = (float)wav_size;
    
    data_src = src->data;
    if((int)dst <= 0)
        data_dst = src->data;
    else
    {
        maRedefineWave(dst,wav_size,src->cn);
        data_dst = dst->data;
        dst->info = src->info;
    }
    
    for(cn = 0;cn<src->cn;cn++)
    {
        sum = 0.0;
        for(i=0;i<wav_size;i++)
            sum = sum + data_src[cn][i];
            
        k = norm_value/sum;
        
        for(i=0;i<wav_size;i++)
            data_dst[cn][i] = data_src[cn][i]*k;
    }
}


/////////////////////////////////////////////////////////
// 接口功能:
//  波形归一化
//
// 参数：
//  (I)src(NO) - 待归一化的时域波形
//  (O)dst(src) - 归一化后的波形
//  (I)norm_value(波形类型最大值) - 归一化系数
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWaveNormalize(MAWave *src,MAWave *dst,float norm_value)
{
    int wav_size;
    float max;
    float k;
    int i,j;
    
    maException((src == NULL),"invalid input",EXIT);
    
    wav_size = src->size;
    if((int)dst <= 0)
        dst = src; 
    
    if(dst != src)
    {
        maRedefineWave(dst,wav_size,src->cn);
        dst->info = src->info;
    }
    
    if(norm_value < 0)
        norm_value = 1.0;
    
    for(j=0;j<src->cn;j++)
    {
        max = 0;
        for(i=0;i<wav_size;i++)
            max = (ABS(src->data[j][i])>max)?ABS(src->data[j][i]):max;
        
        k = norm_value/((float)max);
        
        for(i=0;i<wav_size;i++)
            dst->data[j][i] = (src->data[j][i])*k;
    }       
}
