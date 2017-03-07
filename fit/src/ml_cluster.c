#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"
#include "sample.h"

void maMLNormalize(MASample *DataIn,MASample *DataOut)
{
	int sample_num,feature_num;
	float **in,**out;
	int i,j;
	float sum,sum2;
	float deviation,mean;
	
	maException(INVALID_SAMPLE(DataIn),"invalid input sample",EXIT);
	
	if(DataOut == NULL)
		DataOut = DataIn;
	
	sample_num = DataIn->sample_num;
	feature_num = DataIn->feature_num;
	
	if(DataOut != DataIn)
	{
		maException(INVALID_SAMPLE(DataOut),"invalid input sample",EXIT);
		maException((sample_num!=(DataOut->sample_num)),"sample number between input and output is different",EXIT);
		maException((feature_num!=(DataOut->feature_num)),"feature number between input and output is different",EXIT);
	}
	
	in = DataIn->feature;
	out = DataOut->feature;		
	
	for(i=0;i<feature_num;i++)
	{
		sum = 0.0;
		sum2 = 0.0;
		for(j=0;j<sample_num;j++)
		{
			sum = sum + in[j][i];
			sum2 = sum2 + in[j][i]*in[j][i];
		}
		
		mean = sum/(float)feature_num;
		deviation = maMathSqrt((sum2/(float)feature_num)-mean*mean);
	
		for(j=0;j<sample_num;j++)
			out[j][i] = (in[j][i]-mean)/deviation;
	}
}

float maMLFeatureDistance(float *feature1,float *feature2,int feature_num)
{
	int i;
	float distance;
	
	for(i=0;i<feature_num;i++)
		distance = distance + (feature1[i]-feature2[i])*(feature1[i]-feature2[i]);

	return distance;
}

void maMLKmeanCluster(MASample *DataIn,int label_num)
{
	float *center[32];
	int distance[32];
	float *feature_mean[32];
	int feature_count[32];
	int feature_label;
	int min_distance;
	int i,j,n;
	int flag;
	float **feature;
	int *label;
	int feature_num,sample_num;
	
	
	
	maException(INVALID_SAMPLE(DataIn),"invalid input sample",EXIT);
	
	feature = DataIn->feature;
	feature_num = DataIn->feature_num;
	sample_num = DataIn->sample_num;
	label = DataIn->label;
	
	for(j=0;j<label_num;j++)
	{
		center[j] = (float *)malloc(feature_num*sizeof(float));
		feature_mean[j] = (float *)malloc(feature_num*sizeof(float));
		for(i=0;i<feature_num;i++)
			center[j][i] = feature[j][i];
	}
	
	
	
	n = 0;
	flag = 1;
	while(flag)
	{
		flag = 0;
		for(j=0;j<label_num;j++)
		{
			for(i=0;i<feature_num;i++)
				feature_mean[j][i] = 0.0;
		
			feature_count[j] = 0;
		}
		
		for(i=0;i<sample_num;i++)
		{
			distance[0] = maMLFeatureDistance(feature[i],center[0],feature_num);
			min_distance = distance[0];
			feature_label = 0;
			
			for(j=1;j<label_num;j++)
			{
				distance[j] = maMLFeatureDistance(feature[i],center[j],feature_num);
				if(distance[j]<min_distance)
				{
					min_distance = distance[j];
					feature_label = j;
				}
			}
			
			for(j=0;j<feature_num;j++)
				feature_mean[feature_label][j] = feature_mean[feature_label][j] + feature[i][j];
			feature_count[feature_label]=feature_count[feature_label]+1;
			
			if(label[i] != feature_label)
			{
				flag = 1;
				label[i] = feature_label;
			}				
		}
		
		for(j=0;j<label_num;j++)
		{
			for(i=0;i<feature_num;i++)
				center[j][i] = feature_mean[j][i]/(float)feature_count[j];
		}
		
		n = n+1;
		// printf("n is %d\n",n);
		if(n>=100)
			break;
	}
	
	for(j=0;j<label_num;j++)
	{
		free(center[j]);
		free(feature_mean[j]);
	}
}
	
		
	
	
	
	
	
	
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		