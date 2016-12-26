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

typedef struct erConf
{
	char_t app[APP_MAX];
	char_t arg[ARG_MAX];
	char_t from[USER_MAX];
	char_t to[USER_MAX];
}erConf_s;

typedef enum {PS_RUN, PS_EXITED, PS_ONSIGNAL, PS_STOP, PS_CONTINUE} processState_e;

/*** conf.c ***/
erConf_s* conf_find(char_t* app, char_t* from);
err_t conf_validate_regex(char_t* arg, char_t* reg);
erConf_s* conf_find_ex(char_t* app, char_t* arg, char_t* from);

/*** proc.c ***/
double bch_get();
processState_e process_state(int_t* ex, pid_t pid, bool_t async);
pid_t system_safe(char_t* cmd, char_t* redirectTo, uid_t switchUser);
err_t check_timelimit(pid_t pid, double timeLimit);










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

__always_inline __private uid_t get_real_uid(void){ return getuid(); }
__always_inline __private uid_t get_effective_uid(void) { return geteuid(); }
__always_inline __private err_t set_real_uid(uid_t uid){ return setuid(uid); }
__always_inline __private err_t set_effective_uid(uid_t uid){ return seteuid(uid); }

/*
 * execRock.conf
 * command|regexargument@current_user>to_user
 */


#endif

