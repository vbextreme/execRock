/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#ifndef _CONF_H_INCLUDE_
#define _CONF_H_INCLUDE_

#include "stdextra.h"
#include "terminale.h"
#include "optex.h"
#include "rex.h"

#define PROCESS_ERROR -1
#define PROCESS_CHILD 0
#define DEFAULT_TIME_RELAX 300
#define CONFIG_FILE "/etc/execRock.conf"

#define APP_MAX 128
#define USER_MAX 128
#define ARG_MAX 1024
#define CMD_MAX 2048
#define CHRO_MAX 1024
#define DIR_MAX 1024
#define ARGUMENT_MAX 512

#define SA_MODE_CHR 0x01
#define SA_MODE_TMP 0x02

typedef struct settingApp
{
	uint_t mode;
	uid_t usr;
	gid_t grp;
	char_t chro[CHRO_MAX];
}settingApp_s;


typedef enum {PS_RUN, PS_EXITED, PS_ONSIGNAL, PS_STOP, PS_CONTINUE} processState_e;

/*** conf.c ***/
err_t conf_read_setting(settingApp_s* out, uid_t fromUsr, gid_t fromGrp, char_t* app, char_t** arg, uint_t countArg);

/*** proc.c ***/
double bch_get();
processState_e process_state(int_t* ex, pid_t pid, bool_t async);
pid_t system_safe(char_t* cmd, char_t** arg, char_t* chro, char_t* redirectTo, uid_t switchUser, gid_t switchGroup);
err_t check_timelimit(pid_t pid, double timeLimit);
err_t system_getout(char_t* out, uint_t szout, char_t* cmd, char_t** arg);

/*** file.c ***/
err_t file_exist(char_t* fname);
err_t dir_exists(char_t* d);
char_t* pth_homedir(void);
err_t pth_normalize(char_t* d, char_t* s);
char_t* app_name(char_t* dir);
err_t dir_new(char_t* dir, mode_t pri);
err_t file_copy(char_t* d, char_t* s);
char_t* pth_next(char_t* s);
err_t pth_current(char_t* d);
err_t pth_set(char_t* d);
char_t* str_skip_space(char_t* s);

__always_inline __private err_t get_user_by_uid(uid_t uid, char_t* outUser)
{
	passwd_s* pw = getpwuid(uid);
	if ( 0 == pw ) return -1;
	strcpy(outUser, pw->pw_name);
	return 0;
}

__always_inline __private uid_t get_uid_by_user(char_t* user)
{
	passwd_s* pw = getpwnam(user);
	return pw ? (uid_t)pw->pw_uid : (uid_t)~0;
}

__always_inline __private gid_t get_gid_by_group(char_t* group)
{
	group_s* pw = getgrnam(group);
	return pw ? (gid_t)pw->gr_gid : (gid_t)~0;
}

__always_inline __private uid_t get_real_uid(void){ return getuid(); }
__always_inline __private uid_t get_effective_uid(void) { return geteuid(); }
__always_inline __private err_t set_real_uid(uid_t uid){ return setuid(uid); }
__always_inline __private err_t set_effective_uid(uid_t uid){ return seteuid(uid); }
__always_inline __private uid_t get_real_gid(void){ return getgid(); }
__always_inline __private uid_t get_effective_gid(void) { return getegid(); }
__always_inline __private err_t set_real_gid(uid_t uid){ return setgid(uid); }
__always_inline __private err_t set_effective_gid(uid_t uid){ return setegid(uid); }

__always_inline __private err_t pipe_new(pipe_s* ps)
{
	int p[2];
	if ( pipe(p) ) return -1;
	ps->in = p[0];
	ps->out = p[1];
	return 0;
}



/*
 * *************
 * execRock.conf
 * *************
 * 
 * [application]
 * from=@user || group
 * to=user:group
 * arg="regex" "regex" ...
 * chroot=/dir/chroot || %tmp
 */


#endif

