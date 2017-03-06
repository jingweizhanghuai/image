#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample.h"

void PerpendicularPoint(float *k,float *x,int n)
{
	int j;
	float v;

	v = 0.0;
	for(j=0;j<n;j++)
		v = v + k[j]*k[j];

	v = (0.0-k[n])/v;

	for(j=0;j<n;j++)
		x[j] = k[j]*v;
}

MASample *maCreateSample(int sample_num,int feature_num);

#define DATA(i,j) (data->feature[i][j])

MASample *caculate(MASample *data)
{
	int sample_num;
	int feature_num;

	MASample *support_data;

	char *valid;
	int valid_num;
	float distance;
	float v;
	float *w;

	int j,i,k;

	sample_num = data->sample_num;
	feature_num = data->feature_num;
	valid_num = sample_num;

	valid = (char *)malloc(sample_num*sizeof(char));
	memset(valid,1,sample_num);

	w = (float *)malloc((feature_num+1)*sizeof(float));

	for(i=0;i<sample_num;i++)
	{
		if(!valid[i])
			continue;
			
		v = 0.0;
		for(j=0;j<feature_num;j++)
			v = v + DATA(i,j)*DATA(i,j);

		v = v + 1.0;

		if(data->label[i])
			v = 1.0/v;
		else
			v = (-1.0)/v;

		for(j=0;j<feature_num;j++)
			w[j] = DATA(i,j)*v;
		w[j] = v;	

		for(k=0;k<sample_num;k++)
		{
			if(!valid[k])
				continue;

			if(k==i)
			{
				valid[k] = 1;
				continue;
			}
			
			distance = 0.0;
			for(j=0;j<feature_num;j++)
				distance = distance + w[j]*DATA(k,j);

			distance = distance + w[j];

			valid[k] = (distance<=1);
			valid_num = valid_num - (valid[k] == 0);
		}
	}

	support_data = maCreateSample(valid_num,feature_num);

	k = 0;
	for(i=0;i<sample_num;i++)
	{
		if(valid[j])
		{
			for(j=0;j<feature_num;j++)
				support_data->feature[k][j] = DATA(i,j);

			support_data->label[k] = data->label[j];

			k = k+1;
		}
	}

	return support_data;
	
}

