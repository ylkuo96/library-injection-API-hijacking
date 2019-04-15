#include <stdio.h>
#include <iostream>
#include <dlfcn.h>
#include <unistd.h> // readlink
#include <string.h> // memset
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <map>
#include <stdarg.h>

using namespace std;

//map<DIR *, const char *> dirName;
/*(v) closedir opendir readdir chdir
 *(v) creat open read write 
 *(v) dup dup2 close lstat stat pwrite 
 *(v) fread fwrite fgetc fgets
 *(v) readlink link unlink symlink
 *(v) mkdir rmdir
 *(v) chown chmod remove rename 
 *(v) fprintf fscanf 
 *( ) fopen fclose 
 *(v) fputs_unlocked fwrite_unlocked fflush fflush_unlocked
 */

static int (*libc_open) (const char *, int) = NULL;
static int (*libc_close) (int) = NULL;
static ssize_t (*libc_read) (int, void *, size_t) = NULL;
static ssize_t (*libc_write) (int, const void *, size_t) = NULL;
static int (*libc_dup) (int) = NULL;
static int (*libc_dup2) (int, int) = NULL;
static int (*libc_creat) (const char *, mode_t) = NULL;
static int (*libc_xstat) (int, const char *, struct stat *) = NULL;
static int (*libc_lxstat) (int, const char *, struct stat *) = NULL;
static ssize_t (*libc_pwrite) (int, const void *, size_t, off_t) = NULL;
static FILE * (*libc_fopen) (const char *, const char *) = NULL;
static int (*libc_fclose) (FILE *) = NULL;
static size_t (*libc_fread) (void *, size_t, size_t, FILE *) = NULL;
static size_t (*libc_fwrite) (const void *, size_t, size_t, FILE *) = NULL;
static int (*libc_fgetc) (FILE *) = NULL;
static char * (*libc_fgets) (char *, int, FILE *) = NULL;
static int (*libc_fscanf) (FILE *, const char *, ...) = NULL;
static int (*libc_fprintf) (FILE *, const char *, ...) = NULL;
static DIR * (*libc_opendir) (const char *) = NULL;
static int (*libc_closedir) (DIR *) = NULL;
static struct dirent * (*libc_readdir) (DIR *) = NULL;
static int (*libc_chdir) (const char *) = NULL;
static ssize_t (*libc_readlink) (const char *, char *, size_t) = NULL;
static int (*libc_link) (const char *, const char *) = NULL;
static int (*libc_unlink) (const char *) = NULL;
static int (*libc_symlink) (const char *, const char *) = NULL;
static int (*libc_mkdir) (const char *, mode_t) = NULL;
static int (*libc_rmdir) (const char *) = NULL;
static int (*libc_chown) (const char *, uid_t, gid_t) = NULL;
static int (*libc_chmod) (const char *, mode_t) = NULL;
static int (*libc_remove) (const char *) = NULL;
static int (*libc_rename) (const char *, const char *) = NULL;
static size_t (*libc_fwrite_unlocked) (const void *, size_t, size_t, FILE *) = NULL;
static int (*libc_fputs_unlocked) (const char *, FILE *) = NULL;
static int (*libc_fflush) (FILE *) = NULL;
static int (*libc_fflush_unlocked) (FILE *) = NULL;
void *handle = NULL;

