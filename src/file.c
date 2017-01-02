/*******************************************/
/*** Copyright 2016 vbextreme            ***/
/*** License view file in this directory ***/
/*******************************************/

#include "main.h" 

char_t* pth_next(char_t* s)
{
	if ( *s == '/' ) ++s;
	return strchr(s, '/');
}

err_t pth_current(char_t* d) 
{ 
	return getcwd(d, DIR_MAX) ? 0 : -1; 
}

err_t pth_set(char_t* d) 
{ 
	return chdir(d); 
}

char_t* str_skip_space(char_t* s)
{
	iassert( s != NULL );
	while( *s && (*s == ' ' || *s == '\t') ) ++s;
	return s;
}

err_t pth_back(char_t* d)
{
	uint_t l = strlen(d);
	if ( l < 2 ) return 0;
	
    char_t* f = d + (l - 1);
    if ( *f == '/') --f;

    for (; f > d && *f != '/'; --f);
	*f = 0;
    return 0;
}

char_t* pth_homedir(void)
{
	char_t *hd;
	if ((hd = getenv("HOME")) == NULL) 
		hd = getpwuid(getuid())->pw_dir;
	return hd;
}

err_t pth_normalize(char_t* d, char_t* s)
{
	dbg("src:'%s'", d);
	char_t tmp[DIR_MAX];
	
	if ( !strncmp(s,"..",2) )
	{
		pth_current(tmp);
		do
		{
			pth_back(tmp);
			s += 2;
		}while ( *s == '/' && !strncmp(++s,"..",2) );
		
		snprintf(d, DIR_MAX, "%s/%s", tmp, s);
		dbg("norm:'%s'", d);
		return 0;
	}
	
	if ( *s == '.' )
	{
		if ( s[1] == '/' && s[2] ) 
		{
			pth_current(tmp);
			snprintf(d, DIR_MAX, "%s/%s", tmp, &s[2]);
		}
		else
		{
			pth_current(d);
		}
		return 0;
	}
	
	strcpy(d, s);
    return 0;
}

err_t file_exist(char_t* fname)
{
	file_h* f;
	f = fopen(fname, "r");
	if ( f )
	{
		dbg("file exist");
		fclose(f);
		return 0;
	}
	dbg("file not exist");
	return -1;
}

err_t dir_exists(char_t* d)
{
	dir_h* dir = opendir(d);
	if (dir)
	{
		dbg("dir exist");
		closedir(dir);
		return 0;
	}
	dbg("dir not exist");
	return -1;
}

char_t* app_name(char_t* dir)
{
	dbg("");
	uint_t l = strlen(dir);
	if ( l < 2 ) return 0;
	
    char_t* f = dir + (l - 1);
    if ( *f == '/') --f;

    for (; f > dir && *f != '/'; --f);
	++f;
    return f;
}

err_t dir_new(char_t* dir, mode_t pri)
{
	dbg("dir '%s'", dir);
	
	char_t tmp[DIR_MAX];
	
	if ( !dir_exists(dir) ) return 0;
	
	strcpy(tmp, dir);
	pth_back(tmp);
	dbg ("path back:'%s'", tmp);
	if ( dir_exists(tmp) ) dir_new(tmp, pri);
	
	dbg("new dir '%s'", dir);
	return mkdir(dir, pri);
}

err_t file_copy(char_t* d, char_t* s)
{
	dbg("'%s' => '%s'", s, d);
	char_t tmp[DIR_MAX];
	strcpy(tmp, d);
	pth_back(tmp);
	if ( dir_new(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) )
	{
		dbg("fail to create directory");
		return -1;
	}
	
	//start cpy
	file_h* fs = fopen(s, "r");
	if ( !fs )
	{
		dbg("fail to open src file %s", s);
		return -2;
	}
	
	file_h* fd = fopen(d, "w");
	if ( !fd )
	{
		dbg("fail to open dst file %s", d);
		fclose(fs);
		return -3;
	}
	
	uint_t nr;
	while( (nr = fread(tmp, 1, DIR_MAX, fs)) )
		fwrite(tmp, 1, nr, fd);
	
	fclose(fs);
	fclose(fd);
	return 0;
}
