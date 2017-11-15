#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/daniel/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath,stbuf);

	if(res == -1)
	{
		return -errno;
	}

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	if(strcmp(path,"/")==0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath,"%s%s",dirpath,path);

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if(dp==NULL) return -errno;

	while ( (de = readdir(dp)) != NULL)
	{
		struct stat st;
		memset(&st,0,sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type<<12;
		if(filler(buf,de->d_name,&st,0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);

	(void) fi;

	char ext[1000];
	int i;
	for(i=0; i<strlen(fpath)&&fpath[i]!='.';i++); // membaca ekstensi filenya
		strcpy(ext, fpath+i);

	char cmd[1000];

	fd = open(fpath, O_RDONLY);
	if(fd == -1)
		return -errno;
	else
	{
		if(strcmp(ext,".c")==0 || strcmp(ext,".3gp")==0||strcmp(ext,".doc")==0 || strcmp(ext, ".ditandai")==0)
		{
			system("zenity --width 400 --error --title 'Error' --text 'Terjadi kesalahan! File berisi konten berbahaya.'"); //Menampilkan dialog box error ketika file diakses
			char from[1000], to[1000], newFolder[1000], filename[1000]; 
			strcpy(newFolder,fpath); //mengambil direktori dari file

			for(i=strlen(newFolder)-1;newFolder[i]!='/';newFolder[i--]=0); //hapus filenamenya dan tambah "rahasia"
				strcat(newFolder,"rahasia");

			for(i=strlen(fpath)-1;fpath[i]!='/';i--); //+1 untuk menghilangkan tanda '/'
				strcpy(filename,fpath+(i+1)); 

			struct stat s;
			if (stat(newFolder, &s)!=0) //jika folder tidak ada berarti 0
				mkdir(newFolder,0777); //mkdir 'rahasia' ke direktori mountnya

			sprintf(from, "%s",fpath); //ambil path file awal
			sprintf(to, "%s%s.ditandai",newFolder,filename); //buat pathnya ke "rahasia" dan tambah ekstensi ditandai

			sprintf(cmd,"mv %s%s",from,to); //pindah filenya

			system (cmd);
			return -errno;

			res = pread(fd, buf, size, offset);
			if(res==-1)
				res = -errno;

			close(fd);
		}

		return res;
	}
}

static struct fuse_operations xmp_oper = 
{
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
};

int main(int argc, char *argv[])
{
               umask(0);
               return fuse_main(argc, argv, &xmp_oper, NULL);
}
