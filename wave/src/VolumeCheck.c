#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavetype.h"
#include "exception.h"

void maWavABSMean(MAWave *src,float *mean);
MAWave* maCreateWave(int size,int datatype,int cn);

/////////////////////////////////////////////////////////
// 接口功能:
//  判断音频段是否活跃
//
// 参数：
//  (I)src(NO) - 待判断的原始时域波形段
//  (I)thresh(0.05) - 判断标准（阈值），取值应在[0,1]区间
//
// 返回值：
//  判断结果，1为活跃，0为不活跃
//
// 作者：
//  李庆雨（qingyu.li@impower-tech.com）
//
// 更改日期
//  2017.3.28
/////////////////////////////////////////////////////////
int maWaveActive(MAWave *src,float thresh)
{
    float mean[MA_MAX_WAVE_CN];
    int i;
    
    maException((src==NULL),"invalid input",EXIT);
    if(thresh == MA_DEFAULT)
        thresh = 0.05;
    maException((thresh<0.0),"invalid input",EXIT);
    
    maWavABSMean(src,mean);
    for(i=0;i<src->cn;i++)
        if(mean[i]>=thresh)
            return 1;
    
    return 0;
}

/////////////////////////////////////////////////////////
// 接口功能:
//  获得背景声的大小
//
// 参数：
//  (I)src(NO) - 待检测的原始时域波形段
//  (O)background(No) - 检测的结果（分通道）
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maWaveBackground(MAWave *src,float *background)
{
    int wav_size;
    int i,cn;
    float sum;
    float mean;
    float **data;
    int s,e;
    
    wav_size = src->size;
    data = src->data;
    
    if(wav_size <128)   {s = 0;    e = wav_size;}
    else                {s = 0;    e = 128;}
    for(cn=0;cn<src->cn;cn++)
    {
        sum = 0.0;
        for(i=s;i<e;i++)
            sum = (data[cn][i]>0)?(sum+data[cn][i]):(sum-data[cn][i]);
        background[cn] = ((float)sum)/128.0;
        s = e;    
        e = e+128;
        while(e<=wav_size)
        {
            sum = 0.0;
            for(i=s;i<e;i++)
                sum = (data[cn][i]>0)?(sum+data[cn][i]):(sum-data[cn][i]);
            mean = ((float)sum)/128.0;
            if(mean<background[cn])
                background[cn] = mean;
            s = e;    
            e = e+128;
        }
    }
}

static float MA_Wave_Frame_Background[MA_MAX_WAVE_CN][16];
static int MA_Wave_Frame_Background_n = 0;

/////////////////////////////////////////////////////////
// 接口功能:
//  自适应判断音频段是否活跃
//
// 参数：
//  (I)src(NO) - 待判断的原始时域波形段
//  (I)sensibility(0.5) - 检测灵敏度，取值应在[0,1]区间
//  (I)thresh(0.02) - 判断的最小阈值（超过此阈值才判断），取值应在[0,1]区间
//
// 返回值：
//  判断结果，1为活跃，0为不活跃
/////////////////////////////////////////////////////////
int maWaveAdaptiveActive(MAWave *src,float sensibility,float thresh)
{
    int i;
    int cn;
    int active;
    
    float mean[MA_MAX_WAVE_CN];
    float background[MA_MAX_WAVE_CN];
    
    maException((src==NULL),"invalid input",EXIT);
    
    if(thresh == MA_DEFAULT)
        thresh = 0.02;    
    maException(((thresh<0.0)||(thresh>1.0)),"invalid input",EXIT);
    
    if(sensibility == MA_DEFAULT)
        sensibility = 0.5;
    maException(((sensibility<0.0)||(sensibility>1.0)),"invalid input",EXIT);
    
    maWavABSMean(src,mean);
    
    active = 0;
    for(cn=0;cn<src->cn;cn++)
    {
        if(mean[cn]>thresh)
        {
            active = 1;
            break;
        }        
        
        background[cn] = 0.0;
        for(i=0;i<16;i++)
            background[cn] = background[cn] + MA_Wave_Frame_Background[cn][i];
        background[cn] = background[cn]*(2.5-sensibility)/16.0;        
        // printf("mean is %f,background is %f\n",mean[0],background[cn]);
        
        if(mean[cn]>background[cn])
        {
            active = 1;
            break;
        }        
    }
    
    maWaveBackground(src,background);    
    for(cn=0;cn<src->cn;cn++)
        MA_Wave_Frame_Background[cn][MA_Wave_Frame_Background_n] = background[cn];
    
    MA_Wave_Frame_Background_n = MA_Wave_Frame_Background_n +1;
    if(MA_Wave_Frame_Background_n == 16)
        MA_Wave_Frame_Background_n = 0;
    
    return active;
}


#define CHECK_FRAME_NUM 20
static float g_loudcheck_hst_range[CHECK_FRAME_NUM];
static int g_loudcheck_n = 0;

/////////////////////////////////////////////////////////
// 接口功能:
//  自适应大响声检测
//
// 参数：
//  (I)src(NO) - 待检测的原始波形帧
//  (I)sensibility(0.5) - 检测灵敏度，取值应在[0,1]区间
//  (I)thresh(0.05) - 判断的最小阈值（超过此阈值才判断），取值应在[0,1]区间
//
// 返回值：
//  判断结果，1为异常响声，0为正常响声
/////////////////////////////////////////////////////////
int maLoudSoundCheck(MAWave *src,float sensibility,float thresh)
{
    int i,n;
    int flag;
    
    float mean;
    float hst;
    
    maException((src==NULL),"invalid input",EXIT);
    
    if(thresh == MA_DEFAULT)
        thresh = 0.05;    
    maException(((thresh<0.0)||(thresh>1.0)),"invalid input",EXIT);
    
    if(sensibility == MA_DEFAULT)
        sensibility = 0.5;
    maException(((sensibility<0.0)||(sensibility>1.0)),"invalid input",EXIT);
    
    maWavABSMean(src,&mean);

    flag = 0;
    if(mean>thresh)
    {
        hst = 0.0;
        n = g_loudcheck_n;
        for(i=0;i<CHECK_FRAME_NUM-3;i++)
        {
            hst = (g_loudcheck_hst_range[n] > hst)?g_loudcheck_hst_range[n]:hst;
            n = n+1;
            if(n==CHECK_FRAME_NUM)
                n = 0;
        }
        flag = (mean>hst*(1.7-sensibility));
        // printf("mean is %f,hst is %f,flag is %d\n",mean,hst,flag);
    }
    // else
        // printf("mean is %f\n",mean);
    
    g_loudcheck_hst_range[g_loudcheck_n] = mean;
    g_loudcheck_n = g_loudcheck_n +1;
    if(g_loudcheck_n == CHECK_FRAME_NUM)
        g_loudcheck_n = 0;

    return flag;
}
    