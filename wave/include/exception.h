#ifndef _MA_EWMESSAGE_H_
#define _MA_EWMESSAGE_H_

#define EXIT exit(0)
#define RETURN {return;}
#define RETURN_FALSE {return IMP_FALSE;}
#define RETURN_TRUE  {return IMP_TRUE;}
#define RETURN_NULL  {return NULL;}

#define maException(flag,message,follow) {\
	if(flag)\
	{\
		if(strcmp(#follow,"EXIT") == 0)\
			printf("[%s,line %d]Error: in function %s: %s\n",__FILE__,__LINE__,__FUNCTION__,message);\
		else\
			printf("[%s,line %d]Warning: in function %s: %s\n",__FILE__,__LINE__,__FUNCTION__,message);\
		\
		follow;\
	}\
}

#endif