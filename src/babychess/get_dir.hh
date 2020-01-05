#ifndef GET_DIR_INCLUDED
#define GET_DIR_INCLUDED

#include "string.hh"

enum Install_Dir
{id_bin,     /* Location of executables           */
 id_install};

String Get_Dir(Install_Dir);

#endif /* ! GET_DIR_INCLUDED */
