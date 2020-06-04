/*
**  http defines & prototypes
*/

#ifndef __HTTP_H__
#define __HTTP_H__ 1

#include "util.h"

#define HTTP_GET_VAR_ALLPARAMS  "all"
#define HTTP_GET_VAR_MACADDR    "macaddr"
#define HTTP_GET_VAR_NTPIP      "ntpip"
#define HTTP_GET_VAR_COUNTER    "counter"
#define HTTP_GET_VAR_INCREMENT  "increment"

/*
**	handle incoming HTTP requests
*/
void HttpHandleRequest(void);

#endif

/**/
