#include<stdio.h>
#include<cerrno>
#include<cstring>

#define ERROR_PRINT(str,no) printf("\nERROR:%s\nerrno:%d\nMESSAGE:%s\nFILE:%s::%s(),<LINE:%d>\nRETURN_NUM:%d",strerror(errno),errno,str,__FILE__,__FUNCTION__,__LINE__,no);\
							fflush(stdout); \
							exit(no);