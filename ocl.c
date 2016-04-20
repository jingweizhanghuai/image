#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <CL/cl.h>

#include "type.h"

extern int ocl_unified_mem;
extern int ocl_support;
extern cl_platform_id platform_id;
extern cl_device_id device_id;
extern cl_context context;
extern cl_command_queue command_queue;
extern char *kernel_source;

#define RELEASE_CL {\
    if(command_queue != NULL)\
        clReleaseCommandQueue(command_queue);\
    if(context != NULL)\
        clReleaseContext(context);\
	if(*kernel != NULL)\
        clReleaseKernel(*kernel);\
    if(*program != NULL)\
        clReleaseProgram(*program);\
	if(kernel_source !=NULL)\
		free(kernel_source);\
}\

int imgOCLSetPlatform()
{	
	cl_int ret;

	if(platform_id == NULL)
		clGetPlatformIDs(1,&platform_id,NULL);
	if(platform_id == NULL)
	{
		ocl_support = 0;
		printf("Get OpenCL platform failed!\n");
		return 0;
	}
		
	if(device_id == NULL)
		clGetDeviceIDs(platform_id,CL_DEVICE_TYPE_GPU,1,&device_id,NULL);
	if(device_id == NULL)
	{
		ocl_support = 0;
		printf("No GPU available as a compute device!\n");
		return 0;
	}
	
	// char device_info[300];
	// clGetDeviceInfo(device_id,CL_DEVICE_NAME,300,device_info,NULL);
	// printf("%s\n",device_info);
	
	cl_bool image_suppot;
	clGetDeviceInfo(device_id,CL_DEVICE_IMAGE_SUPPORT,sizeof(cl_bool),&image_suppot,NULL);
	if(image_suppot != CL_TRUE)
	{
		ocl_support = 0;
		printf("image is not suppotted.\n");
		return 0;
	}
	
	clGetDeviceInfo(device_id,CL_DEVICE_HOST_UNIFIED_MEMORY,sizeof(cl_bool),&ocl_unified_mem,NULL);
	
	cl_command_queue_properties prop;
	clGetDeviceInfo(device_id,CL_DEVICE_QUEUE_PROPERTIES,sizeof(cl_command_queue_properties),&prop,NULL);	
	prop = prop&(~CL_QUEUE_PROFILING_ENABLE);
	
	if(context == NULL)
		context = clCreateContext(NULL,1,&device_id,NULL,NULL,&ret);
	if((ret != CL_SUCCESS)||(context == NULL))
	{
		printf("Context not established with opencl error %d.\n",ret);
		return 0;
	}

	if(command_queue == NULL)
		command_queue = clCreateCommandQueue(context,device_id,prop,&ret);
	if((ret != CL_SUCCESS)||(command_queue == NULL))
	{
		printf("Command queue cannot be created with opencl error %d.\n",ret);
		clReleaseContext(context);
		return 0;
	}

	ocl_support = 1;
	return 1;
}

int imgOCLUnifiedMemoryCheck()
{
	clGetDeviceInfo(device_id,CL_DEVICE_HOST_UNIFIED_MEMORY,sizeof(cl_bool),&ocl_unified_mem,NULL);
	return ocl_unified_mem;
}

void imgOCLBuildKernel(char *kernel_source,const char *kernel_name,cl_program *program,cl_kernel *kernel)
{
	if(*kernel !=NULL)
		return;
	
	// printf("aaaaaaaaaaaaaaaaaaa\n");
	cl_int ret;
	
	if(*program ==NULL)
	{
		if(ocl_support == -1)
			imgOCLSetPlatform();
		
		if(ocl_support == 0)
		{
			printf("image is not suppotted.\n");
			RELEASE_CL;
			exit(0);
		}	

		// FILE *fp = fopen(cl_filename,"r");
		// if (fp == NULL)
		// {
			// printf("The specified kernel source file cannot be opened!\n");
			// RELEASE_CL;
			// exit(0);
		// }
		// fseek(fp,0,SEEK_END);
		// const long kernelLength = ftell(fp);
		// fseek(fp,0,SEEK_SET);

		// kernel_source = malloc(kernelLength);

		// fread(kernel_source,1,kernelLength,fp);
		// fclose(fp);	
		const long kernelLength = strlen(kernel_source);
		
		*program = clCreateProgramWithSource(context,1,(const char**)&kernel_source,(const size_t*)&kernelLength,&ret);
		if (ret != CL_SUCCESS)
		{
			printf("IMG Error:\n\tIn imgOCLBuildKernel:Failed to create program with OpenCL error %d.\n",ret);
			exit(0);
		}
		ret = clBuildProgram(*program,1,&device_id,NULL,NULL,NULL);
		if (ret != CL_SUCCESS)
		{
			size_t len;
			char buffer[2048];
			
			printf("IMG Error:\n\tIn imgOCLBuildKernel:Failed to build program executable with OpenCL error %d.\n",ret);
			clGetProgramBuildInfo(*program,device_id,CL_PROGRAM_BUILD_LOG,sizeof(buffer),buffer,&len);
			printf("%s\n",buffer);
			RELEASE_CL;
			exit(0);
		}
		
		free(kernel_source);
		kernel_source = NULL;
	}
	
	*kernel = clCreateKernel(*program,kernel_name,&ret);
    if(*kernel == NULL)
    {
        printf("Kernel failed to create!\n");
        RELEASE_CL;
		exit(0);
    }
}

