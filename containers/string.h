#ifndef STRING_H
#define STRING_H

#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#include <string.h>
#include "containers/vector.h"
#include "containers/vectorTypes.h"

typedef Vector(char) String;

static inline void stringCreate(String* string) {vectorCreate(char)(string);}
static inline void stringDestroy(String* string) {vectorDestroy(char)(string);}
static inline void stringAdd(String* string, char original) {vectorAdd(char)(string, &original);}
static inline void stringInsert(String* string, uint32_t index, char original) {vectorInsert(char)(string, index, &original);}
static inline void stringRemove(String* string, uint32_t index) {vectorRemove(char)(string, index);}
static inline void stringPopN(String* string, uint32_t amt) {vectorPopN(char)(string, amt);}
static inline void stringPop(String* string) {vectorPop(char)(string);}
static inline void stringClear(String* string) {vectorClear(char)(string);}
static inline void stringReserve(String* string, uint32_t capacity) {vectorReserve(char)(string, capacity);}

static inline uint32_t stringLength(const String* string) {return string->size - 1;}

static inline void strSliceToString(String* string, const char* slice, uint32_t len)
{
    stringCreate(string);
    stringReserve(string, len + 1);
    memcpy(string->data, slice, len);
    string->data[len] = 0;
}

static inline void cStrToString(String* string, const char* cStr) {strSliceToString(string, cStr, strlen(cStr));}

static inline void stringCatChar(String* string, char c) {stringAdd(string, '\0'); string->data[string->size - 1] = c;}

void stringCat(String* lhs, const String* rhs);
void stringCatStrSlice(String* lhs, const char* rhs, uint32_t len);
static inline void stringCatCStr(String* lhs, const char* rhs) {stringCatStrSlice(lhs, rhs, strlen(rhs));}

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //STRING_H
