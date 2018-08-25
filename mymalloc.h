#ifndef _mymalloc_h
#define _mymalloc_h
#define malloc(x) mymalloc( x, __FILE__, __LINE__ )
#define free(x) myfree( x, __FILE__, __LINE__ )
void* mymalloc(unsigned int, char*, int);
unsigned int getAvailBlock(unsigned int);
void setSize(unsigned int, unsigned int);
unsigned int getSize(unsigned int);
void myfree(void*, char*, int);
#endif