void imgCreateMatOCLMemory(ImgMat *p)
{
	if(context == NULL)
		imgOCLSetPlatform();
	
	int ret;
	
	int cn = (p->type>>3)+1;
	
	int num;
	num = p->size*cn;
	
	cl_mem *memory;
	memory = malloc(sizeof(cl_mem));
	
	if(ocl_unified_mem == -1)
		imgOCLUnifiedMemoryCheck();
	
	if(ocl_unified_mem)
		*memory = clCreateBuffer(context,CL_MEM_USE_HOST_PTR,num,p->data.ptr,&ret);
	else
		*memory = clCreateBuffer(context,CL_MEM_READ_WRITE,num,NULL,&ret);
	
	#ifdef DEBUG
	if(ret != CL_SUCCESS)
	{
		printf("IMG Error:\n\tIn imgCreateMatOCLMemory: Memory object failed to create with opencl error %d.\n",ret);
		exit(0);
	}
	#endif
	
	p->memory[1] = memory;
	p->memory_valid[1] = 0;
}

void imgWriteMatOCLMemory(ImgMat *p)
{	
	if(p->memory_valid[1] == 1)
		return;
	
	if(p->memory[1] == NULL)
		imgCreateMatOCLMemory(p);
	
	#ifdef DEBUG
	if(p->memory_valid[0] == 0)
	{
		printf("IMG Error:\n\tIn imgWriteMatOCLMemory: The main memory data is invalid.\n");
		exit(0);
	}
	#endif
	
	if(ocl_unified_mem == -1)
		imgOCLUnifiedMemoryCheck();
	
	if(ocl_unified_mem)
	{
		p->memory_valid[1] = p->memory_valid[0];
		return;
	}
	
	cl_mem memory;
	memory = *((cl_mem *)p->memory[1]);
	
	int cn = (p->type>>3)+1;
	int num = p->size*cn;
	
	int ret;
	ret = clEnqueueWriteBuffer(command_queue,memory,CL_TRUE,0,num,p->data.ptr,0,NULL,NULL);
	if(ret != CL_SUCCESS)
	{
		printf("IMG Error:\n\tIn imgWriteMatOCLMemory: Write Memory object failed.\n");
		exit(0);
	}
	
	p->memory_valid[1] = 1;
}
	
void imgReadMatOCLMemory(ImgMat *p)
{	
	if((context == NULL)||(command_queue == NULL))
	{
		printf("IMG Error:\n\tIn imgReadMatOCLMemory: OpenCL platform have not been set.\n");
		exit(0);
	}
	
	if(p->memory_valid[0])
		return;
	
	#ifdef DEBUG
	if((p->memory_valid[1] == 0)||(p->memory[1] == NULL))
	{
		printf("IMG Error:\n\tin imgReadMatOCLMemory: The OCL memory data is invalid.\n");
		exit(0);
	}
	#endif
	
	if(ocl_unified_mem == -1)
		imgOCLUnifiedMemoryCheck();
	
	void *result;
	int ret;
	
	cl_mem *memory;
	memory = ((cl_mem *)p->memory[1]);
	
	int cn = (p->type>>3)+1;
	int num = p->size*cn;
	
	if(ocl_unified_mem)
	{
		result = (int *)clEnqueueMapBuffer(command_queue,*memory,CL_TRUE,CL_MAP_READ|CL_MAP_WRITE,0,num,0,NULL, NULL,&ret);
		// ret = clEnqueueUnmapMemObject(command_queue,*memory,result,0,NULL,NULL);
		if(result!=p->data.ptr)
			ret = ret | clEnqueueReadBuffer(command_queue,*memory,CL_TRUE,0,num,p->data.ptr,0,NULL,NULL);
	}
	else
		ret = clEnqueueReadBuffer(command_queue,*memory,CL_TRUE,0,num,p->data.ptr,0,NULL,NULL);

	if(ret != CL_SUCCESS)
	{
		printf("IMG Error:\n\tRead memory object failed");
		exit(0);
	}
	
	p->memory_valid[0] = 1;
}

void imgCopyMatOCLMemory(ImgMat *src,ImgMat *dst)
{
	if((context == NULL)||(command_queue == NULL))
	{
		printf("IMG Error:\n\tIn imgCopyMatOCLMemory: OpenCL platform have not been set.\n");
		exit(0);
	}
	
	if((src->memory[1]== NULL)||(src->memory_valid[1]==0))
	{
		printf("IMG Error:\n\tThe OCL memory data is invalid.\n");
		exit(0);
	}
	
	cl_mem src_memory = *((cl_mem *)src->memory[1]);
	cl_mem dst_memory = *((cl_mem *)dst->memory[1]);
	
	int cn = (src->type>>3)+1;
	int num = src->size*cn;	
	
	if(dst->memory[1]==NULL)
		imgCreateMatOCLMemory(dst);
	
	clEnqueueCopyBuffer(command_queue,src_memory,dst_memory,0,0,num,0,NULL,NULL);
	
	dst->memory_valid[1] = 1;
}

void imgReleaseMatOCLMemory(ImgMat *src)
{	
	if(src->memory[1] == NULL)
		return;
	
	cl_mem *src_memory;
	src_memory = (cl_mem *)src->memory[1];
	
	clReleaseMemObject(*src_memory);
	
	src->memory[1] = NULL;
	src->memory_valid[1] = 0;
}
