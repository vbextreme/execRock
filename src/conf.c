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
	return ( strchr(term, ch) == NULL ) ? -1 : 0;
} 

__private erConf_s* conf_read(file_h* f)
{
	dbg("");
	iassert( f != NULL );
	
	static erConf_s cf;
	
	fskipemptyline(f);
	freadstring(f, cf.app, sizeof cf.app, " @");
	dbg("read app:%s",cf.app);
	fskipspace(f);
	if ( fcheckok(f, "@") ) return NULL;
	fskipspace(f);
	freadstring(f, cf.from, sizeof cf.from, " >");
	dbg("read from:%s",cf.from);
	fskipspace(f);
	if ( fcheckok(f, ">") ) return NULL;
	fskipspace(f);
	freadstring(f, cf.to, sizeof cf.to, "\n #");
	dbg("read to:%s",cf.from);
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

uid_t conf_toprivileges(char_t* app, char_t* from)
{
	dbg("");
	iassert(app != NULL);
	iassert(from != NULL);
	
	erConf_s* cf = conf_find(app, from);
	if ( cf == NULL ) return (uid_t)~0;
	return get_uid_by_user(cf->to);
}















