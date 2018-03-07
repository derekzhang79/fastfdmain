#include "get_machine_id.h"
#ifdef Q_OS_UNIX
#include <fcntl.h>
#include <stdio.h>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
char *temp;

int BCDToStr2(const char *Src, char *Des, int iDeslen)
{
    if (Src == NULL || Des == NULL)
    {
        return false;
    }

    int iSrcLen_go = iDeslen/2;

    if(iDeslen%2)
    {
        iSrcLen_go = iSrcLen_go +1;
    }

    if (iSrcLen_go > 15)
    {
        iSrcLen_go = 15;
    }


    char chTemp = 0;
    char chDes = 0;
    for (int i = 0; i < iSrcLen_go; i++)
    {
        chTemp = Src[i];

        //printf("%02x\n",chTemp);


        chDes = chTemp >> 4 &0x0F;
        Des[i * 2] = chDes + '0';
        chDes = (chTemp & 0x0F) + '0';
        Des[i * 2 + 1] = chDes;
    }
    return iSrcLen_go;
}
#endif

get_machine_id::get_machine_id()
{
}

QString get_machine_id::get_id()
{
    return "";
}


