/* 
 * SIMPLE HOW-TO for string comparaison
 * using scan memory to compare string of arrays
 * To compile:
 * gcc string_compare.c -o string_compare
 * ./string_compare
 *
 * Author : RAMJANALLY Ghoulseine
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define array_size(x) (sizeof(x)/sizeof(x)[0])
#define fbool(b) (b==true?"true":"false")
#define chareq(c,v) (c==v)

bool 
charlsteq(char value,const char *property) 
{
   return !!memchr(property, value, sizeof(property));
}

bool 
streq(char *value,const char *property) 
{
   return !memcmp(property, value, sizeof(property));
}

bool 
strlsteq(char *value, const char **properties, int len) 
{
   int i = 0;
   bool ret = false;

   while(i < len && ret == false )
   {
     ret = !memcmp(properties[i], value, sizeof(properties[i]));
     i++;
   } 
   return ret;   
}

bool
startwith(char *value,const char *property)
{
  return !memcmp(property,value,strlen(value));
}

void
main(void)
{

  bool ret;
  const char proper = 'a';
  const char property[] = { 'a','b', 'c', 'd' };
  const char *properties[] = { "abcd", "efgh", "jklm", "nopq", "rstu" };

  // compare a char
  ret = chareq('a',proper);
  printf("property 'a' : %s\n",(char *)fbool(ret)); //true

  ret = chareq('z',proper);
  printf("property 'z' : %s\n",(char *)fbool(ret)); //false

  // compare char from an array
  ret = charlsteq('a',property);
  printf("property 'a' : %s\n",(char *)fbool(ret)); //true

  ret = charlsteq('f',property);
  printf("property '' : %s\n",(char *)fbool(ret)); //false

  // compare a string from a char pointer 
  ret = streq("abcd",property);
  printf("property 'abcd'  : %s\n",(char *)fbool(ret)); //true

  ret = streq("efgh",property);
  printf("property 'efgh'  : %s\n",(char *)fbool(ret)); //false

  // compare a string from a list of chars of pointer
  ret = strlsteq("jklm",properties,(int)array_size(properties));
  printf("property 'jklm'  : %s\n",(char *)fbool(ret));  //true

  ret = strlsteq("efg",properties,(int)array_size(properties));
  printf("property 'efg'  : %s\n",(char *)fbool(ret));   //false

  // compare first char of a string
  ret = startwith("abc",property);
  printf("property 'abc'  : %s\n",(char *)fbool(ret)); //true

  ret = startwith("bcd",property);
  printf("property 'bcd'  : %s\n",(char *)fbool(ret)); //false


}