__attribute__((constructor)) int start() {
    char *filename = getenv("MONITOR_OUTPUT");
    if(filename != NULL){
        freopen(filename, "w", stderr);
    }
    
    handle = dlopen("libc.so.6", RTLD_LAZY);
    if(handle == NULL){
        printf("error\n");
        return -1;
    }

    if(libc_open == NULL){
        libc_open = (int (*) (const char *, int))dlsym(handle, "open");
    }

    if(libc_close == NULL){
        libc_close = (int (*) (int))dlsym(handle, "close");
    }

    if(libc_read == NULL){
        libc_read = (ssize_t (*) (int, void *, size_t))dlsym(handle, "read");
    }
    
    if(libc_write == NULL){
        libc_write = (ssize_t (*) (int, const void *, size_t))dlsym(handle, "write");
    }
    
    if(libc_dup == NULL){
        libc_dup = (int (*) (int))dlsym(handle, "dup");
    }
    
    if(libc_dup2 == NULL){
        libc_dup2 = (int (*) (int, int))dlsym(handle, "dup2");
    }

    if(libc_creat == NULL){
        libc_creat = (int (*) (const char *, mode_t))dlsym(handle, "creat");
    }
    
    if(libc_xstat == NULL){
        libc_xstat = (int (*) (int, const char *, struct stat *))dlsym(handle, "__xstat");
    }

    if(libc_lxstat == NULL){
        libc_lxstat = (int (*) (int, const char *, struct stat *))dlsym(handle, "__lxstat");
    }

    if(libc_pwrite == NULL){
        libc_pwrite = (ssize_t (*) (int, const void *, size_t, off_t))dlsym(handle, "pwrite");
    }
    
    if(libc_fopen == NULL){
        libc_fopen = (FILE * (*) (const char *, const char *))dlsym(handle, "fopen");
    }

    if(libc_fclose == NULL){
        libc_fclose = (int (*) (FILE *))dlsym(handle, "fclose");
    }
    
    if(libc_fread == NULL){
        libc_fread = (size_t (*) (void *, size_t, size_t, FILE *))dlsym(handle, "fread");
    }
    
    if(libc_fwrite == NULL){
        libc_fwrite = (size_t (*) (const void *, size_t, size_t, FILE *))dlsym(handle, "fwrite");
    }
    
    if(libc_fgetc == NULL){
        libc_fgetc = (int (*) (FILE *))dlsym(handle, "fgetc");
    }

    if(libc_fgets == NULL){
        libc_fgets = (char * (*) (char *, int, FILE *))dlsym(handle, "fgets");
    }
    
    if(libc_fscanf == NULL){
        libc_fscanf = (int (*) (FILE *, const char *, ...))dlsym(handle, "vfscanf");
    }
    
    if(libc_fprintf == NULL){
        libc_fprintf = (int (*) (FILE *, const char *, ...))dlsym(handle, "vfprintf");
    }
    
    if(libc_closedir == NULL){
        libc_closedir = (int (*) (DIR *))dlsym(handle, "closedir");
    }

    if(libc_opendir == NULL){
        libc_opendir = (DIR * (*) (const char *))dlsym(handle, "opendir");
    }

    if(libc_readdir == NULL){
        libc_readdir = (struct dirent * (*) (DIR *))dlsym(handle, "readdir");
    }

    if(libc_chdir == NULL){
        libc_chdir = (int (*) (const char *))dlsym(handle, "chdir");
    }

    if(libc_readlink == NULL){
        libc_readlink = (ssize_t (*) (const char *, char *, size_t))dlsym(handle, "readlink");
    }

    if(libc_link == NULL){
        libc_link = (int (*) (const char *, const char *))dlsym(handle, "link");
    }
    
    if(libc_unlink == NULL){
        libc_unlink = (int (*) (const char *))dlsym(handle, "unlink");
    }

    if(libc_symlink == NULL){
        libc_symlink = (int (*) (const char *, const char *))dlsym(handle, "symlink");
    }

    if(libc_mkdir == NULL){
        libc_mkdir = (int (*) (const char *, mode_t))dlsym(handle, "mkdir");
    }

    if(libc_rmdir == NULL){
        libc_rmdir = (int (*) (const char *))dlsym(handle, "rmdir");
    }

    if(libc_chown == NULL){
        libc_chown = (int (*) (const char *, uid_t, gid_t))dlsym(handle, "chown");
    }

    if(libc_chmod == NULL){
        libc_chmod = (int (*) (const char *, mode_t))dlsym(handle, "chmod");
    }

    if(libc_remove == NULL){
        libc_remove = (int (*) (const char *))dlsym(handle, "remove");
    }
    
    if(libc_rename == NULL){
        libc_rename = (int (*) (const char *, const char *))dlsym(handle, "rename");
    }

    if(libc_fwrite_unlocked == NULL){
        libc_fwrite_unlocked = (size_t (*) (const void *, size_t, size_t, FILE *))dlsym(handle, "fwrite_unlocked");
    }

    if(libc_fputs_unlocked == NULL){
        libc_fputs_unlocked = (int (*) (const char *, FILE *))dlsym(handle, "fputs_unlocked");
    }
    
    if(libc_fflush == NULL){
        libc_fflush = (int (*) (FILE *))dlsym(handle, "fflush");
    }
    
    if(libc_fflush_unlocked == NULL){
        libc_fflush_unlocked = (int (*) (FILE *))dlsym(handle, "fflush_unlocked");
    }
}

