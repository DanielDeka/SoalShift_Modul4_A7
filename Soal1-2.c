#define FUSE_USE_VERSION 28

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include <sys/stat.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static const char *dirpath = "/home/daniel/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	car fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath,stbuf);

	if(res == -1)
	{
		return -errno;
	}

	return 0;
}
