#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exception.h"

void maMLLinearFitSGD(float *XIn,float *YIn,int N,float *A)
{
	int i;
	int n;
	float err;
	float error0,error1;
	float d0,d1;
	float D0,D1;
	float k;
	float a0,a1;
	
	maException((N<2),"sample number (which <2) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);
	
	A[0] = 1.0;
	A[1] = 0.0;
	
	error0 = 0.0;
	D0 = 0.0;
	D1 = 0.0;
	for(i=0;i<N;i++)
	{
		err = (XIn[i]-YIn[i]);
		error0 = error0 + err*err;
		
		D0 = D0 + err*XIn[i];
		D1 = D1 + err;
	}
	
	// printf("a0 is %f,a1 is %f,error1 is %f,d0 is %f,d1 is %f\n",A[0],A[1],error1,D0,D1);
	k = 1.0/(float)N;	
	
	for(n=0;n<100000;n++)
	{
		a0 = A[0]-k*D0;
		a1 = A[1]-k*D1;
		
		error1 = 0.0;
		d0 = 0.0;
		d1 = 0.0;
		for(i=0;i<N;i++)
		{
			err = (a0*XIn[i]+a1-YIn[i]);
			error1 = error1 + err*err;
			
			d0 = d0 + err*XIn[i];
			d1 = d1 + err;
		}
		
		// printf("k is %f\n",k);
		// printf("a0 is %f,a1 is %f,d0 is %f,d1 is %f\n",a0,a1,d0,d1);
		// printf("error0 is %f,error1 is %f\n",error0,error1);
		
		if(error0<error1)
			k = k*0.05;
		else if(error0-error1<0.0001)
			break;
		else
		{
			k = k+k;

			A[0] = a0;
			A[1] = a1;
			
			D0 = d0;
			D1 = d1;
			
			error0 = error1;
		}		
	}
	// printf("error0 is %f,error1 is %f\n",error0,error1);
	// printf("n is %d\n",n);
}

void maMLParabolaFitSGD(float *XIn,float *YIn,int N,float *A)
{
	int i;
	int n;
	float err;
	float error0,error1;
	float d0,d1,d2;
	float D0,D1,D2;
	float k;
	float a0,a1,a2;
	
	maException((N<3),"sample number (which <3) is not enough.",EXIT);
	maException(((XIn==NULL)||(YIn==NULL)),"no input data.",EXIT);
	maException((A==NULL),"answer out is NULL.",EXIT);

	A[0] = 1.0;
	A[1] = 1.0;
	A[2] = 0.0;
	
	error0 = 0.0;
	D0 = 0.0;
	D1 = 0.0;
	D2 = 0.0;
	for(i=0;i<N;i++)
	{
		err = (XIn[i]*(XIn[i]+1.0)-YIn[i]);
		error0 = error0 + err*err;
		
		D0 = D0 + err*XIn[i]*XIn[i];
		D1 = D1 + err*XIn[i];
		D2 = D2 + err;
	}
	
	// printf("a0 is %f,a1 is %f,error1 is %f,d0 is %f,d1 is %f\n",A[0],A[1],error1,D0,D1);
	k = 1.0/(float)N;	
	
	for(n=0;n<1000000;n++)
	{
		a0 = A[0]-k*D0;
		a1 = A[1]-k*D1;
		a2 = A[2]-k*D2;
		
		error1 = 0.0;
		d0 = 0.0;
		d1 = 0.0;
		d2 = 0.0;
		for(i=0;i<N;i++)
		{
			err = (a0*XIn[i]*XIn[i]+a1*XIn[i]+a2-YIn[i]);
			error1 = error1 + err*err;
			
			d0 = d0 + err*XIn[i]*XIn[i];
			d1 = d1 + err*XIn[i];
			d2 = d2 + err;
		}
		
		// printf("k is %f\n",k);
		// printf("a0 is %f,a1 is %f,d0 is %f,d1 is %f\n",a0,a1,d0,d1);
		// printf("error0 is %f,error1 is %f\n",error0,error1);
		
		if(error0<error1)
			k = k*0.1;
		else if(error0-error1<0.0001)
			break;
		else
		{
			k = k+k;

			A[0] = a0;
			A[1] = a1;
			A[2] = a2;
			
			D0 = d0;
			D1 = d1;
			D2 = d2;
			
			error0 = error1;
		}		
	}
	// printf("error0 is %f,error1 is %f\n",error0,error1);
	// printf("n is %d\n",n);
}

