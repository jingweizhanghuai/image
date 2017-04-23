#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined _MSC_VER
#include <Windows.h>
#endif

#include <portaudio.h>
#include "wavetype.h"
#include "exception.h"

MAWave* maCreateWave(int size,int datatype,int cn);
MAWave* maCreateWaveHeader(int size,int datatype,int cn);

static PaStream* g_record_stream;
static float *g_record_data;
static float *g_record_wave_data;
static float *g_record_read;
static float *g_record_write;
static int g_record_available_num = 0;
static MAWave *g_record_wave;

#define BUFFER_SIZE 2048
#define BIFFER_NUM  64

static int WriteMem(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesperbuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData )
{
    float *p;
    p = g_record_write;
    
    memcpy(p,inputBuffer,BUFFER_SIZE*sizeof(float));
    
    g_record_write = p + BUFFER_SIZE;
    if(g_record_write >= g_record_data + BUFFER_SIZE*BIFFER_NUM)
        g_record_write = g_record_data;
    
    g_record_available_num = g_record_available_num + BUFFER_SIZE;
    
    if(g_record_available_num > BUFFER_SIZE*BIFFER_NUM)
    {
        g_record_read = g_record_write;
        g_record_available_num = BUFFER_SIZE*BIFFER_NUM;
    } 
    
    (void)framesperbuffer;
    (void)outputBuffer; 
    (void)timeInfo;
    (void)statusFlags;    

    return paContinue;
}

/////////////////////////////////////////////////////////
// 接口功能:
//  开始录音
//
// 参数：
//  (I)sample_rate(44100) - 录音的采样率
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maStartSountRecord(int sample_rate)
{
    PaStreamParameters inputParameters;
    
    if(sample_rate == MA_DEFAULT)
        sample_rate = 44100;
    maException((sample_rate <= 0), "invalid input", EXIT);
    
    g_record_data = (float *)malloc(BUFFER_SIZE*BIFFER_NUM*sizeof(float));
    g_record_wave_data = (float *)malloc(BUFFER_SIZE*BIFFER_NUM*sizeof(float));
    g_record_read = g_record_data;
    g_record_write = g_record_data;
    g_record_wave = maCreateWaveHeader(0,1);
    g_record_wave->info.frequency = sample_rate;
    g_record_available_num = 0;
    
    maException(Pa_Initialize(),"Pa_Initialize error",EXIT);
    
    inputParameters.device = Pa_GetDefaultInputDevice(); 
    maException((inputParameters.device == paNoDevice),"Pa_Initialize No default input device",EXIT);
    
    inputParameters.channelCount = 1;                    
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    maException(Pa_OpenStream(&g_record_stream,
                               &inputParameters,
                               NULL,
                               sample_rate,
                               BUFFER_SIZE,
                               paClipOff,
                               WriteMem,
                               NULL),
                 "Pa_OpenStream error",EXIT);
    
    maException(Pa_StartStream(g_record_stream),"Pa_StartStream error",EXIT);
}

/////////////////////////////////////////////////////////
// 接口功能:
//  结束录音
//
// 参数：
//  无
//
// 返回值：
//  无
/////////////////////////////////////////////////////////
void maFinishSoundRecord()
{
    maException(Pa_StopStream(g_record_stream),"Pa_StopStream error",EXIT);
    maException(Pa_CloseStream(g_record_stream),"Pa_CloseStream error",EXIT);    
    
    free(g_record_data);
    free(g_record_wave_data);
    free(g_record_wave);
    
    g_record_wave = NULL;
    g_record_wave_data = NULL;
    g_record_data = NULL;
    
    g_record_stream = NULL;
    g_record_read = NULL;
    g_record_write = NULL;
    
    g_record_available_num = 0;
}

/////////////////////////////////////////////////////////
// 接口功能:
//  获取录音数据段
//
// 参数：
//  (I)size(2048) - 读出流数据的大小（数据点的个数）
//
// 返回值：
//  指向录音波形的指针，读取到的波形为 MA_TYPE_F32 类型
/////////////////////////////////////////////////////////
MAWave *maGetSoundRecordStream(int size)
{
    int size_availabe;
    float *p;
    int l1,l2;
    
    if(size == MA_DEFAULT)
        size = 2048;
    maException((size <= 0), "invalid input", EXIT);

    while(1)
    {        
        if(g_record_available_num < size)
        {
            #if defined _MSC_VER
            Sleep(1);
            #elif defined __GNUC__
            usleep(1000);
            #endif
        }
        else
            break;
    }
    
    // printf("g_record_write is %d,g_record_read is %d,g_record_available_num is %d\n",g_record_write,g_record_read,g_record_available_num);
    
    g_record_wave->size = size;
    
    p = g_record_read;
    g_record_read = p + size;
    if(g_record_read < g_record_data + BUFFER_SIZE*BIFFER_NUM)
    {
        g_record_wave->data[0] = p;
    }
    else
    {
        l1 = (g_record_data + BUFFER_SIZE*BIFFER_NUM) - p;
        l2 = size-l1;
        
        memcpy(g_record_wave_data,p,l1*sizeof(float));
        memcpy(g_record_wave_data+l1,g_record_data,l2*sizeof(float));
        g_record_wave->data[0] = g_record_wave_data;
        
        g_record_read = g_record_data + l2;
    }
    
    g_record_available_num = g_record_available_num - size;
    
    return g_record_wave;
}


