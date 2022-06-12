/*
DSO 2022
Grupo C
Proyecto hecho por: Álvaro, Antonio y Juan
Team pichasgordas

Versión 0.5 ❤️😒😊😭😩😍😔😁💕💕💕💕😊😊😊😊

 */

#include "params.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include "log.h"

// Report errors to logfile and give -errno to caller
static int mi_error(char *str)
{
    int ret = -errno;
    
    log_msg("    ERROR %s: %s\n", str, strerror(errno));
    
    return ret;
}

static void mi_fullpath(char fpath[PATH_MAX], const char *path)
{
    strcpy(fpath, MI_DATA->rootdir);
    strncat(fpath, path, PATH_MAX); 
    log_msg("    mi_fullpath:  rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
	    MI_DATA->rootdir, path, fpath);
}

int mi_getattr(const char *path, struct stat *statbuf)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nmi_getattr(path=\"%s\", statbuf=0x%08x)\n",
	  path, statbuf);
    mi_fullpath(fpath, path);
    
    retstat = lstat(fpath, statbuf);
    if (retstat != 0)
	retstat = mi_error("mi_getattr lstat");
    
    log_stat(statbuf);
    
    return retstat;
}


int mi_mknod(const char *path, mode_t mode, dev_t dev)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nmi_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n",
	  path, mode, dev);
    mi_fullpath(fpath, path);

    if (S_ISREG(mode)) {
        retstat = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
	if (retstat < 0)
	    retstat = mi_error("mi_mknod open");
        else {
            retstat = close(retstat);
	    if (retstat < 0)
		retstat = mi_error("mi_mknod close");
	}
    } else
	if (S_ISFIFO(mode)) {
	    retstat = mkfifo(fpath, mode);
	    if (retstat < 0)
		retstat = mi_error("mi_mknod mkfifo");
	} else {
	    retstat = mknod(fpath, mode, dev);
	    if (retstat < 0)
		retstat = mi_error("mi_mknod mknod");
	}
    
    return retstat;
}

int mi_utime(const char *path, struct utimbuf *ubuf)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nmi_utime(path=\"%s\", ubuf=0x%08x)\n",
	    path, ubuf);
    mi_fullpath(fpath, path);
    
    retstat = utime(fpath, ubuf);
    if (retstat < 0)
	retstat = mi_error("mi_utime utime");
    
    return retstat;
}

int mi_open(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];

    
    log_msg("\nmi_open(path\"%s\", fi=0x%08x)\n",
	    path, fi);
    mi_fullpath(fpath, path);

    
    fd = open(fpath, fi->flags);
    if (fd < 0)
      {
	log_msg ("\nError: %d (%s)\n", errno, strerror (errno));
	  retstat = mi_error("mi_open open");
      }
    fi->fh = fd;
    log_fi(fi);
    
    return retstat;
}
int mi_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nmi_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
	    path, buf, size, offset, fi);
    log_fi(fi);
    
    retstat = pread(fi->fh, buf, size, offset);
    if (retstat < 0)
	retstat = mi_error("mi_read read");
    
    return retstat;
}

int mi_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nmi_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
	    path, buf, size, offset, fi
	    );
    log_fi(fi);
	
    retstat = pwrite(fi->fh, buf, size, offset);
    if (retstat < 0)
	retstat = mi_error("mi_write pwrite");
    
    return retstat;
}
int mi_opendir(const char *path, struct fuse_file_info *fi)
{
    DIR *dp;
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nmi_opendir(path=\"%s\", fi=0x%08x)\n",
	  path, fi);
    mi_fullpath(fpath, path);
    
    dp = opendir(fpath);
    if (dp == NULL)
	retstat = mi_error("mi_opendir opendir");
    
    fi->fh = (intptr_t) dp;
    
    log_fi(fi);
    
    return retstat;
}
int mi_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
	       struct fuse_file_info *fi)
{
    int retstat = 0;
    DIR *dp;
    struct dirent *de;
    
    log_msg("\nmi_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, fi=0x%08x)\n",
	    path, buf, filler, offset, fi);
    dp = (DIR *) (uintptr_t) fi->fh;
    de = readdir(dp);
    if (de == 0) {
	retstat = mi_error("mi_readdir readdir");
	return retstat;
    }
    do {
	log_msg("calling filler with name %s\n", de->d_name);
	if (filler(buf, de->d_name, NULL, 0) != 0) {
	    log_msg("    ERROR mi_readdir filler:  buffer full");
	    return -ENOMEM;
	}
    } while ((de = readdir(dp)) != NULL);
    log_fi(fi);
    return retstat;
}

void *mi_init(struct fuse_conn_info *conn)
{
    
    log_msg("\nmi_init()\n");
    
    return MI_DATA;
}
int mi_access(const char *path, int mask)
{
    int retstat = 0;
    char fpath[PATH_MAX];
   
    log_msg("\nmi_access(path=\"%s\", mask=0%o)\n",
	    path, mask);
    mi_fullpath(fpath, path);
    
    retstat = access(fpath, mask);
    
    if (retstat < 0)
	retstat = mi_error("mi_access access");
    
    return retstat;
}

int mi_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    int fd;
    
    log_msg("\nmi_create(path=\"%s\", mode=0%03o, fi=0x%08x)\n",
	    path, mode, fi);
    mi_fullpath(fpath, path);
    
    fd = creat(fpath, mode);
    if (fd < 0)
	retstat = mi_error("mi_create creat");
    
    fi->fh = fd;
    
    log_fi(fi);
    
    return retstat;
}


int mi_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nmi_fgetattr(path=\"%s\", statbuf=0x%08x, fi=0x%08x)\n",
	    path, statbuf, fi);
    log_fi(fi);
    
    retstat = fstat(fi->fh, statbuf);
    if (retstat < 0)
	retstat = mi_error("mi_fgetattr fstat");
    
    log_stat(statbuf);
    
    return retstat;
}

struct fuse_operations mi_oper = {
  .getattr = mi_getattr,
  .getdir = NULL,
  .mknod = mi_mknod,
  .utime = mi_utime,
  .open = mi_open,
  .read = mi_read,
  .write = mi_write,
  .opendir = mi_opendir,
  .readdir = mi_readdir,
  .init = mi_init,
  .access = mi_access,
  .create = mi_create,
  .fgetattr = mi_fgetattr
};

void mi_usage()
{
    fprintf(stderr, "uso:  anal [FUSE y  opciones de montaje] rootDir punto de montaje\n");
    abort();
}
int main(int argc, char *argv[])
{
    int fuse_stat;
    struct mi_state *mi_data;

    if ((getuid() == 0) || (geteuid() == 0)) {
	fprintf(stderr, "Running anal as root opens unnacceptable security holes\n");
	return 1;
    }
    
    if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-'))
	mi_usage();

    mi_data = malloc(sizeof(struct mi_state));
    if (mi_data == NULL) {
	perror("main calloc");
	abort();
    }

    mi_data->rootdir = realpath(argv[argc-2], NULL);
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
    mi_data->logfile = log_open();
    fprintf(stderr, "about to call fuse_main\n");
    fuse_stat = fuse_main(argc, argv, mi_oper, mi_data);
    fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
    return fuse_stat;
}