#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample.h"

void maMathFootPoint(float **plane,int plane_num,int dim_num,float *locate)
{
	int vector_num;
	float *k1,*k2;
	float *v1,*v2;
	
	int i,j,k;
	
	#define PLANE(x,y) plane[x][y]
	
	k1 = (float *)malloc(dim_num*(dim_num+1)*sizeof(float));
	#define K1(x,y)	k1[(x)*(dim_num+1)+(y)]
	
	vector_num = dim_num - plane_num;
	if(vector_num !=0)
	{
		k2 = (float *)malloc(plane_num*(plane_num+1)*sizeof(float));
		#define K2(x,y)	k2[(x)*(plane_num+1)+(y)]
		
		v1 = (float *)malloc(dim_num * sizeof(float));
		v2 = (float *)malloc(dim_num * sizeof(float));
	}
	
	for(k=0;k<plane_num;k++)
		memcpy(k1+k*(dim_num+1),plane[k],(dim_num+1)*sizeof(float));
	
	for(k=0;k<vector_num;k++)
	{
		
		// K2(0,0) = PLANE[0][1];
		// K2(0,1) = PLANE[0][2];
		// K2(0,2) = PLANE[0][3]+PLANE[0][k];
		// K2(1,0) = PLANE[1][1];
		// K2(1,1) = PLANE[1][2];
		// K2(1,2) = PLANE[1][3];
		
		// maMathLinearEquation(k2,v1,plane_num);
		
		for(i=0;i<plane_num;i++)
		{
			for(j=0;j<plane_num;j++)
			{			
				K2(i,j) = PLANE(i,j+vector_num);
			}
			K2(i,j) = PLANE(i,j+vector_num);
		}
		
		maMathLinearEquation(k2,v1+vector_num,plane_num);
		
		for(i=0;i<plane_num;i++)
		{
			for(j=0;j<plane_num;j++)
			{			
				K2(i,j) = PLANE(i,j+vector_num);
			}
			K2(i,j) = PLANE(i,j+vector_num) + PLANE(i,k);
		}
		
		maMathLinearEquation(k2,v2+vector_num,plane_num);
		
		// for(n=0;n<3;n++)
			// printf("%f,",k2[n]);
		// printf("\n");
		
		// printf("%f,%f,%f\n",v1[0],v1[1],v1[2]);
		// printf("%f,%f,%f\n",v2[0],v2[1],v2[2]);
		// return;
		
		for(j=0;j<vector_num;j++)
		{
			if(j==k)
				K1(k+plane_num,k) = 1.0;
			else
				K1(k+plane_num,k) = 0.0;
		}
		
		for(j=vector_num;j<dim_num;j++)
			K1(k+plane_num,j) = v2[j]-v1[j];
		
		K1(k+plane_num,j) = 0.0;
		
	}
	
	maMathLinearEquation(k1,locate,dim_num);
	
	free(k1);
	if(vector_num!=0)
	{
		free(k2);
		free(v1);
		free(v2);
	}
}


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

