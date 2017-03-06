#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"
#include "sample.h"

MASample *maCreateSample(int sample_num,int feature_num)
{
	MASample *data;
	int i;

	maException(((sample_num<1)||(feature_num<1)),"invalid input",EXIT);
	
	data = (MASample *)malloc(sizeof(MASample));
	
	data->feature_num = feature_num;
	data->sample_num = sample_num;
	
	data->feature = (float **)malloc(sample_num*sizeof(float *));
	
	for(i=0;i<sample_num;i++)
		data->feature[i] = (float *)malloc(feature_num*sizeof(float));
	
	data->value = (float *)malloc(sample_num*sizeof(float));
	
	return data;
}

MASample *maGenerateSample(int sample_num,int feature_num,float **feature,void *value)
{
	MASample *data;
	int i;

	maException(((sample_num<1)||(feature_num<1)||(feature==NULL)||(value==NULL)),"invalid input",EXIT);
	
	data = (MASample *)malloc(sizeof(MASample));
	
	data->feature_num = feature_num;
	data->sample_num = sample_num;
	
	data->feature = feature;
	data->value = value;
}

void maReleaseSample(MASample *data)
{
	int i;

	maException((data==NULL),"invalid input",EXIT);
	
	free(data->value);
	for(i=0;i<data->sample_num;i++)
		free(data->feature[i]);
	free(data->feature);
	
	free(data);
}


	
	
	
