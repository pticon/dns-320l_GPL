/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2002, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: getprinter.h,v 1.27 2002/04/01 17:54:57 papowell Exp $
 ***************************************************************************/




#ifndef _GETPRINTER_H_
#define _GETPRINTER_H_ 1

/* PROTOTYPES */
char *Get_printer(void);
void Fix_Rm_Rp_info(char *report_conflict, int report_len );
void Get_all_printcap_entries(void);
void Show_formatted_info( void );
void Show_all_printcap_entries( void );

#endif
