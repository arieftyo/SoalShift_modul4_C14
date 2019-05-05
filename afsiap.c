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
        struct stat tmp;
        struct stat st;
        strcpy(temp, de->d_name);
        stat(file,&tmp);
        struct passwd *name = getpwuid(tmp.st_uid);
        struct group *grup = getgrgid(tmp.st_gid);

        if( (strcmp(name->pw_name,"chipset") == 0 || strcmp(name->pw_name,"ic_controller") == 0) 
            && strcmp(grup->gr_name,"rusak")==0 
            && ((tmp.st_mode & S_IRUSR) == 0 || (tmp.st_mode & S_IRGRP) == 0 || (tmp.st_mode & S_IROTH) == 0) )
        {
            printf("%s\n",file);
            char root[1000];
            strcpy(root,dirpath);
            char note[10000] = "/filemiris.txt";
            enc(note);
            strcat(root,note);
            FILE * fp;
            fp = fopen (root, "a+");
            char t[1000];
            time_t now = time(NULL);
            strftime(t, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
            char buffer[1000];
            sprintf(buffer,"%s%s-%d-%d-%s",path,temp,name->pw_uid,grup->gr_gid,t);
            fprintf(fp,"%s\n",buffer);
            remove(file);
            fclose(fp);
            chown(root,1000,1000);
        }
        else{
            memset(&st, 0, sizeof(st));
            st.st_ino = de->d_ino;
            st.st_mode = de->d_type << 12;
            strcpy(temp, de->d_name);
            dekripsi(temp);
            int res = (filler(buf, temp, &st, 0));
            if (res != 0)
            break;
        }
        
    }

    closedir(dp);
    return 0;
}

void *gabungVideo(void *param){
    DIR *dp;
    struct dirent *de;
    char *namadepan = (char *) param;
    dp = opendir(dirpath);
    char newVideo[1000];
    char encVideo[1000];
    sprintf(encVideo, "%s", namadepan);
    enc(encVideo);
    sprintf(newVideo, "%s/%s", videosPath, encVideo);
    creat(newVideo, 0777);
    printf("gabung video %s\n", newVideo);
    while((de = readdir(dp)) != NULL){
        char temp[1000];
        sprintf(temp, "%s", de->d_name);
        dec(temp);
        printf("part file %s compare %s \n", temp, namadepan);
        if (strstr(temp, namadepan)){
            FILE *fp = fopen(newVideo, "a+");
            printf("part file status ok");
            char namafile[1000];
            sprintf(namafile, "%s/%s", dirpath, de->d_name);
            printf(" opening %s file and begin to copying ", namafile);
            FILE *fp2 = fopen(namafile, "r");
            
            size_t n, m;
            unsigned char buff[8192];
            do {
                    n = fread(buff, 1, sizeof buff, fp2);
                    if (n) m = fwrite(buff, 1, n, fp);
                    else   m = 0;
            } while ((n > 0) && (n == m));
            
            fclose(fp2);
            fclose(fp);
            printf("\nsucces copying %s\n", temp);
        }
    }
    printf("gabung video selesai\n");
}

static void xmp_init(struct fuse_conn_info *conn){
    char videos[1000] = "Videos";
    enc(videos);
    enc(youtuber);
    enc(filemiris);
    enc(iz1);
    sprintf(videosPath, "%s/%s", dirpath, videos);
    mkdir(videosPath, 0777);
    counter = 0;
    DIR* dp;
    struct dirent *de;
    dp = opendir(dirpath);
    while((de = readdir(dp)) != NULL){
        char temp[1000];
        sprintf(temp, "%s", de->d_name);
        dec(temp);
        printf("init temp %s\n", temp);
        if (isVideo(temp)){
            char namadepan[1000];
            int len = strlen(temp);
            sprintf(namadepan, "%s", temp);
            namadepan[len - 4] = '\0';
            pthread_create(&(buruhVideo[counter++]), NULL, &gabungVideo, (void *)namadepan);
        }
    }
    printf("%d\n", counter);
    return;
}

