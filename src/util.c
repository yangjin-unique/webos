/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yangjin (), jinyang.hust@gmail.com
 *
 * =====================================================================================
 */

#include <ctype.h>
#include "util.h"


int
str_caseless_cmp(char *s1, char *s2)
{
    int rc;

    if (s1 == 0 && s2 == 0) 
        return -1;
    else if (s1 == 0) 
        return -1;
    else if (s2 == 0) 
        return 1;
    
    for (rc = 0;  *s1 != '\0' && *s2 != '\0' && rc == 0; s1++, s2++) 
        rc = tolower((uchar) *s1) - tolower((uchar) *s2);
    
    if (rc) 
        return (rc > 0) ? 1 : -1;
    else if (*s1 == '\0' && *s2 == '\0') 
        return 0;
    else if (*s1 == '\0') 
        return -1;
    else if (*s2 == '\0') 
        return 1;
    
    return 0;
}
