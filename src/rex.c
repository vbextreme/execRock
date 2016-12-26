#include "rex.h"

err_t rex_exec(char_t** stout, char_t** enout, char_t** s, regex_t* r)
{
    dbg("");
	
	regmatch_t rm;
	err_t ret = regexec(r,*s,1,&rm,0);
	if ( 0 == ret )
	{
		*stout = rm.rm_so + *s;
		*enout = rm.rm_eo + *s;
		*s = *s + rm.rm_eo + 1;
		return 0;
	}
	return ret;
}
	
void rex_perror(err_t err, regex_t* r)
{
    dbg("");
	
	char msg[0x1000];
	regerror(err,r,msg,0x1000);
	fprintf(stderr,"rex error: %s\n",msg);
}
