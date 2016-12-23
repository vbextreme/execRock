/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#include "main.h" 

struct argdef myargs[] =	{{ 'c', 'c', "command" , OPT_ARG, 0, "shell this command" },
							 { 't', 't', "time"    , OPT_ARG, 0, "max time execution" },
							 { 'r', 'r', "redirect", OPT_ARG, 0, "redirect to file" },
							 { 'h', 'h', "help"    , OPT_ARG, 0, "help" },
							 { 0  , 0  , 0         , OPT_ARG, 0, 0}
							};

char_t* app_extract( char_t* cmd )
{
	dbg("arg:'%s'",cmd);
	
	static char_t app[APP_MAX];
	char_t* rd = app;
	uint_t size = APP_MAX - 1;
	
	while( size-->0 && (*cmd != ' ' && *cmd != 0) )
		*rd++ = *cmd++;
		
	*rd = 0;
	
	dbg("extract:'%s'",app);
	return app;
}

int main(int_t argCount, char_t** argValue)
{
	opt_init(myargs, argValue, argCount);
	
	int_t ret;
    char_t* carg;
    double timeLimit = 0.0;
    char_t* cmd = NULL;
    char_t* rdTo = NULL;
    
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
			
			case 'h':
				opt_help();
			return 0;
		}
	}
	
	dbg("command::'%s'", cmd);
	dbg("time::%f", timeLimit);
	
	if ( cmd == 0 )
	{
		fprintf(stderr,"error: no command");
		return -1;
	}
	
	char_t* toUser = get_user_by_uid( get_real_uid() );
	uid_t toUID = conf_toprivileges( app_extract(cmd), toUser);
	if ( toUID == (uid_t)~0 )
	{
		fprintf(stderr,"error: privileges\n");
		return -2;
	}
	
	pid_t pidChild = system_safe(cmd, rdTo, toUID);
	if ( pidChild < 0 )
	{
		fprintf(stderr,"error: fork\n");
		return -3;
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