__attribute__((destructor)) void finish() {
    //printf("bbb\n");
}

void Print(FILE *stream, const char *format, ...){
    /*
     * to prevent when implementing fprintf then get into infinite loop
     */
	int (*P) (FILE *, const char *, ...) = NULL;	
	if(handle != NULL && P == NULL){
		P = (int (*) (FILE *, const char *, ...))dlsym(handle, "vfprintf");
	}
	
	va_list ap;
	va_start(ap, format);
	P(stream, format, ap);
}

string getfilename(int fd){
    if(fd == STDIN_FILENO){
        return "<STDIN>";
    }
    else if(fd == STDOUT_FILENO){
        return "<STDOUT>";
    }
    else if(fd == STDERR_FILENO){
        return "<STDERR>";
    }

    string link_path = "/proc/self/fd/" + to_string(fd);
    char target_path[256];
    memset(target_path, 0, sizeof(target_path));
    if(libc_readlink(link_path.c_str(), target_path, sizeof(target_path)) > 0){
        return (string)target_path;
    }
    else{ // -1 == error
        return "NULL";
    }
}

extern "C" int open(const char *pathname, int flags, ...){
    int fd = libc_open(pathname, flags);
    string target_path = getfilename(fd);
    stringstream ss;
    ss << hex << flags;
    string res(ss.str());
    Print(stderr, "# open(\"%s\", 0x%s) = %d\n", target_path.c_str(), res.c_str(), fd); 

    return fd;
}

extern "C" int close(int fd){
    string target_path = getfilename(fd);
    int res = libc_close(fd);
    Print(stderr, "# close(\"%s\") = %d\n", target_path.c_str(), res);

    return res;
}

extern "C" ssize_t read(int fd, void *buf, size_t count){
    string target_path = getfilename(fd);
    ssize_t res = libc_read(fd, buf, count);
    Print(stderr, "# read(\"%s\", %p, %zu) = %zd\n", target_path.c_str(), &buf, count, res);

    return res;
}

extern "C" ssize_t write(int fd, const void *buf, size_t count){
    string target_path = getfilename(fd);
    ssize_t res = libc_write(fd, buf, count);
    Print(stderr, "# write(\"%s\", %p, %zu) = %zd\n", target_path.c_str(), &buf, count, res);
    
    return res;
}

extern "C" int dup(int oldfd){
    int res = libc_dup(oldfd);
    Print(stderr, "# dup(%d) = %d\n", oldfd, res);

    return res;    
}

extern "C" int dup2(int oldfd, int newfd){
    int res = libc_dup2(oldfd, newfd);
    Print(stderr, "# dup(%d, %d) = %d\n", oldfd, newfd, res);

    return res;    
}

extern "C" int creat(const char *pathname, mode_t mode){
    int fd = libc_creat(pathname, mode);
    string target_path = getfilename(fd);
    Print(stderr, "# creat(\"%s\", %o) = %d\n", target_path.c_str(), mode, fd);

    return fd;    
}

extern "C" int __xstat(int __ver, const char *pathname, struct stat *statbuf){
    int res = libc_xstat(__ver, pathname, statbuf);
    mode_t Mode = statbuf->st_mode;
    off_t Size = statbuf->st_size;
    Print(stderr, "# stat(\"%s\", %p {mode=%o, size=%jd}) = %d\n", pathname, &statbuf, Mode, Size, res);
    
    return res;
}

