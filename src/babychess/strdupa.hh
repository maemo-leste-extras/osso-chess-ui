#ifndef STRDUPA_INCLUDED
#define STRDUPA_INCLUDED

//#include "strdupa.config.hh"

#ifdef STRDUPA_NO

#include <alloca.h>

#define strdupa(p)                              \
({						\
     int size= strlen(p);			\
     void *r= alloca(size);			\
     memcpy(r, p, size);			\
     (char *)r;                                 \
})


#else  /* ! STRDUPA_NO */ 
#endif /* ! STRDUPA_NO */ 

#endif /* ! STRDUPA_INCLUDED */
