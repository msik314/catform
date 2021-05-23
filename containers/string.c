#include "util/globalDefs.h"
#include "containers/string.h"  

void stringCat(String* lhs, const String* rhs)
{
    uint32_t lenLeft = stringLength(lhs);
    uint32_t lenRight = stringLength(rhs);
    stringReserve(lhs, lhs->size + lenRight);
    memmove(&lhs->data[lenLeft], rhs->data, lenRight);
    lhs->data[stringLength(lhs)] = '\0';
}

void stringCatStrSlice(String* lhs, const char* rhs, uint32_t len)
{
    uint32_t lenLeft = stringLength(lhs);
    stringReserve(lhs, lhs->size + len);
    memmove(&lhs->data[lenLeft], rhs, len);
    lhs->data[stringLength(lhs)] = '\0';
}
