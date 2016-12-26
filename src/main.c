/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#include "main.h" 

struct argdef myargs[] =	{{ 'c', 'c', "command" , OPT_ARG  , 0, "shell this command" },
							 { 't', 't', "time"    , OPT_ARG  , 0, "max time execution" },
							 { 'r', 'r', "redirect", OPT_ARG  , 0, "redirect to file" },
							 { 'T', 'T', "test"    , OPT_ARG  , 0, "regex to test" },
							 { 'h', 'h', "help"    , OPT_NOARG, 0, "help" },
							 { 0  , 0  , 0         , OPT_NOARG, 0, 0}
							};

__always_inline __private err_t app_extract( char_t* cmd, char_t* outApp, char_t* outArg )
{
	dbg("command:'%s'",cmd);
	
	uint_t size = APP_MAX - 1;
	
	while( size-->0 && (*cmd != ' ' && *cmd != 0 && *cmd != '\n') )
		*outApp++ = *cmd++;	
	*outApp = 0;
	
	if( *cmd == ' ' )
	{
		while( *cmd == ' ' ) ++cmd;
		size = ARG_MAX - 1;
		while( size-->0 && (*cmd != '\n' && *cmd != 0) )
			*outArg++ = *cmd++;	
	}
	*outArg = 0;
	
	return 0;
}

__always_inline __private err_t valid_command( char_t* cmd )
{
	dbg("");
	return strpbrk(cmd, "|;&") ? -1 : 0;
}

int main(int_t argCount, char_t** argValue)
{
	opt_init(myargs, argValue, argCount);
	
	int_t ret;
    char_t* carg;
    double timeLimit = 0.0;
    char_t* cmd = NULL;
    char_t* rdTo = NULL;
    char_t* test = NULL;
    
    while ( (ret = opt_parse(&carg)) >= 0 )
    {
        switch ( ret )
        {
			case 'c':
				cmd = carg;
			break;
			
			case 't':
				timeLimit = (double)strtoul(carg, 0, 10) / 1000.0;
			break;
			
			case 'r':
				rdTo = carg;
			break;
			
			case 'T':
				test = carg;
			break;
			
			default:
				fprintf(stderr,"error: invalid argument\n");
			case 'h':
				opt_help();
			return 0;
		}
	}
	
	dbg("command::'%s'", cmd);
	dbg("time::%f", timeLimit);
	
	if ( cmd == 0 )
	{
		fprintf(stderr,"error: no command\n");
		return -1;
	}
	
	if ( valid_command(cmd) )
	{
		fprintf(stderr,"error: '|&;' invalid command\n");
		return -2;
	}
	
	
	char_t toUser[USER_MAX];
	if ( get_user_by_uid(get_real_uid(), toUser) )
	{
		fprintf(stderr,"error: can't resolve uid\n");
		return -3;
	}
	
	char_t appName[APP_MAX];
	char_t appArg[ARG_MAX];
	if ( app_extract(cmd, appName, appArg) )
	{
		fprintf(stderr,"error: extract app and arg\n");
		return -4;
	}
	
	if ( test )
	{
		if ( 0 ==  conf_validate_regex(appArg, test) )
		{
			puts("regex ok");
		}
		else
		{
			puts("regex fail");
		}
		return 0;
	}
	
	erConf_s* ec = conf_find_ex(appName, appArg, toUser);
	if ( 0 == ec )
	{
		fprintf(stderr,"error: app no match\n");
		return -4;
	}
	
	uid_t toUID = get_uid_by_user(ec->to);
	
	pid_t pidChild = system_safe(cmd, rdTo, toUID);
	if ( pidChild < 0 )
	{
		fprintf(stderr,"error: fork\n");
		return -5;
	}
	
	dbg("PID: %d", pidChild);
	
	processState_e pidState;
	int_t ex;
	
	if ( timeLimit > 0.0 )
	{
		check_timelimit(pidChild, timeLimit);
	}
	else
	{
		pidState = process_state(&ex, pidChild, FALSE);
		if ( pidState != PS_EXITED )
		{
			dbg("kill %d not exited", pidChild);
			kill(pidChild, SIGKILL);
		}
	}
	
	return 0;
}
