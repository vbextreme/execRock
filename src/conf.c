/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#include "main.h" 

__private char_t* conf_gets(char_t* s, uint_t n, file_h* f)
{
	dbg("");
	while ( fgets(s, n, f) )
	{
		char_t* stl = strpbrk(s, "#\n");
		if ( stl ) *stl = 0;
		stl = str_skip_space(s);
		if ( *stl == 0 ) continue;
		uint_t l = strlen(stl);
		char_t* ensp = (stl + l);
		while( ensp > stl && *ensp == ' ' ) --ensp;
		if ( ensp == stl ) continue;
		*ensp = 0;
		memmove( s, stl, (ensp - stl) + 1 );
		dbg("s:'%s'", s);
		return s;
	}
	return NULL;
}

__always_inline __private char_t* conf_is_app(char_t* s)
{
	char_t* ret = strchr(s, '[');
	dbg("is app:%s", ret ? "yes":"no");
	return ((ret) ? ret + 1 : NULL);
}

__always_inline __private char_t* conf_is_type(char_t* s)
{
	dbg("is type:%s", strchr(s, '=') ? "yes":"no");
	return strchr(s, '=');
}

__always_inline __private err_t conf_compare_app(char_t* s, char_t* a)
{
	uint_t l = strlen(a);
	if ( *(s + l) != ']' )
	{
		dbg(".conf not close ] s:'%s' a:'%s' l:%u", s, a, l);
		return -1;
	}
	if ( strncmp(s, a, l) )
	{
		dbg("fail compare '%s' '%s' %u", s, a, l);
		return -1;
	}
	dbg("ok compare '%s' '%s' %u", s, a, l);
	return 0;
}

typedef enum { ST_FROM, ST_TO, ST_ARG, ST_CHROOT, ST_COUNT}settingType_e;
__private char_t* typename[ST_COUNT] = {"from", "to", "arg", "chroot"};

__private settingType_e conf_read_type(char_t* stl)
{
	uint_t iType;
	char_t* s = str_skip_space(stl);
	dbg("find type:'%s'", stl);
		
	for( iType = 0; iType < ST_COUNT; ++iType)
	{
		if ( strnop(s, ==, typename[iType]) )
		{
			dbg("is type %s", typename[iType]);
			return iType;
		}
	}
	
	dbg("no type");
	return -1;
}

__private err_t conf_from(char_t* s, uid_t fUsr, uid_t fGrp)
{
	uid_t id;
	if (*s == '@')
	{
		++s;
		id = get_uid_by_user(s);
		dbg("user id:%u == %u", id, fUsr);
		return ( id == fUsr ) ? 0 : -1;
	}
	id = get_gid_by_group(s);
	dbg("group id:%u == %u", id, fGrp);
	return ( id == fGrp ) ? 0 : -2;
}

__private err_t conf_to(settingApp_s* out, char_t* s)
{
	dbg("");
	char_t* gn = strchr(s, ':');
	if ( !gn )
	{
		dbg("error .conf TO need user:group");
		return -1;
	}
	*gn = 0;
	
	out->usr = get_uid_by_user(s);
	if ( out->usr == (uid_t)~0 )
	{
		dbg("error not find user");
		return -1;
	}
	dbg("find user '%s' uid %u", s, out->usr);
	
	out->grp = get_gid_by_group(gn + 1);
	if ( out->grp == (gid_t)~0 )
	{
		dbg("error not find group");
		return -1;
	}
	dbg("find group '%s' gid %u", gn+1, out->grp);
	
	return 0;
}

__private err_t conf_arg_extract(char_t* d, char_t** src)
{
	char_t* stdest = d;
	dbg("src:\"%s\"", *src);
	if ( !*src )
	{
		dbg("no sources");
		return -1;
	}
	
	char_t* s = strchr(*src, '\'');
	if ( !s )
	{
		dbg("fail to find '");
		*src = s;
		return -1;
	}
	++s;
	
	while( *s && *s != '\'' )
	{
		if ( *s == '\\' ) ++s;
		*d++ = *s++;
	}
	*d = 0;
	dbg("arg:'%s'", stdest);
	if ( *s != '\'' )
	{
		dbg("fail not closed '");
		*src = s;
		return -1;
	}
	*src = s + 1;
	return 0;
}

