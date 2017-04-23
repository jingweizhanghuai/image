#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavetype.h"
#include "exception.h"

/////////////////////////////////////////////////////////
// 接口功能:
//  使用插值的方法做重采样
//
// 参数：
//  (I)src(NO) - 需要重采样的原始波形
//  (O)dst(src) - 重采样后的输出波形
//  (I)src_rate(NO) - 原始波形的采样率，值应大于0
//  (I)dst_rate(NO) - 原始波形的采样率，值应大于0，小于src_rate
//  注：不能同时(dst==MA_DEFAULT)且(dst_rate==MA_DEFAULT)
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maReSampleWithInterpolate(MAWave *src,MAWave *dst,int src_rate,int dst_rate)
{    
    float k;  
    
    int src_size;
    int dst_size;
    
    int n1,n2;
    float w1,w2;
    int i;
    float locate;
    
    int cn;
    
    maException(((int)src <= 0),"invalid input",EXIT);
    if((int)dst <= 0)
        dst = src;
    if((dst==src)&&(dst_rate==MA_DEFAULT))
        return;
    
    if(dst_rate == MA_DEFAULT)
        dst_rate = dst->info.frequency;
    if(src_rate == MA_DEFAULT)
        src_rate = src->info.frequency;
    
    if((dst_rate <= 0)||(src_rate <= 0)||(src_rate == dst_rate))
    {
        if(dst!=src)
        {
            maWaveCopy(src,dst);
            if(dst->info.frequency <= 0)
                dst->info.frequency = src_rate;
            return;
        }
    }
    maException((dst_rate <= 0),"invalid input",EXIT);
    maException((src_rate < dst_rate),"invalid input",EXIT);
    
    src_size = src->size;    
    k = (float)src_rate/(float)dst_rate;
    dst_size = (int)(((float)src_size)/k); 
    maException((dst_size <= 1),"invalid operate",EXIT);      
 
    maRedefineWave(dst,dst_size,src->cn);
    
    dst->info.frequency = dst_rate;
    
    for(cn=0;cn<src->cn;cn++)
    {
        locate = k/2.0;
        for(i=0;i<dst_size;i++)
        {
            locate = locate + k;
            
            n1 = (int)locate;
            n2 = n1+1;
            
            w2 = locate - (float)n1;
            w1 = 1.0 - w2;
            
            dst->data[cn][i] = src->data[cn][n1]*w1+src->data[cn][n2]*w2;
        }
    }
}
    
/////////////////////////////////////////////////////////
// 函数功能:
//  使用平均的方法做重采样
//
// 参数：
//  (I)src(NO) - 需要重采样的原始波形
//  (O)dst(src) - 重采样后的输出波形
//  (I)src_rate(src->info.frequency) - 原始波形的采样率，值应大于0
//  (I)dst_rate(dst->info.frequency) - 原始波形的采样率，值应大于0，小于src_rate
//  注：不能同时(dst==MA_DEFAULT)且(dst_rate==MA_DEFAULT)
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maReSampleWithAverage(MAWave *src,MAWave *dst,int src_rate,int dst_rate)
{    
    float k;  
    
    int src_size;
    int dst_size;
    
    float sum;
    int n1,n2;
    float w1,w2;
    int i,j;
    float locate;
    
    int cn;
    
    maException(((int)src <= 0),"invalid input",EXIT);
    if((int)dst <= 0)
        dst = src;
    if((dst==src)&&(dst_rate==MA_DEFAULT))
        return;
    
    if(dst_rate == MA_DEFAULT)
        dst_rate = dst->info.frequency;
    if(src_rate == MA_DEFAULT)
        src_rate = src->info.frequency;
    
    if((dst_rate <= 0)||(src_rate <= 0)||(src_rate == dst_rate))
    {
        if(dst!=src)
        {
            maWaveCopy(src,dst);
            if(dst->info.frequency <= 0)
                dst->info.frequency = src_rate;
            return;
        }
    }
    maException((dst_rate <= 0),"invalid input",EXIT);
    maException((src_rate < dst_rate),"invalid input",EXIT);
    
    src_size = src->size;    
    k = (float)src_rate/(float)dst_rate;
    dst_size = (int)(((float)src_size)/k);
    maException((dst_size <= 1),"invalid operate",EXIT);    
    
    maRedefineWave(dst,dst_size,src->cn);
    
    dst->info.frequency = dst_rate;
    
    for(cn=0;cn<src->cn;cn++)
    {
        locate = 0;
        for(i=0;i<dst_size;i++)
        {
            n1 = (int)locate;
            w1 = (float)(n1+1)-locate;
            
            locate = locate + k;
            
            n2 = (int)locate;
            w2 = locate-(float)n2;
            
            sum = w1*(src->data[cn][n1]) + w2*(src->data[cn][n2]);
            for(j=n1+1;j<n2;j++)
                sum = sum + src->data[cn][j];
            
            dst->data[cn][i] = sum/k;
        }
    }
}
