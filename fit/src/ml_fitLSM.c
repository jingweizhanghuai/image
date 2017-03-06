#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"

void maMLLinearFitLSM(float *XIn,float *YIn,int N,float *A)
{
	int i;
	float sumx,sumy,sumxy,sumx2;
	
	maException((N<2),"sample number (which <2) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	sumx = 0.0;
	sumy = 0.0;
	sumxy = 0.0;
	sumx2 = 0.0;
	for(i=0;i<N;i++)
	{
		sumx = sumx + XIn[i];
		sumy = sumy + YIn[i];
		sumxy = sumxy + XIn[i]*YIn[i];
		sumx2 = sumx2 + XIn[i]*XIn[i];
	}
	
	A[0] = ((((float)N)*sumxy)-(sumx*sumy))/((((float)N)*sumx2)-(sumx*sumx));
	A[1] = ((sumx2*sumy)-(sumx*sumxy))/((((float)N)*sumx2)-(sumx*sumx));
}

void maMLParabolaFitLSM(float *XIn,float *YIn,int N,float *A)
{
	int i;
	float x,y,x2;
	float data[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	
	maException((N<3),"sample number (which <3) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	for(i=0;i<N;i++)
	{
		x = XIn[i];
		y = YIn[i];
		x2 = XIn[i]*XIn[i];
		
		data[0] = data[0] + x2*x2;
		data[1] = data[1] + x2*x;
		data[2] = data[2] + x2;
		data[3] = data[3] - x2*y;
		
		data[6] = data[6] + x;
		data[7] = data[7] - x*y;
		
		data[11] = data[11] - y;
	}
	
	data[4] = data[1];
	data[5] = data[2];
	data[8] = data[2];
	data[9] = data[6];
	data[10] = (float)N;
	
	maMathLinearEquation(data,A,3);
}

void maMLPolyFitLSM(float *XIn,float *YIn,int N,float *A,int k)
{
	int i,j;
	float *sumx;
	float *sumyx;
	float sumy;
	float *data;
	float x,y,X;
	
	maException((k==0),"degree of polynomial is 0",EXIT);
	maException((N<k+1),"sample number is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	if(k == 1)
	{
		maMLLinearFitLSM(XIn,YIn,N,A);
		return;
	}
	if(k == 2)
	{
		maMLParabolaFitLSM(XIn,YIn,N,A);
		return;
	}
	
	sumx = (float *)malloc(k*2*sizeof(float));
	sumyx = (float *)malloc(k*sizeof(float));
	data = (float *)malloc((k+2)*(k+1)*sizeof(float));
	
	for(i=0;i<k*2;i++)
		sumx[i] = 0.0;
	
	for(i=0;i<k;i++)
		sumyx[i] = 0.0;
	sumy = 0.0;
	
	for(i=0;i<N;i++)
	{
		x = XIn[i];
		y = YIn[i];
		
		X = 1.0;		
		for(j=0;j<k*2;j++)
		{
			X = X*x;
			sumx[j] = sumx[j] + X;
		}
		
		X = 1.0;
		for(j=0;j<k;j++)
		{
			X = X*x;
			sumyx[j] = sumyx[j] + X*y;
		}
		
		sumy = sumy + y;
	}
	
	// for(j=0;j<k*2;j++)
		// printf("sumx%d is %f\n",j+1,sumx[j]);
	// printf("\n\n");
	// for(j=0;j<k;j++)
		// printf("sumyx%d is %f\n",j+1,sumyx[j]);
	
	for(i=0;i<k;i++)
		data[i] = sumx[k-i-1];
	
	data[k] = (float)N;
	data[k+1] = 0.0-sumy;
	
	#define DATA(x,y) data[y*(k+2)+x]
	
	for(j=1;j<=k;j++)
	{
		for(i=0;i<=k;i++)
			DATA(i,j) = sumx[k+j-i-1];
		
		DATA(i,j) = 0.0-sumyx[j-1];
	}
	
	// for(j=0;j<=k;j++)
	// {
		// printf("%d:\t",j);
		// for(i=0;i<=k+1;i++)
			// printf("%f\t",DATA(i,j));
	
		// printf("\n");
	// }
	
	maMathLinearEquation(data,A,k+1);
	
	free(sumx);
	free(sumyx);
	free(data);
}

void maMLLinearFitWeight(float *XIn,float *YIn,float *WIn,int N,float *A)
{
	int i;
	float sum,sumx,sumy,sumxy,sumx2;
	
	maException((N<2),"sample number (which <2) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	maException((WIn==NULL),"no input weight.",{maMLLinearFitLSM(XIn,YIn,N,A);return;});
	
	sumx = 0.0;
	sumy = 0.0;
	sumxy = 0.0;
	sumx2 = 0.0;
	sum = 0.0;
	for(i=0;i<N;i++)
	{
		sum = sum + WIn[i];
		sumx = sumx + WIn[i]*XIn[i];
		sumy = sumy + WIn[i]*YIn[i];
		sumxy = sumxy + WIn[i]*XIn[i]*YIn[i];
		sumx2 = sumx2 + WIn[i]*XIn[i]*XIn[i];
	}
	
	A[0] = ((sum*sumxy)-(sumx*sumy))/((sum*sumx2)-(sumx*sumx));
	A[1] = ((sumx2*sumy)-(sumx*sumxy))/((sum*sumx2)-(sumx*sumx));
}

#define ABS(x) (((x)>0)?(x):(0-(x)))
void maMLLinearFit(float *XIn,float *YIn,int N,float *A)
{
	int i;
	float *w;
	float error0,error1;
	int n;
	float y_max,y_min;
	
	maException((N<2),"sample number (which <2) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	y_max = YIn[0];
	y_min = YIn[0];
	for(i=0;i<N;i++)
	{
		y_max = (YIn[i]>y_max)?YIn[i]:y_max;
		y_min = (YIn[i]<y_min)?YIn[i]:y_min;
	}
	
	w = (float *)malloc(N*sizeof(float));
	
	maMLLinearFitLSM(XIn,YIn,N,A);
	
	error0 = (y_max-y_min)*(y_max-y_min);
	error1 = 0.0;
	for(i=0;i<N;i++)
	{
		w[i] = YIn[i]-(A[0]*XIn[i]+A[1]);
		w[i] = w[i]*w[i];
		error1 = error1+w[i];
	}
	error1 = error1/(float)N;
	
	for(i=0;i<N;i++)
		w[i] = (w[i]>error1)?(1.0/w[i]):1.0;
	
	// printf("error0 is %f,error1 is %f\n",error0,error1);
	
	n = 0;
	while(ABS(error0-error1)>0.001*(y_max-y_min))
	{
		n=n+1;
		if(n>=100)
			break;
		
		maMLLinearFitWeight(XIn,YIn,w,N,A);
		
		error0 = error1;
		error1 = 0.0;
		for(i=0;i<N;i++)
		{
			w[i] = YIn[i]-(A[0]*XIn[i]+A[1]);
			w[i] = w[i]*w[i];
			error1 = error1+w[i];
		}
		error1 = error1/(float)N;
		
		for(i=0;i<N;i++)
			w[i] = (w[i]>error1)?(1.0/w[i]):1.0;
		
		// printf("error0 is %f,error1 is %f\n",error0,error1);
	}
	
	free(w);
}
		
void maMLParabolaFitWeight(float *XIn,float *YIn,float *WIn,int N,float *A)
{
	int i;
	float x,y,x2,w;
	float data[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	
	maException((N<3),"sample number (which <3) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	maException((WIn==NULL),"no input weight.",{maMLParabolaFitLSM(XIn,YIn,N,A);return;});
	
	for(i=0;i<N;i++)
	{
		w = WIn[i];
		x = XIn[i];
		y = YIn[i];
		x2 = XIn[i]*XIn[i];
		
		data[10] = data[10] + w;
		
		data[0] = data[0] + w*x2*x2;
		data[1] = data[1] + w*x2*x;
		data[2] = data[2] + w*x2;
		data[3] = data[3] - w*x2*y;
		
		data[6] = data[6] + w*x;
		data[7] = data[7] - w*x*y;
		
		data[11] = data[11] - w*y;
	}
	
	data[4] = data[1];
	data[5] = data[2];
	data[8] = data[2];
	data[9] = data[6];
	
	maMathLinearEquation(data,A,3);
}

void maMLParabolaFit(float *XIn,float *YIn,int N,float *A)
{
	int i;
	float *w;
	float error0,error1;
	int n;
	float y_max,y_min;
	
	maException((N<3),"sample number (which <3) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	y_max = YIn[0];
	y_min = YIn[0];
	for(i=0;i<N;i++)
	{
		y_max = (YIn[i]>y_max)?YIn[i]:y_max;
		y_min = (YIn[i]<y_min)?YIn[i]:y_min;
	}
	
	w = (float *)malloc(N*sizeof(float));
	
	maMLParabolaFitLSM(XIn,YIn,N,A);
	
	error0 = (y_max-y_min)*(y_max-y_min);
	error1 = 0.0;
	for(i=0;i<N;i++)
	{
		w[i] = YIn[i]-(A[0]*XIn[i]*XIn[i]+A[1]*XIn[i]+A[2]);
		w[i] = w[i]*w[i];
		error1 = error1+w[i];
	}
	error1 = error1/(float)N;
	
	for(i=0;i<N;i++)
		w[i] = (w[i]>error1)?(1.0/w[i]):1.0;
	
	// printf("error0 is %f,error1 is %f\n",error0,error1);
	
	n = 0;
	while(ABS(error0-error1)>0.001*(y_max-y_min))
	{
		n=n+1;
		if(n>=100)
			break;
		
		maMLParabolaFitWeight(XIn,YIn,w,N,A);
		
		error0 = error1;
		error1 = 0.0;
		for(i=0;i<N;i++)
		{
			w[i] = YIn[i]-(A[0]*XIn[i]*XIn[i]+A[1]*XIn[i]+A[2]);
			w[i] = w[i]*w[i];
			error1 = error1+w[i];
		}
		error1 = error1/(float)N;
		
		for(i=0;i<N;i++)
			w[i] = (w[i]>error1)?(1.0/w[i]):1.0;
		
		// printf("error0 is %f,error1 is %f\n",error0,error1);
	}
	
	free(w);
}


void maMLPolyFitWeight(float *XIn,float *YIn,float *WIn,int N,float *A,int k)
{
	int i,j;
	float *sumx;
	float *sumyx;
	float sumy;
	float sum;
	float *data;
	float x,y,w,X;
	
	
	maException((k==0),"degree of polynomial is 0",EXIT);
	maException((N<k+1),"sample number is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	maException((WIn==NULL),"no input weight.",{maMLPolyFitLSM(XIn,YIn,N,A,k);return;});
	
	if(k == 1)
	{
		maMLLinearFitWeight(XIn,YIn,WIn,N,A);
		return;
	}
	if(k == 2)
	{
		maMLParabolaFitWeight(XIn,YIn,WIn,N,A);
		return;
	}
	
	sumx = (float *)malloc(k*2*sizeof(float));
	sumyx = (float *)malloc(k*sizeof(float));
	data = (float *)malloc((k+2)*(k+1)*sizeof(float));
	
	for(i=0;i<k*2;i++)
		sumx[i] = 0.0;
	
	for(i=0;i<k;i++)
		sumyx[i] = 0.0;
	sumy = 0.0;
	sum = 0.0;
	for(i=0;i<N;i++)
	{
		x = XIn[i];
		y = YIn[i];
		w = WIn[i];
		
		X = 1.0;		
		for(j=0;j<k*2;j++)
		{
			X = X*x;
			sumx[j] = sumx[j] + w*X;
		}
		
		X = 1.0;
		for(j=0;j<k;j++)
		{
			X = X*x;
			sumyx[j] = sumyx[j] + w*X*y;
		}
		
		// X = 1.0;		
		// for(j=0;j<k;j++)
		// {
			// X = X*x;
			// sumx[j] = sumx[j] + w*X;
			// sumyx[j] = sumyx[j] + w*X*y;
		// }
		// for(j=k;j<k*2;j++)
		// {
			// X = X*x;
			// sumx[j] = sumx[j] + w*X;
		// }
		
		sumy = sumy + w*y;
		sum = sum + w;
	}
	
	// for(j=0;j<k*2;j++)
		// printf("sumx%d is %f\n",j+1,sumx[j]);
	// printf("\n\n");
	// for(j=0;j<k;j++)
		// printf("sumyx%d is %f\n",j+1,sumyx[j]);
	
	for(i=0;i<k;i++)
		data[i] = sumx[k-i-1];
	
	data[k] = sum;
	data[k+1] = 0.0-sumy;
	
	#define DATA(x,y) data[y*(k+2)+x]
	
	for(j=1;j<=k;j++)
	{
		for(i=0;i<=k;i++)
			DATA(i,j) = sumx[k+j-i-1];
		
		DATA(i,j) = 0.0-sumyx[j-1];
	}
	
	// for(j=0;j<=k;j++)
	// {
		// printf("%d:\t",j);
		// for(i=0;i<=k+1;i++)
			// printf("%f\t",DATA(i,j));
	
		// printf("\n");
	// }
	
	maMathLinearEquation(data,A,k+1);
	
	free(sumx);
	free(sumyx);
	free(data);
}

void maMLPolyFit(float *XIn,float *YIn,int N,float *A,int k)
{
	int i,j;
	float *w;
	float error0,error1;
	float y;
	float X;
	int n;
	float y_max,y_min;
	
	maException((k==0),"degree of polynomial is 0",EXIT);
	maException((N<k+1),"sample number is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	if(k == 1)
	{
		maMLLinearFit(XIn,YIn,N,A);
		return;
	}
	if(k == 2)
	{
		maMLParabolaFit(XIn,YIn,N,A);
		return;
	}
	
	y_max = YIn[0];
	y_min = YIn[0];
	for(i=0;i<N;i++)
	{
		y_max = (YIn[i]>y_max)?YIn[i]:y_max;
		y_min = (YIn[i]<y_min)?YIn[i]:y_min;
	}
	
	w = (float *)malloc(N*sizeof(float));
	
	maMLPolyFitLSM(XIn,YIn,N,A,k);
	
	error0 = (y_max-y_min)*(y_max-y_min);
	error1 = 0.0;
	for(i=0;i<N;i++)
	{
		X = XIn[i];
		y = A[k-1]*X + A[k];
		for(j=k-2;j>=0;j--)
		{
			X = X*XIn[i];
			y = y+A[j]*X;
		}
	
		w[i] = YIn[i]-y;
		w[i] = w[i]*w[i];
		error1 = error1+w[i];
	}
	error1 = error1/(float)N;
	
	for(i=0;i<N;i++)
		w[i] = (w[i]>error1)?(1.0/w[i]):1.0;
	
	// printf("error0 is %f,error1 is %f\n",error0,error1);
	
	n = 0;
	while(ABS(error0-error1)>0.001*(y_max-y_min))
	{
		n=n+1;
		if(n>=100)
			break;
		
		maMLPolyFitWeight(XIn,YIn,w,N,A,k);
		
		error0 = error1;
		error1 = 0.0;
		for(i=0;i<N;i++)
		{
			X = XIn[i];
			y = A[k-1]*X + A[k];
			for(j=k-2;j>=0;j--)
			{
				X = X*XIn[i];
				y = y+A[j]*X;
			}
			
			w[i] = YIn[i]-y;
			w[i] = w[i]*w[i];
			error1 = error1+w[i];
		}
		error1 = error1/(float)N;
		
		for(i=0;i<N;i++)
			w[i] = (w[i]>error1)?(1.0/w[i]):1.0;
		
		// printf("error0 is %f,error1 is %f\n",error0,error1);
	}
	
	free(w);
}
	
		
	

	


