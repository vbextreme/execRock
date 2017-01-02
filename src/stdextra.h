/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#ifndef _STDEXTRA_H_INCLUDE_
#define _STDEXTRA_H_INCLUDE_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio_ext.h>
#include <ctype.h>
#include <memory.h>
#include <time.h>
#include <utime.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/input.h>

#define HUMAN_VISION 300

typedef enum {FALSE, TRUE} bool_t;
typedef char char_t;
typedef int int_t;
typedef unsigned int uint_t;
typedef int_t err_t;
typedef long long_t;
typedef unsigned long ulong_t;
typedef uint_t utf8_t;
typedef FILE file_h;
typedef DIR dir_h;
typedef struct passwd passwd_s;
typedef struct group group_s;

typedef struct pipes
{
	int in;
	int out;
}pipe_s;

/*
  struct passwd {
    char   *pw_name;       // username
    char   *pw_passwd;     // user password
    uid_t   pw_uid;        // user ID
    gid_t   pw_gid;        // group ID
    char   *pw_gecos;      // user information
    char   *pw_dir;        // home directory
    char   *pw_shell;      // shell program
	};
*/

/*
 struct group {
    char   *gr_name;       // group name 
    char   *gr_passwd;     // group password 
    gid_t   gr_gid;        // group ID 
    char  **gr_mem;        // group members 
};
*/

#define strop(A,OP,B) (strcmp(A,B) OP 0)
#define strnop(A,OP,BN) (strncmp(A, BN, strlen(BN)) OP 0)

#define __private static

#define __packed __attribute__((packed))

#define NEW(T) (T*)malloc(sizeof(T))
#define NEWS(T,S) (T*)malloc(sizeof(T) * (S))


#ifdef DEBUG_ENABLE
	#define dbg(FORMAT, arg...) do{fprintf(stderr, "%s: %s(): " FORMAT "\n", __FILE__, __FUNCTION__, ## arg); fflush(stderr);}while(0);
#else
	#define dbg(FORMAT, arg...) do{}while(0)
#endif

#if ASSERT_ENABLE
	#define iassert(C) do{ if ( !(C) ){fprintf(stderr,"assertion fail %s[%u]: %s\n", __FILE__, __LINE__, #C); exit(0);}}while(0)
#else
	#define iassert(C) do{}while(0)
#endif

#endif
