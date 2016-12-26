#ifndef _REX_H_INCLUDED_
#define _REX_H_INCLUDED_

#include "stdextra.h"
#include <regex.h>

#define REX_NOMATCH REG_NOMATCH

/// espressioni regolari
/// "" trova stringa uguale
/// . qualsiasi carattere != \n
/// [] uno di quelli, - nell'intervallo tra, '[' ']'
/// [^] negazione
/// ^ inizio riga
/// $ fine riga
/// () sottoespressione riusabile
/// \n dove n sta per l'indice della sottoespressione
/// * zero o piu volte
/// + una o piu volte
/// ? zero o una volta
/// {min,max} ripetizioni
/// | or

#define rex_mk(REX,PAT) regcomp(REX,PAT,REG_EXTENDED)
#define rex_free(REX) regfree(REX)
err_t rex_exec(char_t** stout, char_t** enout, char_t** s, regex_t* r);
void rex_perror(err_t err, regex_t* r);

#endif
