#ifndef TYPE_NAME_INCLUDED
#define TYPE_NAME_INCLUDED

#include <typeinfo>

#define Type_Name(Type) ((typeid(Type)).name()) 

#endif /* ! TYPE_NAME_INCLUDED */ 
