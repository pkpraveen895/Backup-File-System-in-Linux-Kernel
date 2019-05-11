#ifndef BKPCTL_H
#define BKPCTL_H
#include <linux/ioctl.h>

struct query
{
    char *name;
    int version;
    int youngest;
    int oldest;
    char buff[1024];
};
 
//#define QUERY_GET_VARIABLES _IOR('q', 1, query *)
//#define QUERY_CLR_VARIABLES _IO('q', 2)
#define QUERY_DELETE _IOW('q', 1, query *)
#define QUERY_RESTORE _IOW('q', 2, query *) 

#endif
