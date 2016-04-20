#define IMG_ERR(func,info) {\
	printf("IMG Error:\n\tin #func: %s\n",info);\
	exit(0);\
}

#define FILE_ERROR_CHECK(func,f) {\
	if((f)==NULL)\
	{\
		printf("IMG Error:\n\tIn #func: File cannot be open.\n");\
		exit(0);\
	}\
}

#define SOURCE_ERROR_CHECK(func,src) {\
	if(src==NULL)\
	{\
		printf("IMG Error\n\tin #func: Source is NULL.\n");\
		exit(0);\
	}\
	if((src->memory_valid[0] == 0)&&(src->memory_valid[1] == 0))\
	{\
		printf("IMG Error:\n\tin #func: The src data is invalid.\n");\
		exit(0);\
	}\
}

#define DESTINATION_ERROR_CHECK(func,dst) {\
	if(dst==NULL)\
	{\
		printf("IMG Error\n\tin #func: Destination is NULL,or img.h is not include.\n");\
		exit(0);\
	}\
}
		