__private err_t conf_arg_test(char_t* srx, char_t* arg)
{
	dbg("srx::'%s' arg::'%s'", srx, arg);
	
	regex_t rex;
	err_t ret = rex_mk(&rex, srx);
	if ( ret )
	{
		char msg[0x1000];
		regerror(ret, &rex, msg, 0x1000);
		dbg("rex error: %s", msg);
		return -1;
	}
	
	char_t* sto;
	char_t* eno;
	char_t* f = arg;
	
	ret = rex_exec( &sto, &eno, &f, &rex);
	if ( ret != 0 )
	{
		dbg("rex %s", REX_NOMATCH != ret ? "find error": "no match");
		return -2;
	}
	
	dbg("len arg: %u", strlen(arg));
	dbg("offset start: %u", sto - arg);
	dbg("offset end %u", eno - arg );
	dbg("offset rarg %u", f - arg);
	
	if ( (sto - arg) || (strlen(arg) - (eno-arg)) )
	{
		dbg("error remaning char");
		return -3;
	}
	
	return 0;
}

__private err_t conf_arg(char_t* s, char_t** arg, uint_t countArg)
{
	dbg("");
	char_t frarg[ARG_MAX];
	
	while( countArg-->0 )
	{
		if ( conf_arg_extract(frarg, &s) )
		{
			dbg("fail extract argument");
			return -1;
		}
		if ( conf_arg_test(frarg, *arg) )
		{
			dbg("fail test argument");
			return -1;
		}
		++arg;
	}
	dbg("%s", conf_arg_extract(frarg,&s) ? "fail" : "ok"); 
	return !conf_arg_extract(frarg,&s);
}

__private err_t conf_chro(settingApp_s* out, char_t* s)
{
	dbg("");
	
	if ( strop(s, ==, "no") )
	{
		dbg("no chroot");
		return 0;
	}
	
	if ( strop(s, ==, "tmp") )
	{
		dbg("temp chroot");
		char_t* nm;
		strcpy(out->chro, "/tmp/chroot.XXXXXX");
		if ( !(nm=mkdtemp(out->chro)) )
		{
			dbg("error on create tempchroot");
			return -1;
		}
		
		out->mode |= SA_MODE_TMP | SA_MODE_CHR;
		strcpy(out->chro, nm);
		dbg("create temp dir '%s' mode:%u", out->chro, out->mode);
		
		chmod(out->chro, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		return 0;
	}
	
	if ( dir_exists(s) ) return -1;
	out->mode |= SA_MODE_CHR;
	strcpy(out->chro, s);
	dbg("dir charoot:'%s'", out->chro);
	
	return 0;
}

__private err_t conf_parse_type(file_h* f, settingApp_s* out, uid_t fromUsr, gid_t fromGrp, char_t** arg, uint_t countArg)
{
	dbg("");
	
	char_t line[1024];
	char_t* pl;
	uint_t set = 0;
	
	dbg("Set mode to 0");
	out->mode = 0;
	
	while( conf_gets(line, 1024, f) )
	{
		if ( conf_is_app(line) )
		{
			dbg("error: no complete option");
			return -2;
		}
		if ( !(pl = conf_is_type(line)) )
		{
			dbg("is not type, skip");
			continue;
		}
		
		settingType_e type = conf_read_type(line);
		if ( type < 0 )
		{
			dbg("error type: '%s'", line); 
			continue;
		}
		
		++pl;
		pl = str_skip_space(pl);
		
		switch( type )
		{
			case ST_FROM:
				if ( conf_from(pl, fromUsr, fromGrp) ) return -5;
				set |= 0x01;
			break;
			
			case ST_TO:
				if ( conf_to(out, pl) ) return -5;
				set |= 0x02;
			break;
				
			case ST_ARG:
				if ( conf_arg(pl,arg,countArg) ) return -6;
				set |= 0x04;
			break;
				
			case ST_CHROOT:
				if ( conf_chro(out, pl) ) return -7;
				set |= 0x08;
			break;
			
			default: case ST_COUNT: dbg("only for compiler"); return -8;
		}
		
		if ( set == 0x0F )
		{
			dbg("all arg is set");
			return 0;
		}
		else
		{
			dbg("set: 0x%X", set);
		}
	}
	
	dbg("EOF");
	return -1;
}

err_t conf_read_setting(settingApp_s* out, uid_t fromUsr, gid_t fromGrp, char_t* app, char_t** arg, uint_t countArg)
{
	dbg("");
	iassert( app != 0 );
	iassert( arg != 0 );
	
	char_t line[1024];
	char_t* pl;
	file_h* f = fopen(CONFIG_FILE, "r");
	iassert( f != NULL );
	
	while( conf_gets(line, 1024, f) )
	{
		if ( !(pl = conf_is_app(line)) ) continue;
		TODO_REMOVE_THIS_GOTO:
		if ( conf_compare_app(pl, app) ) continue;
		
		err_t ret = conf_parse_type(f, out, fromUsr, fromGrp, arg, countArg);
		if ( ret == -1 ) break;
		if ( ret == -2 ) goto TODO_REMOVE_THIS_GOTO;
		if ( ret != 0 ) continue;
		
		fclose(f);
		return 0;
	}
	
	fclose(f);
	return -1;
}
