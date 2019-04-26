#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

pid_t child;
int key_en = 17;
int key_dec = 77;
static const char *dirpath = "/home/ariefp/shift4";
const char cc[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
int length_cc = strlen(cc);

void enkripsi(char * enkrip){
    if(strcmp("..", enkrip) ==0 || strcmp(".", enkrip)==0){
        return;
    }
    for(int i=0;i<strlen(enkrip);i++){
        for(int j=0;j<length_cc;j++){
            if(enkrip[i]==cc[j]){
                enkrip[i]= cc[(j+key_en)%length_cc];
                break;
            }
        }
    }
}

void dekripsi(char * dekrip){
    if(strcmp("..", dekrip) ==0 || strcmp(".", dekrip)==0){
        return;
    }
    for(int i=0;i<strlen(dekrip);i++){
        for(int j=0;j<length_cc;j++){
            if(dekrip[i]==cc[j]){
                dekrip[i]= cc[(j+key_dec)%length_cc];
                break;
            }
        }
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];
    char encrypt[1000];
    sprintf(encrypt, "%s", path);
    enkripsi(encrypt);

    printf("\tAttributes of %s requested\n", encrypt);
    sprintf(fpath, "%s%s", dirpath, encrypt);
    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_atime = time( NULL );
    stbuf->st_mtime = time(NULL);
    if ( strcmp( path, "/" ) == 0 )
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 4;
	}
	else
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 4;
		stbuf->st_size = 1024;
	}
    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    char temp[1000];
    char encrypt[1000];
    sprintf(encrypt, "%s", path);
    enkripsi(encrypt);
    sprintf(fpath, "%s%s", dirpath, encrypt);
    printf("AKSES %s\n", fpath);

    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        strcpy(temp, de->d_name);
        dekripsi(temp);
        int res = (filler(buf, temp, &st, 0));
        if (res != 0)
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    char fpath[1000];
    
    int res = 0;
    int fd = 0;
    printf("Reading file %s\n", fpath);
    (void)fi;

    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    close(fd);
    return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    if (S_ISREG(mode)) {
        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(fpath, mode);
    else
        res = mknod(fpath, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}


static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    sprintf(name,"%s",path);
    if(strlen(name)>9 && strncmp(name,"/YOUTUBER",9)==0){
    	enkripsi(name);
    	mode = 0750;
    	sprintf(fpath, "%s%s",dirpath,name);
    	res = mkdir(fpath, mode);
    }
    else{
    	enkripsi(name);
    	sprintf(fpath, "%s%s",dirpath,name);
    	res = mkdir(fpath, mode);
    }
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = rmdir(fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{

    char fpath[1000];
    char name[1000];
	sprintf(name,"%s",path);
    int length = strlen(name);
    int res;
	if(length != 9 && strncmp(name,"/YOUTUBER",9)==0)
	{
		strcat(name,".iz1");
		mode = 0640;
		enkripsi(name);
		sprintf(fpath, "%s%s",dirpath,name);
    	res = creat(fpath, mode);
	}
	else{
    	enkripsi(name);
    	sprintf(fpath, "%s%s",dirpath,name);
    	res = creat(fpath, mode);
	}
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}


static int xmp_chmod(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    if(strlen(name)>9 && strncmp(name,"/YOUTUBER",9)==0 && strcmp(name+strlen(name)-4,".iz1")==0)
	{
		child=fork();
		if(child==0){
			char *argv[5] = {"zenity", "--error","--text=File ekstensi iz1 tidak boleh diubah permissionnya.","--title=Tidak bisa merubah", NULL};
     		execv("/usr/bin/zenity", argv);
		}
	}
	else{
	 sprintf(fpath, "%s%s",dirpath,name);
	 res = chmod(fpath, mode);
	}
    
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    int res;
    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);
    res = open(fpath, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}


static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    int res;
    struct timeval tv[2];

    char fpath[1000];
    char name[1000];
    sprintf(name,"%s",path);
    enkripsi(name);
    sprintf(fpath, "%s%s",dirpath,name);

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(fpath, tv);
    if (res == -1)
        return -errno;

    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.rmdir		= xmp_rmdir,
	//.rename		= xmp_rename,
	.create 	= xmp_create,
	.chmod		= xmp_chmod,
	//.chown		= xmp_chown,
	//.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