extern "C" int __lxstat(int __ver, const char *pathname, struct stat *statbuf){
    int res = libc_lxstat(__ver, pathname, statbuf);
    mode_t Mode = statbuf->st_mode;
    off_t Size = statbuf->st_size;
    Print(stderr, "# lstat(\"%s\", %p {mode=%o, size=%jd}) = %d\n", pathname, &statbuf, Mode, Size, res);
    
    return res;
}

extern "C" ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset){
    ssize_t res = libc_pwrite(fd, buf, count, offset);
    string target_path = getfilename(fd);
    Print(stderr, "# pwrite(\"%s\", %p, %zu, %jd) = %zd\n", target_path.c_str(), &buf, count, offset, res);

    return res;
}

FILE *fopen(const char *pathname, const char *mode){
    // defined a pointer named `res` pointed to a structure `FILE` 
    FILE *res = libc_fopen(pathname, mode);
    // define a `FILE` structure named `f` and assigned `the value of` res
    //FILE f = *res;      // &f
    // define a pointer named `ptr` pointed to a structure `FILE` and assigned `the address of` `f`
    //FILE *ptr = &f;     // %p
    Print(stderr, "# fopen(\"%s\", \"%s\") = %p\n", pathname, mode, res);

    return res;
}

extern "C" int fclose(FILE *stream){
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    int res = libc_fclose(stream);
    Print(stderr, "# fclose(\"%s\") = %d\n", target_path.c_str(), res);
    
    return res;
}

extern "C" size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    size_t res = libc_fread(ptr, size, nmemb, stream);
    Print(stderr, "# fread(%p, %zu, %zu, \"%s\") = %zu\n", &ptr, size, nmemb, target_path.c_str(), res);
    
    return res;
}

extern "C" size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    size_t res = libc_fwrite(ptr, size, nmemb, stream);
    Print(stderr, "# fwrite(%p, %zu, %zu, \"%s\") = %zu\n", &ptr, size, nmemb, target_path.c_str(), res);

    return res;
}

extern "C" int fgetc(FILE *stream){
    int res = libc_fgetc(stream);
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    Print(stderr, "# fgetc(\"%s\") = %d\n", target_path.c_str(), res);

    return res;
}

extern "C" char *fgets(char *s, int size, FILE *stream){
    char *res = libc_fgets(s, size, stream);
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    Print(stderr, "# fgets(\"%s\", %d, \"%s\") = %s\n", s, size, target_path.c_str(), res);

    return res;
}

extern "C" int __isoc99_fscanf(FILE *stream, const char *format, ...){
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    
    va_list ap;
    va_start(ap, format);
    int res = libc_fscanf(stream, format, ap);
    Print(stderr, "# fscanf(\"%s\", \"%s\", ...) = %d\n", target_path.c_str(), format, res);
    va_end(ap);

    return res;
}

extern "C" int fprintf(FILE *stream, const char *format, ...){
    int fd = stream->_fileno;
    string target_path = getfilename(fd);

    va_list ap;
    va_start(ap, format);
    int res = libc_fprintf(stream, format, ap);
    Print(stderr, "# fprintf(\"%s\", \"%s\", ...) = %d\n", target_path.c_str(), format, res);
    va_end(ap);

    return res;
}

extern "C" DIR * opendir(const char * name){
    DIR * res = libc_opendir(name);
    int fd = dirfd(res);
    string target_path = getfilename(fd);
    Print(stderr, "# opendir(\"%s\") = %p\n", target_path.c_str(), &res);
    //dirName.insert(pair<DIR *, const char *>(res, name));

    return res;
}

extern "C" int closedir(DIR *dirp){
    int fd = dirfd(dirp);
    string target_path = getfilename(fd);
    int res = libc_closedir(dirp);
    Print(stderr, "# closedir(\"%s\") = %d\n", target_path.c_str(), res);
    //dirName.erase(dirp);

    return res;
}

