#define STR(x) #x

#define FILE_ERROR_CHECK(func,f) {\
	if((f)==NULL)\
	{\
		printf("IMG Error:\n\tin STR(func): file cannot open.\n");\
		exit(0);\
	}\
}\

#define SOURCE_ERROR_CHECK(func,src) {\
	if(src==NULL)\
	{\
		printf("IMG Error\n\tin STR(func):source is NULL.\n");\
		exit(0);\
	}\
}\

#define DESTINATION_ERROR_CHECK(func,dst) {\
	if(dst==NULL)\
	{\
		printf("IMG Error\n\tin STR(func):destination is NULL,or img.h is not include.\n");\
		exit(0);\
	}\
}\
		
