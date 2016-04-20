#include <CL/cl.h>

#define imgOCLFinish() {\
	if(command_queue != NULL)\
        clReleaseCommandQueue(command_queue);\
    if(context != NULL)\
        clReleaseContext(context);\
	FREE_KERNEL_MEAN_FILTER\
}

int ocl_unified_mem = -1;
int ocl_support = -1;
cl_platform_id platform_id = NULL;
cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue command_queue = NULL;
// char *kernel_source = NULL;

cl_kernel kernel_sub = NULL;
cl_program program_sub = NULL;

cl_program program_mean_filter = NULL;
cl_kernel kernel_mean_filter_r1 = NULL;
cl_kernel kernel_mean_filter_r2 = NULL;
cl_kernel kernel_mean_filter_r3 = NULL;
cl_kernel kernel_mean_filter_rn = NULL;
#define FREE_KERNEL_MEAN_FILTER {\
	if(program_mean_filter !=NULL)\
	{\
		clReleaseProgram(program_mean_filter);\
		if(kernel_mean_filter_r1 != NULL)\
			clReleaseKernel(kernel_mean_filter_r1);\
		if(kernel_mean_filter_r2 != NULL)\
			clReleaseKernel(kernel_mean_filter_r2);\
		if(kernel_mean_filter_r3 != NULL)\
			clReleaseKernel(kernel_mean_filter_r3);\
		if(kernel_mean_filter_rn != NULL)\
			clReleaseKernel(kernel_mean_filter_rn);\
	}\
}
