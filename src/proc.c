/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#include "main.h" 

double bch_get()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec * 1e-6;
}

processState_e process_state(int_t* ex, pid_t pid, bool_t async)
{	
	int_t flag =  WUNTRACED | WCONTINUED;
	if ( async )
	{
		flag |= WNOHANG;
	}
		
	int_t st;
	if ( waitpid(pid, &st, flag) <= 0 ) return -1;
	
	if ( WIFEXITED(st) )
	{
		if ( ex ) *ex = WEXITSTATUS(st);
		return PS_EXITED;
	}
	
	if ( WIFSIGNALED(st) )
	{
		if ( ex ) *ex = WTERMSIG (st);
		return PS_ONSIGNAL;
	}
	
	if ( WIFSTOPPED(st) )
	{
		if ( ex ) *ex = WSTOPSIG(st);
		return PS_STOP;
	}
	
	if ( WIFCONTINUED(st) )
	{
		if ( ex ) *ex = 0;
		return PS_CONTINUE;
	}
		
	return PS_RUN;
}

pid_t system_safe(char_t* cmd, char_t* redirectTo, uid_t switchUser)
{
	dbg("");
	iassert( cmd != NULL );
	
	pid_t ret;

	switch ( (ret = fork()) )
	{
		case PROCESS_ERROR:	return -1;
		
		case PROCESS_CHILD:
			if ( switchUser != (uid_t)~0 )
			{
				dbg("start from user r:%u e:%u", get_real_uid(), get_effective_uid());
				set_real_uid(switchUser);
				set_effective_uid(switchUser);
				dbg("switch user(%u) r:%u e:%u", switchUser, get_real_uid(), get_effective_uid());
			}
			
			if ( redirectTo )
			{
				dbg("redirect to %s", redirectTo);
				file_h* f = fopen(redirectTo, "w");
				iassert( f != NULL );
				int fd = fileno(f);
				iassert( fd > 0 );
				dup2(fd, 1);
				dup2(fd, 2);
				fclose(f);
			}
			
			dbg("exec %s",cmd);
			execl("/bin/bash", "bash", "-c", cmd, (char *)0);
			_exit(-1);
		break;
	}
	
	return ret;	
}

err_t check_timelimit(pid_t pid, double timeLimit)
{
	dbg("");
	double timeStart = bch_get();
	processState_e pidState;
	do
	{
		pidState = process_state(NULL, pid, TRUE);
		con_delay(DEFAULT_TIME_RELAX);
	}while( bch_get() - timeStart < timeLimit && pidState != PS_EXITED );
	
	if( pidState != PS_EXITED )
	{
		dbg("kill %d for timelimit", pid);
		kill(pid, SIGKILL);
		return -1;
	}
	
	return 0;
}
