#include "strDup.h"
#include <string.h>


char* myStrDup(char const* str) 
{
  if (str == NULL) 
  {
	return NULL;
  }
  size_t len = strlen(str) + 1;
  char* copy = new char[len];

  if (copy != NULL) 
  {
    memcpy(copy, str, len);
  }
  return copy;
}

char* myStrDupSize(char const* str) 
{
  if (str == NULL) return NULL;
  size_t len = strlen(str) + 1;
  char* copy = new char[len];

  return copy;
}

