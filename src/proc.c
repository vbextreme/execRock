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

pid_t system_safe(char_t* cmd, char_t** arg, char_t* chro, char_t* redirectTo, uid_t switchUser, gid_t switchGroup)
{
	dbg("");
	iassert( cmd != NULL );
	
	pid_t ret;

	switch ( (ret = fork()) )
	{
		case PROCESS_ERROR:	return -1;
		
		case PROCESS_CHILD:
			if ( chro && *chro != 0 )
			{
				if ( chroot(chro) )
				{
					dbg("fail chroot %d: %s",errno, strerror(errno) );
				}
				else
				{
					dbg("chrooted in %s chro", chro);
					dbg("change directory to root");
					if ( chdir("/") ) dbg("fail chdir %d: %s",errno, strerror(errno) );
					
					char cwd[1024];
					(void) getcwd(cwd, sizeof(cwd));
					dbg("chro:'%s' cwd:'%s'", chro, cwd);
				}
			}
			
			if ( switchUser != (uid_t)~0 )
			{
				dbg("start from user r:%u e:%u", get_real_uid(), get_effective_uid());
				if ( set_real_uid(switchUser) ) dbg("fail set real uid %d: %s",errno, strerror(errno) );
				if ( set_effective_uid(switchUser) ) dbg("fail set effective uid %d: %s",errno, strerror(errno) ); 
				dbg("switch user(%u) r:%u e:%u", switchUser, get_real_uid(), get_effective_uid());
			}
			
			if ( switchGroup != (gid_t)~0 )
			{
				dbg("start from group r:%u e:%u", get_real_gid(), get_effective_gid());
				if ( set_real_gid(switchGroup) ) dbg("fail set real uid %d: %s",errno, strerror(errno) );
				if ( set_effective_gid(switchGroup) ) dbg("fail set effective uid %d: %s",errno, strerror(errno) ); 
				dbg("switch group(%u) r:%u e:%u", switchGroup, get_real_gid(), get_effective_gid());
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
			
			dbg("execv '%s'",cmd);
			if ( execv(cmd, arg) )
			{
				dbg("fail execv %d: %s",errno, strerror(errno) );
			}
			dbg("exit :(");
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

err_t system_getout(char_t* out, uint_t szout, char_t* cmd, char_t** arg)
{
	dbg("");
	iassert( out != NULL );
	iassert( cmd != NULL );
	iassert( szout > 2 );
	
	pid_t ret;
	
	pipe_s rdo;
	if ( pipe_new(&rdo) ) return -1;
	
	switch ( (ret = fork()) )
	{
		case PROCESS_ERROR:	return -1;
		
		case PROCESS_CHILD:
			//dbg("child redirect");
			close(rdo.in);
			dup2(rdo.out, 1);
			dup2(rdo.out, 2);
			close(rdo.out);
			
			//dbg("exec '%s'",cmd);
			if ( execv(cmd, arg) )
			{
				dbg("fail execv %d: %s",errno, strerror(errno) );
			}
			dbg("exit :(");
			_exit(-1);
		break;
	}
	
	close(rdo.out);
	file_h* f = fdopen( rdo.in, "r");
	if ( !f )
	{
		dbg("error open file_h pipe");
		kill(ret, SIGKILL);
		return -1;
	}
	
	--szout;
	int ch;
	while( szout-->0 && (ch = fgetc(f)) != EOF )
		*out++ = ch;
	*out = 0;
	
	fclose(f);
	return 0;	
}
