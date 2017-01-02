/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

//TODO
// rifare test
// CONFIG::
// 		aggiungere redirectto

#include "main.h" 

struct argdef myargs[] =	{
							 { 't', 't', "time"    , OPT_ARG  , 0, "max time execution" },
							 { 'T', 'T', "test"    , OPT_ARG  , 0, "regex to test" },
							 { 'h', 'h', "help"    , OPT_NOARG, 0, "help" },
							 { 0  , 0  , 0         , OPT_NOARG, 0, 0}
							};

char_t* parse_lib(char_t** s)
{
	dbg("");
	char_t* r;
	char_t* d = *s;
	
	d = str_skip_space(d);
	if ( *d != '/' )
	{
		d = strchr(d, '>');
		if ( !d )
		{
			dbg("end parse");
			return 0;
		}
		++d;
		d = str_skip_space(d);
	}
	r = d;
	d = strchr(d, '(');
	--d;
	*d = 0;
	d = strchr(d + 1, '\n');
	if ( !d )
	{
		dbg("end parse");
		return 0;
	}
	++d;
	dbg("next parse:\n%s", d);
	*s = d;
	return r;
}

err_t clone_lib(char_t* dst, char_t* app)
{
	dbg("");
	
	char_t *ldd[] = {"ldd", app, NULL};
	
	char_t out[4096];
	if ( system_getout(out, 4096, "/usr/bin/ldd", ldd) )
	{
		dbg("error ldd");
		return -1;
	}
	
	dbg("system return:\n%s", out);
	
	char_t* pr = out;
	char_t* rt;
	char_t tmp[DIR_MAX];
	
	while( (rt = parse_lib(&pr)) )
	{
		snprintf(tmp, DIR_MAX, "%s%s", dst, rt);
		file_copy(tmp, rt);
		chmod(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
	}
	
	return 0;
}

err_t chroot_tmp_new(settingApp_s* sa, char_t* app )
{
	dbg("");
	
	//char_t* pr = tempnam(NULL, "chroot.");
	//strcpy(outName, pr);
	//dbg("chroot dir:'%s'", outName);
	
	char_t tmp[DIR_MAX];
	snprintf(tmp, DIR_MAX, "%s%s", sa->chro, app);
	if ( file_copy(tmp, app) )
	{
		dbg("error on copy app");
		return -1;
	}
	chmod(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	if ( clone_lib(sa->chro, app) )
	{
		dbg("error on clone lib");
		return -2;
	}
	
	return 0;
}

char_t* strofmvcpy(char_t* d, char_t** st, char_t* t)
{
	char_t* s = *st;
	while( *s && !strchr(t,*s) )
		*d++ = *s++;
	*d = 0;
	*st = *s ? s+1 : s ;
	return d;
}

err_t findapp(char_t* out, char_t* app)
{
	char_t* PATH = getenv("PATH");
	char_t* nx;
	char_t* ph = PATH;
	
	while(1)
	{
		nx = strofmvcpy(out, &ph, ":\n");
		if ( !*out ) break;
		*nx = '/';
		strcpy(nx + 1, app);
		dbg("search on '%s'", out);
		if ( !file_exist(out) ) return 0;
	}
	dbg("app not exist");
	return -1;
}

__always_inline __private err_t valid_command( char_t* cmd )
{
	dbg("cmd:'%s'",cmd);
	return strpbrk(cmd, "|;&\n") ? -1 : 0;
}

int main(int_t argCount, char_t** argValue)
{
	umask(0);
	
	opt_init(myargs, argValue, argCount);
	
	int_t ret;
    char_t* carg;
    double timeLimit = 0.0;
    char_t cmd[DIR_MAX];
    char_t* test = NULL;
    uint_t countArg = 0;
    char_t** aArg = NULL;
    
    do
    {
		ret = opt_parse(&carg);
        switch ( ret )
        {	
			case 't':
				timeLimit = (double)strtoul(carg, 0, 10) / 1000.0;
			break;
			
			case 'T':
				test = carg;
			break;
			
			case OPT_ERROR_NOOPT:
				countArg = opt_parsed();
				aArg = &argValue[countArg];
				countArg = argCount - countArg;
			break;
			
			case OPT_END:
			break;
			
			default:
			case 'h':
				opt_help();
			return 0;
		}
	}while ( (ret) >= 0 );
	
	dbg("time::%f redirect::'%s' test::'%s' countArg::%d", timeLimit, rdTo ? rdTo : "", test ? test : "", countArg);
	
	if ( countArg < 1 )
	{
		fprintf(stderr,"error: no command\n");
		return -1;
	}
	
	if ( valid_command(aArg[0]) )
	{
		fprintf(stderr, "error: '|&;\\n' invalid command\n");
		return -2;
	}
	
	if ( aArg[0][0] != '/' )
	{
		if ( findapp(cmd,aArg[0]) )
		{
			fprintf(stderr, "error: command not exist\n");
			return -3;
		}
	}
	else
	{
		strcpy(cmd, aArg[0]);
	}
	
	settingApp_s sa;
	
	if ( conf_read_setting(&sa, get_real_uid(), get_real_gid(), cmd, aArg + 1, countArg - 1) )
	{
		fprintf(stderr, "error: command not in config\n");
		return -4;
	}
	
	aArg[0] = app_name(cmd);
	
	pid_t pidChild;
	dbg("use mode:%u", sa.mode);
	
	if ( sa.mode & SA_MODE_CHR )
	{
		dbg("setting chroot");
		if ( sa.mode & SA_MODE_TMP )
		{
			if ( chroot_tmp_new(&sa, cmd) )
			{
				fprintf(stderr, "error: create temp chroot\n");
				return -5;
			}
		}
		pidChild = system_safe(cmd, aArg, sa.chro, NULL, sa.usr, sa.grp);
	}
	else
	{
		pidChild = system_safe(cmd, aArg, NULL, NULL, sa.usr, sa.grp);
	}
	
	if ( pidChild < 0 )
	{
		fprintf(stderr,"error: fork\n");
		return -6;
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
