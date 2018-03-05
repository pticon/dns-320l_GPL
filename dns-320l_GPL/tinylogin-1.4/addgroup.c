/* vi: set sw=4 ts=4: */
/*
 * addgroup - add users to /etc/passwd and /etc/shadow
 *
 * Copyright (C) 1999 by Lineo, inc. and John Beppu
 * Copyright (C) 1999,2000,2001 by John Beppu <beppu@codepoet.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "tinylogin.h"

#define GROUP_FILE      "/etc/group"
#define SHADOW_FILE		"/etc/gshadow"


/* structs __________________________ */

/* data _____________________________ */

/* defaults : should this be in an external file? */
static const char default_passwd[] = "x";

/* make sure gr_name isn't taken, make sure gid is kosher
 * return 1 on failure */
static int group_study(const char *filename, struct group *g)
{
	FILE *etc_group;
	FILE *fp;
	gid_t desired;
	struct group *grp;
	//alpha.jack.20090723+
	const int max = 65000;
	const int min = 502;
	//const int max = 999;
	//alpha.jack.20090723 end
/*alpha.jack.20100712+ save gid for quota*/
	int last_gid = 0;
/*alpha.jack.20100712 end*/
	
	grp = getgrnam(g->gr_name);
	if ( grp != NULL )
		return 1;
		
	/*alpha.jack.20100712+ save gid for quota*/
	if (( fp = fopen("/etc/gid","r") ) != NULL )
	{
		fscanf(fp,"%d",&last_gid);	
		fclose(fp);
		#ifdef DBG
		printf("last gid = %d\n",last_gid);
		#endif
		g->gr_gid = last_gid;
	}
	else
	{
		etc_group = xfopen(filename, "r");
		/*alpha.jack.20100712+ end */
	
		/* make sure gr_name isn't taken, make sure gid is kosher */
		desired = g->gr_gid;
		while ((grp = fgetgrent(etc_group))) 
		{
			if ((strcmp(grp->gr_name, g->gr_name)) == 0) {
				error_msg_and_die("%s: group already in use\n", g->gr_name);
			}
//jack20060507-
//			if ((desired) && grp->gr_gid == desired) {
//				error_msg_and_die("%d: gid has already been allocated\n",
//							  desired);
//			}

			if ((grp->gr_gid > g->gr_gid) && (grp->gr_gid < max)) {
				/* can not find /etc/gid file */
				g->gr_gid = grp->gr_gid+1;
				last_gid = g->gr_gid;
				
			}
		}
		
		if ( etc_group != NULL )
			fclose(etc_group);
	}
	
	if ((g->gr_gid >= max) || (g->gr_gid < min))
		g->gr_gid = min;
	
	while(1)
	{
		grp = getgrgid(last_gid);
		if ( grp == NULL )
			break;
		if ((last_gid >= max) || (last_gid < min))
			last_gid= min;
		else
			last_gid++;
	}
	
	g->gr_gid = last_gid;	
	
	/*alpha.jack.20100712+ save gid for quota*/
	if (( fp = fopen("/etc/gid","w+") ) != NULL )
	{
		fprintf(fp,"%d\n",g->gr_gid+1);	
		fclose(fp);
	}
	/*alpha.jack.20100712 end*/
	
	/* gid */
//	if (desired) {
//		g->gr_gid = desired;
//	} else {
//		g->gr_gid++;
//	}
	/* return 1; */
	return 0;
}

/* append a new user to the passwd file */
static int addgroup(const char *filename, char *group, gid_t gid, const char *user)
{
	FILE *etc_group;

#ifdef CONFIG_FEATURE_SHADOWPASSWDS
	FILE *etc_gshadow;
	char *gshadow = SHADOW_FILE;
#endif

	struct group gr;

	/* group:passwd:gid:userlist */
	static const char entryfmt[] = "%s:%s:%d:%s\n";

	/* make sure gid and group haven't already been allocated */
	gr.gr_gid = gid;
	gr.gr_name = group;
	if (group_study(filename, &gr))
		return 1;

	/* add entry to group */
	etc_group = xfopen(filename, "a");

	fprintf(etc_group, entryfmt, group, default_passwd, gr.gr_gid, user);
	fclose(etc_group);


#ifdef CONFIG_FEATURE_SHADOWPASSWDS
	/* add entry to gshadow if necessary */
	if (access(gshadow, F_OK|W_OK) == 0) {
		etc_gshadow = xfopen(gshadow, "a");
		fprintf(etc_gshadow, "%s:!::\n", group);
		fclose(etc_gshadow);
	}
#endif

	/* return 1; */
	return 0;
}

/*
 * addgroup will take a login_name as its first parameter.
 *
 * gid 
 *
 * can be customized via command-line parameters.
 * ________________________________________________________________________ */
int addgroup_main(int argc, char **argv)
{
	char *group;
	char *user;
	gid_t gid = 0;

	if (argc < 2) {
		show_usage();
	}

	if (strncmp(argv[1], "-g", 2) == 0) {
		gid = strtol(argv[2], NULL, 10);
		group = argv[2];
	} else {
		show_usage();
	}
	
	if (argc == 4) {
		user = argv[3];
	} else {
		user = "";
	}
		
	if (geteuid() != 0) {
		error_msg_and_die
			("Only root may add a group to the system.");
	}

	/* werk */
	return addgroup(GROUP_FILE, group, gid, user);
}

/* $Id: addgroup.c,v 1.4 2010/08/23 11:24:28 jack Exp $ */
