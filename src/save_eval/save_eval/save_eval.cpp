// save_eval.cpp : 定义 DLL 应用程序的导出函数。
//

#include "save_eval.h"
#include <stdio.h>

// 这是导出函数的一个示例。
SAVE_EVAL_API void save_eval(char* str,int length)
{
	char sep[] = "\r\n----------------------------------\r\n\r\n";
	FILE * fp = fopen("eval_log.txt","a");
	fwrite(sep,sizeof(sep),1,fp);
	fwrite(str,1,length,fp);
	fclose(fp);
}