int isVideo(const char *path){
    char temp[1000];
    sprintf(temp, "%s", path);
    int len = strlen(temp);
    char *ext;
    ext = &temp[len - 4];
    if (strcmp(ext, ".000") == 0){
        return 1;
    }
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


static int xmp_unlink(const char *path)
{
    char fpath[1000], temp[1000];
    strcpy(temp, path);
    enc(temp);

    if(strcmp(temp,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,temp);

    int res, status;

    if(access(fpath, F_OK)<0)           
        return 0;

    char soal[1000], soal2[1000], waktu[100], file_zip[1000], fileasli[1000], ext[1000],
            namafile[1000];
    
    char fd_recyclebin[100]="/home/awin/revisimod4/oO.k.EOX[)";
    mkdir(fd_recyclebin, 0777);

    int posisi_ext = posisi(path, '.');
    
    if (posisi_ext==0)
        posisi_ext = strlen(path);
    else{
        strcpy(ext, path+posisi_ext);
    }
    strcpy(fileasli, path);
    enc(fileasli);  

    int posisi_slash = posisi(path, '/');
    int dot = posisi_ext-(posisi_slash+1);
    strncpy(namafile, path+1+posisi_slash, dot);
    namafile[dot] = '\0';

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(waktu, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    sprintf(file_zip, "%s_deleted_%s", namafile, waktu);
    char zip_temp[1000], file_zip_temp[1000]; 
    char zip[100]="`S[u";
    enc(file_zip);
    enc(namafile);
    
    sprintf(file_zip_temp, "%s.zip", file_zip);
    sprintf(zip_temp, "%s%s", file_zip, zip);

    char backup[100] = "XB.Jhu"; char recycle[100]= "oO.k.EOX[)";
    sprintf(soal, "cd %s && zip '%s/%s' '%s' '%s/%s'* && rm -f '%s/%s'*", dirpath, recycle, file_zip, fileasli, backup, namafile, backup, namafile);
    sprintf(soal2, "%s && cp '%s/%s' '%s/%s' && rm -f '%s/%s'", soal, recycle, file_zip_temp, recycle, zip_temp, recycle, file_zip_temp); 
    if (fork()==0)
        execlp("bash","bash", "-c", soal2, NULL);

        while((wait(&status))>0);


    res = unlink(fpath);
    
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
             off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1000];
    char name[1000];
    char temp[1000];
    sprintf(name,"%s",path);
    enc(name);
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,name);
    
    (void) fi;
    fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    
    strcpy(temp,path); enc(temp);
    sprintf(name, "%s/%s", dirpath,temp);
    
        if(access(name, R_OK)<0)                
        return res;
    
    char fd_backup[1000]="/home/awin/revisimod4/XB.Jhu";
    mkdir(fd_backup, 0777);
    
    char namafile[1000], ext[100], waktu[1000], filejadi[1000];
    
    int posisi_ext = posisi(path,'.');
    
    if (posisi_ext==0) {
        posisi_ext = strlen(path);
        ext[0] = '\0';
    }
    else{
        strcpy(ext, path+posisi_ext);
    }
    int posisi_slash = posisi(path, '/');
    int dot = posisi_ext-(posisi_slash+1);
    strncpy(namafile, path+1+posisi_slash, dot);
    namafile[dot] = '\0';
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(waktu, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    sprintf(filejadi, "%s_%s", namafile, waktu); 
    strcat(filejadi, ext);  
    enc(filejadi);
    sprintf(name, "%s%s", dirpath, temp);
    
    char final[1000];
    sprintf(final, "%s/%s", fd_backup, filejadi);

    int status;
    if (fork()==0)
        execlp("cp","cp", name, final, NULL);

    while((wait(&status))>0);
    
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
    .unlink     = xmp_unlink,
    .write      = xmp_write,
    .init       = xmp_init,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
