#include <CL/cl.h>

extern cl_context context;
extern cl_command_queue command_queue;

extern int ocl_unified_mem;
extern cl_device_id device_id;

#define READ_ONLY CL_MEM_READ_ONLY
#define READ_WRITE CL_MEM_READ_WRITE
#define WRITE_ONLY CL_MEM_WRITE_ONLY

#define RUN_DEVICE(kernel,width,height,cn) {\
	int ret;\
	ret = clEnqueueNDRangeKernel(command_queue,kernel,3,NULL,(const size_t[]){(width+15)/16,height,3},NULL,0,NULL,NULL);\
	if(ret != CL_SUCCESS)\
	{\
		printf("run kernel error!\n");\
		exit(0);\
	}\
}

#define WRITE_DEVICE_ARG(kernel,index,arg,num) {\
	int ret;\
	ret = clSetKernelArg(kernel,index,num,&arg);\
	if(ret != CL_SUCCESS)\
	{\
		printf("set arguments error with opencl error %d\n",ret);\
		exit(0);\
	}\
}

#define WRITE_DEVICE_BUFF(kernel,index,p_host,p_device,num,cl_type) {\
	if(ocl_unified_mem == -1)\
		clGetDeviceInfo(device_id,CL_DEVICE_HOST_UNIFIED_MEMORY,sizeof(cl_bool),&ocl_unified_mem,NULL);\
	\
	int ret;\
	if(ocl_unified_mem)\
		p_device = clCreateBuffer(context,CL_MEM_USE_HOST_PTR,num,p_host,&ret);\
	else\
	{\
		p_device = clCreateBuffer(context,cl_type,num,NULL,&ret);\
		if(cl_type!=WRITE_ONLY)\
			ret = ret|clEnqueueWriteBuffer(command_queue,p_device,CL_TRUE,0,num,p_host,0,NULL,NULL);\
	}\
	\
	if(ret != CL_SUCCESS)\
	{\
		printf("Memory object failed to create!");\
		exit(0);\
	}\
	\
	ret = clSetKernelArg(kernel,index,sizeof(cl_mem),(void *)&p_device);\
	if(ret != CL_SUCCESS)\
	{\
		printf("Set arguments(index %d) error with opencl error %d.\n",index,ret);\
		exit(0);\
	}\
}

#define READ_DEVICE_BUFF(p_device,p_host,num) {\
	if(ocl_unified_mem == -1)\
		clGetDeviceInfo(device_id,CL_DEVICE_HOST_UNIFIED_MEMORY,sizeof(cl_bool),&ocl_unified_mem,NULL);\
	\
	void *result;\
	int ret;\
	\
	if(ocl_unified_mem)\
	{\
		result = (int *)clEnqueueMapBuffer(command_queue,p_device,CL_TRUE,CL_MAP_READ|CL_MAP_WRITE,0,num,0,NULL, NULL,&ret);\
		ret = clEnqueueUnmapMemObject(command_queue,p_device,result,0,NULL,NULL);\
		if(result!=p_host)\
		{\
			ret = ret | clEnqueueReadBuffer(command_queue,p_device,CL_TRUE,0,num,p_host,0,NULL,NULL);\
		}\
	}\
	else\
		ret = clEnqueueReadBuffer(command_queue,p_device,CL_TRUE,0,num,p_host,0,NULL,NULL);\
	\
	if(ret != CL_SUCCESS)\
	{\
		printf("Read memory object failed");\
		exit(0);\
	}\
}

#define FREE_DEVICE_BUFF(p_device) {\
	clReleaseMemObject(p_device);\
	p_device = NULL;\
}

#define SET_SAMPLER(address_mod,filter_mod) {\
	int ret;\
	clCreateSampler(context,CL_FALSE,address_mod,filter_mod,&ret);\
	if(ret != CL_SUCCESS)\
	{\
		printf("Sampler failed to create!");\
		exit(0);\
	}\
}

#define WRITE_DEVICE_IMG(kernel,index,mat,p_device,cl_type) {\
	if(ocl_unified_mem == -1)\
		clGetDeviceInfo(device_id,CL_DEVICE_HOST_UNIFIED_MEMORY,sizeof(cl_bool),&ocl_unified_mem,NULL);\
	\
	int mat_cn;\
	mat_cn = ((mat->type)>>3)+1;\
	int mat_type;\
	mat_type = (mat->type)&0x07;\
	cl_image_format img_type;\
	if(mat_cn == 1)\
		img_type.image_channel_order = CL_R;\
	else if(mat_cn == 2)\
		img_type.image_channel_order = CL_RA;\
	else if(mat_cn == 3)\
		img_type.image_channel_order = CL_RGB;\
	else if(mat_cn == 2)\
		img_type.image_channel_order = CL_RGBA;\
	\
	if(mat_type==TYPE_8U)\
		img_type.image_channel_data_type = CL_UNSIGNED_INT8;\
	else if(mat_type==TYPE_8S)\
		img_type.image_channel_data_type = CL_SIGNED_INT8;\
	else if(mat_type==TYPE_16U)\
		img_type.image_channel_data_type = CL_UNSIGNED_INT16;\
	else if(mat_type==TYPE_16S)\
		img_type.image_channel_data_type = CL_SIGNED_INT16;\
	else if(mat_type==TYPE_32S)\
		img_type.image_channel_data_type = CL_SIGNED_INT32;\
	else if(mat_type==TYPE_32F)\
		img_type.image_channel_data_type = CL_FLOAT;\
	\
	int ret;\
	if(ocl_unified_mem)\
		p_device = clCreateImage2D(context,CL_MEM_USE_HOST_PTR,&img_type,mat->width,mat->height,0,mat->data.ptr,&ret);\
	else\
	{\
		p_device = clCreateImage2D(context,cl_type,&img_type,mat->width,mat->height,0,NULL,&ret);\
		size_t origin[3] = {0,0,0};\
		size_t region[3] = {0,0,0};\
		ret = ret|clEnqueueWriteImage(command_queue,p_device,CL_TRUE,origin,region,0,0,mat->data.ptr,0,NULL,NULL);\
	}\
	\
	if(ret != CL_SUCCESS)\
	{\
		printf("Memory object failed to create!");\
		exit(0);\
	}\
	\
	ret = clSetKernelArg(kernel,index,sizeof(cl_mem),(void *)&p_device);\
	if(ret != CL_SUCCESS)\
	{\
		printf("Set arguments error!\n");\
		exit(0);\
	}\
}

#define READ_DEVICE_IMG(p_device,mat) {\
	void *result;\
	size_t origin[3] = {0,0,0};\
	size_t region[3] = {0,0,0};\
	int ret;\
	\
	if(ocl_unified_mem)\
	{\
		result = (int *)clEnqueueMapImage(command_queue,p_device,CL_TRUE,CL_MAP_READ|CL_MAP_WRITE,origin,region,0,0,0,NULL,NULL,&ret);\
		if(result!=mat->data.ptr)\
		{\
			ret = clEnqueueReadImage(command_queue,p_device,CL_TRUE,origin,region,0,0,mat->data.ptr,0,NULL,NULL);\
			ret = ret|clEnqueueUnmapMemObject(command_queue,p_device,result,0,NULL,NULL);\
		}\
	}\
	else\
		ret = clEnqueueReadImage(command_queue,p_device,CL_TRUE,origin,region,0,0,mat->data.ptr,0,NULL,NULL);\
	\
	if(ret != CL_SUCCESS)\
	{\
		printf("Read memory object failed");\
		exit(0);\
	}\
}

#define FREE_DEVICE_IMG(p_device) clReleaseMemObject(p_device)
	
		
		

