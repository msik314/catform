#ifndef TAG_H
#define TAG_H
#ifdef __cplusplus
extern "C"
{
#else
#include <stdbool.h>
#endif //__cplusplus

#include <string.h>

typedef struct
{
    char data[16];
}
Tag;

static inline int32_t tagCmp(const Tag* lhs, const Tag* rhs)
{
    return strncmp(lhs->data, rhs->data, sizeof(Tag)); 
}

static inline bool tagEq(const Tag* lhs, const Tag* rhs)
{
    return !tagCmp(lhs, rhs); 
}

static inline bool tagEmpty(const Tag* tag)
{
    return !tag->data[0]; 
}

static inline void tagSet(Tag* tag, const char* str)
{
    strncpy(tag->data, str, sizeof(Tag));
}

#ifdef __cplusplus
};

static inline bool operator==(const Tag& lhs, const Tag& rhs){return !tagCmp(&lhs, &rhs);}
static inline bool operator!=(const Tag& lhs, const Tag& rhs){return tagCmp(&lhs, &rhs);}

static inline Tag operator"" _T(const char* str, size_t num)
{
    Tag tag;
    memcpy(tag.data, str, num <= sizeof(Tag) ? num : sizeof(Tag));
    return tag;
}

#endif //__cplusplus

#endif //TAG_H
