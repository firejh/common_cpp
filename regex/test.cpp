#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//REG_EXTENDED 不支持 \d 格式

char *pick_regex(const char *string,const char *pattern)
{
    int err;
    char errbuf[1024];
    regex_t compiled;
    //if((err = regcomp(&compiled,pattern,REG_EXTENDED|REG_ICASE|REG_NEWLINE)) != 0)
    if((err = regcomp(&compiled,pattern,REG_EXTENDED|REG_NEWLINE)) != 0)
    {
         regerror(err,&compiled,errbuf,sizeof(errbuf));
         printf("err:%s\n",errbuf);
        return NULL;
    }
    
    regmatch_t pmatch[2];
    //err = regexec(&compiled,string,2,pmatch,REG_NOTBOL);
    err = regexec(&compiled,string,2,pmatch,REG_NOTEOL);
    if(err != 0)
    {
        printf("未匹配成功！\n");
        return NULL;
    }
    if(compiled.re_nsub != 1)
        return NULL;
    if(pmatch[1].rm_so == -1)
        return NULL;

    int len = pmatch[1].rm_eo - pmatch[1].rm_so;
    char* value = (char *)malloc(len + 1);
    if(value == NULL)
        return NULL;
    memset(value,0,len + 1);
    memcpy(value,string + pmatch[1].rm_so,len);
    regfree(&compiled);//切记最后要释放掉，否则会造成内存泄露

    return value;
}

int main()
{
    //const char *string = "username=xinger&sex=girl&age=22\r\nschool=BIT&husband=qinger\r\n&like=study&look=pretty\r\nschool=BIT&";
    //const char *pattern = "school=([^&]*)";
    //const char *string = "user:1001:info:100:test";
    //const char *pattern = "(user:[0-9]*:info:[0-9]*:test)";
    const char *string = "user1";
    const char *pattern = "(^user)";
    char *value = pick_regex(string,pattern);
    printf("提取的值为：%s\n",value);
    free(value);

    return 0;
}
