/*
 * $Id: xml-rpc.h,v 1.1 2010/02/25 01:40:32 curtis Exp $
 */

#ifndef _XMLRPC_H_
#define _XMLRPC_H_

#include "webserver.h"

extern void xml_handle(WS_CONNINFO *pwsc);

struct tag_xmlstruct;
typedef struct tag_xmlstruct XMLSTRUCT;

extern XMLSTRUCT *xml_init(WS_CONNINFO *pwsc, int emit_header);
extern void xml_push(XMLSTRUCT *pxml, char *term);
extern void xml_pop(XMLSTRUCT *pxml);
extern void xml_output(XMLSTRUCT *pxml, char *section, char *fmt, ...);
extern void xml_deinit(XMLSTRUCT *pxml);

/* mp3_dir error message curtis@lapha 01_14_2010 */
enum MP3_DIR{ERR_VOL, ERR_EMPTY};


#endif /* _XMLRPC_H_ */