extern "C" struct dirent *readdir(DIR *dirp){
    struct dirent *res = libc_readdir(dirp);
    int fd = dirfd(dirp);
    if(res == NULL){
        Print(stderr, "# readdir(\"%s\") = NULL\n", getfilename(fd).c_str());
    }
    else{
        Print(stderr, "# readdir(\"%s\") = %s\n", getfilename(fd).c_str(), res->d_name);
    }

    return res;
}

extern "C" int chdir(const char *path){
    int res = libc_chdir(path);
    Print(stderr, "# chdir(\"%s\") = %d\n", path, res);

    return res;
}

extern "C" ssize_t readlink(const char *pathname, char *buf, size_t bufsize){
    ssize_t res = libc_readlink(pathname, buf, bufsize);
    Print(stderr, "# readlink(\"%s\", %p, %zu) = %zd\n", pathname, &buf, bufsize, res);

    return res;
}

extern "C" int link(const char *oldpath, const char *newpath){
    int res = libc_link(oldpath, newpath);
    Print(stderr, "# link(\"%s\", \"%s\") = %d\n", oldpath, newpath, res);

    return res;
}

extern "C" int unlink(const char *pathname){
    int res = libc_unlink(pathname);
    Print(stderr, "# unlink(\"%s\") = %d\n", pathname, res);

    return res;
}

extern "C" int symlink(const char *target, const char *linkpath){
    int res = libc_symlink(target, linkpath);
    Print(stderr, "# symlink(\"%s\", \"%s\") = %d\n", target, linkpath, res);

    return res;
}

extern "C" int mkdir(const char *pathname, mode_t mode){
    int res = libc_mkdir(pathname, mode);
    Print(stderr, "# mkdir(\"%s\", 0%o) = %d\n", pathname, mode, res);

    return res;
}

extern "C"  int rmdir(const char *pathname){
    int res = libc_rmdir(pathname);
    Print(stderr, "# rmdir(\"%s\") = %d\n", pathname, res);

    return res;
}

extern "C" int chown(const char *pathname, uid_t owner, gid_t group){
    int res = libc_chown(pathname, owner, group);
    Print(stderr, "# chown(\"%s\", %d, %d) = %d\n", pathname, owner, group, res);

    return res;
}

extern "C" int chmod(const char *pathname, mode_t mode){
    int res = libc_chmod(pathname, mode);
    Print(stderr, "# chmod(\"%s\", 0%o) = %d\n", pathname, mode, res);

    return res;
}

extern "C" int remove(const char *pathname){
    int res = libc_remove(pathname);
    Print(stderr, "# remove(\"%s\") = %d\n", pathname, res);

    return res;

}

extern "C" int rename(const char *oldpath, const char *newpath){
    int res = libc_rename(oldpath, newpath);
    Print(stderr, "# rename(\"%s\", \"%s\") = %d\n", oldpath, newpath, res);

    return res;
}

extern "C" size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream){
    size_t res = libc_fwrite_unlocked(ptr, size, n, stream);
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    Print(stderr, "# fwrite_unlocked(%p, %zu, %zu, \"%s\") = %zu\n", &ptr, size, n, target_path.c_str(), res);

    return res;
}

extern "C" int fputs_unlocked(const char *s, FILE *stream){
    int res = libc_fputs_unlocked(s, stream);
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    Print(stderr, "# fputs_unlocked(%p, \"%s\") = %d\n", &s, target_path.c_str(), res);
    
    return res;
}

extern "C" int fflush(FILE *stream){
    int res = libc_fflush(stream);
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    Print(stderr, "# fflush(\"%s\") = %d\n", target_path.c_str(), res);

    return res;
}

extern "C" int fflush_unlocked(FILE *stream){
    int res = libc_fflush_unlocked(stream);
    int fd = stream->_fileno;
    string target_path = getfilename(fd);
    Print(stderr, "# fflush_unlocked(\"%s\") = %d\n", target_path.c_str(), res);

    return res;
}
