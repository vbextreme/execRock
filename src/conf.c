/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#include "main.h" 

__private void fskipspace(file_h* f)
{
	dbg("");
	iassert( f != NULL );
	
	int_t ch;
	while( (ch = fgetc(f)) == ' ' || ch == '\t' )
	{
		dbg("\tskip:'%c'", ch);
	}
	ungetc(ch, f);
}

__private void fskipemptyline(file_h* f)
{
	dbg("");
	iassert( f != NULL );
	
	int_t ch;
	do
	{
		while( (ch = fgetc(f)) == ' ' || ch == '\t' )
		{
			dbg("\tskip:'%c'", ch);
		}
		dbg("ch is:'%c'",ch);
		if ( ch == '#' )
		{
			dbg("\tcomment");
			while ( (ch = fgetc(f)) != '\n' && ch != EOF )
			{
				dbg("\tskip:'%c'", ch);
			}
		}
	}while( ch == '\n');
	ungetc(ch, f);
}

__private void freadstring(file_h* f, char_t* dest, uint_t size, char_t* term)
{
	dbg("");
	iassert( f != NULL );
	iassert( size > 1 );
	iassert( dest != NULL );
	iassert( term != NULL );
	
	int_t ch;
	--size;
	while( size-->0 )
	{
		ch = fgetc(f);
		if ( ch == EOF || strchr(term, ch) )
		{
			ungetc(ch, f);
			break;
		}
		*dest++ = ch;
	}
	*dest = 0;
}

__private err_t fcheckok(file_h* f, char_t* term)
{
	dbg("");
	iassert( f != NULL );
	iassert( term != NULL );
	
	int_t ch;
	ch = fgetc(f);
	if ( strchr(term, ch) == NULL ) 
	{
		ungetc(ch, f);
		return -1;
	}
	return 0;
} 

__private erConf_s* conf_read(file_h* f)
{
	dbg("");
	iassert( f != NULL );
	
	static erConf_s cf;
	
	fskipemptyline(f);
	freadstring(f, cf.app, sizeof cf.app, " @|");
	dbg("read app:%s",cf.app);
	
	fskipspace(f);
	if ( !fcheckok(f, "|") )
	{
		dbg("have regex");
		fskipspace(f);
		freadstring(f, cf.arg, sizeof cf.arg, "@");
	}
	else
	{
		cf.arg[0] = 0;
	}
	
	if ( fcheckok(f, "@") )
	{
		dbg("fail @");
		return NULL;
	}
	
	fskipspace(f);
	freadstring(f, cf.from, sizeof cf.from, " >");
	dbg("read from:%s",cf.from);
	
	fskipspace(f);
	if ( fcheckok(f, ">") )
	{
		dbg("fail >");
		return NULL;
	}
	
	fskipspace(f);
	freadstring(f, cf.to, sizeof cf.to, "\n #");
	dbg("read to:%s",cf.from);
	
	dbg("parse ok");
	return &cf;
}

erConf_s* conf_find(char_t* app, char_t* from)
{
	dbg("");
	iassert( app != 0 );
	iassert( from != 0 );
	
	file_h* f = fopen(CONFIG_FILE, "r");
	
	erConf_s* cf;
	while( (cf = conf_read(f)) )
	{
		dbg("check %s == %s && %s == %s",app, cf->app, from, cf->from);
		if ( strop(app,==,cf->app) && strop(from,==,cf->from) )
		{
			fclose(f);
			return cf;
		}
	}
	
	fclose(f);
	return NULL;
}

err_t conf_validate_regex(char_t* arg, char_t* reg)
{
	dbg("argument: '%s'", arg);
	if ( *reg == 0 )
	{
		dbg("no regex return ok");
		return 0;
	}
	
	regex_t rex;
	err_t ret = rex_mk(&rex, reg);
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

erConf_s* conf_find_ex(char_t* app, char_t* arg, char_t* from)
{
	dbg("arg '%s'", arg);
	iassert( app != 0 );
	iassert( arg != 0 );
	iassert( from != 0 );
	
	file_h* f = fopen(CONFIG_FILE, "r");
	
	erConf_s* cf;
	while( (cf = conf_read(f)) )
	{
		dbg("check %s == %s && %s == %s",app, cf->app, from, cf->from);
		if ( strop(app,==,cf->app) && strop(from,==,cf->from) && !conf_validate_regex(arg, cf->arg) )
		{
			fclose(f);
			return cf;
		}
	}
	
	fclose(f);
	return NULL;
}








