/*
 * $Id: xml-rpc.h,v 1.1 2010/02/25 01:40:33 curtis Exp $
 */

#ifndef _XMLRPC_H_
#define _XMLRPC_H_

#include "ff-plugins.h"

struct tag_xmlstruct;
typedef struct tag_xmlstruct XMLSTRUCT;
typedef struct tag_ws_conninfo WS_CONNINFO;

extern XMLSTRUCT *xml_init(WS_CONNINFO *pwsc, int emit_header);
extern void xml_push(XMLSTRUCT *pxml, char *term);
extern void xml_pop(XMLSTRUCT *pxml);
extern int xml_output(XMLSTRUCT *pxml, char *section, char *fmt, ...);
extern void xml_deinit(XMLSTRUCT *pxml);


#endif /* _XMLRPC_H_ */
