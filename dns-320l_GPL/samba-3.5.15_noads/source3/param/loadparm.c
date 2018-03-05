/* 
   Unix SMB/CIFS implementation.
   Parameter loading functions
   Copyright (C) Karl Auer 1993-1998

   Largely re-written by Andrew Tridgell, September 1994

   Copyright (C) Simo Sorce 2001
   Copyright (C) Alexander Bokovoy 2002
   Copyright (C) Stefan (metze) Metzmacher 2002
   Copyright (C) Jim McDonough <jmcd@us.ibm.com> 2003
   Copyright (C) Michael Adam 2008
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *  Load parameters.
 *
 *  This module provides suitable callback functions for the params
 *  module. It builds the internal table of service details which is
 *  then used by the rest of the server.
 *
 * To add a parameter:
 *
 * 1) add it to the global or service structure definition
 * 2) add it to the parm_table
 * 3) add it to the list of available functions (eg: using FN_GLOBAL_STRING())
 * 4) If it's a global then initialise it in init_globals. If a local
 *    (ie. service) parameter then initialise it in the sDefault structure
 *  
 *
 * Notes:
 *   The configuration file is processed sequentially for speed. It is NOT
 *   accessed randomly as happens in 'real' Windows. For this reason, there
 *   is a fair bit of sequence-dependent code here - ie., code which assumes
 *   that certain things happen before others. In particular, the code which
 *   happens at the boundary between sections is delicately poised, so be
 *   careful!
 *
 */

#include "includes.h"
#include "printing.h"

#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

#ifdef HAVE_HTTPCONNECTENCRYPT
#include <cups/http.h>
#endif

bool bLoaded = False;

extern userdom_struct current_user_info;

#ifndef GLOBAL_NAME
#define GLOBAL_NAME "global"
#endif

#ifndef PRINTERS_NAME
#define PRINTERS_NAME "printers"
#endif

#ifndef HOMES_NAME
#define HOMES_NAME "homes"
#endif

/* the special value for the include parameter
 * to be interpreted not as a file name but to
 * trigger loading of the global smb.conf options
 * from registry. */
#ifndef INCLUDE_REGISTRY_NAME
#define INCLUDE_REGISTRY_NAME "registry"
#endif

static bool in_client = False;		/* Not in the client by default */
static struct smbconf_csn conf_last_csn;

#define CONFIG_BACKEND_FILE 0
#define CONFIG_BACKEND_REGISTRY 1

static int config_backend = CONFIG_BACKEND_FILE;

/* some helpful bits */
#define LP_SNUM_OK(i) (((i) >= 0) && ((i) < iNumServices) && (ServicePtrs != NULL) && ServicePtrs[(i)]->valid)
#define VALID(i) (ServicePtrs != NULL && ServicePtrs[i]->valid)

#define USERSHARE_VALID 1
#define USERSHARE_PENDING_DELETE 2

static bool defaults_saved = False;

struct param_opt_struct {
	struct param_opt_struct *prev, *next;
	char *key;
	char *value;
	char **list;
};

/*
 * This structure describes global (ie., server-wide) parameters.
 */
struct global {
	int ConfigBackend;
	char *smb_ports;
	char *dos_charset;
	char *unix_charset;
	char *display_charset;
	char *szPrintcapname;
	char *szAddPortCommand;
	char *szEnumPortsCommand;
	char *szAddPrinterCommand;
	char *szDeletePrinterCommand;
	char *szOs2DriverMap;
	char *szLockDir;
	char *szStateDir;
	char *szCacheDir;
	char *szPidDir;
	char *szRootdir;
	char *szDefaultService;
	char *szGetQuota;
	char *szSetQuota;
	char *szMsgCommand;
	char *szServerString;
	char *szAutoServices;
	char *szPasswdProgram;
	char *szPasswdChat;
	char *szLogFile;
	char *szConfigFile;
	char *szSMBPasswdFile;
	char *szPrivateDir;
	char *szPassdbBackend;
	char **szPreloadModules;
	char *szPasswordServer;
	char *szSocketOptions;
	char *szRealm;
	char *szAfsUsernameMap;
	int iAfsTokenLifetime;
	char *szLogNtTokenCommand;
	char *szUsernameMap;
	char *szLogonScript;
	char *szLogonPath;
	char *szLogonDrive;
	char *szLogonHome;
	char **szWINSservers;
	char **szInterfaces;
	char *szRemoteAnnounce;
	char *szRemoteBrowseSync;
	char *szSocketAddress;
	bool bNmbdBindExplicitBroadcast;
	char *szNISHomeMapName;
	char *szAnnounceVersion;	/* This is initialised in init_globals */
	char *szWorkgroup;
	char *szNetbiosName;
	char **szNetbiosAliases;
	char *szNetbiosScope;
	char *szNameResolveOrder;
	char *szPanicAction;
	char *szAddUserScript;
	char *szRenameUserScript;
	char *szDelUserScript;
	char *szAddGroupScript;
	char *szDelGroupScript;
	char *szAddUserToGroupScript;
	char *szDelUserFromGroupScript;
	char *szSetPrimaryGroupScript;
	char *szAddMachineScript;
	char *szShutdownScript;
	char *szAbortShutdownScript;
	char *szUsernameMapScript;
	char *szCheckPasswordScript;
	char *szWINSHook;
	char *szUtmpDir;
	char *szWtmpDir;
	bool bUtmp;
	char *szIdmapUID;
	char *szIdmapGID;
	bool bPassdbExpandExplicit;
	int AlgorithmicRidBase;
	char *szTemplateHomedir;
	char *szTemplateShell;
	char *szWinbindSeparator;
	bool bWinbindEnumUsers;
	bool bWinbindEnumGroups;
	bool bWinbindUseDefaultDomain;
	bool bWinbindTrustedDomainsOnly;
	bool bWinbindNestedGroups;
	int  winbind_expand_groups;
	bool bWinbindRefreshTickets;
	bool bWinbindOfflineLogon;
	bool bWinbindNormalizeNames;
	bool bWinbindRpcOnly;
	bool bCreateKrb5Conf;
	char *szIdmapBackend;
	char *szIdmapAllocBackend;
	char *szAddShareCommand;
	char *szChangeShareCommand;
	char *szDeleteShareCommand;
	char **szEventLogs;
	char *szGuestaccount;
	char *szManglingMethod;
	char **szServicesList;
	char *szUsersharePath;
	char *szUsershareTemplateShare;
	char **szUsersharePrefixAllowList;
	char **szUsersharePrefixDenyList;
	int mangle_prefix;
	int max_log_size;
	char *szLogLevel;
	int max_xmit;
	int max_mux;
	int max_open_files;
	int open_files_db_hash_size;
	int pwordlevel;
	int unamelevel;
	int deadtime;
	bool getwd_cache;
	int maxprotocol;
	int minprotocol;
	int security;
	char **AuthMethods;
	bool paranoid_server_security;
	int maxdisksize;
	int lpqcachetime;
	int iMaxSmbdProcesses;
	bool bDisableSpoolss;
	int syslog;
	int os_level;
	bool enhanced_browsing;
	int max_ttl;
	int max_wins_ttl;
	int min_wins_ttl;
	int lm_announce;
	int lm_interval;
	int announce_as;	/* This is initialised in init_globals */
	int machine_password_timeout;
	int map_to_guest;
	int oplock_break_wait_time;
	int winbind_cache_time;
	int winbind_reconnect_delay;
	int winbind_max_clients;
	char **szWinbindNssInfo;
	int iLockSpinTime;
	char *szLdapMachineSuffix;
	char *szLdapUserSuffix;
	char *szLdapIdmapSuffix;
	char *szLdapGroupSuffix;
	int ldap_ssl;
	bool ldap_ssl_ads;
	int ldap_deref;
	int ldap_follow_referral;
	char *szLdapSuffix;
	char *szLdapAdminDn;
	int ldap_debug_level;
	int ldap_debug_threshold;
	int iAclCompat;
	char *szCupsServer;
	int CupsEncrypt;
	char *szIPrintServer;
	char *ctdbdSocket;
	char **szClusterAddresses;
	bool clustering;
	int ctdb_timeout;
	int ldap_passwd_sync;
	int ldap_replication_sleep;
	int ldap_timeout; /* This is initialised in init_globals */
	int ldap_connection_timeout;
	int ldap_page_size;
	bool ldap_delete_dn;
	bool bMsAddPrinterWizard;
	bool bDNSproxy;
	bool bWINSsupport;
	bool bWINSproxy;
	bool bLocalMaster;
	int  iPreferredMaster;
	int iDomainMaster;
	bool bDomainLogons;
	char **szInitLogonDelayedHosts;
	int InitLogonDelay;
	bool bEncryptPasswords;
	bool bUpdateEncrypt;
	int  clientSchannel;
	int  serverSchannel;
	bool bNullPasswords;
	bool bObeyPamRestrictions;
	bool bLoadPrinters;
	int PrintcapCacheTime;
	bool bLargeReadwrite;
	bool bReadRaw;
	bool bWriteRaw;
	bool bSyslogOnly;
	bool bBrowseList;
	bool bNISHomeMap;
	bool bTimeServer;
	bool bBindInterfacesOnly;
	bool bPamPasswordChange;
	bool bUnixPasswdSync;
	bool bPasswdChatDebug;
	int iPasswdChatTimeout;
	bool bTimestampLogs;
	bool bNTSmbSupport;
	bool bNTPipeSupport;
	bool bNTStatusSupport;
	bool bStatCache;
	int iMaxStatCacheSize;
	bool bKernelOplocks;
	bool bAllowTrustedDomains;
	bool bLanmanAuth;
	bool bNTLMAuth;
	bool bUseSpnego;
	bool bClientLanManAuth;
	bool bClientNTLMv2Auth;
	bool bClientPlaintextAuth;
	bool bClientUseSpnego;
	bool client_use_spnego_principal;
	bool bDebugPrefixTimestamp;
	bool bDebugHiresTimestamp;
	bool bDebugPid;
	bool bDebugUid;
	bool bDebugClass;
	bool bEnableCoreFiles;
	bool bHostMSDfs;
	bool bUseMmap;
	bool bHostnameLookups;
	bool bUnixExtensions;
	bool bDisableNetbios;
	char * szDedicatedKeytabFile;
	int  iKerberosMethod;
	bool bDeferSharingViolations;
	bool bEnablePrivileges;
	bool bASUSupport;
	bool bUsershareOwnerOnly;
	bool bUsershareAllowGuests;
	bool bRegistryShares;
	int restrict_anonymous;
	int name_cache_timeout;
	int client_signing;
	int server_signing;
	int client_ldap_sasl_wrapping;
	int iUsershareMaxShares;
	int iIdmapCacheTime;
	int iIdmapNegativeCacheTime;
	bool bResetOnZeroVC;
	int iKeepalive;
	int iminreceivefile;
	struct param_opt_struct *param_opt;
	int cups_connection_timeout;
	char *szSMBPerfcountModule;
	bool bMapUntrustedToDomain;
};

static struct global Globals;

/*
 * This structure describes a single service.
 */
struct service {
	bool valid;
	bool autoloaded;
	int usershare;
	struct timespec usershare_last_mod;
	char *szService;
	char *szPath;
	char *szUsername;
	char **szInvalidUsers;
	char **szValidUsers;
	char **szAdminUsers;
	char *szCopy;
	char *szInclude;
	char *szPreExec;
	char *szPostExec;
	char *szRootPreExec;
	char *szRootPostExec;
	char *szCupsOptions;
	char *szPrintcommand;
	char *szLpqcommand;
	char *szLprmcommand;
	char *szLppausecommand;
	char *szLpresumecommand;
	char *szQueuepausecommand;
	char *szQueueresumecommand;
	char *szPrintername;
	char *szPrintjobUsername;
	char *szDontdescend;
	char **szHostsallow;
	char **szHostsdeny;
	char *szMagicScript;
	char *szMagicOutput;
	char *szVetoFiles;
	char *szHideFiles;
	char *szVetoOplockFiles;
	char *comment;
	char *force_user;
	char *force_group;
	char **readlist;
	char **writelist;
	char **printer_admin;
	char *volume;
	char *fstype;
	char **szVfsObjects;
	char *szMSDfsProxy;
	char *szAioWriteBehind;
	char *szDfree;
	int iMinPrintSpace;
	int iMaxPrintJobs;
	int iMaxReportedPrintJobs;
	int iWriteCacheSize;
	int iCreate_mask;
	int iCreate_force_mode;
	int iSecurity_mask;
	int iSecurity_force_mode;
	int iDir_mask;
	int iDir_force_mode;
	int iDir_Security_mask;
	int iDir_Security_force_mode;
	int iMaxConnections;
	int iDefaultCase;
	int iPrinting;
	int iOplockContentionLimit;
	int iCSCPolicy;
	int iBlock_size;
	int iDfreeCacheTime;
	bool bPreexecClose;
	bool bRootpreexecClose;
	int  iCaseSensitive;
	bool bCasePreserve;
	bool bShortCasePreserve;
	bool bHideDotFiles;
	bool bHideSpecialFiles;
	bool bHideUnReadable;
	bool bHideUnWriteableFiles;
	bool bBrowseable;
	bool bAccessBasedShareEnum;
	bool bAvailable;
	bool bRead_only;
	bool bNo_set_dir;
	bool bGuest_only;
	bool bAdministrative_share;
	bool bGuest_ok;
	bool bPrint_ok;
	bool bMap_system;
	bool bMap_hidden;
	bool bMap_archive;
	bool bStoreDosAttributes;
	bool bDmapiSupport;
	bool bLocking;
	int iStrictLocking;
	bool bPosixLocking;
	bool bShareModes;
	bool bOpLocks;
	bool bLevel2OpLocks;
	bool bOnlyUser;
	bool bMangledNames;
	bool bWidelinks;
	bool bSymlinks;
	bool bSyncAlways;
	bool bStrictAllocate;
	bool bStrictSync;
	char magic_char;
	struct bitmap *copymap;
	bool bDeleteReadonly;
	bool bFakeOplocks;
	bool bDeleteVetoFiles;
	bool bDosFilemode;
	bool bDosFiletimes;
	bool bDosFiletimeResolution;
	bool bFakeDirCreateTimes;
	bool bBlockingLocks;
	bool bInheritPerms;
	bool bInheritACLS;
	bool bInheritOwner;
	bool bMSDfsRoot;
	bool bUseClientDriver;
	bool bDefaultDevmode;
	bool bForcePrintername;
	bool bNTAclSupport;
	bool bForceUnknownAclUser;
	bool bUseSendfile;
	bool bProfileAcls;
	bool bMap_acl_inherit;
	bool bAfs_Share;
	bool bEASupport;
	bool bAclCheckPermissions;
	bool bAclMapFullControl;
	bool bAclGroupControl;
	bool bChangeNotify;
	bool bKernelChangeNotify;
	int iallocation_roundup_size;
	int iAioReadSize;
	int iAioWriteSize;
	int iMap_readonly;
	int iDirectoryNameCacheSize;
	int ismb_encrypt;
	struct param_opt_struct *param_opt;

	char dummy[3];		/* for alignment */
};


/* This is a default service used to prime a services structure */
static struct service sDefault = {
	True,			/* valid */
	False,			/* not autoloaded */
	0,			/* not a usershare */
	{0, },                  /* No last mod time */
	NULL,			/* szService */
	NULL,			/* szPath */
	NULL,			/* szUsername */
	NULL,			/* szInvalidUsers */
	NULL,			/* szValidUsers */
	NULL,			/* szAdminUsers */
	NULL,			/* szCopy */
	NULL,			/* szInclude */
	NULL,			/* szPreExec */
	NULL,			/* szPostExec */
	NULL,			/* szRootPreExec */
	NULL,			/* szRootPostExec */
	NULL,			/* szCupsOptions */
	NULL,			/* szPrintcommand */
	NULL,			/* szLpqcommand */
	NULL,			/* szLprmcommand */
	NULL,			/* szLppausecommand */
	NULL,			/* szLpresumecommand */
	NULL,			/* szQueuepausecommand */
	NULL,			/* szQueueresumecommand */
	NULL,			/* szPrintername */
	NULL,			/* szPrintjobUsername */
	NULL,			/* szDontdescend */
	NULL,			/* szHostsallow */
	NULL,			/* szHostsdeny */
	NULL,			/* szMagicScript */
	NULL,			/* szMagicOutput */
	NULL,			/* szVetoFiles */
	NULL,			/* szHideFiles */
	NULL,			/* szVetoOplockFiles */
	NULL,			/* comment */
	NULL,			/* force user */
	NULL,			/* force group */
	NULL,			/* readlist */
	NULL,			/* writelist */
	NULL,			/* printer admin */
	NULL,			/* volume */
	NULL,			/* fstype */
	NULL,			/* vfs objects */
	NULL,                   /* szMSDfsProxy */
	NULL,			/* szAioWriteBehind */
	NULL,			/* szDfree */
	0,			/* iMinPrintSpace */
	1000,			/* iMaxPrintJobs */
	0,			/* iMaxReportedPrintJobs */
	0,			/* iWriteCacheSize */
	0777,			/* iCreate_mask */         //royc 20110112 0744
	0777,			/* iCreate_force_mode */   //royc 20110112 0000
	0777,			/* iSecurity_mask */
	0,			/* iSecurity_force_mode */
	0777,			/* iDir_mask */            //royc 20110112 0755
	0777,			/* iDir_force_mode */      //royc 20110112 0000
	0777,			/* iDir_Security_mask */
	0,			/* iDir_Security_force_mode */
	0,			/* iMaxConnections */
	CASE_LOWER,		/* iDefaultCase */
	DEFAULT_PRINTING,	/* iPrinting */
	2,			/* iOplockContentionLimit */
	0,			/* iCSCPolicy */
	1024,           	/* iBlock_size */
	0,			/* iDfreeCacheTime */
	False,			/* bPreexecClose */
	False,			/* bRootpreexecClose */
	Auto,			/* case sensitive */
	True,			/* case preserve */
	True,			/* short case preserve */
	True,			/* bHideDotFiles */
	False,			/* bHideSpecialFiles */
	True,			/* bHideUnReadable */         //royc 20110112 False
	False,			/* bHideUnWriteableFiles */
	True,			/* bBrowseable */
	False,			/* bAccessBasedShareEnum */
	True,			/* bAvailable */
	True,			/* bRead_only */
	True,			/* bNo_set_dir */
	False,			/* bGuest_only */
	False,			/* bAdministrative_share */
	False,			/* bGuest_ok */
	False,			/* bPrint_ok */
	False,			/* bMap_system */
	False,			/* bMap_hidden */
	True,			/* bMap_archive */
	False,			/* bStoreDosAttributes */
	False,			/* bDmapiSupport */
	True,			/* bLocking */
	Auto,			/* iStrictLocking */
	True,			/* bPosixLocking */
	True,			/* bShareModes */
	True,			/* bOpLocks */
	True,			/* bLevel2OpLocks */
	False,			/* bOnlyUser */
	True,			/* bMangledNames */
	False,			/* bWidelinks */
	False,			/* bSymlinks */         //royc 20110112  True
	False,			/* bSyncAlways */
	False,			/* bStrictAllocate */
	False,			/* bStrictSync */
	'~',			/* magic char */
	NULL,			/* copymap */
	False,			/* bDeleteReadonly */
	False,			/* bFakeOplocks */
	False,			/* bDeleteVetoFiles */
	False,			/* bDosFilemode */
	True,			/* bDosFiletimes */
	False,			/* bDosFiletimeResolution */
	False,			/* bFakeDirCreateTimes */
	True,			/* bBlockingLocks */
	False,			/* bInheritPerms */
	False,			/* bInheritACLS */
	False,			/* bInheritOwner */
	False,			/* bMSDfsRoot */
	False,			/* bUseClientDriver */
	True,			/* bDefaultDevmode */
	False,			/* bForcePrintername */
	True,			/* bNTAclSupport */
	False,                  /* bForceUnknownAclUser */
	True,			/* bUseSendfile */         //royc 20110112 False
	False,			/* bProfileAcls */
	False,			/* bMap_acl_inherit */
	False,			/* bAfs_Share */
	False,			/* bEASupport */
	True,			/* bAclCheckPermissions */
	True,			/* bAclMapFullControl */
	False,			/* bAclGroupControl */
	True,			/* bChangeNotify */
	True,			/* bKernelChangeNotify */
	SMB_ROUNDUP_ALLOCATION_SIZE,		/* iallocation_roundup_size */
	0,			/* iAioReadSize */
	0,			/* iAioWriteSize */
	MAP_READONLY_YES,	/* iMap_readonly */
#ifdef BROKEN_DIRECTORY_HANDLING
	0,			/* iDirectoryNameCacheSize */
#else
	100,			/* iDirectoryNameCacheSize */
#endif
	Auto,			/* ismb_encrypt */
	NULL,			/* Parametric options */

	""			/* dummy */
};

/* local variables */
static struct service **ServicePtrs = NULL;
static int iNumServices = 0;
static int iServiceIndex = 0;
static struct db_context *ServiceHash;
static int *invalid_services = NULL;
static int num_invalid_services = 0;
static bool bInGlobalSection = True;
static bool bGlobalOnly = False;
static int server_role;
static int default_server_announce;

#define NUMPARAMETERS (sizeof(parm_table) / sizeof(struct parm_struct))

/* prototypes for the special type handlers */
static bool handle_include( int snum, const char *pszParmValue, char **ptr);
static bool handle_copy( int snum, const char *pszParmValue, char **ptr);
static bool handle_netbios_name( int snum, const char *pszParmValue, char **ptr);
static bool handle_idmap_uid( int snum, const char *pszParmValue, char **ptr);
static bool handle_idmap_gid( int snum, const char *pszParmValue, char **ptr);
static bool handle_debug_list( int snum, const char *pszParmValue, char **ptr );
static bool handle_workgroup( int snum, const char *pszParmValue, char **ptr );
static bool handle_netbios_aliases( int snum, const char *pszParmValue, char **ptr );
static bool handle_netbios_scope( int snum, const char *pszParmValue, char **ptr );
static bool handle_charset( int snum, const char *pszParmValue, char **ptr );
static bool handle_printing( int snum, const char *pszParmValue, char **ptr);
static bool handle_ldap_debug_level( int snum, const char *pszParmValue, char **ptr);

static void set_server_role(void);
static void set_default_server_announce_type(void);
static void set_allowed_client_auth(void);

static void *lp_local_ptr(struct service *service, void *ptr);

static void add_to_file_list(const char *fname, const char *subfname);

static const struct enum_list enum_protocol[] = {
	{PROTOCOL_SMB2, "SMB2"},
	{PROTOCOL_NT1, "NT1"},
	{PROTOCOL_LANMAN2, "LANMAN2"},
	{PROTOCOL_LANMAN1, "LANMAN1"},
	{PROTOCOL_CORE, "CORE"},
	{PROTOCOL_COREPLUS, "COREPLUS"},
	{PROTOCOL_COREPLUS, "CORE+"},
	{-1, NULL}
};

static const struct enum_list enum_security[] = {
	{SEC_SHARE, "SHARE"},
	{SEC_USER, "USER"},
	{SEC_SERVER, "SERVER"},
	{SEC_DOMAIN, "DOMAIN"},
#ifdef HAVE_ADS
	{SEC_ADS, "ADS"},
#endif
	{-1, NULL}
};

static const struct enum_list enum_printing[] = {
	{PRINT_SYSV, "sysv"},
	{PRINT_AIX, "aix"},
	{PRINT_HPUX, "hpux"},
	{PRINT_BSD, "bsd"},
	{PRINT_QNX, "qnx"},
	{PRINT_PLP, "plp"},
	{PRINT_LPRNG, "lprng"},
	{PRINT_CUPS, "cups"},
	{PRINT_IPRINT, "iprint"},
	{PRINT_LPRNT, "nt"},
	{PRINT_LPROS2, "os2"},
#ifdef DEVELOPER
	{PRINT_TEST, "test"},
	{PRINT_VLP, "vlp"},
#endif /* DEVELOPER */
	{-1, NULL}
};

static const struct enum_list enum_ldap_sasl_wrapping[] = {
	{0, "plain"},
	{ADS_AUTH_SASL_SIGN, "sign"},
	{ADS_AUTH_SASL_SEAL, "seal"},
	{-1, NULL}
};

static const struct enum_list enum_ldap_ssl[] = {
	{LDAP_SSL_OFF, "no"},
	{LDAP_SSL_OFF, "off"},
	{LDAP_SSL_START_TLS, "start tls"},
	{LDAP_SSL_START_TLS, "start_tls"},
	{-1, NULL}
};

/* LDAP Dereferencing Alias types */
#define SAMBA_LDAP_DEREF_NEVER		0
#define SAMBA_LDAP_DEREF_SEARCHING	1
#define SAMBA_LDAP_DEREF_FINDING	2
#define SAMBA_LDAP_DEREF_ALWAYS		3

static const struct enum_list enum_ldap_deref[] = {
	{SAMBA_LDAP_DEREF_NEVER, "never"},
	{SAMBA_LDAP_DEREF_SEARCHING, "searching"},
	{SAMBA_LDAP_DEREF_FINDING, "finding"},
	{SAMBA_LDAP_DEREF_ALWAYS, "always"},
	{-1, "auto"}
};

static const struct enum_list enum_ldap_passwd_sync[] = {
	{LDAP_PASSWD_SYNC_OFF, "no"},
	{LDAP_PASSWD_SYNC_OFF, "off"},
	{LDAP_PASSWD_SYNC_ON, "yes"},
	{LDAP_PASSWD_SYNC_ON, "on"},
	{LDAP_PASSWD_SYNC_ONLY, "only"},
	{-1, NULL}
};

/* Types of machine we can announce as. */
#define ANNOUNCE_AS_NT_SERVER 1
#define ANNOUNCE_AS_WIN95 2
#define ANNOUNCE_AS_WFW 3
#define ANNOUNCE_AS_NT_WORKSTATION 4

static const struct enum_list enum_announce_as[] = {
	{ANNOUNCE_AS_NT_SERVER, "NT"},
	{ANNOUNCE_AS_NT_SERVER, "NT Server"},
	{ANNOUNCE_AS_NT_WORKSTATION, "NT Workstation"},
	{ANNOUNCE_AS_WIN95, "win95"},
	{ANNOUNCE_AS_WFW, "WfW"},
	{-1, NULL}
};

static const struct enum_list enum_map_readonly[] = {
	{MAP_READONLY_NO, "no"},
	{MAP_READONLY_NO, "false"},
	{MAP_READONLY_NO, "0"},
	{MAP_READONLY_YES, "yes"},
	{MAP_READONLY_YES, "true"},
	{MAP_READONLY_YES, "1"},
	{MAP_READONLY_PERMISSIONS, "permissions"},
	{MAP_READONLY_PERMISSIONS, "perms"},
	{-1, NULL}
};

static const struct enum_list enum_case[] = {
	{CASE_LOWER, "lower"},
	{CASE_UPPER, "upper"},
	{-1, NULL}
};



static const struct enum_list enum_bool_auto[] = {
	{False, "No"},
	{False, "False"},
	{False, "0"},
	{True, "Yes"},
	{True, "True"},
	{True, "1"},
	{Auto, "Auto"},
	{-1, NULL}
};

/* Client-side offline caching policy types */
#define CSC_POLICY_MANUAL 0
#define CSC_POLICY_DOCUMENTS 1
#define CSC_POLICY_PROGRAMS 2
#define CSC_POLICY_DISABLE 3

static const struct enum_list enum_csc_policy[] = {
	{CSC_POLICY_MANUAL, "manual"},
	{CSC_POLICY_DOCUMENTS, "documents"},
	{CSC_POLICY_PROGRAMS, "programs"},
	{CSC_POLICY_DISABLE, "disable"},
	{-1, NULL}
};

/* SMB signing types. */
static const struct enum_list enum_smb_signing_vals[] = {
	{False, "No"},
	{False, "False"},
	{False, "0"},
	{False, "Off"},
	{False, "disabled"},
	{True, "Yes"},
	{True, "True"},
	{True, "1"},
	{True, "On"},
	{True, "enabled"},
	{Auto, "auto"},
	{Required, "required"},
	{Required, "mandatory"},
	{Required, "force"},
	{Required, "forced"},
	{Required, "enforced"},
	{-1, NULL}
};

/* ACL compatibility options. */
static const struct enum_list enum_acl_compat_vals[] = {
    { ACL_COMPAT_AUTO, "auto" },
    { ACL_COMPAT_WINNT, "winnt" },
    { ACL_COMPAT_WIN2K, "win2k" },
    { -1, NULL}
};

/* 
   Do you want session setups at user level security with a invalid
   password to be rejected or allowed in as guest? WinNT rejects them
   but it can be a pain as it means "net view" needs to use a password

   You have 3 choices in the setting of map_to_guest:

   "Never" means session setups with an invalid password
   are rejected. This is the default.

   "Bad User" means session setups with an invalid password
   are rejected, unless the username does not exist, in which case it
   is treated as a guest login

   "Bad Password" means session setups with an invalid password
   are treated as a guest login

   Note that map_to_guest only has an effect in user or server
   level security.
*/

static const struct enum_list enum_map_to_guest[] = {
	{NEVER_MAP_TO_GUEST, "Never"},
	{MAP_TO_GUEST_ON_BAD_USER, "Bad User"},
	{MAP_TO_GUEST_ON_BAD_PASSWORD, "Bad Password"},
        {MAP_TO_GUEST_ON_BAD_UID, "Bad Uid"},
	{-1, NULL}
};

/* Config backend options */

static const struct enum_list enum_config_backend[] = {
	{CONFIG_BACKEND_FILE, "file"},
	{CONFIG_BACKEND_REGISTRY, "registry"},
	{-1, NULL}
};

/* ADS kerberos ticket verification options */

static const struct enum_list enum_kerberos_method[] = {
	{KERBEROS_VERIFY_SECRETS, "default"},
	{KERBEROS_VERIFY_SECRETS, "secrets only"},
	{KERBEROS_VERIFY_SYSTEM_KEYTAB, "system keytab"},
	{KERBEROS_VERIFY_DEDICATED_KEYTAB, "dedicated keytab"},
	{KERBEROS_VERIFY_SECRETS_AND_KEYTAB, "secrets and keytab"},
	{-1, NULL}
};

/* Note: We do not initialise the defaults union - it is not allowed in ANSI C
 *
 * The FLAG_HIDE is explicit. Parameters set this way do NOT appear in any edit
 * screen in SWAT. This is used to exclude parameters as well as to squash all
 * parameters that have been duplicated by pseudonyms.
 *
 * NOTE: To display a parameter in BASIC view set FLAG_BASIC
 *       Any parameter that does NOT have FLAG_ADVANCED will not disply at all
 *	 Set FLAG_SHARE and FLAG_PRINT to specifically display parameters in
 *        respective views.
 *
 * NOTE2: Handling of duplicated (synonym) parameters:
 *	Only the first occurance of a parameter should be enabled by FLAG_BASIC
 *	and/or FLAG_ADVANCED. All duplicates following the first mention should be
 *	set to FLAG_HIDE. ie: Make you must place the parameter that has the preferred
 *	name first, and all synonyms must follow it with the FLAG_HIDE attribute.
 */

static struct parm_struct parm_table[] = {
	{N_("Base Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "dos charset",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.dos_charset,
		.special	= handle_charset,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED
	},
	{
		.label		= "unix charset",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.unix_charset,
		.special	= handle_charset,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED
	},
	{
		.label		= "display charset",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.display_charset,
		.special	= handle_charset,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED
	},
	{
		.label		= "comment",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.comment,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT
	},
	{
		.label		= "path",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPath,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "directory",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPath,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "workgroup",
		.type		= P_USTRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szWorkgroup,
		.special	= handle_workgroup,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
#ifdef WITH_ADS
	{
		.label		= "realm",
		.type		= P_USTRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRealm,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
#endif
	{
		.label		= "netbios name",
		.type		= P_USTRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szNetbiosName,
		.special	= handle_netbios_name,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "netbios aliases",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szNetbiosAliases,
		.special	= handle_netbios_aliases,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "netbios scope",
		.type		= P_USTRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szNetbiosScope,
		.special	= handle_netbios_scope,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "server string",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szServerString,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "interfaces",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szInterfaces,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "bind interfaces only",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bBindInterfacesOnly,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "config backend",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ConfigBackend,
		.special	= NULL,
		.enum_list	= enum_config_backend,
		.flags		= FLAG_HIDE|FLAG_ADVANCED|FLAG_META,
	},

	{N_("Security Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "security",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.security,
		.special	= NULL,
		.enum_list	= enum_security,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "auth methods",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.AuthMethods,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "encrypt passwords",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bEncryptPasswords,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "update encrypted",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUpdateEncrypt,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client schannel",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.clientSchannel,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "server schannel",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.serverSchannel,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "allow trusted domains",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bAllowTrustedDomains,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "map to guest",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.map_to_guest,
		.special	= NULL,
		.enum_list	= enum_map_to_guest,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "null passwords",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bNullPasswords,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "obey pam restrictions",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bObeyPamRestrictions,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "password server",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPasswordServer,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "smb passwd file",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szSMBPasswdFile,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "private dir",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPrivateDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "passdb backend",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPassdbBackend,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "algorithmic rid base",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.AlgorithmicRidBase,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "root directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRootdir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "root dir",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRootdir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "root",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRootdir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "guest account",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szGuestaccount,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "enable privileges",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bEnablePrivileges,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{
		.label		= "pam password change",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bPamPasswordChange,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "passwd program",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPasswdProgram,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "passwd chat",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPasswdChat,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "passwd chat debug",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bPasswdChatDebug,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "passwd chat timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iPasswdChatTimeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "check password script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szCheckPasswordScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "username map",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUsernameMap,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "password level",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.pwordlevel,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "username level",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.unamelevel,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "unix password sync",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUnixPasswdSync,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "restrict anonymous",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.restrict_anonymous,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "lanman auth",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bLanmanAuth,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ntlm auth",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bNTLMAuth,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client NTLMv2 auth",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bClientNTLMv2Auth,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client lanman auth",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bClientLanManAuth,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client plaintext auth",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bClientPlaintextAuth,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client use spnego principal",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.client_use_spnego_principal,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "username",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szUsername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "user",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szUsername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "users",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szUsername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "invalid users",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szInvalidUsers,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "valid users",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szValidUsers,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "admin users",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szAdminUsers,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "read list",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.readlist,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "write list",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.writelist,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "printer admin",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.printer_admin,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_PRINT | FLAG_DEPRECATED,
	},
	{
		.label		= "force user",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.force_user,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "force group",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.force_group,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "group",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.force_group,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "read only",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bRead_only,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "write ok",
		.type		= P_BOOLREV,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bRead_only,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "writeable",
		.type		= P_BOOLREV,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bRead_only,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "writable",
		.type		= P_BOOLREV,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bRead_only,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "acl check permissions",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAclCheckPermissions,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "acl group control",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAclGroupControl,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "acl map full control",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAclMapFullControl,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "create mask",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iCreate_mask,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "create mode",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iCreate_mask,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "force create mode",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iCreate_force_mode,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "security mask",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iSecurity_mask,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "force security mode",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iSecurity_force_mode,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "directory mask",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDir_mask,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "directory mode",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDir_mask,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "force directory mode",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDir_force_mode,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "directory security mask",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDir_Security_mask,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "force directory security mode",
		.type		= P_OCTAL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDir_Security_force_mode,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "force unknown acl user",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bForceUnknownAclUser,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "inherit permissions",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bInheritPerms,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "inherit acls",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bInheritACLS,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "inherit owner",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bInheritOwner,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "guest only",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bGuest_only,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "only guest",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bGuest_only,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "administrative share",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAdministrative_share,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},

	{
		.label		= "guest ok",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bGuest_ok,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "public",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bGuest_ok,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "only user",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bOnlyUser,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_DEPRECATED,
	},
	{
		.label		= "hosts allow",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szHostsallow,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_GLOBAL | FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "allow hosts",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szHostsallow,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "hosts deny",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szHostsdeny,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_GLOBAL | FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "deny hosts",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szHostsdeny,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "preload modules",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPreloadModules,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "dedicated keytab file",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDedicatedKeytabFile,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "kerberos method",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iKerberosMethod,
		.special	= NULL,
		.enum_list	= enum_kerberos_method,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "map untrusted to domain",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bMapUntrustedToDomain,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},


	{N_("Logging Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "log level",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogLevel,
		.special	= handle_debug_list,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debuglevel",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogLevel,
		.special	= handle_debug_list,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "syslog",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.syslog,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "syslog only",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bSyslogOnly,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "log file",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogFile,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max log size",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.max_log_size,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debug timestamp",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bTimestampLogs,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "timestamp logs",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bTimestampLogs,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debug prefix timestamp",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDebugPrefixTimestamp,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debug hires timestamp",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDebugHiresTimestamp,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debug pid",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDebugPid,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debug uid",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDebugUid,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "debug class",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDebugClass,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "enable core files",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bEnableCoreFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("Protocol Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "allocation roundup size",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iallocation_roundup_size,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "aio read size",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iAioReadSize,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "aio write size",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iAioWriteSize,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "aio write behind",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szAioWriteBehind,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "smb ports",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.smb_ports,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "large readwrite",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bLargeReadwrite,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max protocol",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.maxprotocol,
		.special	= NULL,
		.enum_list	= enum_protocol,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "protocol",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.maxprotocol,
		.special	= NULL,
		.enum_list	= enum_protocol,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "min protocol",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.minprotocol,
		.special	= NULL,
		.enum_list	= enum_protocol,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "min receivefile size",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iminreceivefile,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "read raw",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bReadRaw,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "write raw",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWriteRaw,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "disable netbios",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDisableNetbios,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "reset on zero vc",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bResetOnZeroVC,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "acl compatibility",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iAclCompat,
		.special	= NULL,
		.enum_list	= enum_acl_compat_vals,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "defer sharing violations",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDeferSharingViolations,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "ea support",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bEASupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "nt acl support",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bNTAclSupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "nt pipe support",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bNTPipeSupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "nt status support",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bNTStatusSupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "profile acls",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bProfileAcls,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},
	{
		.label		= "announce version",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAnnounceVersion,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "announce as",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.announce_as,
		.special	= NULL,
		.enum_list	= enum_announce_as,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "map acl inherit",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bMap_acl_inherit,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "afs share",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAfs_Share,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "max mux",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.max_mux,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max xmit",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.max_xmit,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "name resolve order",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szNameResolveOrder,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "max ttl",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.max_ttl,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max wins ttl",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.max_wins_ttl,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "min wins ttl",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.min_wins_ttl,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "time server",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bTimeServer,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "unix extensions",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUnixExtensions,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "use spnego",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUseSpnego,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client signing",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.client_signing,
		.special	= NULL,
		.enum_list	= enum_smb_signing_vals,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "server signing",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.server_signing,
		.special	= NULL,
		.enum_list	= enum_smb_signing_vals,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "smb encrypt",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.ismb_encrypt,
		.special	= NULL,
		.enum_list	= enum_smb_signing_vals,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client use spnego",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bClientUseSpnego,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "client ldap sasl wrapping",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.client_ldap_sasl_wrapping,
		.special	= NULL,
		.enum_list	= enum_ldap_sasl_wrapping,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "enable asu support",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bASUSupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "svcctl list",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szServicesList,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("Tuning Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "block size",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iBlock_size,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "deadtime",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.deadtime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "getwd cache",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.getwd_cache,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "keepalive",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iKeepalive,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "change notify",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bChangeNotify,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "directory name cache size",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDirectoryNameCacheSize,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "kernel change notify",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bKernelChangeNotify,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "lpq cache time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.lpqcachetime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max smbd processes",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iMaxSmbdProcesses,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max connections",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iMaxConnections,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "paranoid server security",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.paranoid_server_security,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max disk size",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.maxdisksize,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "max open files",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.max_open_files,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "min print space",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iMinPrintSpace,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "socket options",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szSocketOptions,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "strict allocate",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bStrictAllocate,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "strict sync",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bStrictSync,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "sync always",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bSyncAlways,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "use mmap",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUseMmap,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "use sendfile",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bUseSendfile,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "hostname lookups",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bHostnameLookups,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "write cache size",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iWriteCacheSize,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "name cache timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.name_cache_timeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ctdbd socket",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ctdbdSocket,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "cluster addresses",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szClusterAddresses,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "clustering",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.clustering,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "ctdb timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ctdb_timeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},

	{N_("Printing Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "max reported print jobs",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iMaxReportedPrintJobs,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "max print jobs",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iMaxPrintJobs,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "load printers",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bLoadPrinters,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "printcap cache time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.PrintcapCacheTime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "printcap name",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPrintcapname,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "printcap",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPrintcapname,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "printable",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bPrint_ok,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "print ok",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bPrint_ok,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "printing",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iPrinting,
		.special	= handle_printing,
		.enum_list	= enum_printing,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "cups options",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szCupsOptions,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "cups server",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szCupsServer,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label          = "cups encrypt",
		.type           = P_ENUM,
		.p_class        = P_GLOBAL,
		.ptr            = &Globals.CupsEncrypt,
		.special        = NULL,
		.enum_list      = enum_bool_auto,
		.flags          = FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{

		.label		= "cups connection timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.cups_connection_timeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "iprint server",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIPrintServer,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "print command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPrintcommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "disable spoolss",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDisableSpoolss,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "enable spoolss",
		.type		= P_BOOLREV,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDisableSpoolss,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "lpq command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szLpqcommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "lprm command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szLprmcommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "lppause command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szLppausecommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "lpresume command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szLpresumecommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "queuepause command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szQueuepausecommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "queueresume command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szQueueresumecommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT | FLAG_GLOBAL,
	},
	{
		.label		= "addport command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddPortCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "enumports command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szEnumPortsCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "addprinter command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddPrinterCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "deleteprinter command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDeletePrinterCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "show add printer wizard",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bMsAddPrinterWizard,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "os2 driver map",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szOs2DriverMap,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{
		.label		= "printer name",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPrintername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "printer",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPrintername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "use client driver",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bUseClientDriver,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "default devmode",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDefaultDevmode,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "force printername",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bForcePrintername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},
	{
		.label		= "printjob username",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPrintjobUsername,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_PRINT,
	},

	{N_("Filename Handling"), P_SEP, P_SEPARATOR},

	{
		.label		= "mangling method",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szManglingMethod,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "mangle prefix",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.mangle_prefix,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{
		.label		= "default case",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDefaultCase,
		.special	= NULL,
		.enum_list	= enum_case,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "case sensitive",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iCaseSensitive,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "casesignames",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iCaseSensitive,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL | FLAG_HIDE,
	},
	{
		.label		= "preserve case",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bCasePreserve,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "short preserve case",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bShortCasePreserve,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "mangling char",
		.type		= P_CHAR,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.magic_char,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "hide dot files",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bHideDotFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "hide special files",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bHideSpecialFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "hide unreadable",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bHideUnReadable,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "hide unwriteable files",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bHideUnWriteableFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "delete veto files",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDeleteVetoFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "veto files",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szVetoFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "hide files",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szHideFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "veto oplock files",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szVetoOplockFiles,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "map archive",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bMap_archive,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "map hidden",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bMap_hidden,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "map system",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bMap_system,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "map readonly",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iMap_readonly,
		.special	= NULL,
		.enum_list	= enum_map_readonly,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "mangled names",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bMangledNames,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "max stat cache size",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iMaxStatCacheSize,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "stat cache",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bStatCache,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "store dos attributes",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bStoreDosAttributes,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "dmapi support",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDmapiSupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},


	{N_("Domain Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "machine password timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.machine_password_timeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_WIZARD,
	},

	{N_("Logon Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "add user script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddUserScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "rename user script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRenameUserScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "delete user script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDelUserScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "add group script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddGroupScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "delete group script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDelGroupScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "add user to group script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddUserToGroupScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "delete user from group script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDelUserFromGroupScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "set primary group script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szSetPrimaryGroupScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "add machine script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddMachineScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "shutdown script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szShutdownScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "abort shutdown script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAbortShutdownScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "username map script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUsernameMapScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "logon script",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogonScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "logon path",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogonPath,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "logon drive",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogonDrive,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "logon home",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogonHome,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "domain logons",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDomainLogons,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{
		.label		= "init logon delayed hosts",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szInitLogonDelayedHosts,
		.special        = NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{
		.label		= "init logon delay",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.InitLogonDelay,
		.special        = NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,

	},

	{N_("Browse Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "os level",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.os_level,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "lm announce",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.lm_announce,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "lm interval",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.lm_interval,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "preferred master",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iPreferredMaster,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "prefered master",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iPreferredMaster,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "local master",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bLocalMaster,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "domain master",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iDomainMaster,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_BASIC | FLAG_ADVANCED,
	},
	{
		.label		= "browse list",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bBrowseList,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "browseable",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bBrowseable,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "browsable",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bBrowseable,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "access based share enum",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAccessBasedShareEnum,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE
	},
	{
		.label		= "enhanced browsing",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.enhanced_browsing,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("WINS Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "dns proxy",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bDNSproxy,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "wins proxy",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWINSproxy,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "wins server",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szWINSservers,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "wins support",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWINSsupport,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_WIZARD,
	},
	{
		.label		= "wins hook",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szWINSHook,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("Locking Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "blocking locks",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bBlockingLocks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "csc policy",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iCSCPolicy,
		.special	= NULL,
		.enum_list	= enum_csc_policy,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "fake oplocks",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bFakeOplocks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "kernel oplocks",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bKernelOplocks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "locking",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bLocking,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "lock spin time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iLockSpinTime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "oplocks",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bOpLocks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "level2 oplocks",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bLevel2OpLocks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "oplock break wait time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.oplock_break_wait_time,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "oplock contention limit",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iOplockContentionLimit,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "posix locking",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bPosixLocking,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "strict locking",
		.type		= P_ENUM,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iStrictLocking,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "share modes",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bShareModes,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL | FLAG_DEPRECATED,
	},

	{N_("Ldap Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "ldap admin dn",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLdapAdminDn,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap delete dn",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_delete_dn,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap group suffix",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLdapGroupSuffix,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap idmap suffix",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLdapIdmapSuffix,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap machine suffix",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLdapMachineSuffix,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap passwd sync",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_passwd_sync,
		.special	= NULL,
		.enum_list	= enum_ldap_passwd_sync,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap password sync",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_passwd_sync,
		.special	= NULL,
		.enum_list	= enum_ldap_passwd_sync,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "ldap replication sleep",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_replication_sleep,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap suffix",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLdapSuffix,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap ssl",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_ssl,
		.special	= NULL,
		.enum_list	= enum_ldap_ssl,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap ssl ads",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_ssl_ads,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap deref",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_deref,
		.special	= NULL,
		.enum_list	= enum_ldap_deref,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap follow referral",
		.type		= P_ENUM,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_follow_referral,
		.special	= NULL,
		.enum_list	= enum_bool_auto,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_timeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap connection timeout",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_connection_timeout,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap page size",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_page_size,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap user suffix",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLdapUserSuffix,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap debug level",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_debug_level,
		.special	= handle_ldap_debug_level,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "ldap debug threshold",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.ldap_debug_threshold,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("EventLog Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "eventlog list",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szEventLogs,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL | FLAG_SHARE,
	},

	{N_("Miscellaneous Options"), P_SEP, P_SEPARATOR},

	{
		.label		= "add share command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAddShareCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "change share command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szChangeShareCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "delete share command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDeleteShareCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "config file",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szConfigFile,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE|FLAG_META,
	},
	{
		.label		= "preload",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAutoServices,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "auto services",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAutoServices,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "lock directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLockDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "lock dir",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLockDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "state directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szStateDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "cache directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szCacheDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "pid directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPidDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
#ifdef WITH_UTMP
	{
		.label		= "utmp directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUtmpDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "wtmp directory",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szWtmpDir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "utmp",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUtmp,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
#endif
	{
		.label		= "default service",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDefaultService,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "default",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szDefaultService,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "message command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szMsgCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "dfree cache time",
		.type		= P_INTEGER,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.iDfreeCacheTime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "dfree command",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szDfree,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "get quota command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szGetQuota,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "set quota command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szSetQuota,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "remote announce",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRemoteAnnounce,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "remote browse sync",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szRemoteBrowseSync,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "socket address",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szSocketAddress,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "nmbd bind explicit broadcast",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bNmbdBindExplicitBroadcast,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "homedir map",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szNISHomeMapName,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "afs username map",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szAfsUsernameMap,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "afs token lifetime",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iAfsTokenLifetime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "log nt token command",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szLogNtTokenCommand,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "time offset",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &extra_time_offset,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "NIS homedir",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bNISHomeMap,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "-valid",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.valid,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "copy",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szCopy,
		.special	= handle_copy,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "include",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szInclude,
		.special	= handle_include,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE|FLAG_META,
	},
	{
		.label		= "preexec",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPreExec,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "exec",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPreExec,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "preexec close",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bPreexecClose,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "postexec",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szPostExec,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "root preexec",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szRootPreExec,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "root preexec close",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bRootpreexecClose,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "root postexec",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szRootPostExec,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "available",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bAvailable,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_BASIC | FLAG_ADVANCED | FLAG_SHARE | FLAG_PRINT,
	},
	{
		.label		= "registry shares",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bRegistryShares,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare allow guests",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUsershareAllowGuests,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare max shares",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iUsershareMaxShares,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare owner only",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bUsershareOwnerOnly,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare path",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUsersharePath,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare prefix allow list",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUsersharePrefixAllowList,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare prefix deny list",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUsersharePrefixDenyList,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "usershare template share",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szUsershareTemplateShare,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "volume",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.volume,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "fstype",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.fstype,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "set directory",
		.type		= P_BOOLREV,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bNo_set_dir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "wide links",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bWidelinks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "follow symlinks",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bSymlinks,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "dont descend",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szDontdescend,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "magic script",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szMagicScript,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "magic output",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szMagicOutput,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "delete readonly",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDeleteReadonly,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "dos filemode",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDosFilemode,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "dos filetimes",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDosFiletimes,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "dos filetime resolution",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bDosFiletimeResolution,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE | FLAG_GLOBAL,
	},
	{
		.label		= "fake directory create times",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bFakeDirCreateTimes,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_GLOBAL,
	},
	{
		.label		= "panic action",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szPanicAction,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "perfcount module",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szSMBPerfcountModule,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("VFS module options"), P_SEP, P_SEPARATOR},

	{
		.label		= "vfs objects",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szVfsObjects,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "vfs object",
		.type		= P_LIST,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szVfsObjects,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},


	{N_("MSDFS options"), P_SEP, P_SEPARATOR},

	{
		.label		= "msdfs root",
		.type		= P_BOOL,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.bMSDfsRoot,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "msdfs proxy",
		.type		= P_STRING,
		.p_class	= P_LOCAL,
		.ptr		= &sDefault.szMSDfsProxy,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED | FLAG_SHARE,
	},
	{
		.label		= "host msdfs",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bHostMSDfs,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{N_("Winbind options"), P_SEP, P_SEPARATOR},

	{
		.label		= "passdb expand explicit",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bPassdbExpandExplicit,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "idmap backend",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIdmapBackend,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "idmap alloc backend",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIdmapAllocBackend,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "idmap cache time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iIdmapCacheTime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "idmap negative cache time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.iIdmapNegativeCacheTime,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "idmap uid",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIdmapUID,
		.special	= handle_idmap_uid,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind uid",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIdmapUID,
		.special	= handle_idmap_uid,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "idmap gid",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIdmapGID,
		.special	= handle_idmap_gid,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind gid",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szIdmapGID,
		.special	= handle_idmap_gid,
		.enum_list	= NULL,
		.flags		= FLAG_HIDE,
	},
	{
		.label		= "template homedir",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szTemplateHomedir,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "template shell",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szTemplateShell,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind separator",
		.type		= P_STRING,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szWinbindSeparator,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind cache time",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.winbind_cache_time,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind reconnect delay",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.winbind_reconnect_delay,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind max clients",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.winbind_max_clients,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind enum users",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindEnumUsers,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind enum groups",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindEnumGroups,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind use default domain",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindUseDefaultDomain,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind trusted domains only",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindTrustedDomainsOnly,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind nested groups",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindNestedGroups,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind expand groups",
		.type		= P_INTEGER,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.winbind_expand_groups,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind nss info",
		.type		= P_LIST,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.szWinbindNssInfo,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind refresh tickets",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindRefreshTickets,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind offline logon",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindOfflineLogon,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind normalize names",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindNormalizeNames,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "winbind rpc only",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bWinbindRpcOnly,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},
	{
		.label		= "create krb5 conf",
		.type		= P_BOOL,
		.p_class	= P_GLOBAL,
		.ptr		= &Globals.bCreateKrb5Conf,
		.special	= NULL,
		.enum_list	= NULL,
		.flags		= FLAG_ADVANCED,
	},

	{NULL,  P_BOOL,  P_NONE,  NULL,  NULL,  NULL,  0}
};

/***************************************************************************
 Initialise the sDefault parameter structure for the printer values.
***************************************************************************/

static void init_printer_values(struct service *pService)
{
	/* choose defaults depending on the type of printing */
	switch (pService->iPrinting) {
		case PRINT_BSD:
		case PRINT_AIX:
		case PRINT_LPRNT:
		case PRINT_LPROS2:
			string_set(&pService->szLpqcommand, "lpq -P'%p'");
			string_set(&pService->szLprmcommand, "lprm -P'%p' %j");
			string_set(&pService->szPrintcommand, "lpr -r -P'%p' %s");
			break;

		case PRINT_LPRNG:
		case PRINT_PLP:
			string_set(&pService->szLpqcommand, "lpq -P'%p'");
			string_set(&pService->szLprmcommand, "lprm -P'%p' %j");
			string_set(&pService->szPrintcommand, "lpr -r -P'%p' %s");
			string_set(&pService->szQueuepausecommand, "lpc stop '%p'");
			string_set(&pService->szQueueresumecommand, "lpc start '%p'");
			string_set(&pService->szLppausecommand, "lpc hold '%p' %j");
			string_set(&pService->szLpresumecommand, "lpc release '%p' %j");
			break;

		case PRINT_CUPS:
		case PRINT_IPRINT:
#ifdef HAVE_CUPS
			/* set the lpq command to contain the destination printer
			   name only.  This is used by cups_queue_get() */
			string_set(&pService->szLpqcommand, "%p");
			string_set(&pService->szLprmcommand, "");
			string_set(&pService->szPrintcommand, "");
			string_set(&pService->szLppausecommand, "");
			string_set(&pService->szLpresumecommand, "");
			string_set(&pService->szQueuepausecommand, "");
			string_set(&pService->szQueueresumecommand, "");
#else
			string_set(&pService->szLpqcommand, "lpq -P'%p'");
			string_set(&pService->szLprmcommand, "lprm -P'%p' %j");
			string_set(&pService->szPrintcommand, "lpr -P'%p' %s; rm %s");
			string_set(&pService->szLppausecommand, "lp -i '%p-%j' -H hold");
			string_set(&pService->szLpresumecommand, "lp -i '%p-%j' -H resume");
			string_set(&pService->szQueuepausecommand, "disable '%p'");
			string_set(&pService->szQueueresumecommand, "enable '%p'");
#endif /* HAVE_CUPS */
			break;

		case PRINT_SYSV:
		case PRINT_HPUX:
			string_set(&pService->szLpqcommand, "lpstat -o%p");
			string_set(&pService->szLprmcommand, "cancel %p-%j");
			string_set(&pService->szPrintcommand, "lp -c -d%p %s; rm %s");
			string_set(&pService->szQueuepausecommand, "disable %p");
			string_set(&pService->szQueueresumecommand, "enable %p");
#ifndef HPUX
			string_set(&pService->szLppausecommand, "lp -i %p-%j -H hold");
			string_set(&pService->szLpresumecommand, "lp -i %p-%j -H resume");
#endif /* HPUX */
			break;

		case PRINT_QNX:
			string_set(&pService->szLpqcommand, "lpq -P%p");
			string_set(&pService->szLprmcommand, "lprm -P%p %j");
			string_set(&pService->szPrintcommand, "lp -r -P%p %s");
			break;

#ifdef DEVELOPER
	case PRINT_TEST:
	case PRINT_VLP:
		string_set(&pService->szPrintcommand, "vlp print %p %s");
		string_set(&pService->szLpqcommand, "vlp lpq %p");
		string_set(&pService->szLprmcommand, "vlp lprm %p %j");
		string_set(&pService->szLppausecommand, "vlp lppause %p %j");
		string_set(&pService->szLpresumecommand, "vlp lpresume %p %j");
		string_set(&pService->szQueuepausecommand, "vlp queuepause %p");
		string_set(&pService->szQueueresumecommand, "vlp queueresume %p");
		break;
#endif /* DEVELOPER */

	}
}
/**
 *  Function to return the default value for the maximum number of open
 *  file descriptors permitted.  This function tries to consult the
 *  kernel-level (sysctl) and ulimit (getrlimit()) values and goes
 *  the smaller of those.
 */
static int max_open_files(void)
{
	int sysctl_max = MAX_OPEN_FILES;
	int rlimit_max = MAX_OPEN_FILES;

#ifdef HAVE_SYSCTLBYNAME
	{
		size_t size = sizeof(sysctl_max);
		sysctlbyname("kern.maxfilesperproc", &sysctl_max, &size, NULL,
			     0);
	}
#endif

#if (defined(HAVE_GETRLIMIT) && defined(RLIMIT_NOFILE))
	{
		struct rlimit rl;

		ZERO_STRUCT(rl);

		if (getrlimit(RLIMIT_NOFILE, &rl) == 0)
			rlimit_max = rl.rlim_cur;

#if defined(RLIM_INFINITY)
		if(rl.rlim_cur == RLIM_INFINITY)
			rlimit_max = MAX_OPEN_FILES;
	}
#endif
#endif

	if (sysctl_max < MIN_OPEN_FILES_WINDOWS) {
		DEBUG(2,("max_open_files: increasing sysctl_max (%d) to "
			"minimum Windows limit (%d)\n",
			sysctl_max,
			MIN_OPEN_FILES_WINDOWS));
		sysctl_max = MIN_OPEN_FILES_WINDOWS;
	}

	if (rlimit_max < MIN_OPEN_FILES_WINDOWS) {
		DEBUG(2,("rlimit_max: increasing rlimit_max (%d) to "
			"minimum Windows limit (%d)\n",
			rlimit_max,
			MIN_OPEN_FILES_WINDOWS));
		rlimit_max = MIN_OPEN_FILES_WINDOWS;
	}

	return MIN(sysctl_max, rlimit_max);
}

/**
 * Common part of freeing allocated data for one parameter.
 */
static void free_one_parameter_common(void *parm_ptr,
				      struct parm_struct parm)
{
	if ((parm.type == P_STRING) ||
	    (parm.type == P_USTRING))
	{
		string_free((char**)parm_ptr);
	} else if (parm.type == P_LIST) {
		TALLOC_FREE(*((char***)parm_ptr));
	}
}

/**
 * Free the allocated data for one parameter for a share
 * given as a service struct.
 */
static void free_one_parameter(struct service *service,
			       struct parm_struct parm)
{
	void *parm_ptr;

	if (parm.p_class != P_LOCAL) {
		return;
	}

	parm_ptr = lp_local_ptr(service, parm.ptr);

	free_one_parameter_common(parm_ptr, parm);
}

/**
 * Free the allocated parameter data of a share given
 * as a service struct.
 */
static void free_parameters(struct service *service)
{
	uint32_t i;

	for (i=0; parm_table[i].label; i++) {
		free_one_parameter(service, parm_table[i]);
	}
}

/**
 * Free the allocated data for one parameter for a given share
 * specified by an snum.
 */
static void free_one_parameter_by_snum(int snum, struct parm_struct parm)
{
	void *parm_ptr;

	if (parm.ptr == NULL) {
		return;
	}

	if (snum < 0) {
		parm_ptr = parm.ptr;
	} else if (parm.p_class != P_LOCAL) {
		return;
	} else {
		parm_ptr = lp_local_ptr_by_snum(snum, parm.ptr);
	}

	free_one_parameter_common(parm_ptr, parm);
}

/**
 * Free the allocated parameter data for a share specified
 * by an snum.
 */
static void free_parameters_by_snum(int snum)
{
	uint32_t i;

	for (i=0; parm_table[i].label; i++) {
		free_one_parameter_by_snum(snum, parm_table[i]);
	}
}

/**
 * Free the allocated global parameters.
 */
static void free_global_parameters(void)
{
	free_parameters_by_snum(GLOBAL_SECTION_SNUM);
}

/***************************************************************************
 Initialise the global parameter structure.
***************************************************************************/

static void init_globals(bool first_time_only)
{
	static bool done_init = False;
	char *s = NULL;
	int i;

        /* If requested to initialize only once and we've already done it... */
        if (first_time_only && done_init) {
                /* ... then we have nothing more to do */
                return;
        }

	if (!done_init) {
		/* The logfile can be set before this is invoked. Free it if so. */
		if (Globals.szLogFile != NULL) {
			string_free(&Globals.szLogFile);
			Globals.szLogFile = NULL;
		}
		done_init = True;
	} else {
		free_global_parameters();
	}

	memset((void *)&Globals, '\0', sizeof(Globals));

	for (i = 0; parm_table[i].label; i++) {
		if ((parm_table[i].type == P_STRING ||
		     parm_table[i].type == P_USTRING) &&
		    parm_table[i].ptr)
		{
			string_set((char **)parm_table[i].ptr, "");
		}
	}

	string_set(&sDefault.fstype, FSTYPE_STRING);
	string_set(&sDefault.szPrintjobUsername, "%U");

	init_printer_values(&sDefault);


	DEBUG(3, ("Initialising global parameters\n"));

	string_set(&Globals.szSMBPasswdFile, get_dyn_SMB_PASSWD_FILE());
	string_set(&Globals.szPrivateDir, get_dyn_PRIVATE_DIR());

	/* use the new 'hash2' method by default, with a prefix of 1 */
	string_set(&Globals.szManglingMethod, "hash2");
	Globals.mangle_prefix = 1;

	string_set(&Globals.szGuestaccount, GUEST_ACCOUNT);

	/* using UTF8 by default allows us to support all chars */
	string_set(&Globals.unix_charset, DEFAULT_UNIX_CHARSET);

#if defined(HAVE_NL_LANGINFO) && defined(CODESET)
	/* If the system supports nl_langinfo(), try to grab the value
	   from the user's locale */
	string_set(&Globals.display_charset, "LOCALE");
#else
	string_set(&Globals.display_charset, DEFAULT_DISPLAY_CHARSET);
#endif

	/* Use codepage 850 as a default for the dos character set */
	string_set(&Globals.dos_charset, DEFAULT_DOS_CHARSET);

	/*
	 * Allow the default PASSWD_CHAT to be overridden in local.h.
	 */
	string_set(&Globals.szPasswdChat, DEFAULT_PASSWD_CHAT);

	set_global_myname(myhostname());
	string_set(&Globals.szNetbiosName,global_myname());

	set_global_myworkgroup(WORKGROUP);
	string_set(&Globals.szWorkgroup, lp_workgroup());

	string_set(&Globals.szPasswdProgram, "");
	string_set(&Globals.szLockDir, get_dyn_LOCKDIR());
	string_set(&Globals.szStateDir, get_dyn_STATEDIR());
	string_set(&Globals.szCacheDir, get_dyn_CACHEDIR());
	string_set(&Globals.szPidDir, get_dyn_PIDDIR());
	string_set(&Globals.szSocketAddress, "0.0.0.0");
	/*
	 * By default support explicit binding to broadcast
	 * addresses.
	 */
	Globals.bNmbdBindExplicitBroadcast = true;

	if (asprintf(&s, "Samba %s", samba_version_string()) < 0) {
		smb_panic("init_globals: ENOMEM");
	}
	string_set(&Globals.szServerString, s);
	SAFE_FREE(s);
	if (asprintf(&s, "%d.%d", DEFAULT_MAJOR_VERSION,
			DEFAULT_MINOR_VERSION) < 0) {
		smb_panic("init_globals: ENOMEM");
	}
	string_set(&Globals.szAnnounceVersion, s);
	SAFE_FREE(s);
#ifdef DEVELOPER
	string_set(&Globals.szPanicAction, "/bin/sleep 999999999");
#endif

	string_set(&Globals.szSocketOptions, DEFAULT_SOCKET_OPTIONS);

	string_set(&Globals.szLogonDrive, "");
	/* %N is the NIS auto.home server if -DAUTOHOME is used, else same as %L */
	string_set(&Globals.szLogonHome, "\\\\%N\\%U");
	string_set(&Globals.szLogonPath, "\\\\%N\\%U\\profile");

	string_set(&Globals.szNameResolveOrder, "lmhosts wins host bcast");
	string_set(&Globals.szPasswordServer, "*");

	Globals.AlgorithmicRidBase = BASE_RID;

	Globals.bLoadPrinters = True;
	Globals.PrintcapCacheTime = 750; 	/* 12.5 minutes */

	Globals.ConfigBackend = config_backend;

	/* Was 65535 (0xFFFF). 0x4101 matches W2K and causes major speed improvements... */
	/* Discovered by 2 days of pain by Don McCall @ HP :-). */
	Globals.max_xmit = 0x4104;
	Globals.max_mux = 50;	/* This is *needed* for profile support. */
	Globals.lpqcachetime = 30;	/* changed to handle large print servers better -- jerry */
	
	//royc 20110112 ================================================
	Globals.bDisableSpoolss = True;
	//Globals.bDisableSpoolss = False;
	//royc end
	
	Globals.iMaxSmbdProcesses = 0;/* no limit specified */
	Globals.pwordlevel = 0;
	Globals.unamelevel = 0;
	
	//royc 20110112 ================================================
	Globals.deadtime = 10;
	//Globals.deadtime = 0;
	//royc end
	
	Globals.getwd_cache = true;
	Globals.bLargeReadwrite = True;
	
	//royc 20110112 ================================================
	Globals.max_log_size = 10;
	//Globals.max_log_size = 5000;
	//royc end
	
	Globals.max_open_files = max_open_files();
	
	//royc 20081009 for limit locking.tdb size
	//Globals.open_files_db_hash_size = SMB_OPEN_DATABASE_TDB_HASH_SIZE;
	Globals.open_files_db_hash_size = 100;
	//royc end
	
	Globals.maxprotocol = PROTOCOL_NT1;
	Globals.minprotocol = PROTOCOL_CORE;
	Globals.security = SEC_USER;
	Globals.paranoid_server_security = True;
	Globals.bEncryptPasswords = True;
	Globals.bUpdateEncrypt = False;
	Globals.clientSchannel = Auto;
	Globals.serverSchannel = Auto;
	Globals.bReadRaw = True;
	Globals.bWriteRaw = True;
	
	//royc 20110112 ================================================
	Globals.bNullPasswords = True;
	//Globals.bNullPasswords = False;
	//royc end
	
	Globals.bObeyPamRestrictions = False;
	Globals.syslog = 1;
	Globals.bSyslogOnly = False;
	Globals.bTimestampLogs = True;
	string_set(&Globals.szLogLevel, "0");
	Globals.bDebugPrefixTimestamp = False;
	Globals.bDebugHiresTimestamp = true;
	Globals.bDebugPid = False;
	Globals.bDebugUid = False;
	Globals.bDebugClass = False;
	Globals.bEnableCoreFiles = True;
	Globals.max_ttl = 60 * 60 * 24 * 3;	/* 3 days default. */
	Globals.max_wins_ttl = 60 * 60 * 24 * 6;	/* 6 days default. */
	Globals.min_wins_ttl = 60 * 60 * 6;	/* 6 hours default. */
	Globals.machine_password_timeout = 60 * 60 * 24 * 7;	/* 7 days default. */
	Globals.lm_announce = 2;	/* = Auto: send only if LM clients found */
	Globals.lm_interval = 60;
	Globals.announce_as = ANNOUNCE_AS_NT_SERVER;
#if (defined(HAVE_NETGROUP) && defined(WITH_AUTOMOUNT))
	Globals.bNISHomeMap = False;
#ifdef WITH_NISPLUS_HOME
	string_set(&Globals.szNISHomeMapName, "auto_home.org_dir");
#else
	string_set(&Globals.szNISHomeMapName, "auto.home");
#endif
#endif
	Globals.bTimeServer = False;
	Globals.bBindInterfacesOnly = False;
	Globals.bUnixPasswdSync = False;
	Globals.bPamPasswordChange = False;
	Globals.bPasswdChatDebug = False;
	Globals.iPasswdChatTimeout = 2; /* 2 second default. */
	Globals.bNTPipeSupport = True;	/* Do NT pipes by default. */
	Globals.bNTStatusSupport = True; /* Use NT status by default. */
	Globals.bStatCache = True;	/* use stat cache by default */
	Globals.iMaxStatCacheSize = 256; /* 256k by default */
	Globals.restrict_anonymous = 0;
	Globals.bClientLanManAuth = False;	/* Do NOT use the LanMan hash if it is available */
	Globals.bClientPlaintextAuth = False;	/* Do NOT use a plaintext password even if is requested by the server */
	Globals.bLanmanAuth = False;	/* Do NOT use the LanMan hash, even if it is supplied */
	Globals.bNTLMAuth = True;	/* Do use NTLMv1 if it is supplied by the client (otherwise NTLMv2) */
	Globals.bClientNTLMv2Auth = False; /* Client should not use NTLMv2, as we can't tell that the server supports it. */
	/* Note, that we will use NTLM2 session security (which is different), if it is available */

	Globals.map_to_guest = 0;	/* By Default, "Never" */
	Globals.oplock_break_wait_time = 0;	/* By Default, 0 msecs. */
	Globals.enhanced_browsing = true;
	Globals.iLockSpinTime = WINDOWS_MINIMUM_LOCK_TIMEOUT_MS; /* msec. */
#ifdef MMAP_BLACKLIST
	Globals.bUseMmap = False;
#else
	Globals.bUseMmap = True;
#endif
	Globals.bUnixExtensions = True;
	Globals.bResetOnZeroVC = False;
	Globals.bCreateKrb5Conf = true;

	/* hostname lookups can be very expensive and are broken on
	   a large number of sites (tridge) */
	Globals.bHostnameLookups = False;

   //royc ============================
	string_set(&Globals.szPassdbBackend, "smbpasswd");   
	//royc end
	
	string_set(&Globals.szLdapSuffix, "");
	string_set(&Globals.szLdapMachineSuffix, "");
	string_set(&Globals.szLdapUserSuffix, "");
	string_set(&Globals.szLdapGroupSuffix, "");
	string_set(&Globals.szLdapIdmapSuffix, "");

	string_set(&Globals.szLdapAdminDn, "");
	Globals.ldap_ssl = LDAP_SSL_START_TLS;
	Globals.ldap_ssl_ads = False;
	Globals.ldap_deref = -1;
	Globals.ldap_passwd_sync = LDAP_PASSWD_SYNC_OFF;
	Globals.ldap_delete_dn = False;
	Globals.ldap_replication_sleep = 1000; /* wait 1 sec for replication */
	Globals.ldap_follow_referral = Auto;
	Globals.ldap_timeout = LDAP_DEFAULT_TIMEOUT;
	Globals.ldap_connection_timeout = LDAP_CONNECTION_DEFAULT_TIMEOUT;
	Globals.ldap_page_size = LDAP_PAGE_SIZE;

	Globals.ldap_debug_level = 0;
	Globals.ldap_debug_threshold = 10;

	/* This is what we tell the afs client. in reality we set the token 
	 * to never expire, though, when this runs out the afs client will 
	 * forget the token. Set to 0 to get NEVERDATE.*/
	Globals.iAfsTokenLifetime = 604800;
	Globals.cups_connection_timeout = CUPS_DEFAULT_CONNECTION_TIMEOUT;

/* these parameters are set to defaults that are more appropriate
   for the increasing samba install base:

   as a member of the workgroup, that will possibly become a
   _local_ master browser (lm = True).  this is opposed to a forced
   local master browser startup (pm = True).

   doesn't provide WINS server service by default (wsupp = False),
   and doesn't provide domain master browser services by default, either.

*/

	Globals.bMsAddPrinterWizard = True;
	Globals.os_level = 20;
	Globals.bLocalMaster = True;
	Globals.iDomainMaster = Auto;	/* depending on bDomainLogons */
	Globals.bDomainLogons = False;
	Globals.bBrowseList = True;
	Globals.bWINSsupport = False;
	Globals.bWINSproxy = False;

	TALLOC_FREE(Globals.szInitLogonDelayedHosts);
	Globals.InitLogonDelay = 100; /* 100 ms default delay */

	Globals.bDNSproxy = True;

	/* this just means to use them if they exist */
	Globals.bKernelOplocks = True;

	Globals.bAllowTrustedDomains = True;
	string_set(&Globals.szIdmapBackend, "tdb");

	string_set(&Globals.szTemplateShell, "/bin/false");
	string_set(&Globals.szTemplateHomedir, "/home/%D/%U");
	string_set(&Globals.szWinbindSeparator, "\\");

	string_set(&Globals.szCupsServer, "");
	string_set(&Globals.szIPrintServer, "");

	string_set(&Globals.ctdbdSocket, "");
	Globals.szClusterAddresses = NULL;
	Globals.clustering = False;
	Globals.ctdb_timeout = 0;

	Globals.winbind_cache_time = 300;	/* 5 minutes */
	Globals.winbind_reconnect_delay = 30;	/* 30 seconds */
	Globals.winbind_max_clients = 200;
	Globals.bWinbindEnumUsers = False;
	Globals.bWinbindEnumGroups = False;
	Globals.bWinbindUseDefaultDomain = False;
	Globals.bWinbindTrustedDomainsOnly = False;
	Globals.bWinbindNestedGroups = True;
	Globals.winbind_expand_groups = 1;
	Globals.szWinbindNssInfo = str_list_make_v3(talloc_autofree_context(), "template", NULL);
	Globals.bWinbindRefreshTickets = False;
	
	//royc 20110112 ================================================
	Globals.bWinbindOfflineLogon = True;
   //Globals.bWinbindOfflineLogon = False;
   //royc end

	Globals.iIdmapCacheTime = 86400 * 7; /* a week by default */
	Globals.iIdmapNegativeCacheTime = 120; /* 2 minutes by default */

	Globals.bPassdbExpandExplicit = False;

	Globals.name_cache_timeout = 660; /* In seconds */

	Globals.bUseSpnego = True;
	Globals.bClientUseSpnego = True;

	Globals.client_signing = Auto;
	Globals.server_signing = False;

	Globals.bDeferSharingViolations = True;
	string_set(&Globals.smb_ports, SMB_PORTS);

	Globals.bEnablePrivileges = True;
	Globals.bHostMSDfs        = True;
	Globals.bASUSupport       = False;

	/* User defined shares. */
	if (asprintf(&s, "%s/usershares", get_dyn_STATEDIR()) < 0) {
		smb_panic("init_globals: ENOMEM");
	}
	string_set(&Globals.szUsersharePath, s);
	SAFE_FREE(s);
	string_set(&Globals.szUsershareTemplateShare, "");
	Globals.iUsershareMaxShares = 0;
	/* By default disallow sharing of directories not owned by the sharer. */
	Globals.bUsershareOwnerOnly = True;
	/* By default disallow guest access to usershares. */
	Globals.bUsershareAllowGuests = False;

	Globals.iKeepalive = DEFAULT_KEEPALIVE;

	/* By default no shares out of the registry */
	Globals.bRegistryShares = False;

	Globals.iminreceivefile = 0;

	Globals.bMapUntrustedToDomain = false;
}

/*******************************************************************
 Convenience routine to grab string parameters into temporary memory
 and run standard_sub_basic on them. The buffers can be written to by
 callers without affecting the source string.
********************************************************************/

static char *lp_string(const char *s)
{
	char *ret;
	TALLOC_CTX *ctx = talloc_tos();

	/* The follow debug is useful for tracking down memory problems
	   especially if you have an inner loop that is calling a lp_*()
	   function that returns a string.  Perhaps this debug should be
	   present all the time? */

#if 0
	DEBUG(10, ("lp_string(%s)\n", s));
#endif
	if (!s) {
		return NULL;
	}

	ret = talloc_sub_basic(ctx,
			get_current_username(),
			current_user_info.domain,
			s);
	if (trim_char(ret, '\"', '\"')) {
		if (strchr(ret,'\"') != NULL) {
			TALLOC_FREE(ret);
			ret = talloc_sub_basic(ctx,
					get_current_username(),
					current_user_info.domain,
					s);
		}
	}
	return ret;
}

/*
   In this section all the functions that are used to access the
   parameters from the rest of the program are defined
*/

#define FN_GLOBAL_STRING(fn_name,ptr) \
 char *fn_name(void) {return(lp_string(*(char **)(ptr) ? *(char **)(ptr) : ""));}
#define FN_GLOBAL_CONST_STRING(fn_name,ptr) \
 const char *fn_name(void) {return(*(const char **)(ptr) ? *(const char **)(ptr) : "");}
#define FN_GLOBAL_LIST(fn_name,ptr) \
 const char **fn_name(void) {return(*(const char ***)(ptr));}
#define FN_GLOBAL_BOOL(fn_name,ptr) \
 bool fn_name(void) {return(*(bool *)(ptr));}
#define FN_GLOBAL_CHAR(fn_name,ptr) \
 char fn_name(void) {return(*(char *)(ptr));}
#define FN_GLOBAL_INTEGER(fn_name,ptr) \
 int fn_name(void) {return(*(int *)(ptr));}

#define FN_LOCAL_STRING(fn_name,val) \
 char *fn_name(int i) {return(lp_string((LP_SNUM_OK(i) && ServicePtrs[(i)]->val) ? ServicePtrs[(i)]->val : sDefault.val));}
#define FN_LOCAL_CONST_STRING(fn_name,val) \
 const char *fn_name(int i) {return (const char *)((LP_SNUM_OK(i) && ServicePtrs[(i)]->val) ? ServicePtrs[(i)]->val : sDefault.val);}
#define FN_LOCAL_LIST(fn_name,val) \
 const char **fn_name(int i) {return(const char **)(LP_SNUM_OK(i)? ServicePtrs[(i)]->val : sDefault.val);}
#define FN_LOCAL_BOOL(fn_name,val) \
 bool fn_name(int i) {return(bool)(LP_SNUM_OK(i)? ServicePtrs[(i)]->val : sDefault.val);}
#define FN_LOCAL_INTEGER(fn_name,val) \
 int fn_name(int i) {return(LP_SNUM_OK(i)? ServicePtrs[(i)]->val : sDefault.val);}

#define FN_LOCAL_PARM_BOOL(fn_name,val) \
 bool fn_name(const struct share_params *p) {return(bool)(LP_SNUM_OK(p->service)? ServicePtrs[(p->service)]->val : sDefault.val);}
#define FN_LOCAL_PARM_INTEGER(fn_name,val) \
 int fn_name(const struct share_params *p) {return(LP_SNUM_OK(p->service)? ServicePtrs[(p->service)]->val : sDefault.val);}
#define FN_LOCAL_PARM_STRING(fn_name,val) \
 char *fn_name(const struct share_params *p) {return(lp_string((LP_SNUM_OK(p->service) && ServicePtrs[(p->service)]->val) ? ServicePtrs[(p->service)]->val : sDefault.val));}
#define FN_LOCAL_CHAR(fn_name,val) \
 char fn_name(const struct share_params *p) {return(LP_SNUM_OK(p->service)? ServicePtrs[(p->service)]->val : sDefault.val);}

FN_GLOBAL_STRING(lp_smb_ports, &Globals.smb_ports)
FN_GLOBAL_STRING(lp_dos_charset, &Globals.dos_charset)
FN_GLOBAL_STRING(lp_unix_charset, &Globals.unix_charset)
FN_GLOBAL_STRING(lp_display_charset, &Globals.display_charset)
FN_GLOBAL_STRING(lp_logfile, &Globals.szLogFile)
FN_GLOBAL_STRING(lp_configfile, &Globals.szConfigFile)
FN_GLOBAL_STRING(lp_smb_passwd_file, &Globals.szSMBPasswdFile)
FN_GLOBAL_STRING(lp_private_dir, &Globals.szPrivateDir)
FN_GLOBAL_STRING(lp_serverstring, &Globals.szServerString)
FN_GLOBAL_INTEGER(lp_printcap_cache_time, &Globals.PrintcapCacheTime)
FN_GLOBAL_STRING(lp_addport_cmd, &Globals.szAddPortCommand)
FN_GLOBAL_STRING(lp_enumports_cmd, &Globals.szEnumPortsCommand)
FN_GLOBAL_STRING(lp_addprinter_cmd, &Globals.szAddPrinterCommand)
FN_GLOBAL_STRING(lp_deleteprinter_cmd, &Globals.szDeletePrinterCommand)
FN_GLOBAL_STRING(lp_os2_driver_map, &Globals.szOs2DriverMap)
FN_GLOBAL_STRING(lp_lockdir, &Globals.szLockDir)
/* If lp_statedir() and lp_cachedir() are explicitely set during the
 * build process or in smb.conf, we use that value.  Otherwise they
 * default to the value of lp_lockdir(). */
char *lp_statedir(void) {
	if ((strcmp(get_dyn_STATEDIR(), get_dyn_LOCKDIR()) != 0) ||
	    (strcmp(get_dyn_STATEDIR(), Globals.szStateDir) != 0))
		return(lp_string(*(char **)(&Globals.szStateDir) ?
		    *(char **)(&Globals.szStateDir) : ""));
	else
		return(lp_string(*(char **)(&Globals.szLockDir) ?
		    *(char **)(&Globals.szLockDir) : ""));
}
char *lp_cachedir(void) {
	if ((strcmp(get_dyn_CACHEDIR(), get_dyn_LOCKDIR()) != 0) ||
	    (strcmp(get_dyn_CACHEDIR(), Globals.szCacheDir) != 0))
		return(lp_string(*(char **)(&Globals.szCacheDir) ?
		    *(char **)(&Globals.szCacheDir) : ""));
	else
		return(lp_string(*(char **)(&Globals.szLockDir) ?
		    *(char **)(&Globals.szLockDir) : ""));
}
FN_GLOBAL_STRING(lp_piddir, &Globals.szPidDir)
FN_GLOBAL_STRING(lp_mangling_method, &Globals.szManglingMethod)
FN_GLOBAL_INTEGER(lp_mangle_prefix, &Globals.mangle_prefix)
FN_GLOBAL_STRING(lp_utmpdir, &Globals.szUtmpDir)
FN_GLOBAL_STRING(lp_wtmpdir, &Globals.szWtmpDir)
FN_GLOBAL_BOOL(lp_utmp, &Globals.bUtmp)
FN_GLOBAL_STRING(lp_rootdir, &Globals.szRootdir)
FN_GLOBAL_STRING(lp_perfcount_module, &Globals.szSMBPerfcountModule)
FN_GLOBAL_STRING(lp_defaultservice, &Globals.szDefaultService)
FN_GLOBAL_STRING(lp_msg_command, &Globals.szMsgCommand)
FN_GLOBAL_STRING(lp_get_quota_command, &Globals.szGetQuota)
FN_GLOBAL_STRING(lp_set_quota_command, &Globals.szSetQuota)
FN_GLOBAL_STRING(lp_auto_services, &Globals.szAutoServices)
FN_GLOBAL_STRING(lp_passwd_program, &Globals.szPasswdProgram)
FN_GLOBAL_STRING(lp_passwd_chat, &Globals.szPasswdChat)
FN_GLOBAL_STRING(lp_passwordserver, &Globals.szPasswordServer)
FN_GLOBAL_STRING(lp_name_resolve_order, &Globals.szNameResolveOrder)
FN_GLOBAL_STRING(lp_realm, &Globals.szRealm)
FN_GLOBAL_CONST_STRING(lp_afs_username_map, &Globals.szAfsUsernameMap)
FN_GLOBAL_INTEGER(lp_afs_token_lifetime, &Globals.iAfsTokenLifetime)
FN_GLOBAL_STRING(lp_log_nt_token_command, &Globals.szLogNtTokenCommand)
FN_GLOBAL_STRING(lp_username_map, &Globals.szUsernameMap)
FN_GLOBAL_CONST_STRING(lp_logon_script, &Globals.szLogonScript)
FN_GLOBAL_CONST_STRING(lp_logon_path, &Globals.szLogonPath)
FN_GLOBAL_CONST_STRING(lp_logon_drive, &Globals.szLogonDrive)
FN_GLOBAL_CONST_STRING(lp_logon_home, &Globals.szLogonHome)
FN_GLOBAL_STRING(lp_remote_announce, &Globals.szRemoteAnnounce)
FN_GLOBAL_STRING(lp_remote_browse_sync, &Globals.szRemoteBrowseSync)
FN_GLOBAL_BOOL(lp_nmbd_bind_explicit_broadcast, &Globals.bNmbdBindExplicitBroadcast)
FN_GLOBAL_LIST(lp_wins_server_list, &Globals.szWINSservers)
FN_GLOBAL_LIST(lp_interfaces, &Globals.szInterfaces)
FN_GLOBAL_STRING(lp_nis_home_map_name, &Globals.szNISHomeMapName)
static FN_GLOBAL_STRING(lp_announce_version, &Globals.szAnnounceVersion)
FN_GLOBAL_LIST(lp_netbios_aliases, &Globals.szNetbiosAliases)
/* FN_GLOBAL_STRING(lp_passdb_backend, &Globals.szPassdbBackend)
 * lp_passdb_backend() should be replace by the this macro again after
 * some releases.
 * */
const char *lp_passdb_backend(void)
{
	char *delim, *quote;

	delim = strchr( Globals.szPassdbBackend, ' ');
	/* no space at all */
	if (delim == NULL) {
		goto out;
	}

	quote = strchr(Globals.szPassdbBackend, '"');
	/* no quote char or non in the first part */
	if (quote == NULL || quote > delim) {
		*delim = '\0';
		goto warn;
	}

	quote = strchr(quote+1, '"');
	if (quote == NULL) {
		DEBUG(0, ("WARNING: Your 'passdb backend' configuration is invalid due to a missing second \" char.\n"));
		goto out;
	} else if (*(quote+1) == '\0') {
		/* space, fitting quote char, and one backend only */
		goto out;
	} else {
		/* terminate string after the fitting quote char */
		*(quote+1) = '\0';
	}

warn:
	DEBUG(0, ("WARNING: Your 'passdb backend' configuration includes multiple backends.  This\n"
		"is deprecated since Samba 3.0.23.  Please check WHATSNEW.txt or the section 'Passdb\n"
		"Changes' from the ChangeNotes as part of the Samba HOWTO collection.  Only the first\n"
		"backend (%s) is used.  The rest is ignored.\n", Globals.szPassdbBackend));

out:
	return Globals.szPassdbBackend;
}
FN_GLOBAL_LIST(lp_preload_modules, &Globals.szPreloadModules)
FN_GLOBAL_STRING(lp_panic_action, &Globals.szPanicAction)
FN_GLOBAL_STRING(lp_adduser_script, &Globals.szAddUserScript)
FN_GLOBAL_STRING(lp_renameuser_script, &Globals.szRenameUserScript)
FN_GLOBAL_STRING(lp_deluser_script, &Globals.szDelUserScript)

FN_GLOBAL_CONST_STRING(lp_guestaccount, &Globals.szGuestaccount)
FN_GLOBAL_STRING(lp_addgroup_script, &Globals.szAddGroupScript)
FN_GLOBAL_STRING(lp_delgroup_script, &Globals.szDelGroupScript)
FN_GLOBAL_STRING(lp_addusertogroup_script, &Globals.szAddUserToGroupScript)
FN_GLOBAL_STRING(lp_deluserfromgroup_script, &Globals.szDelUserFromGroupScript)
FN_GLOBAL_STRING(lp_setprimarygroup_script, &Globals.szSetPrimaryGroupScript)

FN_GLOBAL_STRING(lp_addmachine_script, &Globals.szAddMachineScript)

FN_GLOBAL_STRING(lp_shutdown_script, &Globals.szShutdownScript)
FN_GLOBAL_STRING(lp_abort_shutdown_script, &Globals.szAbortShutdownScript)
FN_GLOBAL_STRING(lp_username_map_script, &Globals.szUsernameMapScript)

FN_GLOBAL_STRING(lp_check_password_script, &Globals.szCheckPasswordScript)

FN_GLOBAL_STRING(lp_wins_hook, &Globals.szWINSHook)
FN_GLOBAL_CONST_STRING(lp_template_homedir, &Globals.szTemplateHomedir)
FN_GLOBAL_CONST_STRING(lp_template_shell, &Globals.szTemplateShell)
FN_GLOBAL_CONST_STRING(lp_winbind_separator, &Globals.szWinbindSeparator)
FN_GLOBAL_INTEGER(lp_acl_compatibility, &Globals.iAclCompat)
FN_GLOBAL_BOOL(lp_winbind_enum_users, &Globals.bWinbindEnumUsers)
FN_GLOBAL_BOOL(lp_winbind_enum_groups, &Globals.bWinbindEnumGroups)
FN_GLOBAL_BOOL(lp_winbind_use_default_domain, &Globals.bWinbindUseDefaultDomain)
FN_GLOBAL_BOOL(lp_winbind_trusted_domains_only, &Globals.bWinbindTrustedDomainsOnly)
FN_GLOBAL_BOOL(lp_winbind_nested_groups, &Globals.bWinbindNestedGroups)
FN_GLOBAL_INTEGER(lp_winbind_expand_groups, &Globals.winbind_expand_groups)
FN_GLOBAL_BOOL(lp_winbind_refresh_tickets, &Globals.bWinbindRefreshTickets)
FN_GLOBAL_BOOL(lp_winbind_offline_logon, &Globals.bWinbindOfflineLogon)
FN_GLOBAL_BOOL(lp_winbind_normalize_names, &Globals.bWinbindNormalizeNames)
FN_GLOBAL_BOOL(lp_winbind_rpc_only, &Globals.bWinbindRpcOnly)
FN_GLOBAL_BOOL(lp_create_krb5_conf, &Globals.bCreateKrb5Conf)

FN_GLOBAL_CONST_STRING(lp_idmap_backend, &Globals.szIdmapBackend)
FN_GLOBAL_STRING(lp_idmap_alloc_backend, &Globals.szIdmapAllocBackend)
FN_GLOBAL_INTEGER(lp_idmap_cache_time, &Globals.iIdmapCacheTime)
FN_GLOBAL_INTEGER(lp_idmap_negative_cache_time, &Globals.iIdmapNegativeCacheTime)
FN_GLOBAL_INTEGER(lp_keepalive, &Globals.iKeepalive)
FN_GLOBAL_BOOL(lp_passdb_expand_explicit, &Globals.bPassdbExpandExplicit)

FN_GLOBAL_STRING(lp_ldap_suffix, &Globals.szLdapSuffix)
FN_GLOBAL_STRING(lp_ldap_admin_dn, &Globals.szLdapAdminDn)
FN_GLOBAL_INTEGER(lp_ldap_ssl, &Globals.ldap_ssl)
FN_GLOBAL_BOOL(lp_ldap_ssl_ads, &Globals.ldap_ssl_ads)
FN_GLOBAL_INTEGER(lp_ldap_deref, &Globals.ldap_deref)
FN_GLOBAL_INTEGER(lp_ldap_follow_referral, &Globals.ldap_follow_referral)
FN_GLOBAL_INTEGER(lp_ldap_passwd_sync, &Globals.ldap_passwd_sync)
FN_GLOBAL_BOOL(lp_ldap_delete_dn, &Globals.ldap_delete_dn)
FN_GLOBAL_INTEGER(lp_ldap_replication_sleep, &Globals.ldap_replication_sleep)
FN_GLOBAL_INTEGER(lp_ldap_timeout, &Globals.ldap_timeout)
FN_GLOBAL_INTEGER(lp_ldap_connection_timeout, &Globals.ldap_connection_timeout)
FN_GLOBAL_INTEGER(lp_ldap_page_size, &Globals.ldap_page_size)
FN_GLOBAL_INTEGER(lp_ldap_debug_level, &Globals.ldap_debug_level)
FN_GLOBAL_INTEGER(lp_ldap_debug_threshold, &Globals.ldap_debug_threshold)
FN_GLOBAL_STRING(lp_add_share_cmd, &Globals.szAddShareCommand)
FN_GLOBAL_STRING(lp_change_share_cmd, &Globals.szChangeShareCommand)
FN_GLOBAL_STRING(lp_delete_share_cmd, &Globals.szDeleteShareCommand)
FN_GLOBAL_STRING(lp_usershare_path, &Globals.szUsersharePath)
FN_GLOBAL_LIST(lp_usershare_prefix_allow_list, &Globals.szUsersharePrefixAllowList)
FN_GLOBAL_LIST(lp_usershare_prefix_deny_list, &Globals.szUsersharePrefixDenyList)

FN_GLOBAL_LIST(lp_eventlog_list, &Globals.szEventLogs)

FN_GLOBAL_BOOL(lp_registry_shares, &Globals.bRegistryShares)
FN_GLOBAL_BOOL(lp_usershare_allow_guests, &Globals.bUsershareAllowGuests)
FN_GLOBAL_BOOL(lp_usershare_owner_only, &Globals.bUsershareOwnerOnly)
FN_GLOBAL_BOOL(lp_disable_netbios, &Globals.bDisableNetbios)
FN_GLOBAL_BOOL(lp_reset_on_zero_vc, &Globals.bResetOnZeroVC)
FN_GLOBAL_BOOL(lp_ms_add_printer_wizard, &Globals.bMsAddPrinterWizard)
FN_GLOBAL_BOOL(lp_dns_proxy, &Globals.bDNSproxy)
FN_GLOBAL_BOOL(lp_wins_support, &Globals.bWINSsupport)
FN_GLOBAL_BOOL(lp_we_are_a_wins_server, &Globals.bWINSsupport)
FN_GLOBAL_BOOL(lp_wins_proxy, &Globals.bWINSproxy)
FN_GLOBAL_BOOL(lp_local_master, &Globals.bLocalMaster)
FN_GLOBAL_BOOL(lp_domain_logons, &Globals.bDomainLogons)
FN_GLOBAL_LIST(lp_init_logon_delayed_hosts, &Globals.szInitLogonDelayedHosts)
FN_GLOBAL_INTEGER(lp_init_logon_delay, &Globals.InitLogonDelay)
FN_GLOBAL_BOOL(lp_load_printers, &Globals.bLoadPrinters)
FN_GLOBAL_BOOL(lp_readraw, &Globals.bReadRaw)
FN_GLOBAL_BOOL(lp_large_readwrite, &Globals.bLargeReadwrite)
FN_GLOBAL_BOOL(lp_writeraw, &Globals.bWriteRaw)
FN_GLOBAL_BOOL(lp_null_passwords, &Globals.bNullPasswords)
FN_GLOBAL_BOOL(lp_obey_pam_restrictions, &Globals.bObeyPamRestrictions)
FN_GLOBAL_BOOL(lp_encrypted_passwords, &Globals.bEncryptPasswords)
FN_GLOBAL_BOOL(lp_update_encrypted, &Globals.bUpdateEncrypt)
FN_GLOBAL_INTEGER(lp_client_schannel, &Globals.clientSchannel)
FN_GLOBAL_INTEGER(lp_server_schannel, &Globals.serverSchannel)
FN_GLOBAL_BOOL(lp_syslog_only, &Globals.bSyslogOnly)
FN_GLOBAL_BOOL(lp_timestamp_logs, &Globals.bTimestampLogs)
FN_GLOBAL_BOOL(lp_debug_prefix_timestamp, &Globals.bDebugPrefixTimestamp)
FN_GLOBAL_BOOL(lp_debug_hires_timestamp, &Globals.bDebugHiresTimestamp)
FN_GLOBAL_BOOL(lp_debug_pid, &Globals.bDebugPid)
FN_GLOBAL_BOOL(lp_debug_uid, &Globals.bDebugUid)
FN_GLOBAL_BOOL(lp_debug_class, &Globals.bDebugClass)
FN_GLOBAL_BOOL(lp_enable_core_files, &Globals.bEnableCoreFiles)
FN_GLOBAL_BOOL(lp_browse_list, &Globals.bBrowseList)
FN_GLOBAL_BOOL(lp_nis_home_map, &Globals.bNISHomeMap)
static FN_GLOBAL_BOOL(lp_time_server, &Globals.bTimeServer)
FN_GLOBAL_BOOL(lp_bind_interfaces_only, &Globals.bBindInterfacesOnly)
FN_GLOBAL_BOOL(lp_pam_password_change, &Globals.bPamPasswordChange)
FN_GLOBAL_BOOL(lp_unix_password_sync, &Globals.bUnixPasswdSync)
FN_GLOBAL_BOOL(lp_passwd_chat_debug, &Globals.bPasswdChatDebug)
FN_GLOBAL_INTEGER(lp_passwd_chat_timeout, &Globals.iPasswdChatTimeout)
FN_GLOBAL_BOOL(lp_nt_pipe_support, &Globals.bNTPipeSupport)
FN_GLOBAL_BOOL(lp_nt_status_support, &Globals.bNTStatusSupport)
FN_GLOBAL_BOOL(lp_stat_cache, &Globals.bStatCache)
FN_GLOBAL_INTEGER(lp_max_stat_cache_size, &Globals.iMaxStatCacheSize)
FN_GLOBAL_BOOL(lp_allow_trusted_domains, &Globals.bAllowTrustedDomains)
FN_GLOBAL_BOOL(lp_map_untrusted_to_domain, &Globals.bMapUntrustedToDomain)
FN_GLOBAL_INTEGER(lp_restrict_anonymous, &Globals.restrict_anonymous)
FN_GLOBAL_BOOL(lp_lanman_auth, &Globals.bLanmanAuth)
FN_GLOBAL_BOOL(lp_ntlm_auth, &Globals.bNTLMAuth)
FN_GLOBAL_BOOL(lp_client_plaintext_auth, &Globals.bClientPlaintextAuth)
FN_GLOBAL_BOOL(lp_client_lanman_auth, &Globals.bClientLanManAuth)
FN_GLOBAL_BOOL(lp_client_ntlmv2_auth, &Globals.bClientNTLMv2Auth)
FN_GLOBAL_BOOL(lp_host_msdfs, &Globals.bHostMSDfs)
FN_GLOBAL_BOOL(lp_kernel_oplocks, &Globals.bKernelOplocks)
FN_GLOBAL_BOOL(lp_enhanced_browsing, &Globals.enhanced_browsing)
FN_GLOBAL_BOOL(lp_use_mmap, &Globals.bUseMmap)
FN_GLOBAL_BOOL(lp_unix_extensions, &Globals.bUnixExtensions)
FN_GLOBAL_BOOL(lp_use_spnego, &Globals.bUseSpnego)
FN_GLOBAL_BOOL(lp_client_use_spnego, &Globals.bClientUseSpnego)
FN_GLOBAL_BOOL(lp_client_use_spnego_principal, &Globals.client_use_spnego_principal)
FN_GLOBAL_BOOL(lp_hostname_lookups, &Globals.bHostnameLookups)
FN_LOCAL_PARM_BOOL(lp_change_notify, bChangeNotify)
FN_LOCAL_PARM_BOOL(lp_kernel_change_notify, bKernelChangeNotify)
FN_GLOBAL_STRING(lp_dedicated_keytab_file, &Globals.szDedicatedKeytabFile)
FN_GLOBAL_INTEGER(lp_kerberos_method, &Globals.iKerberosMethod)
FN_GLOBAL_BOOL(lp_defer_sharing_violations, &Globals.bDeferSharingViolations)
FN_GLOBAL_BOOL(lp_enable_privileges, &Globals.bEnablePrivileges)
FN_GLOBAL_BOOL(lp_enable_asu_support, &Globals.bASUSupport)
FN_GLOBAL_INTEGER(lp_os_level, &Globals.os_level)
FN_GLOBAL_INTEGER(lp_max_ttl, &Globals.max_ttl)
FN_GLOBAL_INTEGER(lp_max_wins_ttl, &Globals.max_wins_ttl)
FN_GLOBAL_INTEGER(lp_min_wins_ttl, &Globals.min_wins_ttl)
FN_GLOBAL_INTEGER(lp_max_log_size, &Globals.max_log_size)
FN_GLOBAL_INTEGER(lp_max_open_files, &Globals.max_open_files)
FN_GLOBAL_INTEGER(lp_open_files_db_hash_size, &Globals.open_files_db_hash_size)
FN_GLOBAL_INTEGER(lp_maxxmit, &Globals.max_xmit)
FN_GLOBAL_INTEGER(lp_maxmux, &Globals.max_mux)
FN_GLOBAL_INTEGER(lp_passwordlevel, &Globals.pwordlevel)
FN_GLOBAL_INTEGER(lp_usernamelevel, &Globals.unamelevel)
FN_GLOBAL_INTEGER(lp_deadtime, &Globals.deadtime)
FN_GLOBAL_BOOL(lp_getwd_cache, &Globals.getwd_cache)
FN_GLOBAL_INTEGER(lp_maxprotocol, &Globals.maxprotocol)
FN_GLOBAL_INTEGER(lp_minprotocol, &Globals.minprotocol)
FN_GLOBAL_INTEGER(lp_security, &Globals.security)
FN_GLOBAL_LIST(lp_auth_methods, &Globals.AuthMethods)
FN_GLOBAL_BOOL(lp_paranoid_server_security, &Globals.paranoid_server_security)
FN_GLOBAL_INTEGER(lp_maxdisksize, &Globals.maxdisksize)
FN_GLOBAL_INTEGER(lp_lpqcachetime, &Globals.lpqcachetime)
FN_GLOBAL_INTEGER(lp_max_smbd_processes, &Globals.iMaxSmbdProcesses)
FN_GLOBAL_BOOL(_lp_disable_spoolss, &Globals.bDisableSpoolss)
FN_GLOBAL_INTEGER(lp_syslog, &Globals.syslog)
static FN_GLOBAL_INTEGER(lp_announce_as, &Globals.announce_as)
FN_GLOBAL_INTEGER(lp_lm_announce, &Globals.lm_announce)
FN_GLOBAL_INTEGER(lp_lm_interval, &Globals.lm_interval)
FN_GLOBAL_INTEGER(lp_machine_password_timeout, &Globals.machine_password_timeout)
FN_GLOBAL_INTEGER(lp_map_to_guest, &Globals.map_to_guest)
FN_GLOBAL_INTEGER(lp_oplock_break_wait_time, &Globals.oplock_break_wait_time)
FN_GLOBAL_INTEGER(lp_lock_spin_time, &Globals.iLockSpinTime)
FN_GLOBAL_INTEGER(lp_usershare_max_shares, &Globals.iUsershareMaxShares)
FN_GLOBAL_CONST_STRING(lp_socket_options, &Globals.szSocketOptions)
FN_GLOBAL_INTEGER(lp_config_backend, &Globals.ConfigBackend)

FN_LOCAL_STRING(lp_preexec, szPreExec)
FN_LOCAL_STRING(lp_postexec, szPostExec)
FN_LOCAL_STRING(lp_rootpreexec, szRootPreExec)
FN_LOCAL_STRING(lp_rootpostexec, szRootPostExec)
FN_LOCAL_STRING(lp_servicename, szService)
FN_LOCAL_CONST_STRING(lp_const_servicename, szService)
FN_LOCAL_STRING(lp_pathname, szPath)
FN_LOCAL_STRING(lp_dontdescend, szDontdescend)
FN_LOCAL_STRING(lp_username, szUsername)
FN_LOCAL_LIST(lp_invalid_users, szInvalidUsers)
FN_LOCAL_LIST(lp_valid_users, szValidUsers)
FN_LOCAL_LIST(lp_admin_users, szAdminUsers)
FN_GLOBAL_LIST(lp_svcctl_list, &Globals.szServicesList)
FN_LOCAL_STRING(lp_cups_options, szCupsOptions)
FN_GLOBAL_STRING(lp_cups_server, &Globals.szCupsServer)
int lp_cups_encrypt(void)
{
	int result = 0;
#ifdef HAVE_HTTPCONNECTENCRYPT
	switch (Globals.CupsEncrypt) {
		case Auto:
			result = HTTP_ENCRYPT_REQUIRED;
			break;
		case True:
			result = HTTP_ENCRYPT_ALWAYS;
			break;
		case False:
			result = HTTP_ENCRYPT_NEVER;
			break;
	}
#endif
	return result;
}
FN_GLOBAL_STRING(lp_iprint_server, &Globals.szIPrintServer)
FN_GLOBAL_INTEGER(lp_cups_connection_timeout, &Globals.cups_connection_timeout)
FN_GLOBAL_CONST_STRING(lp_ctdbd_socket, &Globals.ctdbdSocket)
FN_GLOBAL_LIST(lp_cluster_addresses, &Globals.szClusterAddresses)
FN_GLOBAL_BOOL(lp_clustering, &Globals.clustering)
FN_GLOBAL_INTEGER(lp_ctdb_timeout, &Globals.ctdb_timeout)
FN_LOCAL_STRING(lp_printcommand, szPrintcommand)
FN_LOCAL_STRING(lp_lpqcommand, szLpqcommand)
FN_LOCAL_STRING(lp_lprmcommand, szLprmcommand)
FN_LOCAL_STRING(lp_lppausecommand, szLppausecommand)
FN_LOCAL_STRING(lp_lpresumecommand, szLpresumecommand)
FN_LOCAL_STRING(lp_queuepausecommand, szQueuepausecommand)
FN_LOCAL_STRING(lp_queueresumecommand, szQueueresumecommand)
static FN_LOCAL_STRING(_lp_printername, szPrintername)
FN_LOCAL_CONST_STRING(lp_printjob_username, szPrintjobUsername)
FN_LOCAL_LIST(lp_hostsallow, szHostsallow)
FN_LOCAL_LIST(lp_hostsdeny, szHostsdeny)
FN_LOCAL_STRING(lp_magicscript, szMagicScript)
FN_LOCAL_STRING(lp_magicoutput, szMagicOutput)
FN_LOCAL_STRING(lp_comment, comment)
FN_LOCAL_STRING(lp_force_user, force_user)
FN_LOCAL_STRING(lp_force_group, force_group)
FN_LOCAL_LIST(lp_readlist, readlist)
FN_LOCAL_LIST(lp_writelist, writelist)
FN_LOCAL_LIST(lp_printer_admin, printer_admin)
FN_LOCAL_STRING(lp_fstype, fstype)
FN_LOCAL_LIST(lp_vfs_objects, szVfsObjects)
FN_LOCAL_STRING(lp_msdfs_proxy, szMSDfsProxy)
static FN_LOCAL_STRING(lp_volume, volume)
FN_LOCAL_STRING(lp_veto_files, szVetoFiles)
FN_LOCAL_STRING(lp_hide_files, szHideFiles)
FN_LOCAL_STRING(lp_veto_oplocks, szVetoOplockFiles)
FN_LOCAL_BOOL(lp_msdfs_root, bMSDfsRoot)
FN_LOCAL_STRING(lp_aio_write_behind, szAioWriteBehind)
FN_LOCAL_STRING(lp_dfree_command, szDfree)
FN_LOCAL_BOOL(lp_autoloaded, autoloaded)
FN_LOCAL_BOOL(lp_preexec_close, bPreexecClose)
FN_LOCAL_BOOL(lp_rootpreexec_close, bRootpreexecClose)
FN_LOCAL_INTEGER(lp_casesensitive, iCaseSensitive)
FN_LOCAL_BOOL(lp_preservecase, bCasePreserve)
FN_LOCAL_BOOL(lp_shortpreservecase, bShortCasePreserve)
FN_LOCAL_BOOL(lp_hide_dot_files, bHideDotFiles)
FN_LOCAL_BOOL(lp_hide_special_files, bHideSpecialFiles)
FN_LOCAL_BOOL(lp_hideunreadable, bHideUnReadable)
FN_LOCAL_BOOL(lp_hideunwriteable_files, bHideUnWriteableFiles)
FN_LOCAL_BOOL(lp_browseable, bBrowseable)
FN_LOCAL_BOOL(lp_access_based_share_enum, bAccessBasedShareEnum)
FN_LOCAL_BOOL(lp_readonly, bRead_only)
FN_LOCAL_BOOL(lp_no_set_dir, bNo_set_dir)
FN_LOCAL_BOOL(lp_guest_ok, bGuest_ok)
FN_LOCAL_BOOL(lp_guest_only, bGuest_only)
FN_LOCAL_BOOL(lp_administrative_share, bAdministrative_share)
FN_LOCAL_BOOL(lp_print_ok, bPrint_ok)
FN_LOCAL_BOOL(lp_map_hidden, bMap_hidden)
FN_LOCAL_BOOL(lp_map_archive, bMap_archive)
FN_LOCAL_BOOL(lp_store_dos_attributes, bStoreDosAttributes)
FN_LOCAL_BOOL(lp_dmapi_support, bDmapiSupport)
FN_LOCAL_PARM_BOOL(lp_locking, bLocking)
FN_LOCAL_PARM_INTEGER(lp_strict_locking, iStrictLocking)
FN_LOCAL_PARM_BOOL(lp_posix_locking, bPosixLocking)
FN_LOCAL_BOOL(lp_share_modes, bShareModes)
FN_LOCAL_BOOL(lp_oplocks, bOpLocks)
FN_LOCAL_BOOL(lp_level2_oplocks, bLevel2OpLocks)
FN_LOCAL_BOOL(lp_onlyuser, bOnlyUser)
FN_LOCAL_PARM_BOOL(lp_manglednames, bMangledNames)
FN_LOCAL_BOOL(lp_symlinks, bSymlinks)
FN_LOCAL_BOOL(lp_syncalways, bSyncAlways)
FN_LOCAL_BOOL(lp_strict_allocate, bStrictAllocate)
FN_LOCAL_BOOL(lp_strict_sync, bStrictSync)
FN_LOCAL_BOOL(lp_map_system, bMap_system)
FN_LOCAL_BOOL(lp_delete_readonly, bDeleteReadonly)
FN_LOCAL_BOOL(lp_fake_oplocks, bFakeOplocks)
FN_LOCAL_BOOL(lp_recursive_veto_delete, bDeleteVetoFiles)
FN_LOCAL_BOOL(lp_dos_filemode, bDosFilemode)
FN_LOCAL_BOOL(lp_dos_filetimes, bDosFiletimes)
FN_LOCAL_BOOL(lp_dos_filetime_resolution, bDosFiletimeResolution)
FN_LOCAL_BOOL(lp_fake_dir_create_times, bFakeDirCreateTimes)
FN_LOCAL_BOOL(lp_blocking_locks, bBlockingLocks)
FN_LOCAL_BOOL(lp_inherit_perms, bInheritPerms)
FN_LOCAL_BOOL(lp_inherit_acls, bInheritACLS)
FN_LOCAL_BOOL(lp_inherit_owner, bInheritOwner)
FN_LOCAL_BOOL(lp_use_client_driver, bUseClientDriver)
FN_LOCAL_BOOL(lp_default_devmode, bDefaultDevmode)
FN_LOCAL_BOOL(lp_force_printername, bForcePrintername)
FN_LOCAL_BOOL(lp_nt_acl_support, bNTAclSupport)
FN_LOCAL_BOOL(lp_force_unknown_acl_user, bForceUnknownAclUser)
FN_LOCAL_BOOL(lp_ea_support, bEASupport)
FN_LOCAL_BOOL(_lp_use_sendfile, bUseSendfile)
FN_LOCAL_BOOL(lp_profile_acls, bProfileAcls)
FN_LOCAL_BOOL(lp_map_acl_inherit, bMap_acl_inherit)
FN_LOCAL_BOOL(lp_afs_share, bAfs_Share)
FN_LOCAL_BOOL(lp_acl_check_permissions, bAclCheckPermissions)
FN_LOCAL_BOOL(lp_acl_group_control, bAclGroupControl)
FN_LOCAL_BOOL(lp_acl_map_full_control, bAclMapFullControl)
FN_LOCAL_INTEGER(lp_create_mask, iCreate_mask)
FN_LOCAL_INTEGER(lp_force_create_mode, iCreate_force_mode)
FN_LOCAL_INTEGER(lp_security_mask, iSecurity_mask)
FN_LOCAL_INTEGER(lp_force_security_mode, iSecurity_force_mode)
FN_LOCAL_INTEGER(lp_dir_mask, iDir_mask)
FN_LOCAL_INTEGER(lp_force_dir_mode, iDir_force_mode)
FN_LOCAL_INTEGER(lp_dir_security_mask, iDir_Security_mask)
FN_LOCAL_INTEGER(lp_force_dir_security_mode, iDir_Security_force_mode)
FN_LOCAL_INTEGER(lp_max_connections, iMaxConnections)
FN_LOCAL_INTEGER(lp_defaultcase, iDefaultCase)
FN_LOCAL_INTEGER(lp_minprintspace, iMinPrintSpace)
FN_LOCAL_INTEGER(lp_printing, iPrinting)
FN_LOCAL_INTEGER(lp_max_reported_jobs, iMaxReportedPrintJobs)
FN_LOCAL_INTEGER(lp_oplock_contention_limit, iOplockContentionLimit)
FN_LOCAL_INTEGER(lp_csc_policy, iCSCPolicy)
FN_LOCAL_INTEGER(lp_write_cache_size, iWriteCacheSize)
FN_LOCAL_INTEGER(lp_block_size, iBlock_size)
FN_LOCAL_INTEGER(lp_dfree_cache_time, iDfreeCacheTime)
FN_LOCAL_INTEGER(lp_allocation_roundup_size, iallocation_roundup_size)
FN_LOCAL_INTEGER(lp_aio_read_size, iAioReadSize)
FN_LOCAL_INTEGER(lp_aio_write_size, iAioWriteSize)
FN_LOCAL_INTEGER(lp_map_readonly, iMap_readonly)
FN_LOCAL_INTEGER(lp_directory_name_cache_size, iDirectoryNameCacheSize)
FN_LOCAL_INTEGER(lp_smb_encrypt, ismb_encrypt)
FN_LOCAL_CHAR(lp_magicchar, magic_char)
FN_GLOBAL_INTEGER(lp_winbind_cache_time, &Globals.winbind_cache_time)
FN_GLOBAL_INTEGER(lp_winbind_reconnect_delay, &Globals.winbind_reconnect_delay)
FN_GLOBAL_INTEGER(lp_winbind_max_clients, &Globals.winbind_max_clients)
FN_GLOBAL_LIST(lp_winbind_nss_info, &Globals.szWinbindNssInfo)
FN_GLOBAL_INTEGER(lp_algorithmic_rid_base, &Globals.AlgorithmicRidBase)
FN_GLOBAL_INTEGER(lp_name_cache_timeout, &Globals.name_cache_timeout)
FN_GLOBAL_INTEGER(lp_client_signing, &Globals.client_signing)
FN_GLOBAL_INTEGER(lp_server_signing, &Globals.server_signing)
FN_GLOBAL_INTEGER(lp_client_ldap_sasl_wrapping, &Globals.client_ldap_sasl_wrapping)

/* local prototypes */

static int map_parameter(const char *pszParmName);
static int map_parameter_canonical(const char *pszParmName, bool *inverse);
static const char *get_boolean(bool bool_value);
static int getservicebyname(const char *pszServiceName,
			    struct service *pserviceDest);
static void copy_service(struct service *pserviceDest,
			 struct service *pserviceSource,
			 struct bitmap *pcopymapDest);
static bool do_parameter(const char *pszParmName, const char *pszParmValue,
			 void *userdata);
static bool do_section(const char *pszSectionName, void *userdata);
static void init_copymap(struct service *pservice);
static bool hash_a_service(const char *name, int number);
static void free_service_byindex(int iService);
static void free_param_opts(struct param_opt_struct **popts);
static char * canonicalize_servicename(const char *name);
static void show_parameter(int parmIndex);
static bool is_synonym_of(int parm1, int parm2, bool *inverse);

/*
 * This is a helper function for parametrical options support.  It returns a
 * pointer to parametrical option value if it exists or NULL otherwise. Actual
 * parametrical functions are quite simple
 */
static struct param_opt_struct *get_parametrics(int snum, const char *type,
						const char *option)
{
	bool global_section = False;
	char* param_key;
        struct param_opt_struct *data;
	
	if (snum >= iNumServices) return NULL;
	
	if (snum < 0) { 
		data = Globals.param_opt;
		global_section = True;
	} else {
		data = ServicePtrs[snum]->param_opt;
	}
    
	if (asprintf(&param_key, "%s:%s", type, option) == -1) {
		DEBUG(0,("asprintf failed!\n"));
		return NULL;
	}

	while (data) {
		if (strwicmp(data->key, param_key) == 0) {
			string_free(&param_key);
			return data;
		}
		data = data->next;
	}

	if (!global_section) {
		/* Try to fetch the same option but from globals */
		/* but only if we are not already working with Globals */
		data = Globals.param_opt;
		while (data) {
		        if (strwicmp(data->key, param_key) == 0) {
			        string_free(&param_key);
				return data;
			}
			data = data->next;
		}
	}

	string_free(&param_key);
	
	return NULL;
}


#define MISSING_PARAMETER(name) \
    DEBUG(0, ("%s(): value is NULL or empty!\n", #name))

/*******************************************************************
convenience routine to return int parameters.
********************************************************************/
static int lp_int(const char *s)
{

	if (!s || !*s) {
		MISSING_PARAMETER(lp_int);
		return (-1);
	}

	return (int)strtol(s, NULL, 0);
}

/*******************************************************************
convenience routine to return unsigned long parameters.
********************************************************************/
static unsigned long lp_ulong(const char *s)
{

	if (!s || !*s) {
		MISSING_PARAMETER(lp_ulong);
		return (0);
	}

	return strtoul(s, NULL, 0);
}

/*******************************************************************
convenience routine to return boolean parameters.
********************************************************************/
static bool lp_bool(const char *s)
{
	bool ret = False;

	if (!s || !*s) {
		MISSING_PARAMETER(lp_bool);
		return False;
	}
	
	if (!set_boolean(s, &ret)) {
		DEBUG(0,("lp_bool(%s): value is not boolean!\n",s));
		return False;
	}

	return ret;
}

/*******************************************************************
convenience routine to return enum parameters.
********************************************************************/
static int lp_enum(const char *s,const struct enum_list *_enum)
{
	int i;

	if (!s || !*s || !_enum) {
		MISSING_PARAMETER(lp_enum);
		return (-1);
	}
	
	for (i=0; _enum[i].name; i++) {
		if (strequal(_enum[i].name,s))
			return _enum[i].value;
	}

	DEBUG(0,("lp_enum(%s,enum): value is not in enum_list!\n",s));
	return (-1);
}

#undef MISSING_PARAMETER

/* DO NOT USE lp_parm_string ANYMORE!!!!
 * use lp_parm_const_string or lp_parm_talloc_string
 *
 * lp_parm_string is only used to let old modules find this symbol
 */
#undef lp_parm_string
 char *lp_parm_string(const char *servicename, const char *type, const char *option);
 char *lp_parm_string(const char *servicename, const char *type, const char *option)
{
	return lp_parm_talloc_string(lp_servicenumber(servicename), type, option, NULL);
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */
/* the returned value is talloced on the talloc_tos() */
char *lp_parm_talloc_string(int snum, const char *type, const char *option, const char *def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);
	
	if (data == NULL||data->value==NULL) {
		if (def) {
			return lp_string(def);
		} else {
			return NULL;
		}
	}

	return lp_string(data->value);
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */
const char *lp_parm_const_string(int snum, const char *type, const char *option, const char *def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);
	
	if (data == NULL||data->value==NULL)
		return def;
		
	return data->value;
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */

const char **lp_parm_string_list(int snum, const char *type, const char *option, const char **def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);

	if (data == NULL||data->value==NULL)
		return (const char **)def;
		
	if (data->list==NULL) {
		data->list = str_list_make_v3(talloc_autofree_context(), data->value, NULL);
	}

	return (const char **)data->list;
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */

int lp_parm_int(int snum, const char *type, const char *option, int def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);
	
	if (data && data->value && *data->value)
		return lp_int(data->value);

	return def;
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */

unsigned long lp_parm_ulong(int snum, const char *type, const char *option, unsigned long def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);
	
	if (data && data->value && *data->value)
		return lp_ulong(data->value);

	return def;
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */

bool lp_parm_bool(int snum, const char *type, const char *option, bool def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);
	
	if (data && data->value && *data->value)
		return lp_bool(data->value);

	return def;
}

/* Return parametric option from a given service. Type is a part of option before ':' */
/* Parametric option has following syntax: 'Type: option = value' */

int lp_parm_enum(int snum, const char *type, const char *option,
		 const struct enum_list *_enum, int def)
{
	struct param_opt_struct *data = get_parametrics(snum, type, option);
	
	if (data && data->value && *data->value && _enum)
		return lp_enum(data->value, _enum);

	return def;
}


/***************************************************************************
 Initialise a service to the defaults.
***************************************************************************/

static void init_service(struct service *pservice)
{
	memset((char *)pservice, '\0', sizeof(struct service));
	copy_service(pservice, &sDefault, NULL);
}


/**
 * free a param_opts structure.
 * param_opts handling should be moved to talloc;
 * then this whole functions reduces to a TALLOC_FREE().
 */

static void free_param_opts(struct param_opt_struct **popts)
{
	struct param_opt_struct *opt, *next_opt;

	if (popts == NULL) {
		return;
	}

	if (*popts != NULL) {
		DEBUG(5, ("Freeing parametrics:\n"));
	}
	opt = *popts;
	while (opt != NULL) {
		string_free(&opt->key);
		string_free(&opt->value);
		TALLOC_FREE(opt->list);
		next_opt = opt->next;
		SAFE_FREE(opt);
		opt = next_opt;
	}
	*popts = NULL;
}

/***************************************************************************
 Free the dynamically allocated parts of a service struct.
***************************************************************************/

static void free_service(struct service *pservice)
{
	if (!pservice)
		return;

	if (pservice->szService)
		DEBUG(5, ("free_service: Freeing service %s\n",
		       pservice->szService));

	free_parameters(pservice);

	string_free(&pservice->szService);
	bitmap_free(pservice->copymap);

	free_param_opts(&pservice->param_opt);

	ZERO_STRUCTP(pservice);
}


/***************************************************************************
 remove a service indexed in the ServicePtrs array from the ServiceHash
 and free the dynamically allocated parts
***************************************************************************/

static void free_service_byindex(int idx)
{
	if ( !LP_SNUM_OK(idx) ) 
		return;

	ServicePtrs[idx]->valid = False;
	invalid_services[num_invalid_services++] = idx;

	/* we have to cleanup the hash record */

	if (ServicePtrs[idx]->szService) {
		char *canon_name = canonicalize_servicename(
			ServicePtrs[idx]->szService );
		
		dbwrap_delete_bystring(ServiceHash, canon_name );
		TALLOC_FREE(canon_name);
	}

	free_service(ServicePtrs[idx]);
}

/***************************************************************************
 Add a new service to the services array initialising it with the given 
 service. 
***************************************************************************/

static int add_a_service(const struct service *pservice, const char *name)
{
	int i;
	struct service tservice;
	int num_to_alloc = iNumServices + 1;

	tservice = *pservice;

	/* it might already exist */
	if (name) {
		i = getservicebyname(name, NULL);
		if (i >= 0) {
			/* Clean all parametric options for service */
			/* They will be added during parsing again */
			free_param_opts(&ServicePtrs[i]->param_opt);
			return (i);
		}
	}

	/* find an invalid one */
	i = iNumServices;
	if (num_invalid_services > 0) {
		i = invalid_services[--num_invalid_services];
	}

	/* if not, then create one */
	if (i == iNumServices) {
		struct service **tsp;
		int *tinvalid;
		
		tsp = SMB_REALLOC_ARRAY_KEEP_OLD_ON_ERROR(ServicePtrs, struct service *, num_to_alloc);
		if (tsp == NULL) {
			DEBUG(0,("add_a_service: failed to enlarge ServicePtrs!\n"));
			return (-1);
		}
		ServicePtrs = tsp;
		ServicePtrs[iNumServices] = SMB_MALLOC_P(struct service);
		if (!ServicePtrs[iNumServices]) {
			DEBUG(0,("add_a_service: out of memory!\n"));
			return (-1);
		}
		iNumServices++;

		/* enlarge invalid_services here for now... */
		tinvalid = SMB_REALLOC_ARRAY_KEEP_OLD_ON_ERROR(invalid_services, int,
					     num_to_alloc);
		if (tinvalid == NULL) {
			DEBUG(0,("add_a_service: failed to enlarge "
				 "invalid_services!\n"));
			return (-1);
		}
		invalid_services = tinvalid;
	} else {
		free_service_byindex(i);
	}

	ServicePtrs[i]->valid = True;

	init_service(ServicePtrs[i]);
	copy_service(ServicePtrs[i], &tservice, NULL);
	if (name)
		string_set(&ServicePtrs[i]->szService, name);
		
	DEBUG(8,("add_a_service: Creating snum = %d for %s\n", 
		i, ServicePtrs[i]->szService));

	if (!hash_a_service(ServicePtrs[i]->szService, i)) {
		return (-1);
	}
		
	return (i);
}

/***************************************************************************
  Convert a string to uppercase and remove whitespaces.
***************************************************************************/

static char *canonicalize_servicename(const char *src)
{
	char *result;

	if ( !src ) {
		DEBUG(0,("canonicalize_servicename: NULL source name!\n"));
		return NULL;
	}

	result = talloc_strdup(talloc_tos(), src);
	SMB_ASSERT(result != NULL);

	strlower_m(result);
	return result;
}

/***************************************************************************
  Add a name/index pair for the services array to the hash table.
***************************************************************************/

static bool hash_a_service(const char *name, int idx)
{
	char *canon_name;

	if ( !ServiceHash ) {
		DEBUG(10,("hash_a_service: creating servicehash\n"));
		ServiceHash = db_open_rbt(NULL);
		if ( !ServiceHash ) {
			DEBUG(0,("hash_a_service: open tdb servicehash failed!\n"));
			return False;
		}
	}

	DEBUG(10,("hash_a_service: hashing index %d for service name %s\n",
		idx, name));

	canon_name = canonicalize_servicename( name );

	dbwrap_store_bystring(ServiceHash, canon_name,
			      make_tdb_data((uint8 *)&idx, sizeof(idx)),
			      TDB_REPLACE);

	TALLOC_FREE(canon_name);

	return True;
}

/***************************************************************************
 Add a new home service, with the specified home directory, defaults coming
 from service ifrom.
***************************************************************************/

bool lp_add_home(const char *pszHomename, int iDefaultService,
		 const char *user, const char *pszHomedir)
{
	int i;

	if (pszHomename == NULL || user == NULL || pszHomedir == NULL ||
			pszHomedir[0] == '\0') {
		return false;
	}

	i = add_a_service(ServicePtrs[iDefaultService], pszHomename);

	if (i < 0)
		return (False);

	if (!(*(ServicePtrs[iDefaultService]->szPath))
	    || strequal(ServicePtrs[iDefaultService]->szPath, lp_pathname(GLOBAL_SECTION_SNUM))) {
		string_set(&ServicePtrs[i]->szPath, pszHomedir);
	}

	if (!(*(ServicePtrs[i]->comment))) {
		char *comment = NULL;
		if (asprintf(&comment, "Home directory of %s", user) < 0) {
			return false;
		}
		string_set(&ServicePtrs[i]->comment, comment);
		SAFE_FREE(comment);
	}

	/* set the browseable flag from the global default */

	ServicePtrs[i]->bBrowseable = sDefault.bBrowseable;
	ServicePtrs[i]->bAccessBasedShareEnum = sDefault.bAccessBasedShareEnum;

	ServicePtrs[i]->autoloaded = True;

	DEBUG(3, ("adding home's share [%s] for user '%s' at '%s'\n", pszHomename, 
	       user, ServicePtrs[i]->szPath ));

	return (True);
}

/***************************************************************************
 Add a new service, based on an old one.
***************************************************************************/

int lp_add_service(const char *pszService, int iDefaultService)
{
	if (iDefaultService < 0) {
		return add_a_service(&sDefault, pszService);
	}

	return (add_a_service(ServicePtrs[iDefaultService], pszService));
}

/***************************************************************************
 Add the IPC service.
***************************************************************************/

static bool lp_add_ipc(const char *ipc_name, bool guest_ok)
{
	char *comment = NULL;
	int i = add_a_service(&sDefault, ipc_name);

	if (i < 0)
		return (False);

	if (asprintf(&comment, "IPC Service (%s)",
				Globals.szServerString) < 0) {
		return (False);
	}

	string_set(&ServicePtrs[i]->szPath, tmpdir());
	string_set(&ServicePtrs[i]->szUsername, "");
	string_set(&ServicePtrs[i]->comment, comment);
	string_set(&ServicePtrs[i]->fstype, "IPC");
	ServicePtrs[i]->iMaxConnections = 0;
	ServicePtrs[i]->bAvailable = True;
	ServicePtrs[i]->bRead_only = True;
	ServicePtrs[i]->bGuest_only = False;
	ServicePtrs[i]->bAdministrative_share = True;
	ServicePtrs[i]->bGuest_ok = guest_ok;
	ServicePtrs[i]->bPrint_ok = False;
	ServicePtrs[i]->bBrowseable = sDefault.bBrowseable;

	DEBUG(3, ("adding IPC service\n"));

	SAFE_FREE(comment);
	return (True);
}

/***************************************************************************
 Add a new printer service, with defaults coming from service iFrom.
***************************************************************************/

bool lp_add_printer(const char *pszPrintername, int iDefaultService)
{
	const char *comment = "From Printcap";
	int i = add_a_service(ServicePtrs[iDefaultService], pszPrintername);

	if (i < 0)
		return (False);

	/* note that we do NOT default the availability flag to True - */
	/* we take it from the default service passed. This allows all */
	/* dynamic printers to be disabled by disabling the [printers] */
	/* entry (if/when the 'available' keyword is implemented!).    */

	/* the printer name is set to the service name. */
	string_set(&ServicePtrs[i]->szPrintername, pszPrintername);
	string_set(&ServicePtrs[i]->comment, comment);

	/* set the browseable flag from the gloabl default */
	ServicePtrs[i]->bBrowseable = sDefault.bBrowseable;

	/* Printers cannot be read_only. */
	ServicePtrs[i]->bRead_only = False;
	/* No share modes on printer services. */
	ServicePtrs[i]->bShareModes = False;
	/* No oplocks on printer services. */
	ServicePtrs[i]->bOpLocks = False;
	/* Printer services must be printable. */
	ServicePtrs[i]->bPrint_ok = True;
	
	DEBUG(3, ("adding printer service %s\n", pszPrintername));

	return (True);
}


/***************************************************************************
 Check whether the given parameter name is valid.
 Parametric options (names containing a colon) are considered valid.
***************************************************************************/

bool lp_parameter_is_valid(const char *pszParmName)
{
	return ((map_parameter(pszParmName) != -1) ||
		(strchr(pszParmName, ':') != NULL));
}

/***************************************************************************
 Check whether the given name is the name of a global parameter.
 Returns True for strings belonging to parameters of class
 P_GLOBAL, False for all other strings, also for parametric options
 and strings not belonging to any option.
***************************************************************************/

bool lp_parameter_is_global(const char *pszParmName)
{
	int num = map_parameter(pszParmName);

	if (num >= 0) {
		return (parm_table[num].p_class == P_GLOBAL);
	}

	return False;
}

/**************************************************************************
 Check whether the given name is the canonical name of a parameter.
 Returns False if it is not a valid parameter Name.
 For parametric options, True is returned.
**************************************************************************/

bool lp_parameter_is_canonical(const char *parm_name)
{
	if (!lp_parameter_is_valid(parm_name)) {
		return False;
	}

	return (map_parameter(parm_name) ==
		map_parameter_canonical(parm_name, NULL));
}

/**************************************************************************
 Determine the canonical name for a parameter.
 Indicate when it is an inverse (boolean) synonym instead of a
 "usual" synonym.
**************************************************************************/

bool lp_canonicalize_parameter(const char *parm_name, const char **canon_parm,
			       bool *inverse)
{
	int num;

	if (!lp_parameter_is_valid(parm_name)) {
		*canon_parm = NULL;
		return False;
	}

	num = map_parameter_canonical(parm_name, inverse);
	if (num < 0) {
		/* parametric option */
		*canon_parm = parm_name;
	} else {
		*canon_parm = parm_table[num].label;
	}

	return True;

}

/**************************************************************************
 Determine the canonical name for a parameter.
 Turn the value given into the inverse boolean expression when
 the synonym is an invers boolean synonym.

 Return True if parm_name is a valid parameter name and
 in case it is an invers boolean synonym, if the val string could
 successfully be converted to the reverse bool.
 Return false in all other cases.
**************************************************************************/

bool lp_canonicalize_parameter_with_value(const char *parm_name,
					  const char *val,
					  const char **canon_parm,
					  const char **canon_val)
{
	int num;
	bool inverse;

	if (!lp_parameter_is_valid(parm_name)) {
		*canon_parm = NULL;
		*canon_val = NULL;
		return False;
	}

	num = map_parameter_canonical(parm_name, &inverse);
	if (num < 0) {
		/* parametric option */
		*canon_parm = parm_name;
		*canon_val = val;
	} else {
		*canon_parm = parm_table[num].label;
		if (inverse) {
			if (!lp_invert_boolean(val, canon_val)) {
				*canon_val = NULL;
				return False;
			}
		} else {
			*canon_val = val;
		}
	}

	return True;
}

/***************************************************************************
 Map a parameter's string representation to something we can use. 
 Returns False if the parameter string is not recognised, else TRUE.
***************************************************************************/

static int map_parameter(const char *pszParmName)
{
	int iIndex;

	if (*pszParmName == '-' && !strequal(pszParmName, "-valid"))
		return (-1);

	for (iIndex = 0; parm_table[iIndex].label; iIndex++)
		if (strwicmp(parm_table[iIndex].label, pszParmName) == 0)
			return (iIndex);

	/* Warn only if it isn't parametric option */
	if (strchr(pszParmName, ':') == NULL)
		DEBUG(1, ("Unknown parameter encountered: \"%s\"\n", pszParmName));
	/* We do return 'fail' for parametric options as well because they are
	   stored in different storage
	 */
	return (-1);
}

/***************************************************************************
 Map a parameter's string representation to the index of the canonical
 form of the parameter (it might be a synonym).
 Returns -1 if the parameter string is not recognised.
***************************************************************************/

static int map_parameter_canonical(const char *pszParmName, bool *inverse)
{
	int parm_num, canon_num;
	bool loc_inverse = False;

	parm_num = map_parameter(pszParmName);
	if ((parm_num < 0) || !(parm_table[parm_num].flags & FLAG_HIDE)) {
		/* invalid, parametric or no canidate for synonyms ... */
		goto done;
	}

	for (canon_num = 0; parm_table[canon_num].label; canon_num++) {
		if (is_synonym_of(parm_num, canon_num, &loc_inverse)) {
			parm_num = canon_num;
			goto done;
		}
	}

done:
	if (inverse != NULL) {
		*inverse = loc_inverse;
	}
	return parm_num;
}

/***************************************************************************
 return true if parameter number parm1 is a synonym of parameter
 number parm2 (parm2 being the principal name).
 set inverse to True if parm1 is P_BOOLREV and parm2 is P_BOOL,
 False otherwise.
***************************************************************************/

static bool is_synonym_of(int parm1, int parm2, bool *inverse)
{
	if ((parm_table[parm1].ptr == parm_table[parm2].ptr) &&
	    (parm_table[parm1].flags & FLAG_HIDE) &&
	    !(parm_table[parm2].flags & FLAG_HIDE))
	{
		if (inverse != NULL) {
			if ((parm_table[parm1].type == P_BOOLREV) &&
			    (parm_table[parm2].type == P_BOOL))
			{
				*inverse = True;
			} else {
				*inverse = False;
			}
		}
		return True;
	}
	return False;
}

/***************************************************************************
 Show one parameter's name, type, [values,] and flags.
 (helper functions for show_parameter_list)
***************************************************************************/

static void show_parameter(int parmIndex)
{
	int enumIndex, flagIndex;
	int parmIndex2;
	bool hadFlag;
	bool hadSyn;
	bool inverse;
	const char *type[] = { "P_BOOL", "P_BOOLREV", "P_CHAR", "P_INTEGER",
		"P_OCTAL", "P_LIST", "P_STRING", "P_USTRING",
		"P_ENUM", "P_SEP"};
	unsigned flags[] = { FLAG_BASIC, FLAG_SHARE, FLAG_PRINT, FLAG_GLOBAL,
		FLAG_WIZARD, FLAG_ADVANCED, FLAG_DEVELOPER, FLAG_DEPRECATED,
		FLAG_HIDE, FLAG_DOS_STRING};
	const char *flag_names[] = { "FLAG_BASIC", "FLAG_SHARE", "FLAG_PRINT",
		"FLAG_GLOBAL", "FLAG_WIZARD", "FLAG_ADVANCED", "FLAG_DEVELOPER",
		"FLAG_DEPRECATED", "FLAG_HIDE", "FLAG_DOS_STRING", NULL};

	printf("%s=%s", parm_table[parmIndex].label,
	       type[parm_table[parmIndex].type]);
	if (parm_table[parmIndex].type == P_ENUM) {
		printf(",");
		for (enumIndex=0;
		     parm_table[parmIndex].enum_list[enumIndex].name;
		     enumIndex++)
		{
			printf("%s%s",
			       enumIndex ? "|" : "",
			       parm_table[parmIndex].enum_list[enumIndex].name);
		}
	}
	printf(",");
	hadFlag = False;
	for (flagIndex=0; flag_names[flagIndex]; flagIndex++) {
		if (parm_table[parmIndex].flags & flags[flagIndex]) {
			printf("%s%s",
				hadFlag ? "|" : "",
				flag_names[flagIndex]);
			hadFlag = True;
		}
	}

	/* output synonyms */
	hadSyn = False;
	for (parmIndex2=0; parm_table[parmIndex2].label; parmIndex2++) {
		if (is_synonym_of(parmIndex, parmIndex2, &inverse)) {
			printf(" (%ssynonym of %s)", inverse ? "inverse " : "",
			       parm_table[parmIndex2].label);
		} else if (is_synonym_of(parmIndex2, parmIndex, &inverse)) {
			if (!hadSyn) {
				printf(" (synonyms: ");
				hadSyn = True;
			} else {
				printf(", ");
			}
			printf("%s%s", parm_table[parmIndex2].label,
			       inverse ? "[i]" : "");
		}
	}
	if (hadSyn) {
		printf(")");
	}

	printf("\n");
}

/***************************************************************************
 Show all parameter's name, type, [values,] and flags.
***************************************************************************/

void show_parameter_list(void)
{
	int classIndex, parmIndex;
	const char *section_names[] = { "local", "global", NULL};

	for (classIndex=0; section_names[classIndex]; classIndex++) {
		printf("[%s]\n", section_names[classIndex]);
		for (parmIndex = 0; parm_table[parmIndex].label; parmIndex++) {
			if (parm_table[parmIndex].p_class == classIndex) {
				show_parameter(parmIndex);
			}
		}
	}
}

/***************************************************************************
 Check if a given string correctly represents a boolean value.
***************************************************************************/

bool lp_string_is_valid_boolean(const char *parm_value)
{
	return set_boolean(parm_value, NULL);
}

/***************************************************************************
 Get the standard string representation of a boolean value ("yes" or "no")
***************************************************************************/

static const char *get_boolean(bool bool_value)
{
	static const char *yes_str = "yes";
	static const char *no_str = "no";

	return (bool_value ? yes_str : no_str);
}

/***************************************************************************
 Provide the string of the negated boolean value associated to the boolean
 given as a string. Returns False if the passed string does not correctly
 represent a boolean.
***************************************************************************/

bool lp_invert_boolean(const char *str, const char **inverse_str)
{
	bool val;

	if (!set_boolean(str, &val)) {
		return False;
	}

	*inverse_str = get_boolean(!val);
	return True;
}

/***************************************************************************
 Provide the canonical string representation of a boolean value given
 as a string. Return True on success, False if the string given does
 not correctly represent a boolean.
***************************************************************************/

bool lp_canonicalize_boolean(const char *str, const char**canon_str)
{
	bool val;

	if (!set_boolean(str, &val)) {
		return False;
	}

	*canon_str = get_boolean(val);
	return True;
}

/***************************************************************************
Find a service by name. Otherwise works like get_service.
***************************************************************************/

static int getservicebyname(const char *pszServiceName, struct service *pserviceDest)
{
	int iService = -1;
	char *canon_name;
	TDB_DATA data;

	if (ServiceHash == NULL) {
		return -1;
	}

	canon_name = canonicalize_servicename(pszServiceName);

	data = dbwrap_fetch_bystring(ServiceHash, canon_name, canon_name);

	if ((data.dptr != NULL) && (data.dsize == sizeof(iService))) {
		iService = *(int *)data.dptr;
	}

	TALLOC_FREE(canon_name);

	if ((iService != -1) && (LP_SNUM_OK(iService))
	    && (pserviceDest != NULL)) {
		copy_service(pserviceDest, ServicePtrs[iService], NULL);
	}

	return (iService);
}

/***************************************************************************
 Copy a service structure to another.
 If pcopymapDest is NULL then copy all fields
***************************************************************************/

/**
 * Add a parametric option to a param_opt_struct,
 * replacing old value, if already present.
 */
static void set_param_opt(struct param_opt_struct **opt_list,
			  const char *opt_name,
			  const char *opt_value)
{
	struct param_opt_struct *new_opt, *opt;
	bool not_added;

	if (opt_list == NULL) {
		return;
	}

	opt = *opt_list;
	not_added = true;

	/* Traverse destination */
	while (opt) {
		/* If we already have same option, override it */
		if (strwicmp(opt->key, opt_name) == 0) {
			string_free(&opt->value);
			TALLOC_FREE(opt->list);
			opt->value = SMB_STRDUP(opt_value);
			not_added = false;
			break;
		}
		opt = opt->next;
	}
	if (not_added) {
	    new_opt = SMB_XMALLOC_P(struct param_opt_struct);
	    new_opt->key = SMB_STRDUP(opt_name);
	    new_opt->value = SMB_STRDUP(opt_value);
	    new_opt->list = NULL;
	    DLIST_ADD(*opt_list, new_opt);
	}
}

static void copy_service(struct service *pserviceDest, struct service *pserviceSource,
			 struct bitmap *pcopymapDest)
{
	int i;
	bool bcopyall = (pcopymapDest == NULL);
	struct param_opt_struct *data;

	for (i = 0; parm_table[i].label; i++)
		if (parm_table[i].ptr && parm_table[i].p_class == P_LOCAL &&
		    (bcopyall || bitmap_query(pcopymapDest,i))) {
			void *def_ptr = parm_table[i].ptr;
			void *src_ptr =
				((char *)pserviceSource) + PTR_DIFF(def_ptr,
								    &sDefault);
			void *dest_ptr =
				((char *)pserviceDest) + PTR_DIFF(def_ptr,
								  &sDefault);

			switch (parm_table[i].type) {
				case P_BOOL:
				case P_BOOLREV:
					*(bool *)dest_ptr = *(bool *)src_ptr;
					break;

				case P_INTEGER:
				case P_ENUM:
				case P_OCTAL:
					*(int *)dest_ptr = *(int *)src_ptr;
					break;

				case P_CHAR:
					*(char *)dest_ptr = *(char *)src_ptr;
					break;

				case P_STRING:
					string_set((char **)dest_ptr,
						   *(char **)src_ptr);
					break;

				case P_USTRING:
					string_set((char **)dest_ptr,
						   *(char **)src_ptr);
					strupper_m(*(char **)dest_ptr);
					break;
				case P_LIST:
					TALLOC_FREE(*((char ***)dest_ptr));
					*((char ***)dest_ptr) = str_list_copy(NULL, 
						      *(const char ***)src_ptr);
					break;
				default:
					break;
			}
		}

	if (bcopyall) {
		init_copymap(pserviceDest);
		if (pserviceSource->copymap)
			bitmap_copy(pserviceDest->copymap,
				    pserviceSource->copymap);
	}
	
	data = pserviceSource->param_opt;
	while (data) {
		set_param_opt(&pserviceDest->param_opt, data->key, data->value);
		data = data->next;
	}
}

/***************************************************************************
Check a service for consistency. Return False if the service is in any way
incomplete or faulty, else True.
***************************************************************************/

bool service_ok(int iService)
{
	bool bRetval;

	bRetval = True;
	if (ServicePtrs[iService]->szService[0] == '\0') {
		DEBUG(0, ("The following message indicates an internal error:\n"));
		DEBUG(0, ("No service name in service entry.\n"));
		bRetval = False;
	}

	/* The [printers] entry MUST be printable. I'm all for flexibility, but */
	/* I can't see why you'd want a non-printable printer service...        */
	if (strwicmp(ServicePtrs[iService]->szService, PRINTERS_NAME) == 0) {
		if (!ServicePtrs[iService]->bPrint_ok) {
			DEBUG(0, ("WARNING: [%s] service MUST be printable!\n",
			       ServicePtrs[iService]->szService));
			ServicePtrs[iService]->bPrint_ok = True;
		}
		/* [printers] service must also be non-browsable. */
		if (ServicePtrs[iService]->bBrowseable)
			ServicePtrs[iService]->bBrowseable = False;
	}

	if (ServicePtrs[iService]->szPath[0] == '\0' &&
	    strwicmp(ServicePtrs[iService]->szService, HOMES_NAME) != 0 &&
	    ServicePtrs[iService]->szMSDfsProxy[0] == '\0'
	    ) {
		DEBUG(0, ("WARNING: No path in service %s - making it unavailable!\n",
			ServicePtrs[iService]->szService));
		ServicePtrs[iService]->bAvailable = False;
	}

	/* If a service is flagged unavailable, log the fact at level 1. */
	if (!ServicePtrs[iService]->bAvailable)
		DEBUG(1, ("NOTE: Service %s is flagged unavailable.\n",
			  ServicePtrs[iService]->szService));

	return (bRetval);
}

static struct smbconf_ctx *lp_smbconf_ctx(void)
{
	WERROR werr;
	static struct smbconf_ctx *conf_ctx = NULL;

	if (conf_ctx == NULL) {
		werr = smbconf_init(NULL, &conf_ctx, "registry:");
		if (!W_ERROR_IS_OK(werr)) {
			DEBUG(1, ("error initializing registry configuration: "
				  "%s\n", win_errstr(werr)));
			conf_ctx = NULL;
		}
	}

	return conf_ctx;
}

static bool process_smbconf_service(struct smbconf_service *service)
{
	uint32_t count;
	bool ret;

	if (service == NULL) {
		return false;
	}

	ret = do_section(service->name, NULL);
	if (ret != true) {
		return false;
	}
	for (count = 0; count < service->num_params; count++) {
		ret = do_parameter(service->param_names[count],
				   service->param_values[count],
				   NULL);
		if (ret != true) {
			return false;
		}
	}
	if (iServiceIndex >= 0) {
		return service_ok(iServiceIndex);
	}
	return true;
}

/**
 * load a service from registry and activate it
 */
bool process_registry_service(const char *service_name)
{
	WERROR werr;
	struct smbconf_service *service = NULL;
	TALLOC_CTX *mem_ctx = talloc_stackframe();
	struct smbconf_ctx *conf_ctx = lp_smbconf_ctx();
	bool ret = false;

	if (conf_ctx == NULL) {
		goto done;
	}

	DEBUG(5, ("process_registry_service: service name %s\n", service_name));

	if (!smbconf_share_exists(conf_ctx, service_name)) {
		/*
		 * Registry does not contain data for this service (yet),
		 * but make sure lp_load doesn't return false.
		 */
		ret = true;
		goto done;
	}

	werr = smbconf_get_share(conf_ctx, mem_ctx, service_name, &service);
	if (!W_ERROR_IS_OK(werr)) {
		goto done;
	}

	ret = process_smbconf_service(service);
	if (!ret) {
		goto done;
	}

	/* store the csn */
	smbconf_changed(conf_ctx, &conf_last_csn, NULL, NULL);

done:
	TALLOC_FREE(mem_ctx);
	return ret;
}

/*
 * process_registry_globals
 */
static bool process_registry_globals(void)
{
	bool ret;

	add_to_file_list(INCLUDE_REGISTRY_NAME, INCLUDE_REGISTRY_NAME);

	ret = do_parameter("registry shares", "yes", NULL);
	if (!ret) {
		return ret;
	}

	return process_registry_service(GLOBAL_NAME);
}

bool process_registry_shares(void)
{
	WERROR werr;
	uint32_t count;
	struct smbconf_service **service = NULL;
	uint32_t num_shares = 0;
	TALLOC_CTX *mem_ctx = talloc_stackframe();
	struct smbconf_ctx *conf_ctx = lp_smbconf_ctx();
	bool ret = false;

	if (conf_ctx == NULL) {
		goto done;
	}

	werr = smbconf_get_config(conf_ctx, mem_ctx, &num_shares, &service);
	if (!W_ERROR_IS_OK(werr)) {
		goto done;
	}

	ret = true;

	for (count = 0; count < num_shares; count++) {
		if (strequal(service[count]->name, GLOBAL_NAME)) {
			continue;
		}
		ret = process_smbconf_service(service[count]);
		if (!ret) {
			goto done;
		}
	}

	/* store the csn */
	smbconf_changed(conf_ctx, &conf_last_csn, NULL, NULL);

done:
	TALLOC_FREE(mem_ctx);
	return ret;
}

/**
 * reload those shares from registry that are already
 * activated in the services array.
 */
static bool reload_registry_shares(void)
{
	int i;
	bool ret = true;

	for (i = 0; i < iNumServices; i++) {
		if (!VALID(i)) {
			continue;
		}

		if (ServicePtrs[i]->usershare == USERSHARE_VALID) {
			continue;
		}

		ret = process_registry_service(ServicePtrs[i]->szService);
		if (!ret) {
			goto done;
		}
	}

done:
	return ret;
}


#define MAX_INCLUDE_DEPTH 100

static uint8_t include_depth;

static struct file_lists {
	struct file_lists *next;
	char *name;
	char *subfname;
	time_t modtime;
} *file_lists = NULL;

/*******************************************************************
 Keep a linked list of all config files so we know when one has changed 
 it's date and needs to be reloaded.
********************************************************************/

static void add_to_file_list(const char *fname, const char *subfname)
{
	struct file_lists *f = file_lists;

	while (f) {
		if (f->name && !strcmp(f->name, fname))
			break;
		f = f->next;
	}

	if (!f) {
		f = SMB_MALLOC_P(struct file_lists);
		if (!f)
			return;
		f->next = file_lists;
		f->name = SMB_STRDUP(fname);
		if (!f->name) {
			SAFE_FREE(f);
			return;
		}
		f->subfname = SMB_STRDUP(subfname);
		if (!f->subfname) {
			SAFE_FREE(f);
			return;
		}
		file_lists = f;
		f->modtime = file_modtime(subfname);
	} else {
		time_t t = file_modtime(subfname);
		if (t)
			f->modtime = t;
	}
}

/**
 * Free the file lists
 */
static void free_file_list(void)
{
	struct file_lists *f;
	struct file_lists *next;

	f = file_lists;
	while( f ) {
		next = f->next;
		SAFE_FREE( f->name );
		SAFE_FREE( f->subfname );
		SAFE_FREE( f );
		f = next;
	}
	file_lists = NULL;
}


/**
 * Utility function for outsiders to check if we're running on registry.
 */
bool lp_config_backend_is_registry(void)
{
	return (lp_config_backend() == CONFIG_BACKEND_REGISTRY);
}

/**
 * Utility function to check if the config backend is FILE.
 */
bool lp_config_backend_is_file(void)
{
	return (lp_config_backend() == CONFIG_BACKEND_FILE);
}

/*******************************************************************
 Check if a config file has changed date.
********************************************************************/

bool lp_file_list_changed(void)
{
	struct file_lists *f = file_lists;

 	DEBUG(6, ("lp_file_list_changed()\n"));

	while (f) {
		char *n2 = NULL;
		time_t mod_time;

		if (strequal(f->name, INCLUDE_REGISTRY_NAME)) {
			struct smbconf_ctx *conf_ctx = lp_smbconf_ctx();

			if (conf_ctx == NULL) {
				return false;
			}
			if (smbconf_changed(conf_ctx, &conf_last_csn, NULL,
					    NULL))
			{
				DEBUGADD(6, ("registry config changed\n"));
				return true;
			}
		} else {
			n2 = alloc_sub_basic(get_current_username(),
					    current_user_info.domain,
					    f->name);
			if (!n2) {
				return false;
			}
			DEBUGADD(6, ("file %s -> %s  last mod_time: %s\n",
				     f->name, n2, ctime(&f->modtime)));

			mod_time = file_modtime(n2);

			if (mod_time &&
			    ((f->modtime != mod_time) ||
			     (f->subfname == NULL) ||
			     (strcmp(n2, f->subfname) != 0)))
			{
				DEBUGADD(6,
					 ("file %s modified: %s\n", n2,
					  ctime(&mod_time)));
				f->modtime = mod_time;
				SAFE_FREE(f->subfname);
				f->subfname = n2; /* Passing ownership of
						     return from alloc_sub_basic
						     above. */
				return true;
			}
			SAFE_FREE(n2);
		}
		f = f->next;
	}
	return (False);
}


/***************************************************************************
 Run standard_sub_basic on netbios name... needed because global_myname
 is not accessed through any lp_ macro.
 Note: We must *NOT* use string_set() here as ptr points to global_myname.
***************************************************************************/

static bool handle_netbios_name(int snum, const char *pszParmValue, char **ptr)
{
	bool ret;
	char *netbios_name = alloc_sub_basic(get_current_username(),
					current_user_info.domain,
					pszParmValue);

	ret = set_global_myname(netbios_name);
	SAFE_FREE(netbios_name);
	string_set(&Globals.szNetbiosName,global_myname());

	DEBUG(4, ("handle_netbios_name: set global_myname to: %s\n",
	       global_myname()));

	return ret;
}

static bool handle_charset(int snum, const char *pszParmValue, char **ptr)
{
	if (strcmp(*ptr, pszParmValue) != 0) {
		string_set(ptr, pszParmValue);
		init_iconv();
	}
	return True;
}



static bool handle_workgroup(int snum, const char *pszParmValue, char **ptr)
{
	bool ret;
	
	ret = set_global_myworkgroup(pszParmValue);
	string_set(&Globals.szWorkgroup,lp_workgroup());
	
	return ret;
}

static bool handle_netbios_scope(int snum, const char *pszParmValue, char **ptr)
{
	bool ret;
	
	ret = set_global_scope(pszParmValue);
	string_set(&Globals.szNetbiosScope,global_scope());

	return ret;
}

static bool handle_netbios_aliases(int snum, const char *pszParmValue, char **ptr)
{
	TALLOC_FREE(Globals.szNetbiosAliases);
	Globals.szNetbiosAliases = str_list_make_v3(talloc_autofree_context(), pszParmValue, NULL);
	return set_netbios_aliases((const char **)Globals.szNetbiosAliases);
}

/***************************************************************************
 Handle the include operation.
***************************************************************************/
static bool bAllowIncludeRegistry = true;

static bool handle_include(int snum, const char *pszParmValue, char **ptr)
{
	char *fname;

	if (include_depth >= MAX_INCLUDE_DEPTH) {
		DEBUG(0, ("Error: Maximum include depth (%u) exceeded!\n",
			  include_depth));
		return false;
	}

	if (strequal(pszParmValue, INCLUDE_REGISTRY_NAME)) {
		if (!bAllowIncludeRegistry) {
			return true;
		}
		if (bInGlobalSection) {
			bool ret;
			include_depth++;
			ret = process_registry_globals();
			include_depth--;
			return ret;
		} else {
			DEBUG(1, ("\"include = registry\" only effective "
				  "in %s section\n", GLOBAL_NAME));
			return false;
		}
	}

	fname = alloc_sub_basic(get_current_username(),
				current_user_info.domain,
				pszParmValue);

	add_to_file_list(pszParmValue, fname);

	string_set(ptr, fname);

	if (file_exist(fname)) {
		bool ret;
		include_depth++;
		ret = pm_process(fname, do_section, do_parameter, NULL);
		include_depth--;
		SAFE_FREE(fname);
		return ret;
	}

	DEBUG(2, ("Can't find include file %s\n", fname));
	SAFE_FREE(fname);
	return true;
}

/***************************************************************************
 Handle the interpretation of the copy parameter.
***************************************************************************/

static bool handle_copy(int snum, const char *pszParmValue, char **ptr)
{
	bool bRetval;
	int iTemp;
	struct service serviceTemp;

	string_set(ptr, pszParmValue);

	init_service(&serviceTemp);

	bRetval = False;

	DEBUG(3, ("Copying service from service %s\n", pszParmValue));

	if ((iTemp = getservicebyname(pszParmValue, &serviceTemp)) >= 0) {
		if (iTemp == iServiceIndex) {
			DEBUG(0, ("Can't copy service %s - unable to copy self!\n", pszParmValue));
		} else {
			copy_service(ServicePtrs[iServiceIndex],
				     &serviceTemp,
				     ServicePtrs[iServiceIndex]->copymap);
			bRetval = True;
		}
	} else {
		DEBUG(0, ("Unable to copy service - source not found: %s\n", pszParmValue));
		bRetval = False;
	}

	free_service(&serviceTemp);
	return (bRetval);
}

static bool handle_ldap_debug_level(int snum, const char *pszParmValue, char **ptr)
{
	Globals.ldap_debug_level = lp_int(pszParmValue);
	init_ldap_debugging();
	return true;
}

/***************************************************************************
 Handle idmap/non unix account uid and gid allocation parameters.  The format of these
 parameters is:

 [global]

        idmap uid = 1000-1999
        idmap gid = 700-899

 We only do simple parsing checks here.  The strings are parsed into useful
 structures in the idmap daemon code.

***************************************************************************/

/* Some lp_ routines to return idmap [ug]id information */

static uid_t idmap_uid_low, idmap_uid_high;
static gid_t idmap_gid_low, idmap_gid_high;

bool lp_idmap_uid(uid_t *low, uid_t *high)
{
        if (idmap_uid_low == 0 || idmap_uid_high == 0)
                return False;

        if (low)
                *low = idmap_uid_low;

        if (high)
                *high = idmap_uid_high;

        return True;
}

bool lp_idmap_gid(gid_t *low, gid_t *high)
{
        if (idmap_gid_low == 0 || idmap_gid_high == 0)
                return False;

        if (low)
                *low = idmap_gid_low;

        if (high)
                *high = idmap_gid_high;

        return True;
}

/* Do some simple checks on "idmap [ug]id" parameter values */

static bool handle_idmap_uid(int snum, const char *pszParmValue, char **ptr)
{
	uint32 low, high;

	if (sscanf(pszParmValue, "%u - %u", &low, &high) != 2 || high < low)
		return False;

	/* Parse OK */

	string_set(ptr, pszParmValue);

        idmap_uid_low = low;
        idmap_uid_high = high;

	return True;
}

static bool handle_idmap_gid(int snum, const char *pszParmValue, char **ptr)
{
	uint32 low, high;

	if (sscanf(pszParmValue, "%u - %u", &low, &high) != 2 || high < low)
		return False;

	/* Parse OK */

	string_set(ptr, pszParmValue);

        idmap_gid_low = low;
        idmap_gid_high = high;

	return True;
}

/***************************************************************************
 Handle the DEBUG level list.
***************************************************************************/

static bool handle_debug_list( int snum, const char *pszParmValueIn, char **ptr )
{
	string_set(ptr, pszParmValueIn);
	return debug_parse_levels(pszParmValueIn);
}

/***************************************************************************
 Handle ldap suffixes - default to ldapsuffix if sub-suffixes are not defined.
***************************************************************************/

static const char *append_ldap_suffix( const char *str )
{
	const char *suffix_string;


	suffix_string = talloc_asprintf(talloc_tos(), "%s,%s", str,
					Globals.szLdapSuffix );
	if ( !suffix_string ) {
		DEBUG(0,("append_ldap_suffix: talloc_asprintf() failed!\n"));
		return "";
	}

	return suffix_string;
}

const char *lp_ldap_machine_suffix(void)
{
	if (Globals.szLdapMachineSuffix[0])
		return append_ldap_suffix(Globals.szLdapMachineSuffix);

	return lp_string(Globals.szLdapSuffix);
}

const char *lp_ldap_user_suffix(void)
{
	if (Globals.szLdapUserSuffix[0])
		return append_ldap_suffix(Globals.szLdapUserSuffix);

	return lp_string(Globals.szLdapSuffix);
}

const char *lp_ldap_group_suffix(void)
{
	if (Globals.szLdapGroupSuffix[0])
		return append_ldap_suffix(Globals.szLdapGroupSuffix);

	return lp_string(Globals.szLdapSuffix);
}

const char *lp_ldap_idmap_suffix(void)
{
	if (Globals.szLdapIdmapSuffix[0])
		return append_ldap_suffix(Globals.szLdapIdmapSuffix);

	return lp_string(Globals.szLdapSuffix);
}

/****************************************************************************
 set the value for a P_ENUM
 ***************************************************************************/

static void lp_set_enum_parm( struct parm_struct *parm, const char *pszParmValue,
                              int *ptr )
{
	int i;

	for (i = 0; parm->enum_list[i].name; i++) {
		if ( strequal(pszParmValue, parm->enum_list[i].name)) {
			*ptr = parm->enum_list[i].value;
			return;
		}
	}
	DEBUG(0, ("WARNING: Ignoring invalid value '%s' for parameter '%s'\n",
		  pszParmValue, parm->label));
}

/***************************************************************************
***************************************************************************/

static bool handle_printing(int snum, const char *pszParmValue, char **ptr)
{
	static int parm_num = -1;
	struct service *s;

	if ( parm_num == -1 )
		parm_num = map_parameter( "printing" );

	lp_set_enum_parm( &parm_table[parm_num], pszParmValue, (int*)ptr );

	if ( snum < 0 )
		s = &sDefault;
	else
		s = ServicePtrs[snum];

	init_printer_values( s );

	return True;
}


/***************************************************************************
 Initialise a copymap.
***************************************************************************/

static void init_copymap(struct service *pservice)
{
	int i;
	if (pservice->copymap) {
		bitmap_free(pservice->copymap);
	}
	pservice->copymap = bitmap_allocate(NUMPARAMETERS);
	if (!pservice->copymap)
		DEBUG(0,
		      ("Couldn't allocate copymap!! (size %d)\n",
		       (int)NUMPARAMETERS));
	else
		for (i = 0; i < NUMPARAMETERS; i++)
			bitmap_set(pservice->copymap, i);
}

/***************************************************************************
 Return the local pointer to a parameter given a service struct and the
 pointer into the default structure.
***************************************************************************/

static void *lp_local_ptr(struct service *service, void *ptr)
{
	return (void *)(((char *)service) + PTR_DIFF(ptr, &sDefault));
}

/***************************************************************************
 Return the local pointer to a parameter given the service number and the 
 pointer into the default structure.
***************************************************************************/

void *lp_local_ptr_by_snum(int snum, void *ptr)
{
	return lp_local_ptr(ServicePtrs[snum], ptr);
}

/***************************************************************************
 Process a parameter for a particular service number. If snum < 0
 then assume we are in the globals.
***************************************************************************/

bool lp_do_parameter(int snum, const char *pszParmName, const char *pszParmValue)
{
	int parmnum, i;
	void *parm_ptr = NULL;	/* where we are going to store the result */
	void *def_ptr = NULL;
	struct param_opt_struct **opt_list;

	parmnum = map_parameter(pszParmName);

	if (parmnum < 0) {
		if (strchr(pszParmName, ':') == NULL) {
			DEBUG(0, ("Ignoring unknown parameter \"%s\"\n",
				  pszParmName));
			return (True);
		}

		/*
		 * We've got a parametric option
		 */

		opt_list = (snum < 0)
			? &Globals.param_opt : &ServicePtrs[snum]->param_opt;
		set_param_opt(opt_list, pszParmName, pszParmValue);

		return (True);
	}

	if (parm_table[parmnum].flags & FLAG_DEPRECATED) {
		DEBUG(1, ("WARNING: The \"%s\" option is deprecated\n",
			  pszParmName));
	}

	def_ptr = parm_table[parmnum].ptr;

	/* we might point at a service, the default service or a global */
	if (snum < 0) {
		parm_ptr = def_ptr;
	} else {
		if (parm_table[parmnum].p_class == P_GLOBAL) {
			DEBUG(0,
			      ("Global parameter %s found in service section!\n",
			       pszParmName));
			return (True);
		}
		parm_ptr = lp_local_ptr_by_snum(snum, def_ptr);
	}

	if (snum >= 0) {
		if (!ServicePtrs[snum]->copymap)
			init_copymap(ServicePtrs[snum]);

		/* this handles the aliases - set the copymap for other entries with
		   the same data pointer */
		for (i = 0; parm_table[i].label; i++)
			if (parm_table[i].ptr == parm_table[parmnum].ptr)
				bitmap_clear(ServicePtrs[snum]->copymap, i);
	}

	/* if it is a special case then go ahead */
	if (parm_table[parmnum].special) {
		return parm_table[parmnum].special(snum, pszParmValue,
						   (char **)parm_ptr);
	}

	/* now switch on the type of variable it is */
	switch (parm_table[parmnum].type)
	{
		case P_BOOL:
			*(bool *)parm_ptr = lp_bool(pszParmValue);
			break;

		case P_BOOLREV:
			*(bool *)parm_ptr = !lp_bool(pszParmValue);
			break;

		case P_INTEGER:
			*(int *)parm_ptr = lp_int(pszParmValue);
			break;

		case P_CHAR:
			*(char *)parm_ptr = *pszParmValue;
			break;

		case P_OCTAL:
			i = sscanf(pszParmValue, "%o", (int *)parm_ptr);
			if ( i != 1 ) {
			    DEBUG ( 0, ("Invalid octal number %s\n", pszParmName ));
			}
			break;

		case P_LIST:
			TALLOC_FREE(*((char ***)parm_ptr));
			*(char ***)parm_ptr = str_list_make_v3(
				talloc_autofree_context(), pszParmValue, NULL);
			break;

		case P_STRING:
			string_set((char **)parm_ptr, pszParmValue);
			break;

		case P_USTRING:
			string_set((char **)parm_ptr, pszParmValue);
			strupper_m(*(char **)parm_ptr);
			break;

		case P_ENUM:
			lp_set_enum_parm( &parm_table[parmnum], pszParmValue, (int*)parm_ptr );
			break;
		case P_SEP:
			break;
	}

	return (True);
}

/***************************************************************************
 Process a parameter.
***************************************************************************/

static bool do_parameter(const char *pszParmName, const char *pszParmValue,
			 void *userdata)
{
	if (!bInGlobalSection && bGlobalOnly)
		return (True);

	DEBUGADD(4, ("doing parameter %s = %s\n", pszParmName, pszParmValue));

	return (lp_do_parameter(bInGlobalSection ? -2 : iServiceIndex,
				pszParmName, pszParmValue));
}

/***************************************************************************
 Print a parameter of the specified type.
***************************************************************************/

static void print_parameter(struct parm_struct *p, void *ptr, FILE * f)
{
	int i;
	switch (p->type)
	{
		case P_ENUM:
			for (i = 0; p->enum_list[i].name; i++) {
				if (*(int *)ptr == p->enum_list[i].value) {
					fprintf(f, "%s",
						p->enum_list[i].name);
					break;
				}
			}
			break;

		case P_BOOL:
			fprintf(f, "%s", BOOLSTR(*(bool *)ptr));
			break;

		case P_BOOLREV:
			fprintf(f, "%s", BOOLSTR(!*(bool *)ptr));
			break;

		case P_INTEGER:
			fprintf(f, "%d", *(int *)ptr);
			break;

		case P_CHAR:
			fprintf(f, "%c", *(char *)ptr);
			break;

		case P_OCTAL: {
			char *o = octal_string(*(int *)ptr);
			fprintf(f, "%s", o);
			TALLOC_FREE(o);
			break;
		}

		case P_LIST:
			if ((char ***)ptr && *(char ***)ptr) {
				char **list = *(char ***)ptr;
				for (; *list; list++) {
					/* surround strings with whitespace in double quotes */
					if ( strchr_m( *list, ' ' ) )
						fprintf(f, "\"%s\"%s", *list, ((*(list+1))?", ":""));
					else
						fprintf(f, "%s%s", *list, ((*(list+1))?", ":""));
				}
			}
			break;

		case P_STRING:
		case P_USTRING:
			if (*(char **)ptr) {
				fprintf(f, "%s", *(char **)ptr);
			}
			break;
		case P_SEP:
			break;
	}
}

/***************************************************************************
 Check if two parameters are equal.
***************************************************************************/

static bool equal_parameter(parm_type type, void *ptr1, void *ptr2)
{
	switch (type) {
		case P_BOOL:
		case P_BOOLREV:
			return (*((bool *)ptr1) == *((bool *)ptr2));

		case P_INTEGER:
		case P_ENUM:
		case P_OCTAL:
			return (*((int *)ptr1) == *((int *)ptr2));

		case P_CHAR:
			return (*((char *)ptr1) == *((char *)ptr2));

		case P_LIST:
			return str_list_equal(*(const char ***)ptr1, *(const char ***)ptr2);

		case P_STRING:
		case P_USTRING:
		{
			char *p1 = *(char **)ptr1, *p2 = *(char **)ptr2;
			if (p1 && !*p1)
				p1 = NULL;
			if (p2 && !*p2)
				p2 = NULL;
			return (p1 == p2 || strequal(p1, p2));
		}
		case P_SEP:
			break;
	}
	return (False);
}

/***************************************************************************
 Initialize any local varients in the sDefault table.
***************************************************************************/

void init_locals(void)
{
	/* None as yet. */
}

/***************************************************************************
 Process a new section (service). At this stage all sections are services.
 Later we'll have special sections that permit server parameters to be set.
 Returns True on success, False on failure. 
***************************************************************************/

static bool do_section(const char *pszSectionName, void *userdata)
{
	bool bRetval;
	bool isglobal = ((strwicmp(pszSectionName, GLOBAL_NAME) == 0) ||
			 (strwicmp(pszSectionName, GLOBAL_NAME2) == 0));
	bRetval = False;

	/* if we were in a global section then do the local inits */
	if (bInGlobalSection && !isglobal)
		init_locals();

	/* if we've just struck a global section, note the fact. */
	bInGlobalSection = isglobal;

	/* check for multiple global sections */
	if (bInGlobalSection) {
		DEBUG(3, ("Processing section \"[%s]\"\n", pszSectionName));
		return (True);
	}

	if (!bInGlobalSection && bGlobalOnly)
		return (True);

	/* if we have a current service, tidy it up before moving on */
	bRetval = True;

	if (iServiceIndex >= 0)
		bRetval = service_ok(iServiceIndex);

	/* if all is still well, move to the next record in the services array */
	if (bRetval) {
		/* We put this here to avoid an odd message order if messages are */
		/* issued by the post-processing of a previous section. */
		DEBUG(2, ("Processing section \"[%s]\"\n", pszSectionName));

		if ((iServiceIndex = add_a_service(&sDefault, pszSectionName))
		    < 0) {
			DEBUG(0, ("Failed to add a new service\n"));
			return (False);
		}
	}

	return (bRetval);
}


/***************************************************************************
 Determine if a partcular base parameter is currentl set to the default value.
***************************************************************************/

static bool is_default(int i)
{
	if (!defaults_saved)
		return False;
	switch (parm_table[i].type) {
		case P_LIST:
			return str_list_equal((const char **)parm_table[i].def.lvalue, 
						*(const char ***)parm_table[i].ptr);
		case P_STRING:
		case P_USTRING:
			return strequal(parm_table[i].def.svalue,
					*(char **)parm_table[i].ptr);
		case P_BOOL:
		case P_BOOLREV:
			return parm_table[i].def.bvalue ==
				*(bool *)parm_table[i].ptr;
		case P_CHAR:
			return parm_table[i].def.cvalue ==
				*(char *)parm_table[i].ptr;
		case P_INTEGER:
		case P_OCTAL:
		case P_ENUM:
			return parm_table[i].def.ivalue ==
				*(int *)parm_table[i].ptr;
		case P_SEP:
			break;
	}
	return False;
}

/***************************************************************************
Display the contents of the global structure.
***************************************************************************/

static void dump_globals(FILE *f)
{
	int i;
	struct param_opt_struct *data;
	
	fprintf(f, "[global]\n");

	for (i = 0; parm_table[i].label; i++)
		if (parm_table[i].p_class == P_GLOBAL &&
		    !(parm_table[i].flags & FLAG_META) &&
		    parm_table[i].ptr &&
		    (i == 0 || (parm_table[i].ptr != parm_table[i - 1].ptr))) {
			if (defaults_saved && is_default(i))
				continue;
			fprintf(f, "\t%s = ", parm_table[i].label);
			print_parameter(&parm_table[i], parm_table[i].ptr, f);
			fprintf(f, "\n");
	}
	if (Globals.param_opt != NULL) {
		data = Globals.param_opt;
		while(data) {
			fprintf(f, "\t%s = %s\n", data->key, data->value);
			data = data->next;
		}
        }

}

/***************************************************************************
 Return True if a local parameter is currently set to the global default.
***************************************************************************/

bool lp_is_default(int snum, struct parm_struct *parm)
{
	int pdiff = PTR_DIFF(parm->ptr, &sDefault);

	return equal_parameter(parm->type,
			       ((char *)ServicePtrs[snum]) + pdiff,
			       ((char *)&sDefault) + pdiff);
}

/***************************************************************************
 Display the contents of a single services record.
***************************************************************************/

static void dump_a_service(struct service *pService, FILE * f)
{
	int i;
	struct param_opt_struct *data;
	
	if (pService != &sDefault)
		fprintf(f, "[%s]\n", pService->szService);

	for (i = 0; parm_table[i].label; i++) {

		if (parm_table[i].p_class == P_LOCAL &&
		    !(parm_table[i].flags & FLAG_META) &&
		    parm_table[i].ptr &&
		    (*parm_table[i].label != '-') &&
		    (i == 0 || (parm_table[i].ptr != parm_table[i - 1].ptr))) 
		{
		
			int pdiff = PTR_DIFF(parm_table[i].ptr, &sDefault);

			if (pService == &sDefault) {
				if (defaults_saved && is_default(i))
					continue;
			} else {
				if (equal_parameter(parm_table[i].type,
						    ((char *)pService) +
						    pdiff,
						    ((char *)&sDefault) +
						    pdiff))
					continue;
			}

			fprintf(f, "\t%s = ", parm_table[i].label);
			print_parameter(&parm_table[i],
					((char *)pService) + pdiff, f);
			fprintf(f, "\n");
		}
	}

		if (pService->param_opt != NULL) {
			data = pService->param_opt;
			while(data) {
				fprintf(f, "\t%s = %s\n", data->key, data->value);
				data = data->next;
			}
        	}
}

/***************************************************************************
 Display the contents of a parameter of a single services record.
***************************************************************************/

bool dump_a_parameter(int snum, char *parm_name, FILE * f, bool isGlobal)
{
	int i;
	bool result = False;
	parm_class p_class;
	unsigned flag = 0;
	fstring local_parm_name;
	char *parm_opt;
	const char *parm_opt_value;

	/* check for parametrical option */
	fstrcpy( local_parm_name, parm_name);
	parm_opt = strchr( local_parm_name, ':');

	if (parm_opt) {
		*parm_opt = '\0';
		parm_opt++;
		if (strlen(parm_opt)) {
			parm_opt_value = lp_parm_const_string( snum,
				local_parm_name, parm_opt, NULL);
			if (parm_opt_value) {
				printf( "%s\n", parm_opt_value);
				result = True;
			}
		}
		return result;
	}

	/* check for a key and print the value */
	if (isGlobal) {
		p_class = P_GLOBAL;
		flag = FLAG_GLOBAL;
	} else
		p_class = P_LOCAL;

	for (i = 0; parm_table[i].label; i++) {
		if (strwicmp(parm_table[i].label, parm_name) == 0 &&
		    !(parm_table[i].flags & FLAG_META) &&
		    (parm_table[i].p_class == p_class || parm_table[i].flags & flag) &&
		    parm_table[i].ptr &&
		    (*parm_table[i].label != '-') &&
		    (i == 0 || (parm_table[i].ptr != parm_table[i - 1].ptr))) 
		{
			void *ptr;

			if (isGlobal) {
				ptr = parm_table[i].ptr;
			} else {
				struct service *pService = ServicePtrs[snum];
				ptr = ((char *)pService) +
					PTR_DIFF(parm_table[i].ptr, &sDefault);
			}

			print_parameter(&parm_table[i],
					ptr, f);
			fprintf(f, "\n");
			result = True;
			break;
		}
	}

	return result;
}

/***************************************************************************
 Return info about the requested parameter (given as a string).
 Return NULL when the string is not a valid parameter name.
***************************************************************************/

struct parm_struct *lp_get_parameter(const char *param_name)
{
	int num = map_parameter(param_name);

	if (num < 0) {
		return NULL;
	}

	return &parm_table[num];
}

/***************************************************************************
 Return info about the next parameter in a service.
 snum==GLOBAL_SECTION_SNUM gives the globals.
 Return NULL when out of parameters.
***************************************************************************/

struct parm_struct *lp_next_parameter(int snum, int *i, int allparameters)
{
	if (snum < 0) {
		/* do the globals */
		for (; parm_table[*i].label; (*i)++) {
			if (parm_table[*i].p_class == P_SEPARATOR)
				return &parm_table[(*i)++];

			if (!parm_table[*i].ptr
			    || (*parm_table[*i].label == '-'))
				continue;

			if ((*i) > 0
			    && (parm_table[*i].ptr ==
				parm_table[(*i) - 1].ptr))
				continue;
			
			if (is_default(*i) && !allparameters)
				continue;

			return &parm_table[(*i)++];
		}
	} else {
		struct service *pService = ServicePtrs[snum];

		for (; parm_table[*i].label; (*i)++) {
			if (parm_table[*i].p_class == P_SEPARATOR)
				return &parm_table[(*i)++];

			if (parm_table[*i].p_class == P_LOCAL &&
			    parm_table[*i].ptr &&
			    (*parm_table[*i].label != '-') &&
			    ((*i) == 0 ||
			     (parm_table[*i].ptr !=
			      parm_table[(*i) - 1].ptr)))
			{
				int pdiff =
					PTR_DIFF(parm_table[*i].ptr,
						 &sDefault);

				if (allparameters ||
				    !equal_parameter(parm_table[*i].type,
						     ((char *)pService) +
						     pdiff,
						     ((char *)&sDefault) +
						     pdiff))
				{
					return &parm_table[(*i)++];
				}
			}
		}
	}

	return NULL;
}


#if 0
/***************************************************************************
 Display the contents of a single copy structure.
***************************************************************************/
static void dump_copy_map(bool *pcopymap)
{
	int i;
	if (!pcopymap)
		return;

	printf("\n\tNon-Copied parameters:\n");

	for (i = 0; parm_table[i].label; i++)
		if (parm_table[i].p_class == P_LOCAL &&
		    parm_table[i].ptr && !pcopymap[i] &&
		    (i == 0 || (parm_table[i].ptr != parm_table[i - 1].ptr)))
		{
			printf("\t\t%s\n", parm_table[i].label);
		}
}
#endif

/***************************************************************************
 Return TRUE if the passed service number is within range.
***************************************************************************/

bool lp_snum_ok(int iService)
{
	return (LP_SNUM_OK(iService) && ServicePtrs[iService]->bAvailable);
}

/***************************************************************************
 Auto-load some home services.
***************************************************************************/

static void lp_add_auto_services(char *str)
{
	char *s;
	char *p;
	int homes;
	char *saveptr;

	if (!str)
		return;

	s = SMB_STRDUP(str);
	if (!s)
		return;

	homes = lp_servicenumber(HOMES_NAME);

	for (p = strtok_r(s, LIST_SEP, &saveptr); p;
	     p = strtok_r(NULL, LIST_SEP, &saveptr)) {
		char *home;

		if (lp_servicenumber(p) >= 0)
			continue;

		home = get_user_home_dir(talloc_tos(), p);

		if (home && home[0] && homes >= 0)
			lp_add_home(p, homes, p, home);

		TALLOC_FREE(home);
	}
	SAFE_FREE(s);
}

/***************************************************************************
 Auto-load one printer.
***************************************************************************/

void lp_add_one_printer(const char *name, const char *comment, void *pdata)
{
	int printers = lp_servicenumber(PRINTERS_NAME);
	int i;

	if (lp_servicenumber(name) < 0) {
		lp_add_printer(name, printers);
		if ((i = lp_servicenumber(name)) >= 0) {
			string_set(&ServicePtrs[i]->comment, comment);
			ServicePtrs[i]->autoloaded = True;
		}
	}
}

/***************************************************************************
 Have we loaded a services file yet?
***************************************************************************/

bool lp_loaded(void)
{
	return (bLoaded);
}

/***************************************************************************
 Unload unused services.
***************************************************************************/

void lp_killunused(bool (*snumused) (int))
{
	int i;
	for (i = 0; i < iNumServices; i++) {
		if (!VALID(i))
			continue;

		/* don't kill autoloaded or usershare services */
		if ( ServicePtrs[i]->autoloaded ||
				ServicePtrs[i]->usershare == USERSHARE_VALID) {
			continue;
		}

		if (!snumused || !snumused(i)) {
			free_service_byindex(i);
		}
	}
}

/**
 * Kill all except autoloaded and usershare services - convenience wrapper
 */
void lp_kill_all_services(void)
{
	lp_killunused(NULL);
}

/***************************************************************************
 Unload a service.
***************************************************************************/

void lp_killservice(int iServiceIn)
{
	if (VALID(iServiceIn)) {
		free_service_byindex(iServiceIn);
	}
}

/***************************************************************************
 Save the curent values of all global and sDefault parameters into the 
 defaults union. This allows swat and testparm to show only the
 changed (ie. non-default) parameters.
***************************************************************************/

static void lp_save_defaults(void)
{
	int i;
	for (i = 0; parm_table[i].label; i++) {
		if (i > 0 && parm_table[i].ptr == parm_table[i - 1].ptr)
			continue;
		switch (parm_table[i].type) {
			case P_LIST:
				parm_table[i].def.lvalue = str_list_copy(
					NULL, *(const char ***)parm_table[i].ptr);
				break;
			case P_STRING:
			case P_USTRING:
				if (parm_table[i].ptr) {
					parm_table[i].def.svalue = SMB_STRDUP(*(char **)parm_table[i].ptr);
				} else {
					parm_table[i].def.svalue = NULL;
				}
				break;
			case P_BOOL:
			case P_BOOLREV:
				parm_table[i].def.bvalue =
					*(bool *)parm_table[i].ptr;
				break;
			case P_CHAR:
				parm_table[i].def.cvalue =
					*(char *)parm_table[i].ptr;
				break;
			case P_INTEGER:
			case P_OCTAL:
			case P_ENUM:
				parm_table[i].def.ivalue =
					*(int *)parm_table[i].ptr;
				break;
			case P_SEP:
				break;
		}
	}
	defaults_saved = True;
}

/*******************************************************************
 Set the server type we will announce as via nmbd.
********************************************************************/

static const struct srv_role_tab {
	uint32 role;
	const char *role_str;
} srv_role_tab [] = {
	{ ROLE_STANDALONE, "ROLE_STANDALONE" },
	{ ROLE_DOMAIN_MEMBER, "ROLE_DOMAIN_MEMBER" },
	{ ROLE_DOMAIN_BDC, "ROLE_DOMAIN_BDC" },
	{ ROLE_DOMAIN_PDC, "ROLE_DOMAIN_PDC" },
	{ 0, NULL }
};

const char* server_role_str(uint32 role)
{
	int i = 0;
	for (i=0; srv_role_tab[i].role_str; i++) {
		if (role == srv_role_tab[i].role) {
			return srv_role_tab[i].role_str;
		}
	}
	return NULL;
}

static void set_server_role(void)
{
	server_role = ROLE_STANDALONE;

	switch (lp_security()) {
		case SEC_SHARE:
			if (lp_domain_logons())
				DEBUG(0, ("Server's Role (logon server) conflicts with share-level security\n"));
			break;
		case SEC_SERVER:
			if (lp_domain_logons())
				DEBUG(0, ("Server's Role (logon server) conflicts with server-level security\n"));
			/* this used to be considered ROLE_DOMAIN_MEMBER but that's just wrong */
			server_role = ROLE_STANDALONE;
			break;
		case SEC_DOMAIN:
			if (lp_domain_logons()) {
				DEBUG(1, ("Server's Role (logon server) NOT ADVISED with domain-level security\n"));
				server_role = ROLE_DOMAIN_BDC;
				break;
			}
			server_role = ROLE_DOMAIN_MEMBER;
			break;
		case SEC_ADS:
			if (lp_domain_logons()) {
				server_role = ROLE_DOMAIN_PDC;
				break;
			}
			server_role = ROLE_DOMAIN_MEMBER;
			break;
		case SEC_USER:
			if (lp_domain_logons()) {

				if (Globals.iDomainMaster) /* auto or yes */ 
					server_role = ROLE_DOMAIN_PDC;
				else
					server_role = ROLE_DOMAIN_BDC;
			}
			break;
		default:
			DEBUG(0, ("Server's Role undefined due to unknown security mode\n"));
			break;
	}

	DEBUG(10, ("set_server_role: role = %s\n", server_role_str(server_role)));
}

/***********************************************************
 If we should send plaintext/LANMAN passwords in the clinet
************************************************************/

static void set_allowed_client_auth(void)
{
	if (Globals.bClientNTLMv2Auth) {
		Globals.bClientLanManAuth = False;
	}
	if (!Globals.bClientLanManAuth) {
		Globals.bClientPlaintextAuth = False;
	}
}

/***************************************************************************
 JRA.
 The following code allows smbd to read a user defined share file.
 Yes, this is my intent. Yes, I'm comfortable with that...

 THE FOLLOWING IS SECURITY CRITICAL CODE.

 It washes your clothes, it cleans your house, it guards you while you sleep...
 Do not f%^k with it....
***************************************************************************/

#define MAX_USERSHARE_FILE_SIZE (10*1024)

/***************************************************************************
 Check allowed stat state of a usershare file.
 Ensure we print out who is dicking with us so the admin can
 get their sorry ass fired.
***************************************************************************/

static bool check_usershare_stat(const char *fname,
				 const SMB_STRUCT_STAT *psbuf)
{
	if (!S_ISREG(psbuf->st_ex_mode)) {
		DEBUG(0,("check_usershare_stat: file %s owned by uid %u is "
			"not a regular file\n",
			fname, (unsigned int)psbuf->st_ex_uid ));
		return False;
	}

	/* Ensure this doesn't have the other write bit set. */
	if (psbuf->st_ex_mode & S_IWOTH) {
		DEBUG(0,("check_usershare_stat: file %s owned by uid %u allows "
			"public write. Refusing to allow as a usershare file.\n",
			fname, (unsigned int)psbuf->st_ex_uid ));
		return False;
	}

	/* Should be 10k or less. */
	if (psbuf->st_ex_size > MAX_USERSHARE_FILE_SIZE) {
		DEBUG(0,("check_usershare_stat: file %s owned by uid %u is "
			"too large (%u) to be a user share file.\n",
			fname, (unsigned int)psbuf->st_ex_uid,
			(unsigned int)psbuf->st_ex_size ));
		return False;
	}

	return True;
}

/***************************************************************************
 Parse the contents of a usershare file.
***************************************************************************/

enum usershare_err parse_usershare_file(TALLOC_CTX *ctx,
			SMB_STRUCT_STAT *psbuf,
			const char *servicename,
			int snum,
			char **lines,
			int numlines,
			char **pp_sharepath,
			char **pp_comment,
			SEC_DESC **ppsd,
			bool *pallow_guest)
{
	const char **prefixallowlist = lp_usershare_prefix_allow_list();
	const char **prefixdenylist = lp_usershare_prefix_deny_list();
	int us_vers;
	SMB_STRUCT_DIR *dp;
	SMB_STRUCT_STAT sbuf;
	char *sharepath = NULL;
	char *comment = NULL;

	*pp_sharepath = NULL;
	*pp_comment = NULL;

	*pallow_guest = False;

	if (numlines < 4) {
		return USERSHARE_MALFORMED_FILE;
	}

	if (strcmp(lines[0], "#VERSION 1") == 0) {
		us_vers = 1;
	} else if (strcmp(lines[0], "#VERSION 2") == 0) {
		us_vers = 2;
		if (numlines < 5) {
			return USERSHARE_MALFORMED_FILE;
		}
	} else {
		return USERSHARE_BAD_VERSION;
	}

	if (strncmp(lines[1], "path=", 5) != 0) {
		return USERSHARE_MALFORMED_PATH;
	}

	sharepath = talloc_strdup(ctx, &lines[1][5]);
	if (!sharepath) {
		return USERSHARE_POSIX_ERR;
	}
	trim_string(sharepath, " ", " ");

	if (strncmp(lines[2], "comment=", 8) != 0) {
		return USERSHARE_MALFORMED_COMMENT_DEF;
	}

	comment = talloc_strdup(ctx, &lines[2][8]);
	if (!comment) {
		return USERSHARE_POSIX_ERR;
	}
	trim_string(comment, " ", " ");
	trim_char(comment, '"', '"');

	if (strncmp(lines[3], "usershare_acl=", 14) != 0) {
		return USERSHARE_MALFORMED_ACL_DEF;
	}

	if (!parse_usershare_acl(ctx, &lines[3][14], ppsd)) {
		return USERSHARE_ACL_ERR;
	}

	if (us_vers == 2) {
		if (strncmp(lines[4], "guest_ok=", 9) != 0) {
			return USERSHARE_MALFORMED_ACL_DEF;
		}
		if (lines[4][9] == 'y') {
			*pallow_guest = True;
		}
	}

	if (snum != -1 && (strcmp(sharepath, ServicePtrs[snum]->szPath) == 0)) {
		/* Path didn't change, no checks needed. */
		*pp_sharepath = sharepath;
		*pp_comment = comment;
		return USERSHARE_OK;
	}

	/* The path *must* be absolute. */
	if (sharepath[0] != '/') {
		DEBUG(2,("parse_usershare_file: share %s: path %s is not an absolute path.\n",
			servicename, sharepath));
		return USERSHARE_PATH_NOT_ABSOLUTE;
	}

	/* If there is a usershare prefix deny list ensure one of these paths
	   doesn't match the start of the user given path. */
	if (prefixdenylist) {
		int i;
		for ( i=0; prefixdenylist[i]; i++ ) {
			DEBUG(10,("parse_usershare_file: share %s : checking prefixdenylist[%d]='%s' against %s\n",
				servicename, i, prefixdenylist[i], sharepath ));
			if (memcmp( sharepath, prefixdenylist[i], strlen(prefixdenylist[i])) == 0) {
				DEBUG(2,("parse_usershare_file: share %s path %s starts with one of the "
					"usershare prefix deny list entries.\n",
					servicename, sharepath));
				return USERSHARE_PATH_IS_DENIED;
			}
		}
	}

	/* If there is a usershare prefix allow list ensure one of these paths
	   does match the start of the user given path. */

	if (prefixallowlist) {
		int i;
		for ( i=0; prefixallowlist[i]; i++ ) {
			DEBUG(10,("parse_usershare_file: share %s checking prefixallowlist[%d]='%s' against %s\n",
				servicename, i, prefixallowlist[i], sharepath ));
			if (memcmp( sharepath, prefixallowlist[i], strlen(prefixallowlist[i])) == 0) {
				break;
			}
		}
		if (prefixallowlist[i] == NULL) {
			DEBUG(2,("parse_usershare_file: share %s path %s doesn't start with one of the "
				"usershare prefix allow list entries.\n",
				servicename, sharepath));
			return USERSHARE_PATH_NOT_ALLOWED;
		}
        }

	/* Ensure this is pointing to a directory. */
	dp = sys_opendir(sharepath);

	if (!dp) {
		DEBUG(2,("parse_usershare_file: share %s path %s is not a directory.\n",
			servicename, sharepath));
		return USERSHARE_PATH_NOT_DIRECTORY;
	}

	/* Ensure the owner of the usershare file has permission to share
	   this directory. */

	if (sys_stat(sharepath, &sbuf, false) == -1) {
		DEBUG(2,("parse_usershare_file: share %s : stat failed on path %s. %s\n",
			servicename, sharepath, strerror(errno) ));
		sys_closedir(dp);
		return USERSHARE_POSIX_ERR;
	}

	sys_closedir(dp);

	if (!S_ISDIR(sbuf.st_ex_mode)) {
		DEBUG(2,("parse_usershare_file: share %s path %s is not a directory.\n",
			servicename, sharepath ));
		return USERSHARE_PATH_NOT_DIRECTORY;
	}

	/* Check if sharing is restricted to owner-only. */
	/* psbuf is the stat of the usershare definition file,
	   sbuf is the stat of the target directory to be shared. */

	if (lp_usershare_owner_only()) {
		/* root can share anything. */
		if ((psbuf->st_ex_uid != 0) && (sbuf.st_ex_uid != psbuf->st_ex_uid)) {
			return USERSHARE_PATH_NOT_ALLOWED;
		}
	}

	*pp_sharepath = sharepath;
	*pp_comment = comment;
	return USERSHARE_OK;
}

/***************************************************************************
 Deal with a usershare file.
 Returns:
	>= 0 - snum
	-1 - Bad name, invalid contents.
	   - service name already existed and not a usershare, problem
	    with permissions to share directory etc.
***************************************************************************/

static int process_usershare_file(const char *dir_name, const char *file_name, int snum_template)
{
	SMB_STRUCT_STAT sbuf;
	SMB_STRUCT_STAT lsbuf;
	char *fname = NULL;
	char *sharepath = NULL;
	char *comment = NULL;
	fstring service_name;
	char **lines = NULL;
	int numlines = 0;
	int fd = -1;
	int iService = -1;
	TALLOC_CTX *ctx = NULL;
	SEC_DESC *psd = NULL;
	bool guest_ok = False;

	/* Ensure share name doesn't contain invalid characters. */
	if (!validate_net_name(file_name, INVALID_SHARENAME_CHARS, strlen(file_name))) {
		DEBUG(0,("process_usershare_file: share name %s contains "
			"invalid characters (any of %s)\n",
			file_name, INVALID_SHARENAME_CHARS ));
		return -1;
	}

	fstrcpy(service_name, file_name);

	if (asprintf(&fname, "%s/%s", dir_name, file_name) < 0) {
	}

	/* Minimize the race condition by doing an lstat before we
	   open and fstat. Ensure this isn't a symlink link. */

	if (sys_lstat(fname, &lsbuf, false) != 0) {
		DEBUG(0,("process_usershare_file: stat of %s failed. %s\n",
			fname, strerror(errno) ));
		SAFE_FREE(fname);
		return -1;
	}

	/* This must be a regular file, not a symlink, directory or
	   other strange filetype. */
	if (!check_usershare_stat(fname, &lsbuf)) {
		SAFE_FREE(fname);
		return -1;
	}

	{
		char *canon_name = canonicalize_servicename(service_name);
		TDB_DATA data = dbwrap_fetch_bystring(
			ServiceHash, canon_name, canon_name);

		iService = -1;

		if ((data.dptr != NULL) && (data.dsize == sizeof(iService))) {
			iService = *(int *)data.dptr;
		}
		TALLOC_FREE(canon_name);
	}

	if (iService != -1 &&
	    timespec_compare(&ServicePtrs[iService]->usershare_last_mod,
			     &lsbuf.st_ex_mtime) == 0) {
		/* Nothing changed - Mark valid and return. */
		DEBUG(10,("process_usershare_file: service %s not changed.\n",
			service_name ));
		ServicePtrs[iService]->usershare = USERSHARE_VALID;
		SAFE_FREE(fname);
		return iService;
	}

	/* Try and open the file read only - no symlinks allowed. */
#ifdef O_NOFOLLOW
	fd = sys_open(fname, O_RDONLY|O_NOFOLLOW, 0);
#else
	fd = sys_open(fname, O_RDONLY, 0);
#endif

	if (fd == -1) {
		DEBUG(0,("process_usershare_file: unable to open %s. %s\n",
			fname, strerror(errno) ));
		SAFE_FREE(fname);
		return -1;
	}

	/* Now fstat to be *SURE* it's a regular file. */
	if (sys_fstat(fd, &sbuf, false) != 0) {
		close(fd);
		DEBUG(0,("process_usershare_file: fstat of %s failed. %s\n",
			fname, strerror(errno) ));
		SAFE_FREE(fname);
		return -1;
	}

	/* Is it the same dev/inode as was lstated ? */
	if (lsbuf.st_ex_dev != sbuf.st_ex_dev || lsbuf.st_ex_ino != sbuf.st_ex_ino) {
		close(fd);
		DEBUG(0,("process_usershare_file: fstat of %s is a different file from lstat. "
			"Symlink spoofing going on ?\n", fname ));
		SAFE_FREE(fname);
		return -1;
	}

	/* This must be a regular file, not a symlink, directory or
	   other strange filetype. */
	if (!check_usershare_stat(fname, &sbuf)) {
		SAFE_FREE(fname);
		return -1;
	}

	lines = fd_lines_load(fd, &numlines, MAX_USERSHARE_FILE_SIZE, NULL);

	close(fd);
	if (lines == NULL) {
		DEBUG(0,("process_usershare_file: loading file %s owned by %u failed.\n",
			fname, (unsigned int)sbuf.st_ex_uid ));
		SAFE_FREE(fname);
		return -1;
	}

	SAFE_FREE(fname);

	/* Should we allow printers to be shared... ? */
	ctx = talloc_init("usershare_sd_xctx");
	if (!ctx) {
		TALLOC_FREE(lines);
		return 1;
	}

	if (parse_usershare_file(ctx, &sbuf, service_name,
			iService, lines, numlines, &sharepath,
			&comment, &psd, &guest_ok) != USERSHARE_OK) {
		talloc_destroy(ctx);
		TALLOC_FREE(lines);
		return -1;
	}

	TALLOC_FREE(lines);

	/* Everything ok - add the service possibly using a template. */
	if (iService < 0) {
		const struct service *sp = &sDefault;
		if (snum_template != -1) {
			sp = ServicePtrs[snum_template];
		}

		if ((iService = add_a_service(sp, service_name)) < 0) {
			DEBUG(0, ("process_usershare_file: Failed to add "
				"new service %s\n", service_name));
			talloc_destroy(ctx);
			return -1;
		}

		/* Read only is controlled by usershare ACL below. */
		ServicePtrs[iService]->bRead_only = False;
	}

	/* Write the ACL of the new/modified share. */
	if (!set_share_security(service_name, psd)) {
		 DEBUG(0, ("process_usershare_file: Failed to set share "
			"security for user share %s\n",
			service_name ));
		lp_remove_service(iService);
		talloc_destroy(ctx);
		return -1;
	}

	/* If from a template it may be marked invalid. */
	ServicePtrs[iService]->valid = True;

	/* Set the service as a valid usershare. */
	ServicePtrs[iService]->usershare = USERSHARE_VALID;

	/* Set guest access. */
	if (lp_usershare_allow_guests()) {
		ServicePtrs[iService]->bGuest_ok = guest_ok;
	}

	/* And note when it was loaded. */
	ServicePtrs[iService]->usershare_last_mod = sbuf.st_ex_mtime;
	string_set(&ServicePtrs[iService]->szPath, sharepath);
	string_set(&ServicePtrs[iService]->comment, comment);

	talloc_destroy(ctx);

	return iService;
}

/***************************************************************************
 Checks if a usershare entry has been modified since last load.
***************************************************************************/

static bool usershare_exists(int iService, struct timespec *last_mod)
{
	SMB_STRUCT_STAT lsbuf;
	const char *usersharepath = Globals.szUsersharePath;
	char *fname;

	if (asprintf(&fname, "%s/%s",
				usersharepath,
				ServicePtrs[iService]->szService) < 0) {
		return false;
	}

	if (sys_lstat(fname, &lsbuf, false) != 0) {
		SAFE_FREE(fname);
		return false;
	}

	if (!S_ISREG(lsbuf.st_ex_mode)) {
		SAFE_FREE(fname);
		return false;
	}

	SAFE_FREE(fname);
	*last_mod = lsbuf.st_ex_mtime;
	return true;
}

/***************************************************************************
 Load a usershare service by name. Returns a valid servicenumber or -1.
***************************************************************************/

int load_usershare_service(const char *servicename)
{
	SMB_STRUCT_STAT sbuf;
	const char *usersharepath = Globals.szUsersharePath;
	int max_user_shares = Globals.iUsershareMaxShares;
	int snum_template = -1;

	if (*usersharepath == 0 ||  max_user_shares == 0) {
		return -1;
	}

	if (sys_stat(usersharepath, &sbuf, false) != 0) {
		DEBUG(0,("load_usershare_service: stat of %s failed. %s\n",
			usersharepath, strerror(errno) ));
		return -1;
	}

	if (!S_ISDIR(sbuf.st_ex_mode)) {
		DEBUG(0,("load_usershare_service: %s is not a directory.\n",
			usersharepath ));
		return -1;
	}

	/*
	 * This directory must be owned by root, and have the 't' bit set.
	 * It also must not be writable by "other".
	 */

#ifdef S_ISVTX
	if (sbuf.st_ex_uid != 0 || !(sbuf.st_ex_mode & S_ISVTX) || (sbuf.st_ex_mode & S_IWOTH)) {
#else
	if (sbuf.st_ex_uid != 0 || (sbuf.st_ex_mode & S_IWOTH)) {
#endif
		DEBUG(0,("load_usershare_service: directory %s is not owned by root "
			"or does not have the sticky bit 't' set or is writable by anyone.\n",
			usersharepath ));
		return -1;
	}

	/* Ensure the template share exists if it's set. */
	if (Globals.szUsershareTemplateShare[0]) {
		/* We can't use lp_servicenumber here as we are recommending that
		   template shares have -valid=False set. */
		for (snum_template = iNumServices - 1; snum_template >= 0; snum_template--) {
			if (ServicePtrs[snum_template]->szService &&
					strequal(ServicePtrs[snum_template]->szService,
						Globals.szUsershareTemplateShare)) {
				break;
			}
		}

		if (snum_template == -1) {
			DEBUG(0,("load_usershare_service: usershare template share %s "
				"does not exist.\n",
				Globals.szUsershareTemplateShare ));
			return -1;
		}
	}

	return process_usershare_file(usersharepath, servicename, snum_template);
}

/***************************************************************************
 Load all user defined shares from the user share directory.
 We only do this if we're enumerating the share list.
 This is the function that can delete usershares that have
 been removed.
***************************************************************************/

int load_usershare_shares(void)
{
	SMB_STRUCT_DIR *dp;
	SMB_STRUCT_STAT sbuf;
	SMB_STRUCT_DIRENT *de;
	int num_usershares = 0;
	int max_user_shares = Globals.iUsershareMaxShares;
	unsigned int num_dir_entries, num_bad_dir_entries, num_tmp_dir_entries;
	unsigned int allowed_bad_entries = ((2*max_user_shares)/10);
	unsigned int allowed_tmp_entries = ((2*max_user_shares)/10);
	int iService;
	int snum_template = -1;
	const char *usersharepath = Globals.szUsersharePath;
	int ret = lp_numservices();

	if (max_user_shares == 0 || *usersharepath == '\0') {
		return lp_numservices();
	}

	if (sys_stat(usersharepath, &sbuf, false) != 0) {
		DEBUG(0,("load_usershare_shares: stat of %s failed. %s\n",
			usersharepath, strerror(errno) ));
		return ret;
	}

	/*
	 * This directory must be owned by root, and have the 't' bit set.
	 * It also must not be writable by "other".
	 */

#ifdef S_ISVTX
	if (sbuf.st_ex_uid != 0 || !(sbuf.st_ex_mode & S_ISVTX) || (sbuf.st_ex_mode & S_IWOTH)) {
#else
	if (sbuf.st_ex_uid != 0 || (sbuf.st_ex_mode & S_IWOTH)) {
#endif
		DEBUG(0,("load_usershare_shares: directory %s is not owned by root "
			"or does not have the sticky bit 't' set or is writable by anyone.\n",
			usersharepath ));
		return ret;
	}

	/* Ensure the template share exists if it's set. */
	if (Globals.szUsershareTemplateShare[0]) {
		/* We can't use lp_servicenumber here as we are recommending that
		   template shares have -valid=False set. */
		for (snum_template = iNumServices - 1; snum_template >= 0; snum_template--) {
			if (ServicePtrs[snum_template]->szService &&
					strequal(ServicePtrs[snum_template]->szService,
						Globals.szUsershareTemplateShare)) {
				break;
			}
		}

		if (snum_template == -1) {
			DEBUG(0,("load_usershare_shares: usershare template share %s "
				"does not exist.\n",
				Globals.szUsershareTemplateShare ));
			return ret;
		}
	}

	/* Mark all existing usershares as pending delete. */
	for (iService = iNumServices - 1; iService >= 0; iService--) {
		if (VALID(iService) && ServicePtrs[iService]->usershare) {
			ServicePtrs[iService]->usershare = USERSHARE_PENDING_DELETE;
		}
	}

	dp = sys_opendir(usersharepath);
	if (!dp) {
		DEBUG(0,("load_usershare_shares:: failed to open directory %s. %s\n",
			usersharepath, strerror(errno) ));
		return ret;
	}

	for (num_dir_entries = 0, num_bad_dir_entries = 0, num_tmp_dir_entries = 0;
			(de = sys_readdir(dp));
			num_dir_entries++ ) {
		int r;
		const char *n = de->d_name;

		/* Ignore . and .. */
		if (*n == '.') {
			if ((n[1] == '\0') || (n[1] == '.' && n[2] == '\0')) {
				continue;
			}
		}

		if (n[0] == ':') {
			/* Temporary file used when creating a share. */
			num_tmp_dir_entries++;
		}

		/* Allow 20% tmp entries. */
		if (num_tmp_dir_entries > allowed_tmp_entries) {
			DEBUG(0,("load_usershare_shares: too many temp entries (%u) "
				"in directory %s\n",
				num_tmp_dir_entries, usersharepath));
			break;
		}

		r = process_usershare_file(usersharepath, n, snum_template);
		if (r == 0) {
			/* Update the services count. */
			num_usershares++;
			if (num_usershares >= max_user_shares) {
				DEBUG(0,("load_usershare_shares: max user shares reached "
					"on file %s in directory %s\n",
					n, usersharepath ));
				break;
			}
		} else if (r == -1) {
			num_bad_dir_entries++;
		}

		/* Allow 20% bad entries. */
		if (num_bad_dir_entries > allowed_bad_entries) {
			DEBUG(0,("load_usershare_shares: too many bad entries (%u) "
				"in directory %s\n",
				num_bad_dir_entries, usersharepath));
			break;
		}

		/* Allow 20% bad entries. */
		if (num_dir_entries > max_user_shares + allowed_bad_entries) {
			DEBUG(0,("load_usershare_shares: too many total entries (%u) "
			"in directory %s\n",
			num_dir_entries, usersharepath));
			break;
		}
	}

	sys_closedir(dp);

	/* Sweep through and delete any non-refreshed usershares that are
	   not currently in use. */
	for (iService = iNumServices - 1; iService >= 0; iService--) {
		if (VALID(iService) && (ServicePtrs[iService]->usershare == USERSHARE_PENDING_DELETE)) {
			if (conn_snum_used(iService)) {
				continue;
			}
			/* Remove from the share ACL db. */
			DEBUG(10,("load_usershare_shares: Removing deleted usershare %s\n",
				lp_servicename(iService) ));
			delete_share_security(lp_servicename(iService));
			free_service_byindex(iService);
		}
	}

	return lp_numservices();
}

/********************************************************
 Destroy global resources allocated in this file
********************************************************/

void gfree_loadparm(void)
{
	int i;

	free_file_list();

	/* Free resources allocated to services */

	for ( i = 0; i < iNumServices; i++ ) {
		if ( VALID(i) ) {
			free_service_byindex(i);
		}
	}

	SAFE_FREE( ServicePtrs );
	iNumServices = 0;

	/* Now release all resources allocated to global
	   parameters and the default service */

	free_global_parameters();
}


/***************************************************************************
 Allow client apps to specify that they are a client
***************************************************************************/
void lp_set_in_client(bool b)
{
    in_client = b;
}


/***************************************************************************
 Determine if we're running in a client app
***************************************************************************/
bool lp_is_in_client(void)
{
    return in_client;
}

/***************************************************************************
 Load the services array from the services file. Return True on success, 
 False on failure.
***************************************************************************/

bool lp_load_ex(const char *pszFname,
		bool global_only,
		bool save_defaults,
		bool add_ipc,
		bool initialize_globals,
		bool allow_include_registry,
		bool allow_registry_shares)
{
	char *n2 = NULL;
	bool bRetval;

	bRetval = False;

	DEBUG(3, ("lp_load_ex: refreshing parameters\n"));

	bInGlobalSection = True;
	bGlobalOnly = global_only;
	bAllowIncludeRegistry = allow_include_registry;

	init_globals(! initialize_globals);
	debug_init();

	free_file_list();

	if (save_defaults) {
		init_locals();
		lp_save_defaults();
	}

	free_param_opts(&Globals.param_opt);

	/* We get sections first, so have to start 'behind' to make up */
	iServiceIndex = -1;

	if (lp_config_backend_is_file()) {
		n2 = alloc_sub_basic(get_current_username(),
					current_user_info.domain,
					pszFname);
		if (!n2) {
			smb_panic("lp_load_ex: out of memory");
		}

		add_to_file_list(pszFname, n2);

		bRetval = pm_process(n2, do_section, do_parameter, NULL);
		SAFE_FREE(n2);

		/* finish up the last section */
		DEBUG(4, ("pm_process() returned %s\n", BOOLSTR(bRetval)));
		if (bRetval) {
			if (iServiceIndex >= 0) {
				bRetval = service_ok(iServiceIndex);
			}
		}

		if (lp_config_backend_is_registry()) {
			/* config backend changed to registry in config file */
			/*
			 * We need to use this extra global variable here to
			 * survive restart: init_globals uses this as a default
			 * for ConfigBackend. Otherwise, init_globals would
			 *  send us into an endless loop here.
			 */
			config_backend = CONFIG_BACKEND_REGISTRY;
			/* start over */
			DEBUG(1, ("lp_load_ex: changing to config backend "
				  "registry\n"));
			init_globals(false);
			lp_kill_all_services();
			return lp_load_ex(pszFname, global_only, save_defaults,
					  add_ipc, initialize_globals,
					  allow_include_registry,
					  allow_registry_shares);
		}
	} else if (lp_config_backend_is_registry()) {
		bRetval = process_registry_globals();
	} else {
		DEBUG(0, ("Illegal config  backend given: %d\n",
			  lp_config_backend()));
		bRetval = false;
	}

	if (bRetval && lp_registry_shares()) {
		if (allow_registry_shares) {
			bRetval = process_registry_shares();
		} else {
			bRetval = reload_registry_shares();
		}
	}

	lp_add_auto_services(lp_auto_services());

	if (add_ipc) {
		/* When 'restrict anonymous = 2' guest connections to ipc$
		   are denied */
		lp_add_ipc("IPC$", (lp_restrict_anonymous() < 2));
		if ( lp_enable_asu_support() ) {
			lp_add_ipc("ADMIN$", false);
		}
	}

	set_server_role();
	set_default_server_announce_type();
	set_allowed_client_auth();

	bLoaded = True;

	/* Now we check bWINSsupport and set szWINSserver to 127.0.0.1 */
	/* if bWINSsupport is true and we are in the client            */
	if (lp_is_in_client() && Globals.bWINSsupport) {
		lp_do_parameter(GLOBAL_SECTION_SNUM, "wins server", "127.0.0.1");
	}

	init_iconv();

	bAllowIncludeRegistry = true;

	return (bRetval);
}

bool lp_load(const char *pszFname,
	     bool global_only,
	     bool save_defaults,
	     bool add_ipc,
	     bool initialize_globals)
{
	return lp_load_ex(pszFname,
			  global_only,
			  save_defaults,
			  add_ipc,
			  initialize_globals,
			  true, false);
}

bool lp_load_initial_only(const char *pszFname)
{
	return lp_load_ex(pszFname,
			  true,
			  false,
			  false,
			  true,
			  false,
			  false);
}

bool lp_load_with_registry_shares(const char *pszFname,
				  bool global_only,
				  bool save_defaults,
				  bool add_ipc,
				  bool initialize_globals)
{
	return lp_load_ex(pszFname,
			  global_only,
			  save_defaults,
			  add_ipc,
			  initialize_globals,
			  true,
			  true);
}

/***************************************************************************
 Return the max number of services.
***************************************************************************/

int lp_numservices(void)
{
	return (iNumServices);
}

/***************************************************************************
Display the contents of the services array in human-readable form.
***************************************************************************/

void lp_dump(FILE *f, bool show_defaults, int maxtoprint)
{
	int iService;

	if (show_defaults)
		defaults_saved = False;

	dump_globals(f);

	dump_a_service(&sDefault, f);

	for (iService = 0; iService < maxtoprint; iService++) {
		fprintf(f,"\n");
		lp_dump_one(f, show_defaults, iService);
	}
}

/***************************************************************************
Display the contents of one service in human-readable form.
***************************************************************************/

void lp_dump_one(FILE * f, bool show_defaults, int snum)
{
	if (VALID(snum)) {
		if (ServicePtrs[snum]->szService[0] == '\0')
			return;
		dump_a_service(ServicePtrs[snum], f);
	}
}

/***************************************************************************
Return the number of the service with the given name, or -1 if it doesn't
exist. Note that this is a DIFFERENT ANIMAL from the internal function
getservicebyname()! This works ONLY if all services have been loaded, and
does not copy the found service.
***************************************************************************/
typedef struct {
	int upper_case;
	int lower_case;
} case_t;

#define FULL_TABLE_CN 1
static case_t character_table [] = {

#ifdef FULL_TABLE_CH
{.upper_case = 0xf0909080,    .lower_case = 0xf09090a8},  //  𐐀 <> 𐐨 
{.upper_case = 0xf0909081,    .lower_case = 0xf09090a9},  //  𐐁 <> 𐐩 
{.upper_case = 0xf0909082,    .lower_case = 0xf09090aa},  //  𐐂 <> 𐐪 
{.upper_case = 0xf0909083,    .lower_case = 0xf09090ab},  //  𐐃 <> 𐐫 
{.upper_case = 0xf0909084,    .lower_case = 0xf09090ac},  //  𐐄 <> 𐐬 
{.upper_case = 0xf0909085,    .lower_case = 0xf09090ad},  //  𐐅 <> 𐐭 
{.upper_case = 0xf0909086,    .lower_case = 0xf09090ae},  //  𐐆 <> 𐐮 
{.upper_case = 0xf0909087,    .lower_case = 0xf09090af},  //  𐐇 <> 𐐯 
{.upper_case = 0xf0909088,    .lower_case = 0xf09090b0},  //  𐐈 <> 𐐰 
{.upper_case = 0xf0909089,    .lower_case = 0xf09090b1},  //  𐐉 <> 𐐱 
{.upper_case = 0xf090908a,    .lower_case = 0xf09090b2},  //  𐐊 <> 𐐲 
{.upper_case = 0xf090908b,    .lower_case = 0xf09090b3},  //  𐐋 <> 𐐳 
{.upper_case = 0xf090908c,    .lower_case = 0xf09090b4},  //  𐐌 <> 𐐴 
{.upper_case = 0xf090908d,    .lower_case = 0xf09090b5},  //  𐐍 <> 𐐵 
{.upper_case = 0xf090908e,    .lower_case = 0xf09090b6},  //  𐐎 <> 𐐶 
{.upper_case = 0xf090908f,    .lower_case = 0xf09090b7},  //  𐐏 <> 𐐷 
{.upper_case = 0xf0909090,    .lower_case = 0xf09090b8},  //  𐐐 <> 𐐸 
{.upper_case = 0xf0909091,    .lower_case = 0xf09090b9},  //  𐐑 <> 𐐹 
{.upper_case = 0xf0909092,    .lower_case = 0xf09090ba},  //  𐐒 <> 𐐺 
{.upper_case = 0xf0909093,    .lower_case = 0xf09090bb},  //  𐐓 <> 𐐻 
{.upper_case = 0xf0909094,    .lower_case = 0xf09090bc},  //  𐐔 <> 𐐼 
{.upper_case = 0xf0909095,    .lower_case = 0xf09090bd},  //  𐐕 <> 𐐽 
{.upper_case = 0xf0909096,    .lower_case = 0xf09090be},  //  𐐖 <> 𐐾 
{.upper_case = 0xf0909097,    .lower_case = 0xf09090bf},  //  𐐗 <> 𐐿 
{.upper_case = 0xf0909098,    .lower_case = 0xf0909180},  //  𐐘 <> 𐑀 
{.upper_case = 0xf0909099,    .lower_case = 0xf0909181},  //  𐐙 <> 𐑁 
{.upper_case = 0xf090909a,    .lower_case = 0xf0909182},  //  𐐚 <> 𐑂 
{.upper_case = 0xf090909b,    .lower_case = 0xf0909183},  //  𐐛 <> 𐑃 
{.upper_case = 0xf090909c,    .lower_case = 0xf0909184},  //  𐐜 <> 𐑄 
{.upper_case = 0xf090909d,    .lower_case = 0xf0909185},  //  𐐝 <> 𐑅 
{.upper_case = 0xf090909e,    .lower_case = 0xf0909186},  //  𐐞 <> 𐑆 
{.upper_case = 0xf090909f,    .lower_case = 0xf0909187},  //  𐐟 <> 𐑇 
{.upper_case = 0xf09090a0,    .lower_case = 0xf0909188},  //  𐐠 <> 𐑈 
{.upper_case = 0xf09090a1,    .lower_case = 0xf0909189},  //  𐐡 <> 𐑉 
{.upper_case = 0xf09090a2,    .lower_case = 0xf090918a},  //  𐐢 <> 𐑊 
{.upper_case = 0xf09090a3,    .lower_case = 0xf090918b},  //  𐐣 <> 𐑋 
{.upper_case = 0xf09090a4,    .lower_case = 0xf090918c},  //  𐐤 <> 𐑌 
{.upper_case = 0xf09090a5,    .lower_case = 0xf090918d},  //  𐐥 <> 𐑍 
{.upper_case = 0xf09090a6,    .lower_case = 0xf090918e},  //  𐐦 <> 𐑎 
{.upper_case = 0xf09090a7,    .lower_case = 0xf090918f},  //  𐐧 <> 𐑏 
{.upper_case = 0xf09d9080,    .lower_case = 0xf09d909a},  //  𝐀 <> 𝐚 
{.upper_case = 0xf09d9081,    .lower_case = 0xf09d909b},  //  𝐁 <> 𝐛 
{.upper_case = 0xf09d9082,    .lower_case = 0xf09d909c},  //  𝐂 <> 𝐜 
{.upper_case = 0xf09d9083,    .lower_case = 0xf09d909d},  //  𝐃 <> 𝐝 
{.upper_case = 0xf09d9084,    .lower_case = 0xf09d909e},  //  𝐄 <> 𝐞 
{.upper_case = 0xf09d9085,    .lower_case = 0xf09d909f},  //  𝐅 <> 𝐟 
{.upper_case = 0xf09d9086,    .lower_case = 0xf09d90a0},  //  𝐆 <> 𝐠 
{.upper_case = 0xf09d9087,    .lower_case = 0xf09d90a1},  //  𝐇 <> 𝐡 
{.upper_case = 0xf09d9088,    .lower_case = 0xf09d90a2},  //  𝐈 <> 𝐢 
{.upper_case = 0xf09d9089,    .lower_case = 0xf09d90a3},  //  𝐉 <> 𝐣 
{.upper_case = 0xf09d908a,    .lower_case = 0xf09d90a4},  //  𝐊 <> 𝐤 
{.upper_case = 0xf09d908b,    .lower_case = 0xf09d90a5},  //  𝐋 <> 𝐥 
{.upper_case = 0xf09d908c,    .lower_case = 0xf09d90a6},  //  𝐌 <> 𝐦 
{.upper_case = 0xf09d908d,    .lower_case = 0xf09d90a7},  //  𝐍 <> 𝐧 
{.upper_case = 0xf09d908e,    .lower_case = 0xf09d90a8},  //  𝐎 <> 𝐨 
{.upper_case = 0xf09d908f,    .lower_case = 0xf09d90a9},  //  𝐏 <> 𝐩 
{.upper_case = 0xf09d9090,    .lower_case = 0xf09d90aa},  //  𝐐 <> 𝐪 
{.upper_case = 0xf09d9091,    .lower_case = 0xf09d90ab},  //  𝐑 <> 𝐫 
{.upper_case = 0xf09d9092,    .lower_case = 0xf09d90ac},  //  𝐒 <> 𝐬 
{.upper_case = 0xf09d9093,    .lower_case = 0xf09d90ad},  //  𝐓 <> 𝐭 
{.upper_case = 0xf09d9094,    .lower_case = 0xf09d90ae},  //  𝐔 <> 𝐮 
{.upper_case = 0xf09d9095,    .lower_case = 0xf09d90af},  //  𝐕 <> 𝐯 
{.upper_case = 0xf09d9096,    .lower_case = 0xf09d90b0},  //  𝐖 <> 𝐰 
{.upper_case = 0xf09d9097,    .lower_case = 0xf09d90b1},  //  𝐗 <> 𝐱 
{.upper_case = 0xf09d9098,    .lower_case = 0xf09d90b2},  //  𝐘 <> 𝐲 
{.upper_case = 0xf09d9099,    .lower_case = 0xf09d90b3},  //  𝐙 <> 𝐳 
{.upper_case = 0xf09d90b4,    .lower_case = 0xf09d918e},  //  𝐴 <> 𝑎 
{.upper_case = 0xf09d90b5,    .lower_case = 0xf09d918f},  //  𝐵 <> 𝑏 
{.upper_case = 0xf09d90b6,    .lower_case = 0xf09d9190},  //  𝐶 <> 𝑐 
{.upper_case = 0xf09d90b7,    .lower_case = 0xf09d9191},  //  𝐷 <> 𝑑 
{.upper_case = 0xf09d90b8,    .lower_case = 0xf09d9192},  //  𝐸 <> 𝑒 
{.upper_case = 0xf09d90b9,    .lower_case = 0xf09d9193},  //  𝐹 <> 𝑓 
{.upper_case = 0xf09d90ba,    .lower_case = 0xf09d9194},  //  𝐺 <> 𝑔 
{.upper_case = 0xf09d90bc,    .lower_case = 0xf09d9196},  //  𝐼 <> 𝑖 
{.upper_case = 0xf09d90bd,    .lower_case = 0xf09d9197},  //  𝐽 <> 𝑗 
{.upper_case = 0xf09d90be,    .lower_case = 0xf09d9198},  //  𝐾 <> 𝑘 
{.upper_case = 0xf09d90bf,    .lower_case = 0xf09d9199},  //  𝐿 <> 𝑙 
{.upper_case = 0xf09d9180,    .lower_case = 0xf09d919a},  //  𝑀 <> 𝑚 
{.upper_case = 0xf09d9181,    .lower_case = 0xf09d919b},  //  𝑁 <> 𝑛 
{.upper_case = 0xf09d9182,    .lower_case = 0xf09d919c},  //  𝑂 <> 𝑜 
{.upper_case = 0xf09d9183,    .lower_case = 0xf09d919d},  //  𝑃 <> 𝑝 
{.upper_case = 0xf09d9184,    .lower_case = 0xf09d919e},  //  𝑄 <> 𝑞 
{.upper_case = 0xf09d9185,    .lower_case = 0xf09d919f},  //  𝑅 <> 𝑟 
{.upper_case = 0xf09d9186,    .lower_case = 0xf09d91a0},  //  𝑆 <> 𝑠 
{.upper_case = 0xf09d9187,    .lower_case = 0xf09d91a1},  //  𝑇 <> 𝑡 
{.upper_case = 0xf09d9188,    .lower_case = 0xf09d91a2},  //  𝑈 <> 𝑢 
{.upper_case = 0xf09d9189,    .lower_case = 0xf09d91a3},  //  𝑉 <> 𝑣 
{.upper_case = 0xf09d918a,    .lower_case = 0xf09d91a4},  //  𝑊 <> 𝑤 
{.upper_case = 0xf09d918b,    .lower_case = 0xf09d91a5},  //  𝑋 <> 𝑥 
{.upper_case = 0xf09d918c,    .lower_case = 0xf09d91a6},  //  𝑌 <> 𝑦 
{.upper_case = 0xf09d918d,    .lower_case = 0xf09d91a7},  //  𝑍 <> 𝑧 
{.upper_case = 0xf09d91a8,    .lower_case = 0xf09d9282},  //  𝑨 <> 𝒂 
{.upper_case = 0xf09d91a9,    .lower_case = 0xf09d9283},  //  𝑩 <> 𝒃 
{.upper_case = 0xf09d91aa,    .lower_case = 0xf09d9284},  //  𝑪 <> 𝒄 
{.upper_case = 0xf09d91ab,    .lower_case = 0xf09d9285},  //  𝑫 <> 𝒅 
{.upper_case = 0xf09d91ac,    .lower_case = 0xf09d9286},  //  𝑬 <> 𝒆 
{.upper_case = 0xf09d91ad,    .lower_case = 0xf09d9287},  //  𝑭 <> 𝒇 
{.upper_case = 0xf09d91ae,    .lower_case = 0xf09d9288},  //  𝑮 <> 𝒈 
{.upper_case = 0xf09d91af,    .lower_case = 0xf09d9289},  //  𝑯 <> 𝒉 
{.upper_case = 0xf09d91b0,    .lower_case = 0xf09d928a},  //  𝑰 <> 𝒊 
{.upper_case = 0xf09d91b1,    .lower_case = 0xf09d928b},  //  𝑱 <> 𝒋 
{.upper_case = 0xf09d91b2,    .lower_case = 0xf09d928c},  //  𝑲 <> 𝒌 
{.upper_case = 0xf09d91b3,    .lower_case = 0xf09d928d},  //  𝑳 <> 𝒍 
{.upper_case = 0xf09d91b4,    .lower_case = 0xf09d928e},  //  𝑴 <> 𝒎 
{.upper_case = 0xf09d91b5,    .lower_case = 0xf09d928f},  //  𝑵 <> 𝒏 
{.upper_case = 0xf09d91b6,    .lower_case = 0xf09d9290},  //  𝑶 <> 𝒐 
{.upper_case = 0xf09d91b7,    .lower_case = 0xf09d9291},  //  𝑷 <> 𝒑 
{.upper_case = 0xf09d91b8,    .lower_case = 0xf09d9292},  //  𝑸 <> 𝒒 
{.upper_case = 0xf09d91b9,    .lower_case = 0xf09d9293},  //  𝑹 <> 𝒓 
{.upper_case = 0xf09d91ba,    .lower_case = 0xf09d9294},  //  𝑺 <> 𝒔 
{.upper_case = 0xf09d91bb,    .lower_case = 0xf09d9295},  //  𝑻 <> 𝒕 
{.upper_case = 0xf09d91bc,    .lower_case = 0xf09d9296},  //  𝑼 <> 𝒖 
{.upper_case = 0xf09d91bd,    .lower_case = 0xf09d9297},  //  𝑽 <> 𝒗 
{.upper_case = 0xf09d91be,    .lower_case = 0xf09d9298},  //  𝑾 <> 𝒘 
{.upper_case = 0xf09d91bf,    .lower_case = 0xf09d9299},  //  𝑿 <> 𝒙 
{.upper_case = 0xf09d9280,    .lower_case = 0xf09d929a},  //  𝒀 <> 𝒚 
{.upper_case = 0xf09d9281,    .lower_case = 0xf09d929b},  //  𝒁 <> 𝒛 
{.upper_case = 0xf09d929c,    .lower_case = 0xf09d92b6},  //  𝒜 <> 𝒶 
{.upper_case = 0xf09d929e,    .lower_case = 0xf09d92b8},  //  𝒞 <> 𝒸 
{.upper_case = 0xf09d929f,    .lower_case = 0xf09d92b9},  //  𝒟 <> 𝒹 
{.upper_case = 0xf09d92a5,    .lower_case = 0xf09d92bf},  //  𝒥 <> 𝒿 
{.upper_case = 0xf09d92a6,    .lower_case = 0xf09d9380},  //  𝒦 <> 𝓀 
{.upper_case = 0xf09d92a9,    .lower_case = 0xf09d9383},  //  𝒩 <> 𝓃 
{.upper_case = 0xf09d92ab,    .lower_case = 0xf09d9385},  //  𝒫 <> 𝓅 
{.upper_case = 0xf09d92ac,    .lower_case = 0xf09d9386},  //  𝒬 <> 𝓆 
{.upper_case = 0xf09d92ae,    .lower_case = 0xf09d9388},  //  𝒮 <> 𝓈 
{.upper_case = 0xf09d92af,    .lower_case = 0xf09d9389},  //  𝒯 <> 𝓉 
{.upper_case = 0xf09d92b0,    .lower_case = 0xf09d938a},  //  𝒰 <> 𝓊 
{.upper_case = 0xf09d92b1,    .lower_case = 0xf09d938b},  //  𝒱 <> 𝓋 
{.upper_case = 0xf09d92b2,    .lower_case = 0xf09d938c},  //  𝒲 <> 𝓌 
{.upper_case = 0xf09d92b3,    .lower_case = 0xf09d938d},  //  𝒳 <> 𝓍 
{.upper_case = 0xf09d92b4,    .lower_case = 0xf09d938e},  //  𝒴 <> 𝓎 
{.upper_case = 0xf09d92b5,    .lower_case = 0xf09d938f},  //  𝒵 <> 𝓏 
{.upper_case = 0xf09d9390,    .lower_case = 0xf09d93aa},  //  𝓐 <> 𝓪 
{.upper_case = 0xf09d9391,    .lower_case = 0xf09d93ab},  //  𝓑 <> 𝓫 
{.upper_case = 0xf09d9392,    .lower_case = 0xf09d93ac},  //  𝓒 <> 𝓬 
{.upper_case = 0xf09d9393,    .lower_case = 0xf09d93ad},  //  𝓓 <> 𝓭 
{.upper_case = 0xf09d9394,    .lower_case = 0xf09d93ae},  //  𝓔 <> 𝓮 
{.upper_case = 0xf09d9395,    .lower_case = 0xf09d93af},  //  𝓕 <> 𝓯 
{.upper_case = 0xf09d9396,    .lower_case = 0xf09d93b0},  //  𝓖 <> 𝓰 
{.upper_case = 0xf09d9397,    .lower_case = 0xf09d93b1},  //  𝓗 <> 𝓱 
{.upper_case = 0xf09d9398,    .lower_case = 0xf09d93b2},  //  𝓘 <> 𝓲 
{.upper_case = 0xf09d9399,    .lower_case = 0xf09d93b3},  //  𝓙 <> 𝓳 
{.upper_case = 0xf09d939a,    .lower_case = 0xf09d93b4},  //  𝓚 <> 𝓴 
{.upper_case = 0xf09d939b,    .lower_case = 0xf09d93b5},  //  𝓛 <> 𝓵 
{.upper_case = 0xf09d939c,    .lower_case = 0xf09d93b6},  //  𝓜 <> 𝓶 
{.upper_case = 0xf09d939d,    .lower_case = 0xf09d93b7},  //  𝓝 <> 𝓷 
{.upper_case = 0xf09d939e,    .lower_case = 0xf09d93b8},  //  𝓞 <> 𝓸 
{.upper_case = 0xf09d939f,    .lower_case = 0xf09d93b9},  //  𝓟 <> 𝓹 
{.upper_case = 0xf09d93a0,    .lower_case = 0xf09d93ba},  //  𝓠 <> 𝓺 
{.upper_case = 0xf09d93a1,    .lower_case = 0xf09d93bb},  //  𝓡 <> 𝓻 
{.upper_case = 0xf09d93a2,    .lower_case = 0xf09d93bc},  //  𝓢 <> 𝓼 
{.upper_case = 0xf09d93a3,    .lower_case = 0xf09d93bd},  //  𝓣 <> 𝓽 
{.upper_case = 0xf09d93a4,    .lower_case = 0xf09d93be},  //  𝓤 <> 𝓾 
{.upper_case = 0xf09d93a5,    .lower_case = 0xf09d93bf},  //  𝓥 <> 𝓿 
{.upper_case = 0xf09d93a6,    .lower_case = 0xf09d9480},  //  𝓦 <> 𝔀 
{.upper_case = 0xf09d93a7,    .lower_case = 0xf09d9481},  //  𝓧 <> 𝔁 
{.upper_case = 0xf09d93a8,    .lower_case = 0xf09d9482},  //  𝓨 <> 𝔂 
{.upper_case = 0xf09d93a9,    .lower_case = 0xf09d9483},  //  𝓩 <> 𝔃 
{.upper_case = 0xf09d9484,    .lower_case = 0xf09d949e},  //  𝔄 <> 𝔞 
{.upper_case = 0xf09d9485,    .lower_case = 0xf09d949f},  //  𝔅 <> 𝔟 
{.upper_case = 0xf09d9487,    .lower_case = 0xf09d94a1},  //  𝔇 <> 𝔡 
{.upper_case = 0xf09d9488,    .lower_case = 0xf09d94a2},  //  𝔈 <> 𝔢 
{.upper_case = 0xf09d9489,    .lower_case = 0xf09d94a3},  //  𝔉 <> 𝔣 
{.upper_case = 0xf09d948a,    .lower_case = 0xf09d94a4},  //  𝔊 <> 𝔤 
{.upper_case = 0xf09d948d,    .lower_case = 0xf09d94a7},  //  𝔍 <> 𝔧 
{.upper_case = 0xf09d948e,    .lower_case = 0xf09d94a8},  //  𝔎 <> 𝔨 
{.upper_case = 0xf09d948f,    .lower_case = 0xf09d94a9},  //  𝔏 <> 𝔩 
{.upper_case = 0xf09d9490,    .lower_case = 0xf09d94aa},  //  𝔐 <> 𝔪 
{.upper_case = 0xf09d9491,    .lower_case = 0xf09d94ab},  //  𝔑 <> 𝔫 
{.upper_case = 0xf09d9492,    .lower_case = 0xf09d94ac},  //  𝔒 <> 𝔬 
{.upper_case = 0xf09d9493,    .lower_case = 0xf09d94ad},  //  𝔓 <> 𝔭 
{.upper_case = 0xf09d9494,    .lower_case = 0xf09d94ae},  //  𝔔 <> 𝔮 
{.upper_case = 0xf09d9496,    .lower_case = 0xf09d94b0},  //  𝔖 <> 𝔰 
{.upper_case = 0xf09d9497,    .lower_case = 0xf09d94b1},  //  𝔗 <> 𝔱 
{.upper_case = 0xf09d9498,    .lower_case = 0xf09d94b2},  //  𝔘 <> 𝔲 
{.upper_case = 0xf09d9499,    .lower_case = 0xf09d94b3},  //  𝔙 <> 𝔳 
{.upper_case = 0xf09d949a,    .lower_case = 0xf09d94b4},  //  𝔚 <> 𝔴 
{.upper_case = 0xf09d949b,    .lower_case = 0xf09d94b5},  //  𝔛 <> 𝔵 
{.upper_case = 0xf09d949c,    .lower_case = 0xf09d94b6},  //  𝔜 <> 𝔶 
{.upper_case = 0xf09d94b8,    .lower_case = 0xf09d9592},  //  𝔸 <> 𝕒 
{.upper_case = 0xf09d94b9,    .lower_case = 0xf09d9593},  //  𝔹 <> 𝕓 
{.upper_case = 0xf09d94bb,    .lower_case = 0xf09d9595},  //  𝔻 <> 𝕕 
{.upper_case = 0xf09d94bc,    .lower_case = 0xf09d9596},  //  𝔼 <> 𝕖 
{.upper_case = 0xf09d94bd,    .lower_case = 0xf09d9597},  //  𝔽 <> 𝕗 
{.upper_case = 0xf09d94be,    .lower_case = 0xf09d9598},  //  𝔾 <> 𝕘 
{.upper_case = 0xf09d9580,    .lower_case = 0xf09d959a},  //  𝕀 <> 𝕚 
{.upper_case = 0xf09d9581,    .lower_case = 0xf09d959b},  //  𝕁 <> 𝕛 
{.upper_case = 0xf09d9582,    .lower_case = 0xf09d959c},  //  𝕂 <> 𝕜 
{.upper_case = 0xf09d9583,    .lower_case = 0xf09d959d},  //  𝕃 <> 𝕝 
{.upper_case = 0xf09d9584,    .lower_case = 0xf09d959e},  //  𝕄 <> 𝕞 
{.upper_case = 0xf09d9586,    .lower_case = 0xf09d95a0},  //  𝕆 <> 𝕠 
{.upper_case = 0xf09d958a,    .lower_case = 0xf09d95a4},  //  𝕊 <> 𝕤 
{.upper_case = 0xf09d958b,    .lower_case = 0xf09d95a5},  //  𝕋 <> 𝕥 
{.upper_case = 0xf09d958c,    .lower_case = 0xf09d95a6},  //  𝕌 <> 𝕦 
{.upper_case = 0xf09d958d,    .lower_case = 0xf09d95a7},  //  𝕍 <> 𝕧 
{.upper_case = 0xf09d958e,    .lower_case = 0xf09d95a8},  //  𝕎 <> 𝕨 
{.upper_case = 0xf09d958f,    .lower_case = 0xf09d95a9},  //  𝕏 <> 𝕩 
{.upper_case = 0xf09d9590,    .lower_case = 0xf09d95aa},  //  𝕐 <> 𝕪 
{.upper_case = 0xf09d95ac,    .lower_case = 0xf09d9686},  //  𝕬 <> 𝖆 
{.upper_case = 0xf09d95ad,    .lower_case = 0xf09d9687},  //  𝕭 <> 𝖇 
{.upper_case = 0xf09d95ae,    .lower_case = 0xf09d9688},  //  𝕮 <> 𝖈 
{.upper_case = 0xf09d95af,    .lower_case = 0xf09d9689},  //  𝕯 <> 𝖉 
{.upper_case = 0xf09d95b0,    .lower_case = 0xf09d968a},  //  𝕰 <> 𝖊 
{.upper_case = 0xf09d95b1,    .lower_case = 0xf09d968b},  //  𝕱 <> 𝖋 
{.upper_case = 0xf09d95b2,    .lower_case = 0xf09d968c},  //  𝕲 <> 𝖌 
{.upper_case = 0xf09d95b3,    .lower_case = 0xf09d968d},  //  𝕳 <> 𝖍 
{.upper_case = 0xf09d95b4,    .lower_case = 0xf09d968e},  //  𝕴 <> 𝖎 
{.upper_case = 0xf09d95b5,    .lower_case = 0xf09d968f},  //  𝕵 <> 𝖏 
{.upper_case = 0xf09d95b6,    .lower_case = 0xf09d9690},  //  𝕶 <> 𝖐 
{.upper_case = 0xf09d95b7,    .lower_case = 0xf09d9691},  //  𝕷 <> 𝖑 
{.upper_case = 0xf09d95b8,    .lower_case = 0xf09d9692},  //  𝕸 <> 𝖒 
{.upper_case = 0xf09d95b9,    .lower_case = 0xf09d9693},  //  𝕹 <> 𝖓 
{.upper_case = 0xf09d95ba,    .lower_case = 0xf09d9694},  //  𝕺 <> 𝖔 
{.upper_case = 0xf09d95bb,    .lower_case = 0xf09d9695},  //  𝕻 <> 𝖕 
{.upper_case = 0xf09d95bc,    .lower_case = 0xf09d9696},  //  𝕼 <> 𝖖 
{.upper_case = 0xf09d95bd,    .lower_case = 0xf09d9697},  //  𝕽 <> 𝖗 
{.upper_case = 0xf09d95be,    .lower_case = 0xf09d9698},  //  𝕾 <> 𝖘 
{.upper_case = 0xf09d95bf,    .lower_case = 0xf09d9699},  //  𝕿 <> 𝖙 
{.upper_case = 0xf09d9680,    .lower_case = 0xf09d969a},  //  𝖀 <> 𝖚 
{.upper_case = 0xf09d9681,    .lower_case = 0xf09d969b},  //  𝖁 <> 𝖛 
{.upper_case = 0xf09d9682,    .lower_case = 0xf09d969c},  //  𝖂 <> 𝖜 
{.upper_case = 0xf09d9683,    .lower_case = 0xf09d969d},  //  𝖃 <> 𝖝 
{.upper_case = 0xf09d9684,    .lower_case = 0xf09d969e},  //  𝖄 <> 𝖞 
{.upper_case = 0xf09d9685,    .lower_case = 0xf09d969f},  //  𝖅 <> 𝖟 
{.upper_case = 0xf09d96a0,    .lower_case = 0xf09d96ba},  //  𝖠 <> 𝖺 
{.upper_case = 0xf09d96a1,    .lower_case = 0xf09d96bb},  //  𝖡 <> 𝖻 
{.upper_case = 0xf09d96a2,    .lower_case = 0xf09d96bc},  //  𝖢 <> 𝖼 
{.upper_case = 0xf09d96a3,    .lower_case = 0xf09d96bd},  //  𝖣 <> 𝖽 
{.upper_case = 0xf09d96a4,    .lower_case = 0xf09d96be},  //  𝖤 <> 𝖾 
{.upper_case = 0xf09d96a5,    .lower_case = 0xf09d96bf},  //  𝖥 <> 𝖿 
{.upper_case = 0xf09d96a6,    .lower_case = 0xf09d9780},  //  𝖦 <> 𝗀 
{.upper_case = 0xf09d96a7,    .lower_case = 0xf09d9781},  //  𝖧 <> 𝗁 
{.upper_case = 0xf09d96a8,    .lower_case = 0xf09d9782},  //  𝖨 <> 𝗂 
{.upper_case = 0xf09d96a9,    .lower_case = 0xf09d9783},  //  𝖩 <> 𝗃 
{.upper_case = 0xf09d96aa,    .lower_case = 0xf09d9784},  //  𝖪 <> 𝗄 
{.upper_case = 0xf09d96ab,    .lower_case = 0xf09d9785},  //  𝖫 <> 𝗅 
{.upper_case = 0xf09d96ac,    .lower_case = 0xf09d9786},  //  𝖬 <> 𝗆 
{.upper_case = 0xf09d96ad,    .lower_case = 0xf09d9787},  //  𝖭 <> 𝗇 
{.upper_case = 0xf09d96ae,    .lower_case = 0xf09d9788},  //  𝖮 <> 𝗈 
{.upper_case = 0xf09d96af,    .lower_case = 0xf09d9789},  //  𝖯 <> 𝗉 
{.upper_case = 0xf09d96b0,    .lower_case = 0xf09d978a},  //  𝖰 <> 𝗊 
{.upper_case = 0xf09d96b1,    .lower_case = 0xf09d978b},  //  𝖱 <> 𝗋 
{.upper_case = 0xf09d96b2,    .lower_case = 0xf09d978c},  //  𝖲 <> 𝗌 
{.upper_case = 0xf09d96b3,    .lower_case = 0xf09d978d},  //  𝖳 <> 𝗍 
{.upper_case = 0xf09d96b4,    .lower_case = 0xf09d978e},  //  𝖴 <> 𝗎 
{.upper_case = 0xf09d96b5,    .lower_case = 0xf09d978f},  //  𝖵 <> 𝗏 
{.upper_case = 0xf09d96b6,    .lower_case = 0xf09d9790},  //  𝖶 <> 𝗐 
{.upper_case = 0xf09d96b7,    .lower_case = 0xf09d9791},  //  𝖷 <> 𝗑 
{.upper_case = 0xf09d96b8,    .lower_case = 0xf09d9792},  //  𝖸 <> 𝗒 
{.upper_case = 0xf09d96b9,    .lower_case = 0xf09d9793},  //  𝖹 <> 𝗓 
{.upper_case = 0xf09d9794,    .lower_case = 0xf09d97ae},  //  𝗔 <> 𝗮 
{.upper_case = 0xf09d9795,    .lower_case = 0xf09d97af},  //  𝗕 <> 𝗯 
{.upper_case = 0xf09d9796,    .lower_case = 0xf09d97b0},  //  𝗖 <> 𝗰 
{.upper_case = 0xf09d9797,    .lower_case = 0xf09d97b1},  //  𝗗 <> 𝗱 
{.upper_case = 0xf09d9798,    .lower_case = 0xf09d97b2},  //  𝗘 <> 𝗲 
{.upper_case = 0xf09d9799,    .lower_case = 0xf09d97b3},  //  𝗙 <> 𝗳 
{.upper_case = 0xf09d979a,    .lower_case = 0xf09d97b4},  //  𝗚 <> 𝗴 
{.upper_case = 0xf09d979b,    .lower_case = 0xf09d97b5},  //  𝗛 <> 𝗵 
{.upper_case = 0xf09d979c,    .lower_case = 0xf09d97b6},  //  𝗜 <> 𝗶 
{.upper_case = 0xf09d979d,    .lower_case = 0xf09d97b7},  //  𝗝 <> 𝗷 
{.upper_case = 0xf09d979e,    .lower_case = 0xf09d97b8},  //  𝗞 <> 𝗸 
{.upper_case = 0xf09d979f,    .lower_case = 0xf09d97b9},  //  𝗟 <> 𝗹 
{.upper_case = 0xf09d97a0,    .lower_case = 0xf09d97ba},  //  𝗠 <> 𝗺 
{.upper_case = 0xf09d97a1,    .lower_case = 0xf09d97bb},  //  𝗡 <> 𝗻 
{.upper_case = 0xf09d97a2,    .lower_case = 0xf09d97bc},  //  𝗢 <> 𝗼 
{.upper_case = 0xf09d97a3,    .lower_case = 0xf09d97bd},  //  𝗣 <> 𝗽 
{.upper_case = 0xf09d97a4,    .lower_case = 0xf09d97be},  //  𝗤 <> 𝗾 
{.upper_case = 0xf09d97a5,    .lower_case = 0xf09d97bf},  //  𝗥 <> 𝗿 
{.upper_case = 0xf09d97a6,    .lower_case = 0xf09d9880},  //  𝗦 <> 𝘀 
{.upper_case = 0xf09d97a7,    .lower_case = 0xf09d9881},  //  𝗧 <> 𝘁 
{.upper_case = 0xf09d97a8,    .lower_case = 0xf09d9882},  //  𝗨 <> 𝘂 
{.upper_case = 0xf09d97a9,    .lower_case = 0xf09d9883},  //  𝗩 <> 𝘃 
{.upper_case = 0xf09d97aa,    .lower_case = 0xf09d9884},  //  𝗪 <> 𝘄 
{.upper_case = 0xf09d97ab,    .lower_case = 0xf09d9885},  //  𝗫 <> 𝘅 
{.upper_case = 0xf09d97ac,    .lower_case = 0xf09d9886},  //  𝗬 <> 𝘆 
{.upper_case = 0xf09d97ad,    .lower_case = 0xf09d9887},  //  𝗭 <> 𝘇 
{.upper_case = 0xf09d9888,    .lower_case = 0xf09d98a2},  //  𝘈 <> 𝘢 
{.upper_case = 0xf09d9889,    .lower_case = 0xf09d98a3},  //  𝘉 <> 𝘣 
{.upper_case = 0xf09d988a,    .lower_case = 0xf09d98a4},  //  𝘊 <> 𝘤 
{.upper_case = 0xf09d988b,    .lower_case = 0xf09d98a5},  //  𝘋 <> 𝘥 
{.upper_case = 0xf09d988c,    .lower_case = 0xf09d98a6},  //  𝘌 <> 𝘦 
{.upper_case = 0xf09d988d,    .lower_case = 0xf09d98a7},  //  𝘍 <> 𝘧 
{.upper_case = 0xf09d988e,    .lower_case = 0xf09d98a8},  //  𝘎 <> 𝘨 
{.upper_case = 0xf09d988f,    .lower_case = 0xf09d98a9},  //  𝘏 <> 𝘩 
{.upper_case = 0xf09d9890,    .lower_case = 0xf09d98aa},  //  𝘐 <> 𝘪 
{.upper_case = 0xf09d9891,    .lower_case = 0xf09d98ab},  //  𝘑 <> 𝘫 
{.upper_case = 0xf09d9892,    .lower_case = 0xf09d98ac},  //  𝘒 <> 𝘬 
{.upper_case = 0xf09d9893,    .lower_case = 0xf09d98ad},  //  𝘓 <> 𝘭 
{.upper_case = 0xf09d9894,    .lower_case = 0xf09d98ae},  //  𝘔 <> 𝘮 
{.upper_case = 0xf09d9895,    .lower_case = 0xf09d98af},  //  𝘕 <> 𝘯 
{.upper_case = 0xf09d9896,    .lower_case = 0xf09d98b0},  //  𝘖 <> 𝘰 
{.upper_case = 0xf09d9897,    .lower_case = 0xf09d98b1},  //  𝘗 <> 𝘱 
{.upper_case = 0xf09d9898,    .lower_case = 0xf09d98b2},  //  𝘘 <> 𝘲 
{.upper_case = 0xf09d9899,    .lower_case = 0xf09d98b3},  //  𝘙 <> 𝘳 
{.upper_case = 0xf09d989a,    .lower_case = 0xf09d98b4},  //  𝘚 <> 𝘴 
{.upper_case = 0xf09d989b,    .lower_case = 0xf09d98b5},  //  𝘛 <> 𝘵 
{.upper_case = 0xf09d989c,    .lower_case = 0xf09d98b6},  //  𝘜 <> 𝘶 
{.upper_case = 0xf09d989d,    .lower_case = 0xf09d98b7},  //  𝘝 <> 𝘷 
{.upper_case = 0xf09d989e,    .lower_case = 0xf09d98b8},  //  𝘞 <> 𝘸 
{.upper_case = 0xf09d989f,    .lower_case = 0xf09d98b9},  //  𝘟 <> 𝘹 
{.upper_case = 0xf09d98a0,    .lower_case = 0xf09d98ba},  //  𝘠 <> 𝘺 
{.upper_case = 0xf09d98a1,    .lower_case = 0xf09d98bb},  //  𝘡 <> 𝘻 
{.upper_case = 0xf09d98bc,    .lower_case = 0xf09d9996},  //  𝘼 <> 𝙖 
{.upper_case = 0xf09d98bd,    .lower_case = 0xf09d9997},  //  𝘽 <> 𝙗 
{.upper_case = 0xf09d98be,    .lower_case = 0xf09d9998},  //  𝘾 <> 𝙘 
{.upper_case = 0xf09d98bf,    .lower_case = 0xf09d9999},  //  𝘿 <> 𝙙 
{.upper_case = 0xf09d9980,    .lower_case = 0xf09d999a},  //  𝙀 <> 𝙚 
{.upper_case = 0xf09d9981,    .lower_case = 0xf09d999b},  //  𝙁 <> 𝙛 
{.upper_case = 0xf09d9982,    .lower_case = 0xf09d999c},  //  𝙂 <> 𝙜 
{.upper_case = 0xf09d9983,    .lower_case = 0xf09d999d},  //  𝙃 <> 𝙝 
{.upper_case = 0xf09d9984,    .lower_case = 0xf09d999e},  //  𝙄 <> 𝙞 
{.upper_case = 0xf09d9985,    .lower_case = 0xf09d999f},  //  𝙅 <> 𝙟 
{.upper_case = 0xf09d9986,    .lower_case = 0xf09d99a0},  //  𝙆 <> 𝙠 
{.upper_case = 0xf09d9987,    .lower_case = 0xf09d99a1},  //  𝙇 <> 𝙡 
{.upper_case = 0xf09d9988,    .lower_case = 0xf09d99a2},  //  𝙈 <> 𝙢 
{.upper_case = 0xf09d9989,    .lower_case = 0xf09d99a3},  //  𝙉 <> 𝙣 
{.upper_case = 0xf09d998a,    .lower_case = 0xf09d99a4},  //  𝙊 <> 𝙤 
{.upper_case = 0xf09d998b,    .lower_case = 0xf09d99a5},  //  𝙋 <> 𝙥 
{.upper_case = 0xf09d998c,    .lower_case = 0xf09d99a6},  //  𝙌 <> 𝙦 
{.upper_case = 0xf09d998d,    .lower_case = 0xf09d99a7},  //  𝙍 <> 𝙧 
{.upper_case = 0xf09d998e,    .lower_case = 0xf09d99a8},  //  𝙎 <> 𝙨 
{.upper_case = 0xf09d998f,    .lower_case = 0xf09d99a9},  //  𝙏 <> 𝙩 
{.upper_case = 0xf09d9990,    .lower_case = 0xf09d99aa},  //  𝙐 <> 𝙪 
{.upper_case = 0xf09d9991,    .lower_case = 0xf09d99ab},  //  𝙑 <> 𝙫 
{.upper_case = 0xf09d9992,    .lower_case = 0xf09d99ac},  //  𝙒 <> 𝙬 
{.upper_case = 0xf09d9993,    .lower_case = 0xf09d99ad},  //  𝙓 <> 𝙭 
{.upper_case = 0xf09d9994,    .lower_case = 0xf09d99ae},  //  𝙔 <> 𝙮 
{.upper_case = 0xf09d9995,    .lower_case = 0xf09d99af},  //  𝙕 <> 𝙯 
{.upper_case = 0xf09d99b0,    .lower_case = 0xf09d9a8a},  //  𝙰 <> 𝚊 
{.upper_case = 0xf09d99b1,    .lower_case = 0xf09d9a8b},  //  𝙱 <> 𝚋 
{.upper_case = 0xf09d99b2,    .lower_case = 0xf09d9a8c},  //  𝙲 <> 𝚌 
{.upper_case = 0xf09d99b3,    .lower_case = 0xf09d9a8d},  //  𝙳 <> 𝚍 
{.upper_case = 0xf09d99b4,    .lower_case = 0xf09d9a8e},  //  𝙴 <> 𝚎 
{.upper_case = 0xf09d99b5,    .lower_case = 0xf09d9a8f},  //  𝙵 <> 𝚏 
{.upper_case = 0xf09d99b6,    .lower_case = 0xf09d9a90},  //  𝙶 <> 𝚐 
{.upper_case = 0xf09d99b7,    .lower_case = 0xf09d9a91},  //  𝙷 <> 𝚑 
{.upper_case = 0xf09d99b8,    .lower_case = 0xf09d9a92},  //  𝙸 <> 𝚒 
{.upper_case = 0xf09d99b9,    .lower_case = 0xf09d9a93},  //  𝙹 <> 𝚓 
{.upper_case = 0xf09d99ba,    .lower_case = 0xf09d9a94},  //  𝙺 <> 𝚔 
{.upper_case = 0xf09d99bb,    .lower_case = 0xf09d9a95},  //  𝙻 <> 𝚕 
{.upper_case = 0xf09d99bc,    .lower_case = 0xf09d9a96},  //  𝙼 <> 𝚖 
{.upper_case = 0xf09d99bd,    .lower_case = 0xf09d9a97},  //  𝙽 <> 𝚗 
{.upper_case = 0xf09d99be,    .lower_case = 0xf09d9a98},  //  𝙾 <> 𝚘 
{.upper_case = 0xf09d99bf,    .lower_case = 0xf09d9a99},  //  𝙿 <> 𝚙 
{.upper_case = 0xf09d9a80,    .lower_case = 0xf09d9a9a},  //  𝚀 <> 𝚚 
{.upper_case = 0xf09d9a81,    .lower_case = 0xf09d9a9b},  //  𝚁 <> 𝚛 
{.upper_case = 0xf09d9a82,    .lower_case = 0xf09d9a9c},  //  𝚂 <> 𝚜 
{.upper_case = 0xf09d9a83,    .lower_case = 0xf09d9a9d},  //  𝚃 <> 𝚝 
{.upper_case = 0xf09d9a84,    .lower_case = 0xf09d9a9e},  //  𝚄 <> 𝚞 
{.upper_case = 0xf09d9a85,    .lower_case = 0xf09d9a9f},  //  𝚅 <> 𝚟 
{.upper_case = 0xf09d9a86,    .lower_case = 0xf09d9aa0},  //  𝚆 <> 𝚠 
{.upper_case = 0xf09d9a87,    .lower_case = 0xf09d9aa1},  //  𝚇 <> 𝚡 
{.upper_case = 0xf09d9a88,    .lower_case = 0xf09d9aa2},  //  𝚈 <> 𝚢 
{.upper_case = 0xf09d9a89,    .lower_case = 0xf09d9aa3},  //  𝚉 <> 𝚣 
{.upper_case = 0xf09d9aa8,    .lower_case = 0xf09d9b82},  //  𝚨 <> 𝛂 
{.upper_case = 0xf09d9aa9,    .lower_case = 0xf09d9b83},  //  𝚩 <> 𝛃 
{.upper_case = 0xf09d9aaa,    .lower_case = 0xf09d9b84},  //  𝚪 <> 𝛄 
{.upper_case = 0xf09d9aab,    .lower_case = 0xf09d9b85},  //  𝚫 <> 𝛅 
{.upper_case = 0xf09d9aac,    .lower_case = 0xf09d9b86},  //  𝚬 <> 𝛆 
{.upper_case = 0xf09d9aad,    .lower_case = 0xf09d9b87},  //  𝚭 <> 𝛇 
{.upper_case = 0xf09d9aae,    .lower_case = 0xf09d9b88},  //  𝚮 <> 𝛈 
{.upper_case = 0xf09d9aaf,    .lower_case = 0xf09d9b89},  //  𝚯 <> 𝛉 
{.upper_case = 0xf09d9ab0,    .lower_case = 0xf09d9b8a},  //  𝚰 <> 𝛊 
{.upper_case = 0xf09d9ab1,    .lower_case = 0xf09d9b8b},  //  𝚱 <> 𝛋 
{.upper_case = 0xf09d9ab2,    .lower_case = 0xf09d9b8c},  //  𝚲 <> 𝛌 
{.upper_case = 0xf09d9ab3,    .lower_case = 0xf09d9b8d},  //  𝚳 <> 𝛍 
{.upper_case = 0xf09d9ab4,    .lower_case = 0xf09d9b8e},  //  𝚴 <> 𝛎 
{.upper_case = 0xf09d9ab5,    .lower_case = 0xf09d9b8f},  //  𝚵 <> 𝛏 
{.upper_case = 0xf09d9ab6,    .lower_case = 0xf09d9b90},  //  𝚶 <> 𝛐 
{.upper_case = 0xf09d9ab7,    .lower_case = 0xf09d9b91},  //  𝚷 <> 𝛑 
{.upper_case = 0xf09d9ab8,    .lower_case = 0xf09d9b92},  //  𝚸 <> 𝛒 
{.upper_case = 0xf09d9aba,    .lower_case = 0xf09d9b94},  //  𝚺 <> 𝛔 
{.upper_case = 0xf09d9abb,    .lower_case = 0xf09d9b95},  //  𝚻 <> 𝛕 
{.upper_case = 0xf09d9abc,    .lower_case = 0xf09d9b96},  //  𝚼 <> 𝛖 
{.upper_case = 0xf09d9abd,    .lower_case = 0xf09d9b97},  //  𝚽 <> 𝛗 
{.upper_case = 0xf09d9abe,    .lower_case = 0xf09d9b98},  //  𝚾 <> 𝛘 
{.upper_case = 0xf09d9abf,    .lower_case = 0xf09d9b99},  //  𝚿 <> 𝛙 
{.upper_case = 0xf09d9b80,    .lower_case = 0xf09d9b9a},  //  𝛀 <> 𝛚 
{.upper_case = 0xf09d9ba2,    .lower_case = 0xf09d9bbc},  //  𝛢 <> 𝛼 
{.upper_case = 0xf09d9ba3,    .lower_case = 0xf09d9bbd},  //  𝛣 <> 𝛽 
{.upper_case = 0xf09d9ba4,    .lower_case = 0xf09d9bbe},  //  𝛤 <> 𝛾 
{.upper_case = 0xf09d9ba5,    .lower_case = 0xf09d9bbf},  //  𝛥 <> 𝛿 
{.upper_case = 0xf09d9ba6,    .lower_case = 0xf09d9c80},  //  𝛦 <> 𝜀 
{.upper_case = 0xf09d9ba7,    .lower_case = 0xf09d9c81},  //  𝛧 <> 𝜁 
{.upper_case = 0xf09d9ba8,    .lower_case = 0xf09d9c82},  //  𝛨 <> 𝜂 
{.upper_case = 0xf09d9ba9,    .lower_case = 0xf09d9c83},  //  𝛩 <> 𝜃 
{.upper_case = 0xf09d9baa,    .lower_case = 0xf09d9c84},  //  𝛪 <> 𝜄 
{.upper_case = 0xf09d9bab,    .lower_case = 0xf09d9c85},  //  𝛫 <> 𝜅 
{.upper_case = 0xf09d9bac,    .lower_case = 0xf09d9c86},  //  𝛬 <> 𝜆 
{.upper_case = 0xf09d9bad,    .lower_case = 0xf09d9c87},  //  𝛭 <> 𝜇 
{.upper_case = 0xf09d9bae,    .lower_case = 0xf09d9c88},  //  𝛮 <> 𝜈 
{.upper_case = 0xf09d9baf,    .lower_case = 0xf09d9c89},  //  𝛯 <> 𝜉 
{.upper_case = 0xf09d9bb0,    .lower_case = 0xf09d9c8a},  //  𝛰 <> 𝜊 
{.upper_case = 0xf09d9bb1,    .lower_case = 0xf09d9c8b},  //  𝛱 <> 𝜋 
{.upper_case = 0xf09d9bb2,    .lower_case = 0xf09d9c8c},  //  𝛲 <> 𝜌 
{.upper_case = 0xf09d9bb4,    .lower_case = 0xf09d9c8e},  //  𝛴 <> 𝜎 
{.upper_case = 0xf09d9bb5,    .lower_case = 0xf09d9c8f},  //  𝛵 <> 𝜏 
{.upper_case = 0xf09d9bb6,    .lower_case = 0xf09d9c90},  //  𝛶 <> 𝜐 
{.upper_case = 0xf09d9bb7,    .lower_case = 0xf09d9c91},  //  𝛷 <> 𝜑 
{.upper_case = 0xf09d9bb8,    .lower_case = 0xf09d9c92},  //  𝛸 <> 𝜒 
{.upper_case = 0xf09d9bb9,    .lower_case = 0xf09d9c93},  //  𝛹 <> 𝜓 
{.upper_case = 0xf09d9bba,    .lower_case = 0xf09d9c94},  //  𝛺 <> 𝜔 
{.upper_case = 0xf09d9c9c,    .lower_case = 0xf09d9cb6},  //  𝜜 <> 𝜶 
{.upper_case = 0xf09d9c9d,    .lower_case = 0xf09d9cb7},  //  𝜝 <> 𝜷 
{.upper_case = 0xf09d9c9e,    .lower_case = 0xf09d9cb8},  //  𝜞 <> 𝜸 
{.upper_case = 0xf09d9c9f,    .lower_case = 0xf09d9cb9},  //  𝜟 <> 𝜹 
{.upper_case = 0xf09d9ca0,    .lower_case = 0xf09d9cba},  //  𝜠 <> 𝜺 
{.upper_case = 0xf09d9ca1,    .lower_case = 0xf09d9cbb},  //  𝜡 <> 𝜻 
{.upper_case = 0xf09d9ca2,    .lower_case = 0xf09d9cbc},  //  𝜢 <> 𝜼 
{.upper_case = 0xf09d9ca3,    .lower_case = 0xf09d9cbd},  //  𝜣 <> 𝜽 
{.upper_case = 0xf09d9ca4,    .lower_case = 0xf09d9cbe},  //  𝜤 <> 𝜾 
{.upper_case = 0xf09d9ca5,    .lower_case = 0xf09d9cbf},  //  𝜥 <> 𝜿 
{.upper_case = 0xf09d9ca6,    .lower_case = 0xf09d9d80},  //  𝜦 <> 𝝀 
{.upper_case = 0xf09d9ca7,    .lower_case = 0xf09d9d81},  //  𝜧 <> 𝝁 
{.upper_case = 0xf09d9ca8,    .lower_case = 0xf09d9d82},  //  𝜨 <> 𝝂 
{.upper_case = 0xf09d9ca9,    .lower_case = 0xf09d9d83},  //  𝜩 <> 𝝃 
{.upper_case = 0xf09d9caa,    .lower_case = 0xf09d9d84},  //  𝜪 <> 𝝄 
{.upper_case = 0xf09d9cab,    .lower_case = 0xf09d9d85},  //  𝜫 <> 𝝅 
{.upper_case = 0xf09d9cac,    .lower_case = 0xf09d9d86},  //  𝜬 <> 𝝆 
{.upper_case = 0xf09d9cae,    .lower_case = 0xf09d9d88},  //  𝜮 <> 𝝈 
{.upper_case = 0xf09d9caf,    .lower_case = 0xf09d9d89},  //  𝜯 <> 𝝉 
{.upper_case = 0xf09d9cb0,    .lower_case = 0xf09d9d8a},  //  𝜰 <> 𝝊 
{.upper_case = 0xf09d9cb1,    .lower_case = 0xf09d9d8b},  //  𝜱 <> 𝝋 
{.upper_case = 0xf09d9cb2,    .lower_case = 0xf09d9d8c},  //  𝜲 <> 𝝌 
{.upper_case = 0xf09d9cb3,    .lower_case = 0xf09d9d8d},  //  𝜳 <> 𝝍 
{.upper_case = 0xf09d9cb4,    .lower_case = 0xf09d9d8e},  //  𝜴 <> 𝝎 
{.upper_case = 0xf09d9d96,    .lower_case = 0xf09d9db0},  //  𝝖 <> 𝝰 
{.upper_case = 0xf09d9d97,    .lower_case = 0xf09d9db1},  //  𝝗 <> 𝝱 
{.upper_case = 0xf09d9d98,    .lower_case = 0xf09d9db2},  //  𝝘 <> 𝝲 
{.upper_case = 0xf09d9d99,    .lower_case = 0xf09d9db3},  //  𝝙 <> 𝝳 
{.upper_case = 0xf09d9d9a,    .lower_case = 0xf09d9db4},  //  𝝚 <> 𝝴 
{.upper_case = 0xf09d9d9b,    .lower_case = 0xf09d9db5},  //  𝝛 <> 𝝵 
{.upper_case = 0xf09d9d9c,    .lower_case = 0xf09d9db6},  //  𝝜 <> 𝝶 
{.upper_case = 0xf09d9d9d,    .lower_case = 0xf09d9db7},  //  𝝝 <> 𝝷 
{.upper_case = 0xf09d9d9e,    .lower_case = 0xf09d9db8},  //  𝝞 <> 𝝸 
{.upper_case = 0xf09d9d9f,    .lower_case = 0xf09d9db9},  //  𝝟 <> 𝝹 
{.upper_case = 0xf09d9da0,    .lower_case = 0xf09d9dba},  //  𝝠 <> 𝝺 
{.upper_case = 0xf09d9da1,    .lower_case = 0xf09d9dbb},  //  𝝡 <> 𝝻 
{.upper_case = 0xf09d9da2,    .lower_case = 0xf09d9dbc},  //  𝝢 <> 𝝼 
{.upper_case = 0xf09d9da3,    .lower_case = 0xf09d9dbd},  //  𝝣 <> 𝝽 
{.upper_case = 0xf09d9da4,    .lower_case = 0xf09d9dbe},  //  𝝤 <> 𝝾 
{.upper_case = 0xf09d9da5,    .lower_case = 0xf09d9dbf},  //  𝝥 <> 𝝿 
{.upper_case = 0xf09d9da6,    .lower_case = 0xf09d9e80},  //  𝝦 <> 𝞀 
{.upper_case = 0xf09d9da8,    .lower_case = 0xf09d9e82},  //  𝝨 <> 𝞂 
{.upper_case = 0xf09d9da9,    .lower_case = 0xf09d9e83},  //  𝝩 <> 𝞃 
{.upper_case = 0xf09d9daa,    .lower_case = 0xf09d9e84},  //  𝝪 <> 𝞄 
{.upper_case = 0xf09d9dab,    .lower_case = 0xf09d9e85},  //  𝝫 <> 𝞅 
{.upper_case = 0xf09d9dac,    .lower_case = 0xf09d9e86},  //  𝝬 <> 𝞆 
{.upper_case = 0xf09d9dad,    .lower_case = 0xf09d9e87},  //  𝝭 <> 𝞇 
{.upper_case = 0xf09d9dae,    .lower_case = 0xf09d9e88},  //  𝝮 <> 𝞈 
{.upper_case = 0xf09d9e90,    .lower_case = 0xf09d9eaa},  //  𝞐 <> 𝞪 
{.upper_case = 0xf09d9e91,    .lower_case = 0xf09d9eab},  //  𝞑 <> 𝞫 
{.upper_case = 0xf09d9e92,    .lower_case = 0xf09d9eac},  //  𝞒 <> 𝞬 
{.upper_case = 0xf09d9e93,    .lower_case = 0xf09d9ead},  //  𝞓 <> 𝞭 
{.upper_case = 0xf09d9e94,    .lower_case = 0xf09d9eae},  //  𝞔 <> 𝞮 
{.upper_case = 0xf09d9e95,    .lower_case = 0xf09d9eaf},  //  𝞕 <> 𝞯 
{.upper_case = 0xf09d9e96,    .lower_case = 0xf09d9eb0},  //  𝞖 <> 𝞰 
{.upper_case = 0xf09d9e97,    .lower_case = 0xf09d9eb1},  //  𝞗 <> 𝞱 
{.upper_case = 0xf09d9e98,    .lower_case = 0xf09d9eb2},  //  𝞘 <> 𝞲 
{.upper_case = 0xf09d9e99,    .lower_case = 0xf09d9eb3},  //  𝞙 <> 𝞳 
{.upper_case = 0xf09d9e9a,    .lower_case = 0xf09d9eb4},  //  𝞚 <> 𝞴 
{.upper_case = 0xf09d9e9b,    .lower_case = 0xf09d9eb5},  //  𝞛 <> 𝞵 
{.upper_case = 0xf09d9e9c,    .lower_case = 0xf09d9eb6},  //  𝞜 <> 𝞶 
{.upper_case = 0xf09d9e9d,    .lower_case = 0xf09d9eb7},  //  𝞝 <> 𝞷 
{.upper_case = 0xf09d9e9e,    .lower_case = 0xf09d9eb8},  //  𝞞 <> 𝞸 
{.upper_case = 0xf09d9e9f,    .lower_case = 0xf09d9eb9},  //  𝞟 <> 𝞹 
{.upper_case = 0xf09d9ea0,    .lower_case = 0xf09d9eba},  //  𝞠 <> 𝞺 
{.upper_case = 0xf09d9ea2,    .lower_case = 0xf09d9ebc},  //  𝞢 <> 𝞼 
{.upper_case = 0xf09d9ea3,    .lower_case = 0xf09d9ebd},  //  𝞣 <> 𝞽 
{.upper_case = 0xf09d9ea4,    .lower_case = 0xf09d9ebe},  //  𝞤 <> 𝞾 
{.upper_case = 0xf09d9ea5,    .lower_case = 0xf09d9ebf},  //  𝞥 <> 𝞿 
{.upper_case = 0xf09d9ea6,    .lower_case = 0xf09d9f80},  //  𝞦 <> 𝟀 
{.upper_case = 0xf09d9ea7,    .lower_case = 0xf09d9f81},  //  𝞧 <> 𝟁 
{.upper_case = 0xf09d9ea8,    .lower_case = 0xf09d9f82},  //  𝞨 <> 𝟂 
{.upper_case = 0xf09d9f8a,    .lower_case = 0xf09d9f8b},  //  𝟊 <> 𝟋 
{.upper_case = 0xf09f94a0,    .lower_case = 0xf09f94a1},  //  🔠 <> 🔡 
{.upper_case = 0xf3a08181,    .lower_case = 0xf3a081a1},  //  󠁁 <> 󠁡 
{.upper_case = 0xf3a08182,    .lower_case = 0xf3a081a2},  //  󠁂 <> 󠁢 
{.upper_case = 0xf3a08183,    .lower_case = 0xf3a081a3},  //  󠁃 <> 󠁣 
{.upper_case = 0xf3a08184,    .lower_case = 0xf3a081a4},  //  󠁄 <> 󠁤 
{.upper_case = 0xf3a08185,    .lower_case = 0xf3a081a5},  //  󠁅 <> 󠁥 
{.upper_case = 0xf3a08186,    .lower_case = 0xf3a081a6},  //  󠁆 <> 󠁦 
{.upper_case = 0xf3a08187,    .lower_case = 0xf3a081a7},  //  󠁇 <> 󠁧 
{.upper_case = 0xf3a08188,    .lower_case = 0xf3a081a8},  //  󠁈 <> 󠁨 
{.upper_case = 0xf3a08189,    .lower_case = 0xf3a081a9},  //  󠁉 <> 󠁩 
{.upper_case = 0xf3a0818a,    .lower_case = 0xf3a081aa},  //  󠁊 <> 󠁪 
{.upper_case = 0xf3a0818b,    .lower_case = 0xf3a081ab},  //  󠁋 <> 󠁫 
{.upper_case = 0xf3a0818c,    .lower_case = 0xf3a081ac},  //  󠁌 <> 󠁬 
{.upper_case = 0xf3a0818d,    .lower_case = 0xf3a081ad},  //  󠁍 <> 󠁭 
{.upper_case = 0xf3a0818e,    .lower_case = 0xf3a081ae},  //  󠁎 <> 󠁮 
{.upper_case = 0xf3a0818f,    .lower_case = 0xf3a081af},  //  󠁏 <> 󠁯 
{.upper_case = 0xf3a08190,    .lower_case = 0xf3a081b0},  //  󠁐 <> 󠁰 
{.upper_case = 0xf3a08191,    .lower_case = 0xf3a081b1},  //  󠁑 <> 󠁱 
{.upper_case = 0xf3a08192,    .lower_case = 0xf3a081b2},  //  󠁒 <> 󠁲 
{.upper_case = 0xf3a08193,    .lower_case = 0xf3a081b3},  //  󠁓 <> 󠁳 
{.upper_case = 0xf3a08194,    .lower_case = 0xf3a081b4},  //  󠁔 <> 󠁴 
{.upper_case = 0xf3a08195,    .lower_case = 0xf3a081b5},  //  󠁕 <> 󠁵 
{.upper_case = 0xf3a08196,    .lower_case = 0xf3a081b6},  //  󠁖 <> 󠁶 
{.upper_case = 0xf3a08197,    .lower_case = 0xf3a081b7},  //  󠁗 <> 󠁷 
{.upper_case = 0xf3a08198,    .lower_case = 0xf3a081b8},  //  󠁘 <> 󠁸 
{.upper_case = 0xf3a08199,    .lower_case = 0xf3a081b9},  //  󠁙 <> 󠁹 
{.upper_case = 0xf3a0819a,    .lower_case = 0xf3a081ba},  //  󠁚 <> 󠁺 
{.upper_case = 0xe1ba9e,    .lower_case = 0xc39f},  //  ẞ <> ß 
{.upper_case = 0xc380,    .lower_case = 0xc3a0},  //  À <> à 
{.upper_case = 0xc381,    .lower_case = 0xc3a1},  //  Á <> á 
{.upper_case = 0xc382,    .lower_case = 0xc3a2},  //  Â <> â 
{.upper_case = 0xc383,    .lower_case = 0xc3a3},  //  Ã <> ã 
{.upper_case = 0xc384,    .lower_case = 0xc3a4},  //  Ä <> ä 
{.upper_case = 0xc385,    .lower_case = 0xc3a5},  //  Å <> å 
{.upper_case = 0xc386,    .lower_case = 0xc3a6},  //  Æ <> æ 
{.upper_case = 0xc387,    .lower_case = 0xc3a7},  //  Ç <> ç 
{.upper_case = 0xc388,    .lower_case = 0xc3a8},  //  È <> è 
{.upper_case = 0xc389,    .lower_case = 0xc3a9},  //  É <> é 
{.upper_case = 0xc38a,    .lower_case = 0xc3aa},  //  Ê <> ê 
{.upper_case = 0xc38b,    .lower_case = 0xc3ab},  //  Ë <> ë 
{.upper_case = 0xc38c,    .lower_case = 0xc3ac},  //  Ì <> ì 
{.upper_case = 0xc38d,    .lower_case = 0xc3ad},  //  Í <> í 
{.upper_case = 0xc38e,    .lower_case = 0xc3ae},  //  Î <> î 
{.upper_case = 0xc38f,    .lower_case = 0xc3af},  //  Ï <> ï 
{.upper_case = 0xc390,    .lower_case = 0xc3b0},  //  Ð <> ð 
{.upper_case = 0xc391,    .lower_case = 0xc3b1},  //  Ñ <> ñ 
{.upper_case = 0xc392,    .lower_case = 0xc3b2},  //  Ò <> ò 
{.upper_case = 0xc393,    .lower_case = 0xc3b3},  //  Ó <> ó 
{.upper_case = 0xc394,    .lower_case = 0xc3b4},  //  Ô <> ô 
{.upper_case = 0xc395,    .lower_case = 0xc3b5},  //  Õ <> õ 
{.upper_case = 0xc396,    .lower_case = 0xc3b6},  //  Ö <> ö 
{.upper_case = 0xc398,    .lower_case = 0xc3b8},  //  Ø <> ø 
{.upper_case = 0xc399,    .lower_case = 0xc3b9},  //  Ù <> ù 
{.upper_case = 0xc39a,    .lower_case = 0xc3ba},  //  Ú <> ú 
{.upper_case = 0xc39b,    .lower_case = 0xc3bb},  //  Û <> û 
{.upper_case = 0xc39c,    .lower_case = 0xc3bc},  //  Ü <> ü 
{.upper_case = 0xc39d,    .lower_case = 0xc3bd},  //  Ý <> ý 
{.upper_case = 0xc39e,    .lower_case = 0xc3be},  //  Þ <> þ 
{.upper_case = 0xc5b8,    .lower_case = 0xc3bf},  //  Ÿ <> ÿ 
{.upper_case = 0xc480,    .lower_case = 0xc481},  //  Ā <> ā 
{.upper_case = 0xc482,    .lower_case = 0xc483},  //  Ă <> ă 
{.upper_case = 0xc484,    .lower_case = 0xc485},  //  Ą <> ą 
{.upper_case = 0xc486,    .lower_case = 0xc487},  //  Ć <> ć 
{.upper_case = 0xc488,    .lower_case = 0xc489},  //  Ĉ <> ĉ 
{.upper_case = 0xc48a,    .lower_case = 0xc48b},  //  Ċ <> ċ 
{.upper_case = 0xc48c,    .lower_case = 0xc48d},  //  Č <> č 
{.upper_case = 0xc48e,    .lower_case = 0xc48f},  //  Ď <> ď 
{.upper_case = 0xc490,    .lower_case = 0xc491},  //  Đ <> đ 
{.upper_case = 0xc492,    .lower_case = 0xc493},  //  Ē <> ē 
{.upper_case = 0xc494,    .lower_case = 0xc495},  //  Ĕ <> ĕ 
{.upper_case = 0xc496,    .lower_case = 0xc497},  //  Ė <> ė 
{.upper_case = 0xc498,    .lower_case = 0xc499},  //  Ę <> ę 
{.upper_case = 0xc49a,    .lower_case = 0xc49b},  //  Ě <> ě 
{.upper_case = 0xc49c,    .lower_case = 0xc49d},  //  Ĝ <> ĝ 
{.upper_case = 0xc49e,    .lower_case = 0xc49f},  //  Ğ <> ğ 
{.upper_case = 0xc4a0,    .lower_case = 0xc4a1},  //  Ġ <> ġ 
{.upper_case = 0xc4a2,    .lower_case = 0xc4a3},  //  Ģ <> ģ 
{.upper_case = 0xc4a4,    .lower_case = 0xc4a5},  //  Ĥ <> ĥ 
{.upper_case = 0xc4a6,    .lower_case = 0xc4a7},  //  Ħ <> ħ 
{.upper_case = 0xc4a8,    .lower_case = 0xc4a9},  //  Ĩ <> ĩ 
{.upper_case = 0xc4aa,    .lower_case = 0xc4ab},  //  Ī <> ī 
{.upper_case = 0xc4ac,    .lower_case = 0xc4ad},  //  Ĭ <> ĭ 
{.upper_case = 0xc4ae,    .lower_case = 0xc4af},  //  Į <> į 
{.upper_case = 0xc4b2,    .lower_case = 0xc4b3},  //  Ĳ <> ĳ 
{.upper_case = 0xc4b4,    .lower_case = 0xc4b5},  //  Ĵ <> ĵ 
{.upper_case = 0xc4b6,    .lower_case = 0xc4b7},  //  Ķ <> ķ 
{.upper_case = 0xc4b9,    .lower_case = 0xc4ba},  //  Ĺ <> ĺ 
{.upper_case = 0xc4bb,    .lower_case = 0xc4bc},  //  Ļ <> ļ 
{.upper_case = 0xc4bd,    .lower_case = 0xc4be},  //  Ľ <> ľ 
{.upper_case = 0xc4bf,    .lower_case = 0xc580},  //  Ŀ <> ŀ 
{.upper_case = 0xc581,    .lower_case = 0xc582},  //  Ł <> ł 
{.upper_case = 0xc583,    .lower_case = 0xc584},  //  Ń <> ń 
{.upper_case = 0xc585,    .lower_case = 0xc586},  //  Ņ <> ņ 
{.upper_case = 0xc587,    .lower_case = 0xc588},  //  Ň <> ň 
{.upper_case = 0xc58a,    .lower_case = 0xc58b},  //  Ŋ <> ŋ 
{.upper_case = 0xc58c,    .lower_case = 0xc58d},  //  Ō <> ō 
{.upper_case = 0xc58e,    .lower_case = 0xc58f},  //  Ŏ <> ŏ 
{.upper_case = 0xc590,    .lower_case = 0xc591},  //  Ő <> ő 
{.upper_case = 0xc592,    .lower_case = 0xc593},  //  Œ <> œ 
{.upper_case = 0xc594,    .lower_case = 0xc595},  //  Ŕ <> ŕ 
{.upper_case = 0xc596,    .lower_case = 0xc597},  //  Ŗ <> ŗ 
{.upper_case = 0xc598,    .lower_case = 0xc599},  //  Ř <> ř 
{.upper_case = 0xc59a,    .lower_case = 0xc59b},  //  Ś <> ś 
{.upper_case = 0xc59c,    .lower_case = 0xc59d},  //  Ŝ <> ŝ 
{.upper_case = 0xc59e,    .lower_case = 0xc59f},  //  Ş <> ş 
{.upper_case = 0xc5a0,    .lower_case = 0xc5a1},  //  Š <> š 
{.upper_case = 0xc5a2,    .lower_case = 0xc5a3},  //  Ţ <> ţ 
{.upper_case = 0xc5a4,    .lower_case = 0xc5a5},  //  Ť <> ť 
{.upper_case = 0xc5a6,    .lower_case = 0xc5a7},  //  Ŧ <> ŧ 
{.upper_case = 0xc5a8,    .lower_case = 0xc5a9},  //  Ũ <> ũ 
{.upper_case = 0xc5aa,    .lower_case = 0xc5ab},  //  Ū <> ū 
{.upper_case = 0xc5ac,    .lower_case = 0xc5ad},  //  Ŭ <> ŭ 
{.upper_case = 0xc5ae,    .lower_case = 0xc5af},  //  Ů <> ů 
{.upper_case = 0xc5b0,    .lower_case = 0xc5b1},  //  Ű <> ű 
{.upper_case = 0xc5b2,    .lower_case = 0xc5b3},  //  Ų <> ų 
{.upper_case = 0xc5b4,    .lower_case = 0xc5b5},  //  Ŵ <> ŵ 
{.upper_case = 0xc5b6,    .lower_case = 0xc5b7},  //  Ŷ <> ŷ 
{.upper_case = 0xc5b9,    .lower_case = 0xc5ba},  //  Ź <> ź 
{.upper_case = 0xc5bb,    .lower_case = 0xc5bc},  //  Ż <> ż 
{.upper_case = 0xc5bd,    .lower_case = 0xc5be},  //  Ž <> ž 
{.upper_case = 0xc983,    .lower_case = 0xc680},  //  Ƀ <> ƀ 
{.upper_case = 0xc682,    .lower_case = 0xc683},  //  Ƃ <> ƃ 
{.upper_case = 0xc684,    .lower_case = 0xc685},  //  Ƅ <> ƅ 
{.upper_case = 0xc687,    .lower_case = 0xc688},  //  Ƈ <> ƈ 
{.upper_case = 0xc68b,    .lower_case = 0xc68c},  //  Ƌ <> ƌ 
{.upper_case = 0xc691,    .lower_case = 0xc692},  //  Ƒ <> ƒ 
{.upper_case = 0xc698,    .lower_case = 0xc699},  //  Ƙ <> ƙ 
{.upper_case = 0xc8bd,    .lower_case = 0xc69a},  //  Ƚ <> ƚ 
{.upper_case = 0xc8a0,    .lower_case = 0xc69e},  //  Ƞ <> ƞ 
{.upper_case = 0xc6a0,    .lower_case = 0xc6a1},  //  Ơ <> ơ 
{.upper_case = 0xc6a2,    .lower_case = 0xc6a3},  //  Ƣ <> ƣ 
{.upper_case = 0xc6a4,    .lower_case = 0xc6a5},  //  Ƥ <> ƥ 
{.upper_case = 0xc6a7,    .lower_case = 0xc6a8},  //  Ƨ <> ƨ 
{.upper_case = 0xc6ac,    .lower_case = 0xc6ad},  //  Ƭ <> ƭ 
{.upper_case = 0xc6af,    .lower_case = 0xc6b0},  //  Ư <> ư 
{.upper_case = 0xc6b3,    .lower_case = 0xc6b4},  //  Ƴ <> ƴ 
{.upper_case = 0xc6b5,    .lower_case = 0xc6b6},  //  Ƶ <> ƶ 
{.upper_case = 0xc6b8,    .lower_case = 0xc6b9},  //  Ƹ <> ƹ 
{.upper_case = 0xc6bc,    .lower_case = 0xc6bd},  //  Ƽ <> ƽ 
{.upper_case = 0xc784,    .lower_case = 0xc786},  //  Ǆ <> ǆ 
{.upper_case = 0xc787,    .lower_case = 0xc789},  //  Ǉ <> ǉ 
{.upper_case = 0xc78a,    .lower_case = 0xc78c},  //  Ǌ <> ǌ 
{.upper_case = 0xc78d,    .lower_case = 0xc78e},  //  Ǎ <> ǎ 
{.upper_case = 0xc78f,    .lower_case = 0xc790},  //  Ǐ <> ǐ 
{.upper_case = 0xc791,    .lower_case = 0xc792},  //  Ǒ <> ǒ 
{.upper_case = 0xc793,    .lower_case = 0xc794},  //  Ǔ <> ǔ 
{.upper_case = 0xc795,    .lower_case = 0xc796},  //  Ǖ <> ǖ 
{.upper_case = 0xc797,    .lower_case = 0xc798},  //  Ǘ <> ǘ 
{.upper_case = 0xc799,    .lower_case = 0xc79a},  //  Ǚ <> ǚ 
{.upper_case = 0xc79b,    .lower_case = 0xc79c},  //  Ǜ <> ǜ 
{.upper_case = 0xc79e,    .lower_case = 0xc79f},  //  Ǟ <> ǟ 
{.upper_case = 0xc7a0,    .lower_case = 0xc7a1},  //  Ǡ <> ǡ 
{.upper_case = 0xc7a2,    .lower_case = 0xc7a3},  //  Ǣ <> ǣ 
{.upper_case = 0xc7a4,    .lower_case = 0xc7a5},  //  Ǥ <> ǥ 
{.upper_case = 0xc7a6,    .lower_case = 0xc7a7},  //  Ǧ <> ǧ 
{.upper_case = 0xc7a8,    .lower_case = 0xc7a9},  //  Ǩ <> ǩ 
{.upper_case = 0xc7aa,    .lower_case = 0xc7ab},  //  Ǫ <> ǫ 
{.upper_case = 0xc7ac,    .lower_case = 0xc7ad},  //  Ǭ <> ǭ 
{.upper_case = 0xc7ae,    .lower_case = 0xc7af},  //  Ǯ <> ǯ 
{.upper_case = 0xc7b1,    .lower_case = 0xc7b3},  //  Ǳ <> ǳ 
{.upper_case = 0xc7b4,    .lower_case = 0xc7b5},  //  Ǵ <> ǵ 
{.upper_case = 0xc7b8,    .lower_case = 0xc7b9},  //  Ǹ <> ǹ 
{.upper_case = 0xc7ba,    .lower_case = 0xc7bb},  //  Ǻ <> ǻ 
{.upper_case = 0xc7bc,    .lower_case = 0xc7bd},  //  Ǽ <> ǽ 
{.upper_case = 0xc7be,    .lower_case = 0xc7bf},  //  Ǿ <> ǿ 
{.upper_case = 0xc880,    .lower_case = 0xc881},  //  Ȁ <> ȁ 
{.upper_case = 0xc882,    .lower_case = 0xc883},  //  Ȃ <> ȃ 
{.upper_case = 0xc884,    .lower_case = 0xc885},  //  Ȅ <> ȅ 
{.upper_case = 0xc886,    .lower_case = 0xc887},  //  Ȇ <> ȇ 
{.upper_case = 0xc888,    .lower_case = 0xc889},  //  Ȉ <> ȉ 
{.upper_case = 0xc88a,    .lower_case = 0xc88b},  //  Ȋ <> ȋ 
{.upper_case = 0xc88c,    .lower_case = 0xc88d},  //  Ȍ <> ȍ 
{.upper_case = 0xc88e,    .lower_case = 0xc88f},  //  Ȏ <> ȏ 
{.upper_case = 0xc890,    .lower_case = 0xc891},  //  Ȑ <> ȑ 
{.upper_case = 0xc892,    .lower_case = 0xc893},  //  Ȓ <> ȓ 
{.upper_case = 0xc894,    .lower_case = 0xc895},  //  Ȕ <> ȕ 
{.upper_case = 0xc896,    .lower_case = 0xc897},  //  Ȗ <> ȗ 
{.upper_case = 0xc898,    .lower_case = 0xc899},  //  Ș <> ș 
{.upper_case = 0xc89a,    .lower_case = 0xc89b},  //  Ț <> ț 
{.upper_case = 0xc89c,    .lower_case = 0xc89d},  //  Ȝ <> ȝ 
{.upper_case = 0xc89e,    .lower_case = 0xc89f},  //  Ȟ <> ȟ 
{.upper_case = 0xc8a2,    .lower_case = 0xc8a3},  //  Ȣ <> ȣ 
{.upper_case = 0xc8a4,    .lower_case = 0xc8a5},  //  Ȥ <> ȥ 
{.upper_case = 0xc8a6,    .lower_case = 0xc8a7},  //  Ȧ <> ȧ 
{.upper_case = 0xc8a8,    .lower_case = 0xc8a9},  //  Ȩ <> ȩ 
{.upper_case = 0xc8aa,    .lower_case = 0xc8ab},  //  Ȫ <> ȫ 
{.upper_case = 0xc8ac,    .lower_case = 0xc8ad},  //  Ȭ <> ȭ 
{.upper_case = 0xc8ae,    .lower_case = 0xc8af},  //  Ȯ <> ȯ 
{.upper_case = 0xc8b0,    .lower_case = 0xc8b1},  //  Ȱ <> ȱ 
{.upper_case = 0xc8b2,    .lower_case = 0xc8b3},  //  Ȳ <> ȳ 
{.upper_case = 0xc8bb,    .lower_case = 0xc8bc},  //  Ȼ <> ȼ 
{.upper_case = 0xe2b1be,    .lower_case = 0xc8bf},  //  Ȿ <> ȿ 
{.upper_case = 0xe2b1bf,    .lower_case = 0xc980},  //  Ɀ <> ɀ 
{.upper_case = 0xc981,    .lower_case = 0xc982},  //  Ɂ <> ɂ 
{.upper_case = 0xc986,    .lower_case = 0xc987},  //  Ɇ <> ɇ 
{.upper_case = 0xc988,    .lower_case = 0xc989},  //  Ɉ <> ɉ 
{.upper_case = 0xc98c,    .lower_case = 0xc98d},  //  Ɍ <> ɍ 
{.upper_case = 0xc98e,    .lower_case = 0xc98f},  //  Ɏ <> ɏ 
{.upper_case = 0xe2b1af,    .lower_case = 0xc990},  //  Ɐ <> ɐ 
{.upper_case = 0xe2b1ad,    .lower_case = 0xc991},  //  Ɑ <> ɑ 
{.upper_case = 0xe2b1b0,    .lower_case = 0xc992},  //  Ɒ <> ɒ 
{.upper_case = 0xc681,    .lower_case = 0xc993},  //  Ɓ <> ɓ 
{.upper_case = 0xc686,    .lower_case = 0xc994},  //  Ɔ <> ɔ 
{.upper_case = 0xc68a,    .lower_case = 0xc997},  //  Ɗ <> ɗ 
{.upper_case = 0xc68e,    .lower_case = 0xc998},  //  Ǝ <> ɘ 
{.upper_case = 0xc68f,    .lower_case = 0xc999},  //  Ə <> ə 
{.upper_case = 0xc690,    .lower_case = 0xc99b},  //  Ɛ <> ɛ 
{.upper_case = 0xc693,    .lower_case = 0xc9a0},  //  Ɠ <> ɠ 
{.upper_case = 0xc694,    .lower_case = 0xc9a3},  //  Ɣ <> ɣ 
{.upper_case = 0xea9e8d,    .lower_case = 0xc9a5},  //  Ɥ <> ɥ 
{.upper_case = 0xc697,    .lower_case = 0xc9a8},  //  Ɨ <> ɨ 
{.upper_case = 0xc696,    .lower_case = 0xc9a9},  //  Ɩ <> ɩ 
{.upper_case = 0xe2b1a2,    .lower_case = 0xc9ab},  //  Ɫ <> ɫ 
{.upper_case = 0xc69c,    .lower_case = 0xc9af},  //  Ɯ <> ɯ 
{.upper_case = 0xe2b1ae,    .lower_case = 0xc9b1},  //  Ɱ <> ɱ 
{.upper_case = 0xc69d,    .lower_case = 0xc9b2},  //  Ɲ <> ɲ 
{.upper_case = 0xe2b1a4,    .lower_case = 0xc9bd},  //  Ɽ <> ɽ 
{.upper_case = 0xc6a9,    .lower_case = 0xca83},  //  Ʃ <> ʃ 
{.upper_case = 0xc6ae,    .lower_case = 0xca88},  //  Ʈ <> ʈ 
{.upper_case = 0xc984,    .lower_case = 0xca89},  //  Ʉ <> ʉ 
{.upper_case = 0xc6b1,    .lower_case = 0xca8a},  //  Ʊ <> ʊ 
{.upper_case = 0xc6b2,    .lower_case = 0xca8b},  //  Ʋ <> ʋ 
{.upper_case = 0xc985,    .lower_case = 0xca8c},  //  Ʌ <> ʌ 
{.upper_case = 0xc6b7,    .lower_case = 0xca92},  //  Ʒ <> ʒ 
{.upper_case = 0xe1b4b4,    .lower_case = 0xcab0},  //  ᴴ <> ʰ 
{.upper_case = 0xe1b4b6,    .lower_case = 0xcab2},  //  ᴶ <> ʲ 
{.upper_case = 0xe1b4bf,    .lower_case = 0xcab3},  //  ᴿ <> ʳ 
{.upper_case = 0xe1b582,    .lower_case = 0xcab7},  //  ᵂ <> ʷ 
{.upper_case = 0xe1b4b8,    .lower_case = 0xcba1},  //  ᴸ <> ˡ 
{.upper_case = 0xcdb0,    .lower_case = 0xcdb1},  //  Ͱ <> ͱ 
{.upper_case = 0xcdb2,    .lower_case = 0xcdb3},  //  Ͳ <> ͳ 
{.upper_case = 0xcdb6,    .lower_case = 0xcdb7},  //  Ͷ <> ͷ 
{.upper_case = 0xcfbd,    .lower_case = 0xcdbb},  //  Ͻ <> ͻ 
{.upper_case = 0xcfbe,    .lower_case = 0xcdbc},  //  Ͼ <> ͼ 
{.upper_case = 0xcfbf,    .lower_case = 0xcdbd},  //  Ͽ <> ͽ 
{.upper_case = 0xce86,    .lower_case = 0xceac},  //  Ά <> ά 
{.upper_case = 0xce86,    .lower_case = 0xceac},  //  Ά <> ά 
{.upper_case = 0xce88,    .lower_case = 0xcead},  //  Έ <> έ 
{.upper_case = 0xce88,    .lower_case = 0xcead},  //  Έ <> έ 
{.upper_case = 0xce89,    .lower_case = 0xceae},  //  Ή <> ή 
{.upper_case = 0xce89,    .lower_case = 0xceae},  //  Ή <> ή 
{.upper_case = 0xce8a,    .lower_case = 0xceaf},  //  Ί <> ί 
{.upper_case = 0xce8a,    .lower_case = 0xceaf},  //  Ί <> ί 
{.upper_case = 0xce91,    .lower_case = 0xceb1},  //  Α <> α 
{.upper_case = 0xce92,    .lower_case = 0xceb2},  //  Β <> β 
{.upper_case = 0xce93,    .lower_case = 0xceb3},  //  Γ <> γ 
{.upper_case = 0xce94,    .lower_case = 0xceb4},  //  Δ <> δ 
{.upper_case = 0xce95,    .lower_case = 0xceb5},  //  Ε <> ε 
{.upper_case = 0xce96,    .lower_case = 0xceb6},  //  Ζ <> ζ 
{.upper_case = 0xce97,    .lower_case = 0xceb7},  //  Η <> η 
{.upper_case = 0xce98,    .lower_case = 0xceb8},  //  Θ <> θ 
{.upper_case = 0xce99,    .lower_case = 0xceb9},  //  Ι <> ι 
{.upper_case = 0xce9a,    .lower_case = 0xceba},  //  Κ <> κ 
{.upper_case = 0xce9b,    .lower_case = 0xcebb},  //  Λ <> λ 
{.upper_case = 0xce9c,    .lower_case = 0xcebc},  //  Μ <> μ 
{.upper_case = 0xce9d,    .lower_case = 0xcebd},  //  Ν <> ν 
{.upper_case = 0xce9e,    .lower_case = 0xcebe},  //  Ξ <> ξ 
{.upper_case = 0xce9f,    .lower_case = 0xcebf},  //  Ο <> ο 
{.upper_case = 0xcea0,    .lower_case = 0xcf80},  //  Π <> π 
{.upper_case = 0xcea1,    .lower_case = 0xcf81},  //  Ρ <> ρ 
{.upper_case = 0xcea3,    .lower_case = 0xcf83},  //  Σ <> σ 
{.upper_case = 0xcea4,    .lower_case = 0xcf84},  //  Τ <> τ 
{.upper_case = 0xcea5,    .lower_case = 0xcf85},  //  Υ <> υ 
{.upper_case = 0xcea6,    .lower_case = 0xcf86},  //  Φ <> φ 
{.upper_case = 0xcea7,    .lower_case = 0xcf87},  //  Χ <> χ 
{.upper_case = 0xcea8,    .lower_case = 0xcf88},  //  Ψ <> ψ 
{.upper_case = 0xcea9,    .lower_case = 0xcf89},  //  Ω <> ω 
{.upper_case = 0xceaa,    .lower_case = 0xcf8a},  //  Ϊ <> ϊ 
{.upper_case = 0xceab,    .lower_case = 0xcf8b},  //  Ϋ <> ϋ 
{.upper_case = 0xce8c,    .lower_case = 0xcf8c},  //  Ό <> ό 
{.upper_case = 0xce8c,    .lower_case = 0xcf8c},  //  Ό <> ό 
{.upper_case = 0xce8e,    .lower_case = 0xcf8d},  //  Ύ <> ύ 
{.upper_case = 0xce8e,    .lower_case = 0xcf8d},  //  Ύ <> ύ 
{.upper_case = 0xce8f,    .lower_case = 0xcf8e},  //  Ώ <> ώ 
{.upper_case = 0xce8f,    .lower_case = 0xcf8e},  //  Ώ <> ώ 
{.upper_case = 0xcfa2,    .lower_case = 0xcfa3},  //  Ϣ <> ϣ 
{.upper_case = 0xcfa4,    .lower_case = 0xcfa5},  //  Ϥ <> ϥ 
{.upper_case = 0xcfa6,    .lower_case = 0xcfa7},  //  Ϧ <> ϧ 
{.upper_case = 0xcfa8,    .lower_case = 0xcfa9},  //  Ϩ <> ϩ 
{.upper_case = 0xcfaa,    .lower_case = 0xcfab},  //  Ϫ <> ϫ 
{.upper_case = 0xcfac,    .lower_case = 0xcfad},  //  Ϭ <> ϭ 
{.upper_case = 0xcfae,    .lower_case = 0xcfaf},  //  Ϯ <> ϯ 
{.upper_case = 0xcfb7,    .lower_case = 0xcfb8},  //  Ϸ <> ϸ 
{.upper_case = 0xcfba,    .lower_case = 0xcfbb},  //  Ϻ <> ϻ 
{.upper_case = 0xd090,    .lower_case = 0xd0b0},  //  А <> а 
{.upper_case = 0xd091,    .lower_case = 0xd0b1},  //  Б <> б 
{.upper_case = 0xd092,    .lower_case = 0xd0b2},  //  В <> в 
{.upper_case = 0xd093,    .lower_case = 0xd0b3},  //  Г <> г 
{.upper_case = 0xd094,    .lower_case = 0xd0b4},  //  Д <> д 
{.upper_case = 0xd095,    .lower_case = 0xd0b5},  //  Е <> е 
{.upper_case = 0xd096,    .lower_case = 0xd0b6},  //  Ж <> ж 
{.upper_case = 0xd097,    .lower_case = 0xd0b7},  //  З <> з 
{.upper_case = 0xd098,    .lower_case = 0xd0b8},  //  И <> и 
{.upper_case = 0xd099,    .lower_case = 0xd0b9},  //  Й <> й 
{.upper_case = 0xd09a,    .lower_case = 0xd0ba},  //  К <> к 
{.upper_case = 0xd09b,    .lower_case = 0xd0bb},  //  Л <> л 
{.upper_case = 0xd09c,    .lower_case = 0xd0bc},  //  М <> м 
{.upper_case = 0xd09d,    .lower_case = 0xd0bd},  //  Н <> н 
{.upper_case = 0xd09e,    .lower_case = 0xd0be},  //  О <> о 
{.upper_case = 0xd09f,    .lower_case = 0xd0bf},  //  П <> п 
{.upper_case = 0xd0a0,    .lower_case = 0xd180},  //  Р <> р 
{.upper_case = 0xd0a1,    .lower_case = 0xd181},  //  С <> с 
{.upper_case = 0xd0a2,    .lower_case = 0xd182},  //  Т <> т 
{.upper_case = 0xd0a3,    .lower_case = 0xd183},  //  У <> у 
{.upper_case = 0xd0a4,    .lower_case = 0xd184},  //  Ф <> ф 
{.upper_case = 0xd0a5,    .lower_case = 0xd185},  //  Х <> х 
{.upper_case = 0xd0a6,    .lower_case = 0xd186},  //  Ц <> ц 
{.upper_case = 0xd0a7,    .lower_case = 0xd187},  //  Ч <> ч 
{.upper_case = 0xd0a8,    .lower_case = 0xd188},  //  Ш <> ш 
{.upper_case = 0xd0a9,    .lower_case = 0xd189},  //  Щ <> щ 
{.upper_case = 0xd0aa,    .lower_case = 0xd18a},  //  Ъ <> ъ 
{.upper_case = 0xd0ab,    .lower_case = 0xd18b},  //  Ы <> ы 
{.upper_case = 0xd0ac,    .lower_case = 0xd18c},  //  Ь <> ь 
{.upper_case = 0xd0ad,    .lower_case = 0xd18d},  //  Э <> э 
{.upper_case = 0xd0ae,    .lower_case = 0xd18e},  //  Ю <> ю 
{.upper_case = 0xd0af,    .lower_case = 0xd18f},  //  Я <> я 
{.upper_case = 0xd080,    .lower_case = 0xd190},  //  Ѐ <> ѐ 
{.upper_case = 0xd081,    .lower_case = 0xd191},  //  Ё <> ё 
{.upper_case = 0xd082,    .lower_case = 0xd192},  //  Ђ <> ђ 
{.upper_case = 0xd083,    .lower_case = 0xd193},  //  Ѓ <> ѓ 
{.upper_case = 0xd084,    .lower_case = 0xd194},  //  Є <> є 
{.upper_case = 0xd085,    .lower_case = 0xd195},  //  Ѕ <> ѕ 
{.upper_case = 0xd086,    .lower_case = 0xd196},  //  І <> і 
{.upper_case = 0xd087,    .lower_case = 0xd197},  //  Ї <> ї 
{.upper_case = 0xd088,    .lower_case = 0xd198},  //  Ј <> ј 
{.upper_case = 0xd089,    .lower_case = 0xd199},  //  Љ <> љ 
{.upper_case = 0xd08a,    .lower_case = 0xd19a},  //  Њ <> њ 
{.upper_case = 0xd08b,    .lower_case = 0xd19b},  //  Ћ <> ћ 
{.upper_case = 0xd08c,    .lower_case = 0xd19c},  //  Ќ <> ќ 
{.upper_case = 0xd08d,    .lower_case = 0xd19d},  //  Ѝ <> ѝ 
{.upper_case = 0xd08e,    .lower_case = 0xd19e},  //  Ў <> ў 
{.upper_case = 0xd08f,    .lower_case = 0xd19f},  //  Џ <> џ 
{.upper_case = 0xd1a0,    .lower_case = 0xd1a1},  //  Ѡ <> ѡ 
{.upper_case = 0xd1a2,    .lower_case = 0xd1a3},  //  Ѣ <> ѣ 
{.upper_case = 0xd1a4,    .lower_case = 0xd1a5},  //  Ѥ <> ѥ 
{.upper_case = 0xd1a6,    .lower_case = 0xd1a7},  //  Ѧ <> ѧ 
{.upper_case = 0xd1a8,    .lower_case = 0xd1a9},  //  Ѩ <> ѩ 
{.upper_case = 0xd1aa,    .lower_case = 0xd1ab},  //  Ѫ <> ѫ 
{.upper_case = 0xd1ac,    .lower_case = 0xd1ad},  //  Ѭ <> ѭ 
{.upper_case = 0xd1ae,    .lower_case = 0xd1af},  //  Ѯ <> ѯ 
{.upper_case = 0xd1b0,    .lower_case = 0xd1b1},  //  Ѱ <> ѱ 
{.upper_case = 0xd1b2,    .lower_case = 0xd1b3},  //  Ѳ <> ѳ 
{.upper_case = 0xd1b4,    .lower_case = 0xd1b5},  //  Ѵ <> ѵ 
{.upper_case = 0xd1b6,    .lower_case = 0xd1b7},  //  Ѷ <> ѷ 
{.upper_case = 0xd1b8,    .lower_case = 0xd1b9},  //  Ѹ <> ѹ 
{.upper_case = 0xd1ba,    .lower_case = 0xd1bb},  //  Ѻ <> ѻ 
{.upper_case = 0xd1bc,    .lower_case = 0xd1bd},  //  Ѽ <> ѽ 
{.upper_case = 0xd1be,    .lower_case = 0xd1bf},  //  Ѿ <> ѿ 
{.upper_case = 0xd280,    .lower_case = 0xd281},  //  Ҁ <> ҁ 
{.upper_case = 0xd28a,    .lower_case = 0xd28b},  //  Ҋ <> ҋ 
{.upper_case = 0xd28c,    .lower_case = 0xd28d},  //  Ҍ <> ҍ 
{.upper_case = 0xd28e,    .lower_case = 0xd28f},  //  Ҏ <> ҏ 
{.upper_case = 0xd290,    .lower_case = 0xd291},  //  Ґ <> ґ 
{.upper_case = 0xd292,    .lower_case = 0xd293},  //  Ғ <> ғ 
{.upper_case = 0xd294,    .lower_case = 0xd295},  //  Ҕ <> ҕ 
{.upper_case = 0xd296,    .lower_case = 0xd297},  //  Җ <> җ 
{.upper_case = 0xd298,    .lower_case = 0xd299},  //  Ҙ <> ҙ 
{.upper_case = 0xd29a,    .lower_case = 0xd29b},  //  Қ <> қ 
{.upper_case = 0xd29c,    .lower_case = 0xd29d},  //  Ҝ <> ҝ 
{.upper_case = 0xd29e,    .lower_case = 0xd29f},  //  Ҟ <> ҟ 
{.upper_case = 0xd2a0,    .lower_case = 0xd2a1},  //  Ҡ <> ҡ 
{.upper_case = 0xd2a2,    .lower_case = 0xd2a3},  //  Ң <> ң 
{.upper_case = 0xd2a4,    .lower_case = 0xd2a5},  //  Ҥ <> ҥ 
{.upper_case = 0xd2a6,    .lower_case = 0xd2a7},  //  Ҧ <> ҧ 
{.upper_case = 0xd2a8,    .lower_case = 0xd2a9},  //  Ҩ <> ҩ 
{.upper_case = 0xd2aa,    .lower_case = 0xd2ab},  //  Ҫ <> ҫ 
{.upper_case = 0xd2ac,    .lower_case = 0xd2ad},  //  Ҭ <> ҭ 
{.upper_case = 0xd2ae,    .lower_case = 0xd2af},  //  Ү <> ү 
{.upper_case = 0xd2b0,    .lower_case = 0xd2b1},  //  Ұ <> ұ 
{.upper_case = 0xd2b2,    .lower_case = 0xd2b3},  //  Ҳ <> ҳ 
{.upper_case = 0xd2b4,    .lower_case = 0xd2b5},  //  Ҵ <> ҵ 
{.upper_case = 0xd2b6,    .lower_case = 0xd2b7},  //  Ҷ <> ҷ 
{.upper_case = 0xd2b8,    .lower_case = 0xd2b9},  //  Ҹ <> ҹ 
{.upper_case = 0xd2ba,    .lower_case = 0xd2bb},  //  Һ <> һ 
{.upper_case = 0xd2bc,    .lower_case = 0xd2bd},  //  Ҽ <> ҽ 
{.upper_case = 0xd2be,    .lower_case = 0xd2bf},  //  Ҿ <> ҿ 
{.upper_case = 0xd381,    .lower_case = 0xd382},  //  Ӂ <> ӂ 
{.upper_case = 0xd383,    .lower_case = 0xd384},  //  Ӄ <> ӄ 
{.upper_case = 0xd385,    .lower_case = 0xd386},  //  Ӆ <> ӆ 
{.upper_case = 0xd387,    .lower_case = 0xd388},  //  Ӈ <> ӈ 
{.upper_case = 0xd389,    .lower_case = 0xd38a},  //  Ӊ <> ӊ 
{.upper_case = 0xd38b,    .lower_case = 0xd38c},  //  Ӌ <> ӌ 
{.upper_case = 0xd38d,    .lower_case = 0xd38e},  //  Ӎ <> ӎ 
{.upper_case = 0xd390,    .lower_case = 0xd391},  //  Ӑ <> ӑ 
{.upper_case = 0xd392,    .lower_case = 0xd393},  //  Ӓ <> ӓ 
{.upper_case = 0xd394,    .lower_case = 0xd395},  //  Ӕ <> ӕ 
{.upper_case = 0xd396,    .lower_case = 0xd397},  //  Ӗ <> ӗ 
{.upper_case = 0xd398,    .lower_case = 0xd399},  //  Ә <> ә 
{.upper_case = 0xd39a,    .lower_case = 0xd39b},  //  Ӛ <> ӛ 
{.upper_case = 0xd39c,    .lower_case = 0xd39d},  //  Ӝ <> ӝ 
{.upper_case = 0xd39e,    .lower_case = 0xd39f},  //  Ӟ <> ӟ 
{.upper_case = 0xd3a0,    .lower_case = 0xd3a1},  //  Ӡ <> ӡ 
{.upper_case = 0xd3a2,    .lower_case = 0xd3a3},  //  Ӣ <> ӣ 
{.upper_case = 0xd3a4,    .lower_case = 0xd3a5},  //  Ӥ <> ӥ 
{.upper_case = 0xd3a6,    .lower_case = 0xd3a7},  //  Ӧ <> ӧ 
{.upper_case = 0xd3a8,    .lower_case = 0xd3a9},  //  Ө <> ө 
{.upper_case = 0xd3aa,    .lower_case = 0xd3ab},  //  Ӫ <> ӫ 
{.upper_case = 0xd3ac,    .lower_case = 0xd3ad},  //  Ӭ <> ӭ 
{.upper_case = 0xd3ae,    .lower_case = 0xd3af},  //  Ӯ <> ӯ 
{.upper_case = 0xd3b0,    .lower_case = 0xd3b1},  //  Ӱ <> ӱ 
{.upper_case = 0xd3b2,    .lower_case = 0xd3b3},  //  Ӳ <> ӳ 
{.upper_case = 0xd3b4,    .lower_case = 0xd3b5},  //  Ӵ <> ӵ 
{.upper_case = 0xd3b6,    .lower_case = 0xd3b7},  //  Ӷ <> ӷ 
{.upper_case = 0xd3b8,    .lower_case = 0xd3b9},  //  Ӹ <> ӹ 
{.upper_case = 0xd3ba,    .lower_case = 0xd3bb},  //  Ӻ <> ӻ 
{.upper_case = 0xd3bc,    .lower_case = 0xd3bd},  //  Ӽ <> ӽ 
{.upper_case = 0xd3be,    .lower_case = 0xd3bf},  //  Ӿ <> ӿ 
{.upper_case = 0xd480,    .lower_case = 0xd481},  //  Ԁ <> ԁ 
{.upper_case = 0xd482,    .lower_case = 0xd483},  //  Ԃ <> ԃ 
{.upper_case = 0xd484,    .lower_case = 0xd485},  //  Ԅ <> ԅ 
{.upper_case = 0xd486,    .lower_case = 0xd487},  //  Ԇ <> ԇ 
{.upper_case = 0xd488,    .lower_case = 0xd489},  //  Ԉ <> ԉ 
{.upper_case = 0xd48a,    .lower_case = 0xd48b},  //  Ԋ <> ԋ 
{.upper_case = 0xd48c,    .lower_case = 0xd48d},  //  Ԍ <> ԍ 
{.upper_case = 0xd48e,    .lower_case = 0xd48f},  //  Ԏ <> ԏ 
{.upper_case = 0xd490,    .lower_case = 0xd491},  //  Ԑ <> ԑ 
{.upper_case = 0xd492,    .lower_case = 0xd493},  //  Ԓ <> ԓ 
{.upper_case = 0xd494,    .lower_case = 0xd495},  //  Ԕ <> ԕ 
{.upper_case = 0xd496,    .lower_case = 0xd497},  //  Ԗ <> ԗ 
{.upper_case = 0xd498,    .lower_case = 0xd499},  //  Ԙ <> ԙ 
{.upper_case = 0xd49a,    .lower_case = 0xd49b},  //  Ԛ <> ԛ 
{.upper_case = 0xd49c,    .lower_case = 0xd49d},  //  Ԝ <> ԝ 
{.upper_case = 0xd49e,    .lower_case = 0xd49f},  //  Ԟ <> ԟ 
{.upper_case = 0xd4a0,    .lower_case = 0xd4a1},  //  Ԡ <> ԡ 
{.upper_case = 0xd4a2,    .lower_case = 0xd4a3},  //  Ԣ <> ԣ 
{.upper_case = 0xd4a4,    .lower_case = 0xd4a5},  //  Ԥ <> ԥ 
{.upper_case = 0xd4a6,    .lower_case = 0xd4a7},  //  Ԧ <> ԧ 
{.upper_case = 0xd4b1,    .lower_case = 0xd5a1},  //  Ա <> ա 
{.upper_case = 0xd4b2,    .lower_case = 0xd5a2},  //  Բ <> բ 
{.upper_case = 0xd4b3,    .lower_case = 0xd5a3},  //  Գ <> գ 
{.upper_case = 0xd4b4,    .lower_case = 0xd5a4},  //  Դ <> դ 
{.upper_case = 0xd4b5,    .lower_case = 0xd5a5},  //  Ե <> ե 
{.upper_case = 0xd4b6,    .lower_case = 0xd5a6},  //  Զ <> զ 
{.upper_case = 0xd4b7,    .lower_case = 0xd5a7},  //  Է <> է 
{.upper_case = 0xd4b8,    .lower_case = 0xd5a8},  //  Ը <> ը 
{.upper_case = 0xd4b9,    .lower_case = 0xd5a9},  //  Թ <> թ 
{.upper_case = 0xd4ba,    .lower_case = 0xd5aa},  //  Ժ <> ժ 
{.upper_case = 0xd4bb,    .lower_case = 0xd5ab},  //  Ի <> ի 
{.upper_case = 0xd4bc,    .lower_case = 0xd5ac},  //  Լ <> լ 
{.upper_case = 0xd4bd,    .lower_case = 0xd5ad},  //  Խ <> խ 
{.upper_case = 0xd4be,    .lower_case = 0xd5ae},  //  Ծ <> ծ 
{.upper_case = 0xd4bf,    .lower_case = 0xd5af},  //  Կ <> կ 
{.upper_case = 0xd580,    .lower_case = 0xd5b0},  //  Հ <> հ 
{.upper_case = 0xd581,    .lower_case = 0xd5b1},  //  Ձ <> ձ 
{.upper_case = 0xd582,    .lower_case = 0xd5b2},  //  Ղ <> ղ 
{.upper_case = 0xd583,    .lower_case = 0xd5b3},  //  Ճ <> ճ 
{.upper_case = 0xd584,    .lower_case = 0xd5b4},  //  Մ <> մ 
{.upper_case = 0xd585,    .lower_case = 0xd5b5},  //  Յ <> յ 
{.upper_case = 0xd586,    .lower_case = 0xd5b6},  //  Ն <> ն 
{.upper_case = 0xd587,    .lower_case = 0xd5b7},  //  Շ <> շ 
{.upper_case = 0xd588,    .lower_case = 0xd5b8},  //  Ո <> ո 
{.upper_case = 0xd589,    .lower_case = 0xd5b9},  //  Չ <> չ 
{.upper_case = 0xd58a,    .lower_case = 0xd5ba},  //  Պ <> պ 
{.upper_case = 0xd58b,    .lower_case = 0xd5bb},  //  Ջ <> ջ 
{.upper_case = 0xd58c,    .lower_case = 0xd5bc},  //  Ռ <> ռ 
{.upper_case = 0xd58d,    .lower_case = 0xd5bd},  //  Ս <> ս 
{.upper_case = 0xd58e,    .lower_case = 0xd5be},  //  Վ <> վ 
{.upper_case = 0xd58f,    .lower_case = 0xd5bf},  //  Տ <> տ 
{.upper_case = 0xd590,    .lower_case = 0xd680},  //  Ր <> ր 
{.upper_case = 0xd591,    .lower_case = 0xd681},  //  Ց <> ց 
{.upper_case = 0xd592,    .lower_case = 0xd682},  //  Ւ <> ւ 
{.upper_case = 0xd593,    .lower_case = 0xd683},  //  Փ <> փ 
{.upper_case = 0xd594,    .lower_case = 0xd684},  //  Ք <> ք 
{.upper_case = 0xd595,    .lower_case = 0xd685},  //  Օ <> օ 
{.upper_case = 0xd596,    .lower_case = 0xd686},  //  Ֆ <> ֆ 
{.upper_case = 0xe1b4ac,    .lower_case = 0xe1b583},  //  ᴬ <> ᵃ 
{.upper_case = 0xe1b4ae,    .lower_case = 0xe1b587},  //  ᴮ <> ᵇ 
{.upper_case = 0xe1b4b0,    .lower_case = 0xe1b588},  //  ᴰ <> ᵈ 
{.upper_case = 0xe1b4b1,    .lower_case = 0xe1b589},  //  ᴱ <> ᵉ 
{.upper_case = 0xe1b4b3,    .lower_case = 0xe1b58d},  //  ᴳ <> ᵍ 
{.upper_case = 0xe1b4b7,    .lower_case = 0xe1b58f},  //  ᴷ <> ᵏ 
{.upper_case = 0xe1b4b9,    .lower_case = 0xe1b590},  //  ᴹ <> ᵐ 
{.upper_case = 0xe1b4bc,    .lower_case = 0xe1b592},  //  ᴼ <> ᵒ 
{.upper_case = 0xe1b4be,    .lower_case = 0xe1b596},  //  ᴾ <> ᵖ 
{.upper_case = 0xe1b580,    .lower_case = 0xe1b597},  //  ᵀ <> ᵗ 
{.upper_case = 0xe1b581,    .lower_case = 0xe1b598},  //  ᵁ <> ᵘ 
{.upper_case = 0xe2b1bd,    .lower_case = 0xe1b59b},  //  ⱽ <> ᵛ 
{.upper_case = 0xea9dbd,    .lower_case = 0xe1b5b9},  //  Ᵹ <> ᵹ 
{.upper_case = 0xe2b1a3,    .lower_case = 0xe1b5bd},  //  Ᵽ <> ᵽ 
{.upper_case = 0xe1b880,    .lower_case = 0xe1b881},  //  Ḁ <> ḁ 
{.upper_case = 0xe1b882,    .lower_case = 0xe1b883},  //  Ḃ <> ḃ 
{.upper_case = 0xe1b884,    .lower_case = 0xe1b885},  //  Ḅ <> ḅ 
{.upper_case = 0xe1b886,    .lower_case = 0xe1b887},  //  Ḇ <> ḇ 
{.upper_case = 0xe1b888,    .lower_case = 0xe1b889},  //  Ḉ <> ḉ 
{.upper_case = 0xe1b88a,    .lower_case = 0xe1b88b},  //  Ḋ <> ḋ 
{.upper_case = 0xe1b88c,    .lower_case = 0xe1b88d},  //  Ḍ <> ḍ 
{.upper_case = 0xe1b88e,    .lower_case = 0xe1b88f},  //  Ḏ <> ḏ 
{.upper_case = 0xe1b890,    .lower_case = 0xe1b891},  //  Ḑ <> ḑ 
{.upper_case = 0xe1b892,    .lower_case = 0xe1b893},  //  Ḓ <> ḓ 
{.upper_case = 0xe1b894,    .lower_case = 0xe1b895},  //  Ḕ <> ḕ 
{.upper_case = 0xe1b896,    .lower_case = 0xe1b897},  //  Ḗ <> ḗ 
{.upper_case = 0xe1b898,    .lower_case = 0xe1b899},  //  Ḙ <> ḙ 
{.upper_case = 0xe1b89a,    .lower_case = 0xe1b89b},  //  Ḛ <> ḛ 
{.upper_case = 0xe1b89c,    .lower_case = 0xe1b89d},  //  Ḝ <> ḝ 
{.upper_case = 0xe1b89e,    .lower_case = 0xe1b89f},  //  Ḟ <> ḟ 
{.upper_case = 0xe1b8a0,    .lower_case = 0xe1b8a1},  //  Ḡ <> ḡ 
{.upper_case = 0xe1b8a2,    .lower_case = 0xe1b8a3},  //  Ḣ <> ḣ 
{.upper_case = 0xe1b8a4,    .lower_case = 0xe1b8a5},  //  Ḥ <> ḥ 
{.upper_case = 0xe1b8a6,    .lower_case = 0xe1b8a7},  //  Ḧ <> ḧ 
{.upper_case = 0xe1b8a8,    .lower_case = 0xe1b8a9},  //  Ḩ <> ḩ 
{.upper_case = 0xe1b8aa,    .lower_case = 0xe1b8ab},  //  Ḫ <> ḫ 
{.upper_case = 0xe1b8ac,    .lower_case = 0xe1b8ad},  //  Ḭ <> ḭ 
{.upper_case = 0xe1b8ae,    .lower_case = 0xe1b8af},  //  Ḯ <> ḯ 
{.upper_case = 0xe1b8b0,    .lower_case = 0xe1b8b1},  //  Ḱ <> ḱ 
{.upper_case = 0xe1b8b2,    .lower_case = 0xe1b8b3},  //  Ḳ <> ḳ 
{.upper_case = 0xe1b8b4,    .lower_case = 0xe1b8b5},  //  Ḵ <> ḵ 
{.upper_case = 0xe1b8b6,    .lower_case = 0xe1b8b7},  //  Ḷ <> ḷ 
{.upper_case = 0xe1b8b8,    .lower_case = 0xe1b8b9},  //  Ḹ <> ḹ 
{.upper_case = 0xe1b8ba,    .lower_case = 0xe1b8bb},  //  Ḻ <> ḻ 
{.upper_case = 0xe1b8bc,    .lower_case = 0xe1b8bd},  //  Ḽ <> ḽ 
{.upper_case = 0xe1b8be,    .lower_case = 0xe1b8bf},  //  Ḿ <> ḿ 
{.upper_case = 0xe1b980,    .lower_case = 0xe1b981},  //  Ṁ <> ṁ 
{.upper_case = 0xe1b982,    .lower_case = 0xe1b983},  //  Ṃ <> ṃ 
{.upper_case = 0xe1b984,    .lower_case = 0xe1b985},  //  Ṅ <> ṅ 
{.upper_case = 0xe1b986,    .lower_case = 0xe1b987},  //  Ṇ <> ṇ 
{.upper_case = 0xe1b988,    .lower_case = 0xe1b989},  //  Ṉ <> ṉ 
{.upper_case = 0xe1b98a,    .lower_case = 0xe1b98b},  //  Ṋ <> ṋ 
{.upper_case = 0xe1b98c,    .lower_case = 0xe1b98d},  //  Ṍ <> ṍ 
{.upper_case = 0xe1b98e,    .lower_case = 0xe1b98f},  //  Ṏ <> ṏ 
{.upper_case = 0xe1b990,    .lower_case = 0xe1b991},  //  Ṑ <> ṑ 
{.upper_case = 0xe1b992,    .lower_case = 0xe1b993},  //  Ṓ <> ṓ 
{.upper_case = 0xe1b994,    .lower_case = 0xe1b995},  //  Ṕ <> ṕ 
{.upper_case = 0xe1b996,    .lower_case = 0xe1b997},  //  Ṗ <> ṗ 
{.upper_case = 0xe1b998,    .lower_case = 0xe1b999},  //  Ṙ <> ṙ 
{.upper_case = 0xe1b99a,    .lower_case = 0xe1b99b},  //  Ṛ <> ṛ 
{.upper_case = 0xe1b99c,    .lower_case = 0xe1b99d},  //  Ṝ <> ṝ 
{.upper_case = 0xe1b99e,    .lower_case = 0xe1b99f},  //  Ṟ <> ṟ 
{.upper_case = 0xe1b9a0,    .lower_case = 0xe1b9a1},  //  Ṡ <> ṡ 
{.upper_case = 0xe1b9a2,    .lower_case = 0xe1b9a3},  //  Ṣ <> ṣ 
{.upper_case = 0xe1b9a4,    .lower_case = 0xe1b9a5},  //  Ṥ <> ṥ 
{.upper_case = 0xe1b9a6,    .lower_case = 0xe1b9a7},  //  Ṧ <> ṧ 
{.upper_case = 0xe1b9a8,    .lower_case = 0xe1b9a9},  //  Ṩ <> ṩ 
{.upper_case = 0xe1b9aa,    .lower_case = 0xe1b9ab},  //  Ṫ <> ṫ 
{.upper_case = 0xe1b9ac,    .lower_case = 0xe1b9ad},  //  Ṭ <> ṭ 
{.upper_case = 0xe1b9ae,    .lower_case = 0xe1b9af},  //  Ṯ <> ṯ 
{.upper_case = 0xe1b9b0,    .lower_case = 0xe1b9b1},  //  Ṱ <> ṱ 
{.upper_case = 0xe1b9b2,    .lower_case = 0xe1b9b3},  //  Ṳ <> ṳ 
{.upper_case = 0xe1b9b4,    .lower_case = 0xe1b9b5},  //  Ṵ <> ṵ 
{.upper_case = 0xe1b9b6,    .lower_case = 0xe1b9b7},  //  Ṷ <> ṷ 
{.upper_case = 0xe1b9b8,    .lower_case = 0xe1b9b9},  //  Ṹ <> ṹ 
{.upper_case = 0xe1b9ba,    .lower_case = 0xe1b9bb},  //  Ṻ <> ṻ 
{.upper_case = 0xe1b9bc,    .lower_case = 0xe1b9bd},  //  Ṽ <> ṽ 
{.upper_case = 0xe1b9be,    .lower_case = 0xe1b9bf},  //  Ṿ <> ṿ 
{.upper_case = 0xe1ba80,    .lower_case = 0xe1ba81},  //  Ẁ <> ẁ 
{.upper_case = 0xe1ba82,    .lower_case = 0xe1ba83},  //  Ẃ <> ẃ 
{.upper_case = 0xe1ba84,    .lower_case = 0xe1ba85},  //  Ẅ <> ẅ 
{.upper_case = 0xe1ba86,    .lower_case = 0xe1ba87},  //  Ẇ <> ẇ 
{.upper_case = 0xe1ba88,    .lower_case = 0xe1ba89},  //  Ẉ <> ẉ 
{.upper_case = 0xe1ba8a,    .lower_case = 0xe1ba8b},  //  Ẋ <> ẋ 
{.upper_case = 0xe1ba8c,    .lower_case = 0xe1ba8d},  //  Ẍ <> ẍ 
{.upper_case = 0xe1ba8e,    .lower_case = 0xe1ba8f},  //  Ẏ <> ẏ 
{.upper_case = 0xe1ba90,    .lower_case = 0xe1ba91},  //  Ẑ <> ẑ 
{.upper_case = 0xe1ba92,    .lower_case = 0xe1ba93},  //  Ẓ <> ẓ 
{.upper_case = 0xe1ba94,    .lower_case = 0xe1ba95},  //  Ẕ <> ẕ 
{.upper_case = 0xe1baa0,    .lower_case = 0xe1baa1},  //  Ạ <> ạ 
{.upper_case = 0xe1baa2,    .lower_case = 0xe1baa3},  //  Ả <> ả 
{.upper_case = 0xe1baa4,    .lower_case = 0xe1baa5},  //  Ấ <> ấ 
{.upper_case = 0xe1baa6,    .lower_case = 0xe1baa7},  //  Ầ <> ầ 
{.upper_case = 0xe1baa8,    .lower_case = 0xe1baa9},  //  Ẩ <> ẩ 
{.upper_case = 0xe1baaa,    .lower_case = 0xe1baab},  //  Ẫ <> ẫ 
{.upper_case = 0xe1baac,    .lower_case = 0xe1baad},  //  Ậ <> ậ 
{.upper_case = 0xe1baae,    .lower_case = 0xe1baaf},  //  Ắ <> ắ 
{.upper_case = 0xe1bab0,    .lower_case = 0xe1bab1},  //  Ằ <> ằ 
{.upper_case = 0xe1bab2,    .lower_case = 0xe1bab3},  //  Ẳ <> ẳ 
{.upper_case = 0xe1bab4,    .lower_case = 0xe1bab5},  //  Ẵ <> ẵ 
{.upper_case = 0xe1bab6,    .lower_case = 0xe1bab7},  //  Ặ <> ặ 
{.upper_case = 0xe1bab8,    .lower_case = 0xe1bab9},  //  Ẹ <> ẹ 
{.upper_case = 0xe1baba,    .lower_case = 0xe1babb},  //  Ẻ <> ẻ 
{.upper_case = 0xe1babc,    .lower_case = 0xe1babd},  //  Ẽ <> ẽ 
{.upper_case = 0xe1babe,    .lower_case = 0xe1babf},  //  Ế <> ế 
{.upper_case = 0xe1bb80,    .lower_case = 0xe1bb81},  //  Ề <> ề 
{.upper_case = 0xe1bb82,    .lower_case = 0xe1bb83},  //  Ể <> ể 
{.upper_case = 0xe1bb84,    .lower_case = 0xe1bb85},  //  Ễ <> ễ 
{.upper_case = 0xe1bb86,    .lower_case = 0xe1bb87},  //  Ệ <> ệ 
{.upper_case = 0xe1bb88,    .lower_case = 0xe1bb89},  //  Ỉ <> ỉ 
{.upper_case = 0xe1bb8a,    .lower_case = 0xe1bb8b},  //  Ị <> ị 
{.upper_case = 0xe1bb8c,    .lower_case = 0xe1bb8d},  //  Ọ <> ọ 
{.upper_case = 0xe1bb8e,    .lower_case = 0xe1bb8f},  //  Ỏ <> ỏ 
{.upper_case = 0xe1bb90,    .lower_case = 0xe1bb91},  //  Ố <> ố 
{.upper_case = 0xe1bb92,    .lower_case = 0xe1bb93},  //  Ồ <> ồ 
{.upper_case = 0xe1bb94,    .lower_case = 0xe1bb95},  //  Ổ <> ổ 
{.upper_case = 0xe1bb96,    .lower_case = 0xe1bb97},  //  Ỗ <> ỗ 
{.upper_case = 0xe1bb98,    .lower_case = 0xe1bb99},  //  Ộ <> ộ 
{.upper_case = 0xe1bb9a,    .lower_case = 0xe1bb9b},  //  Ớ <> ớ 
{.upper_case = 0xe1bb9c,    .lower_case = 0xe1bb9d},  //  Ờ <> ờ 
{.upper_case = 0xe1bb9e,    .lower_case = 0xe1bb9f},  //  Ở <> ở 
{.upper_case = 0xe1bba0,    .lower_case = 0xe1bba1},  //  Ỡ <> ỡ 
{.upper_case = 0xe1bba2,    .lower_case = 0xe1bba3},  //  Ợ <> ợ 
{.upper_case = 0xe1bba4,    .lower_case = 0xe1bba5},  //  Ụ <> ụ 
{.upper_case = 0xe1bba6,    .lower_case = 0xe1bba7},  //  Ủ <> ủ 
{.upper_case = 0xe1bba8,    .lower_case = 0xe1bba9},  //  Ứ <> ứ 
{.upper_case = 0xe1bbaa,    .lower_case = 0xe1bbab},  //  Ừ <> ừ 
{.upper_case = 0xe1bbac,    .lower_case = 0xe1bbad},  //  Ử <> ử 
{.upper_case = 0xe1bbae,    .lower_case = 0xe1bbaf},  //  Ữ <> ữ 
{.upper_case = 0xe1bbb0,    .lower_case = 0xe1bbb1},  //  Ự <> ự 
{.upper_case = 0xe1bbb2,    .lower_case = 0xe1bbb3},  //  Ỳ <> ỳ 
{.upper_case = 0xe1bbb4,    .lower_case = 0xe1bbb5},  //  Ỵ <> ỵ 
{.upper_case = 0xe1bbb6,    .lower_case = 0xe1bbb7},  //  Ỷ <> ỷ 
{.upper_case = 0xe1bbb8,    .lower_case = 0xe1bbb9},  //  Ỹ <> ỹ 
{.upper_case = 0xe1bbba,    .lower_case = 0xe1bbbb},  //  Ỻ <> ỻ 
{.upper_case = 0xe1bbbc,    .lower_case = 0xe1bbbd},  //  Ỽ <> ỽ 
{.upper_case = 0xe1bbbe,    .lower_case = 0xe1bbbf},  //  Ỿ <> ỿ 
{.upper_case = 0xe1bc88,    .lower_case = 0xe1bc80},  //  Ἀ <> ἀ 
{.upper_case = 0xe1bc89,    .lower_case = 0xe1bc81},  //  Ἁ <> ἁ 
{.upper_case = 0xe1bc8a,    .lower_case = 0xe1bc82},  //  Ἂ <> ἂ 
{.upper_case = 0xe1bc8b,    .lower_case = 0xe1bc83},  //  Ἃ <> ἃ 
{.upper_case = 0xe1bc8c,    .lower_case = 0xe1bc84},  //  Ἄ <> ἄ 
{.upper_case = 0xe1bc8d,    .lower_case = 0xe1bc85},  //  Ἅ <> ἅ 
{.upper_case = 0xe1bc8e,    .lower_case = 0xe1bc86},  //  Ἆ <> ἆ 
{.upper_case = 0xe1bc8f,    .lower_case = 0xe1bc87},  //  Ἇ <> ἇ 
{.upper_case = 0xe1bc98,    .lower_case = 0xe1bc90},  //  Ἐ <> ἐ 
{.upper_case = 0xe1bc99,    .lower_case = 0xe1bc91},  //  Ἑ <> ἑ 
{.upper_case = 0xe1bc9a,    .lower_case = 0xe1bc92},  //  Ἒ <> ἒ 
{.upper_case = 0xe1bc9b,    .lower_case = 0xe1bc93},  //  Ἓ <> ἓ 
{.upper_case = 0xe1bc9c,    .lower_case = 0xe1bc94},  //  Ἔ <> ἔ 
{.upper_case = 0xe1bc9d,    .lower_case = 0xe1bc95},  //  Ἕ <> ἕ 
{.upper_case = 0xe1bca8,    .lower_case = 0xe1bca0},  //  Ἠ <> ἠ 
{.upper_case = 0xe1bca9,    .lower_case = 0xe1bca1},  //  Ἡ <> ἡ 
{.upper_case = 0xe1bcaa,    .lower_case = 0xe1bca2},  //  Ἢ <> ἢ 
{.upper_case = 0xe1bcab,    .lower_case = 0xe1bca3},  //  Ἣ <> ἣ 
{.upper_case = 0xe1bcac,    .lower_case = 0xe1bca4},  //  Ἤ <> ἤ 
{.upper_case = 0xe1bcad,    .lower_case = 0xe1bca5},  //  Ἥ <> ἥ 
{.upper_case = 0xe1bcae,    .lower_case = 0xe1bca6},  //  Ἦ <> ἦ 
{.upper_case = 0xe1bcaf,    .lower_case = 0xe1bca7},  //  Ἧ <> ἧ 
{.upper_case = 0xe1bcb8,    .lower_case = 0xe1bcb0},  //  Ἰ <> ἰ 
{.upper_case = 0xe1bcb9,    .lower_case = 0xe1bcb1},  //  Ἱ <> ἱ 
{.upper_case = 0xe1bcba,    .lower_case = 0xe1bcb2},  //  Ἲ <> ἲ 
{.upper_case = 0xe1bcbb,    .lower_case = 0xe1bcb3},  //  Ἳ <> ἳ 
{.upper_case = 0xe1bcbc,    .lower_case = 0xe1bcb4},  //  Ἴ <> ἴ 
{.upper_case = 0xe1bcbd,    .lower_case = 0xe1bcb5},  //  Ἵ <> ἵ 
{.upper_case = 0xe1bcbe,    .lower_case = 0xe1bcb6},  //  Ἶ <> ἶ 
{.upper_case = 0xe1bcbf,    .lower_case = 0xe1bcb7},  //  Ἷ <> ἷ 
{.upper_case = 0xe1bd88,    .lower_case = 0xe1bd80},  //  Ὀ <> ὀ 
{.upper_case = 0xe1bd89,    .lower_case = 0xe1bd81},  //  Ὁ <> ὁ 
{.upper_case = 0xe1bd8a,    .lower_case = 0xe1bd82},  //  Ὂ <> ὂ 
{.upper_case = 0xe1bd8b,    .lower_case = 0xe1bd83},  //  Ὃ <> ὃ 
{.upper_case = 0xe1bd8c,    .lower_case = 0xe1bd84},  //  Ὄ <> ὄ 
{.upper_case = 0xe1bd8d,    .lower_case = 0xe1bd85},  //  Ὅ <> ὅ 
{.upper_case = 0xe1bd99,    .lower_case = 0xe1bd91},  //  Ὑ <> ὑ 
{.upper_case = 0xe1bd9b,    .lower_case = 0xe1bd93},  //  Ὓ <> ὓ 
{.upper_case = 0xe1bd9d,    .lower_case = 0xe1bd95},  //  Ὕ <> ὕ 
{.upper_case = 0xe1bd9f,    .lower_case = 0xe1bd97},  //  Ὗ <> ὗ 
{.upper_case = 0xe1bda8,    .lower_case = 0xe1bda0},  //  Ὠ <> ὠ 
{.upper_case = 0xe1bda9,    .lower_case = 0xe1bda1},  //  Ὡ <> ὡ 
{.upper_case = 0xe1bdaa,    .lower_case = 0xe1bda2},  //  Ὢ <> ὢ 
{.upper_case = 0xe1bdab,    .lower_case = 0xe1bda3},  //  Ὣ <> ὣ 
{.upper_case = 0xe1bdac,    .lower_case = 0xe1bda4},  //  Ὤ <> ὤ 
{.upper_case = 0xe1bdad,    .lower_case = 0xe1bda5},  //  Ὥ <> ὥ 
{.upper_case = 0xe1bdae,    .lower_case = 0xe1bda6},  //  Ὦ <> ὦ 
{.upper_case = 0xe1bdaf,    .lower_case = 0xe1bda7},  //  Ὧ <> ὧ 
{.upper_case = 0xe1beba,    .lower_case = 0xe1bdb0},  //  Ὰ <> ὰ 
{.upper_case = 0xe1bf88,    .lower_case = 0xe1bdb2},  //  Ὲ <> ὲ 
{.upper_case = 0xe1bf8a,    .lower_case = 0xe1bdb4},  //  Ὴ <> ὴ 
{.upper_case = 0xe1bf9a,    .lower_case = 0xe1bdb6},  //  Ὶ <> ὶ 
{.upper_case = 0xe1bfb8,    .lower_case = 0xe1bdb8},  //  Ὸ <> ὸ 
{.upper_case = 0xe1bfaa,    .lower_case = 0xe1bdba},  //  Ὺ <> ὺ 
{.upper_case = 0xe1bfba,    .lower_case = 0xe1bdbc},  //  Ὼ <> ὼ 
{.upper_case = 0xe1beb8,    .lower_case = 0xe1beb0},  //  Ᾰ <> ᾰ 
{.upper_case = 0xe1beb9,    .lower_case = 0xe1beb1},  //  Ᾱ <> ᾱ 
{.upper_case = 0xe1bf98,    .lower_case = 0xe1bf90},  //  Ῐ <> ῐ 
{.upper_case = 0xe1bf99,    .lower_case = 0xe1bf91},  //  Ῑ <> ῑ 
{.upper_case = 0xe1bfa8,    .lower_case = 0xe1bfa0},  //  Ῠ <> ῠ 
{.upper_case = 0xe1bfa9,    .lower_case = 0xe1bfa1},  //  Ῡ <> ῡ 
{.upper_case = 0xe1bfac,    .lower_case = 0xe1bfa5},  //  Ῥ <> ῥ 
{.upper_case = 0xe28492,    .lower_case = 0xe28493},  //  ℒ <> ℓ 
{.upper_case = 0xe284b0,    .lower_case = 0xe284af},  //  ℰ <> ℯ 
{.upper_case = 0xe284bf,    .lower_case = 0xe284bc},  //  ℿ <> ℼ 
{.upper_case = 0xe284be,    .lower_case = 0xe284bd},  //  ℾ <> ℽ 
{.upper_case = 0xe28585,    .lower_case = 0xe28586},  //  ⅅ <> ⅆ 
{.upper_case = 0xe284b2,    .lower_case = 0xe2858e},  //  Ⅎ <> ⅎ 
{.upper_case = 0xf09f8490,    .lower_case = 0xe2929c},  //  🄐 <> ⒜ 
{.upper_case = 0xf09f8491,    .lower_case = 0xe2929d},  //  🄑 <> ⒝ 
{.upper_case = 0xf09f8492,    .lower_case = 0xe2929e},  //  🄒 <> ⒞ 
{.upper_case = 0xf09f8493,    .lower_case = 0xe2929f},  //  🄓 <> ⒟ 
{.upper_case = 0xf09f8494,    .lower_case = 0xe292a0},  //  🄔 <> ⒠ 
{.upper_case = 0xf09f8495,    .lower_case = 0xe292a1},  //  🄕 <> ⒡ 
{.upper_case = 0xf09f8496,    .lower_case = 0xe292a2},  //  🄖 <> ⒢ 
{.upper_case = 0xf09f8497,    .lower_case = 0xe292a3},  //  🄗 <> ⒣ 
{.upper_case = 0xf09f8498,    .lower_case = 0xe292a4},  //  🄘 <> ⒤ 
{.upper_case = 0xf09f8499,    .lower_case = 0xe292a5},  //  🄙 <> ⒥ 
{.upper_case = 0xf09f849a,    .lower_case = 0xe292a6},  //  🄚 <> ⒦ 
{.upper_case = 0xf09f849b,    .lower_case = 0xe292a7},  //  🄛 <> ⒧ 
{.upper_case = 0xf09f849c,    .lower_case = 0xe292a8},  //  🄜 <> ⒨ 
{.upper_case = 0xf09f849d,    .lower_case = 0xe292a9},  //  🄝 <> ⒩ 
{.upper_case = 0xf09f849e,    .lower_case = 0xe292aa},  //  🄞 <> ⒪ 
{.upper_case = 0xf09f849f,    .lower_case = 0xe292ab},  //  🄟 <> ⒫ 
{.upper_case = 0xf09f84a0,    .lower_case = 0xe292ac},  //  🄠 <> ⒬ 
{.upper_case = 0xf09f84a1,    .lower_case = 0xe292ad},  //  🄡 <> ⒭ 
{.upper_case = 0xf09f84a2,    .lower_case = 0xe292ae},  //  🄢 <> ⒮ 
{.upper_case = 0xf09f84a3,    .lower_case = 0xe292af},  //  🄣 <> ⒯ 
{.upper_case = 0xf09f84a4,    .lower_case = 0xe292b0},  //  🄤 <> ⒰ 
{.upper_case = 0xf09f84a5,    .lower_case = 0xe292b1},  //  🄥 <> ⒱ 
{.upper_case = 0xf09f84a6,    .lower_case = 0xe292b2},  //  🄦 <> ⒲ 
{.upper_case = 0xf09f84a7,    .lower_case = 0xe292b3},  //  🄧 <> ⒳ 
{.upper_case = 0xf09f84a8,    .lower_case = 0xe292b4},  //  🄨 <> ⒴ 
{.upper_case = 0xf09f84a9,    .lower_case = 0xe292b5},  //  🄩 <> ⒵ 
{.upper_case = 0xe292b6,    .lower_case = 0xe29390},  //  Ⓐ <> ⓐ 
{.upper_case = 0xe292b7,    .lower_case = 0xe29391},  //  Ⓑ <> ⓑ 
{.upper_case = 0xe292b8,    .lower_case = 0xe29392},  //  Ⓒ <> ⓒ 
{.upper_case = 0xe292b9,    .lower_case = 0xe29393},  //  Ⓓ <> ⓓ 
{.upper_case = 0xe292ba,    .lower_case = 0xe29394},  //  Ⓔ <> ⓔ 
{.upper_case = 0xe292bb,    .lower_case = 0xe29395},  //  Ⓕ <> ⓕ 
{.upper_case = 0xe292bc,    .lower_case = 0xe29396},  //  Ⓖ <> ⓖ 
{.upper_case = 0xe292bd,    .lower_case = 0xe29397},  //  Ⓗ <> ⓗ 
{.upper_case = 0xe292be,    .lower_case = 0xe29398},  //  Ⓘ <> ⓘ 
{.upper_case = 0xe292bf,    .lower_case = 0xe29399},  //  Ⓙ <> ⓙ 
{.upper_case = 0xe29380,    .lower_case = 0xe2939a},  //  Ⓚ <> ⓚ 
{.upper_case = 0xe29381,    .lower_case = 0xe2939b},  //  Ⓛ <> ⓛ 
{.upper_case = 0xe29382,    .lower_case = 0xe2939c},  //  Ⓜ <> ⓜ 
{.upper_case = 0xe29383,    .lower_case = 0xe2939d},  //  Ⓝ <> ⓝ 
{.upper_case = 0xe29384,    .lower_case = 0xe2939e},  //  Ⓞ <> ⓞ 
{.upper_case = 0xe29385,    .lower_case = 0xe2939f},  //  Ⓟ <> ⓟ 
{.upper_case = 0xe29386,    .lower_case = 0xe293a0},  //  Ⓠ <> ⓠ 
{.upper_case = 0xe29387,    .lower_case = 0xe293a1},  //  Ⓡ <> ⓡ 
{.upper_case = 0xe29388,    .lower_case = 0xe293a2},  //  Ⓢ <> ⓢ 
{.upper_case = 0xe29389,    .lower_case = 0xe293a3},  //  Ⓣ <> ⓣ 
{.upper_case = 0xe2938a,    .lower_case = 0xe293a4},  //  Ⓤ <> ⓤ 
{.upper_case = 0xe2938b,    .lower_case = 0xe293a5},  //  Ⓥ <> ⓥ 
{.upper_case = 0xe2938c,    .lower_case = 0xe293a6},  //  Ⓦ <> ⓦ 
{.upper_case = 0xe2938d,    .lower_case = 0xe293a7},  //  Ⓧ <> ⓧ 
{.upper_case = 0xe2938e,    .lower_case = 0xe293a8},  //  Ⓨ <> ⓨ 
{.upper_case = 0xe2938f,    .lower_case = 0xe293a9},  //  Ⓩ <> ⓩ 
{.upper_case = 0xe2b080,    .lower_case = 0xe2b0b0},  //  Ⰰ <> ⰰ 
{.upper_case = 0xe2b081,    .lower_case = 0xe2b0b1},  //  Ⰱ <> ⰱ 
{.upper_case = 0xe2b082,    .lower_case = 0xe2b0b2},  //  Ⰲ <> ⰲ 
{.upper_case = 0xe2b083,    .lower_case = 0xe2b0b3},  //  Ⰳ <> ⰳ 
{.upper_case = 0xe2b084,    .lower_case = 0xe2b0b4},  //  Ⰴ <> ⰴ 
{.upper_case = 0xe2b085,    .lower_case = 0xe2b0b5},  //  Ⰵ <> ⰵ 
{.upper_case = 0xe2b086,    .lower_case = 0xe2b0b6},  //  Ⰶ <> ⰶ 
{.upper_case = 0xe2b087,    .lower_case = 0xe2b0b7},  //  Ⰷ <> ⰷ 
{.upper_case = 0xe2b088,    .lower_case = 0xe2b0b8},  //  Ⰸ <> ⰸ 
{.upper_case = 0xe2b089,    .lower_case = 0xe2b0b9},  //  Ⰹ <> ⰹ 
{.upper_case = 0xe2b08a,    .lower_case = 0xe2b0ba},  //  Ⰺ <> ⰺ 
{.upper_case = 0xe2b08b,    .lower_case = 0xe2b0bb},  //  Ⰻ <> ⰻ 
{.upper_case = 0xe2b08c,    .lower_case = 0xe2b0bc},  //  Ⰼ <> ⰼ 
{.upper_case = 0xe2b08d,    .lower_case = 0xe2b0bd},  //  Ⰽ <> ⰽ 
{.upper_case = 0xe2b08e,    .lower_case = 0xe2b0be},  //  Ⰾ <> ⰾ 
{.upper_case = 0xe2b08f,    .lower_case = 0xe2b0bf},  //  Ⰿ <> ⰿ 
{.upper_case = 0xe2b090,    .lower_case = 0xe2b180},  //  Ⱀ <> ⱀ 
{.upper_case = 0xe2b091,    .lower_case = 0xe2b181},  //  Ⱁ <> ⱁ 
{.upper_case = 0xe2b092,    .lower_case = 0xe2b182},  //  Ⱂ <> ⱂ 
{.upper_case = 0xe2b093,    .lower_case = 0xe2b183},  //  Ⱃ <> ⱃ 
{.upper_case = 0xe2b094,    .lower_case = 0xe2b184},  //  Ⱄ <> ⱄ 
{.upper_case = 0xe2b095,    .lower_case = 0xe2b185},  //  Ⱅ <> ⱅ 
{.upper_case = 0xe2b096,    .lower_case = 0xe2b186},  //  Ⱆ <> ⱆ 
{.upper_case = 0xe2b097,    .lower_case = 0xe2b187},  //  Ⱇ <> ⱇ 
{.upper_case = 0xe2b098,    .lower_case = 0xe2b188},  //  Ⱈ <> ⱈ 
{.upper_case = 0xe2b099,    .lower_case = 0xe2b189},  //  Ⱉ <> ⱉ 
{.upper_case = 0xe2b09a,    .lower_case = 0xe2b18a},  //  Ⱊ <> ⱊ 
{.upper_case = 0xe2b09b,    .lower_case = 0xe2b18b},  //  Ⱋ <> ⱋ 
{.upper_case = 0xe2b09c,    .lower_case = 0xe2b18c},  //  Ⱌ <> ⱌ 
{.upper_case = 0xe2b09d,    .lower_case = 0xe2b18d},  //  Ⱍ <> ⱍ 
{.upper_case = 0xe2b09e,    .lower_case = 0xe2b18e},  //  Ⱎ <> ⱎ 
{.upper_case = 0xe2b09f,    .lower_case = 0xe2b18f},  //  Ⱏ <> ⱏ 
{.upper_case = 0xe2b0a0,    .lower_case = 0xe2b190},  //  Ⱐ <> ⱐ 
{.upper_case = 0xe2b0a1,    .lower_case = 0xe2b191},  //  Ⱑ <> ⱑ 
{.upper_case = 0xe2b0a2,    .lower_case = 0xe2b192},  //  Ⱒ <> ⱒ 
{.upper_case = 0xe2b0a3,    .lower_case = 0xe2b193},  //  Ⱓ <> ⱓ 
{.upper_case = 0xe2b0a4,    .lower_case = 0xe2b194},  //  Ⱔ <> ⱔ 
{.upper_case = 0xe2b0a5,    .lower_case = 0xe2b195},  //  Ⱕ <> ⱕ 
{.upper_case = 0xe2b0a6,    .lower_case = 0xe2b196},  //  Ⱖ <> ⱖ 
{.upper_case = 0xe2b0a7,    .lower_case = 0xe2b197},  //  Ⱗ <> ⱗ 
{.upper_case = 0xe2b0a8,    .lower_case = 0xe2b198},  //  Ⱘ <> ⱘ 
{.upper_case = 0xe2b0a9,    .lower_case = 0xe2b199},  //  Ⱙ <> ⱙ 
{.upper_case = 0xe2b0aa,    .lower_case = 0xe2b19a},  //  Ⱚ <> ⱚ 
{.upper_case = 0xe2b0ab,    .lower_case = 0xe2b19b},  //  Ⱛ <> ⱛ 
{.upper_case = 0xe2b0ac,    .lower_case = 0xe2b19c},  //  Ⱜ <> ⱜ 
{.upper_case = 0xe2b0ad,    .lower_case = 0xe2b19d},  //  Ⱝ <> ⱝ 
{.upper_case = 0xe2b0ae,    .lower_case = 0xe2b19e},  //  Ⱞ <> ⱞ 
{.upper_case = 0xe2b1a0,    .lower_case = 0xe2b1a1},  //  Ⱡ <> ⱡ 
{.upper_case = 0xc8ba,    .lower_case = 0xe2b1a5},  //  Ⱥ <> ⱥ 
{.upper_case = 0xc8be,    .lower_case = 0xe2b1a6},  //  Ⱦ <> ⱦ 
{.upper_case = 0xe2b1a7,    .lower_case = 0xe2b1a8},  //  Ⱨ <> ⱨ 
{.upper_case = 0xe2b1a9,    .lower_case = 0xe2b1aa},  //  Ⱪ <> ⱪ 
{.upper_case = 0xe2b1ab,    .lower_case = 0xe2b1ac},  //  Ⱬ <> ⱬ 
{.upper_case = 0xe2b1b2,    .lower_case = 0xe2b1b3},  //  Ⱳ <> ⱳ 
{.upper_case = 0xe2b1b5,    .lower_case = 0xe2b1b6},  //  Ⱶ <> ⱶ 
{.upper_case = 0xe2b280,    .lower_case = 0xe2b281},  //  Ⲁ <> ⲁ 
{.upper_case = 0xe2b282,    .lower_case = 0xe2b283},  //  Ⲃ <> ⲃ 
{.upper_case = 0xe2b284,    .lower_case = 0xe2b285},  //  Ⲅ <> ⲅ 
{.upper_case = 0xe2b286,    .lower_case = 0xe2b287},  //  Ⲇ <> ⲇ 
{.upper_case = 0xe2b288,    .lower_case = 0xe2b289},  //  Ⲉ <> ⲉ 
{.upper_case = 0xe2b28a,    .lower_case = 0xe2b28b},  //  Ⲋ <> ⲋ 
{.upper_case = 0xe2b28c,    .lower_case = 0xe2b28d},  //  Ⲍ <> ⲍ 
{.upper_case = 0xe2b28e,    .lower_case = 0xe2b28f},  //  Ⲏ <> ⲏ 
{.upper_case = 0xe2b290,    .lower_case = 0xe2b291},  //  Ⲑ <> ⲑ 
{.upper_case = 0xe2b292,    .lower_case = 0xe2b293},  //  Ⲓ <> ⲓ 
{.upper_case = 0xe2b294,    .lower_case = 0xe2b295},  //  Ⲕ <> ⲕ 
{.upper_case = 0xe2b296,    .lower_case = 0xe2b297},  //  Ⲗ <> ⲗ 
{.upper_case = 0xe2b298,    .lower_case = 0xe2b299},  //  Ⲙ <> ⲙ 
{.upper_case = 0xe2b29a,    .lower_case = 0xe2b29b},  //  Ⲛ <> ⲛ 
{.upper_case = 0xe2b29c,    .lower_case = 0xe2b29d},  //  Ⲝ <> ⲝ 
{.upper_case = 0xe2b29e,    .lower_case = 0xe2b29f},  //  Ⲟ <> ⲟ 
{.upper_case = 0xe2b2a0,    .lower_case = 0xe2b2a1},  //  Ⲡ <> ⲡ 
{.upper_case = 0xe2b2a2,    .lower_case = 0xe2b2a3},  //  Ⲣ <> ⲣ 
{.upper_case = 0xe2b2a4,    .lower_case = 0xe2b2a5},  //  Ⲥ <> ⲥ 
{.upper_case = 0xe2b2a6,    .lower_case = 0xe2b2a7},  //  Ⲧ <> ⲧ 
{.upper_case = 0xe2b2a8,    .lower_case = 0xe2b2a9},  //  Ⲩ <> ⲩ 
{.upper_case = 0xe2b2aa,    .lower_case = 0xe2b2ab},  //  Ⲫ <> ⲫ 
{.upper_case = 0xe2b2ac,    .lower_case = 0xe2b2ad},  //  Ⲭ <> ⲭ 
{.upper_case = 0xe2b2ae,    .lower_case = 0xe2b2af},  //  Ⲯ <> ⲯ 
{.upper_case = 0xe2b2b0,    .lower_case = 0xe2b2b1},  //  Ⲱ <> ⲱ 
{.upper_case = 0xe2b2b2,    .lower_case = 0xe2b2b3},  //  Ⲳ <> ⲳ 
{.upper_case = 0xe2b2b4,    .lower_case = 0xe2b2b5},  //  Ⲵ <> ⲵ 
{.upper_case = 0xe2b2b6,    .lower_case = 0xe2b2b7},  //  Ⲷ <> ⲷ 
{.upper_case = 0xe2b2b8,    .lower_case = 0xe2b2b9},  //  Ⲹ <> ⲹ 
{.upper_case = 0xe2b2ba,    .lower_case = 0xe2b2bb},  //  Ⲻ <> ⲻ 
{.upper_case = 0xe2b2bc,    .lower_case = 0xe2b2bd},  //  Ⲽ <> ⲽ 
{.upper_case = 0xe2b2be,    .lower_case = 0xe2b2bf},  //  Ⲿ <> ⲿ 
{.upper_case = 0xe2b380,    .lower_case = 0xe2b381},  //  Ⳁ <> ⳁ 
{.upper_case = 0xe2b382,    .lower_case = 0xe2b383},  //  Ⳃ <> ⳃ 
{.upper_case = 0xe2b384,    .lower_case = 0xe2b385},  //  Ⳅ <> ⳅ 
{.upper_case = 0xe2b386,    .lower_case = 0xe2b387},  //  Ⳇ <> ⳇ 
{.upper_case = 0xe2b388,    .lower_case = 0xe2b389},  //  Ⳉ <> ⳉ 
{.upper_case = 0xe2b38a,    .lower_case = 0xe2b38b},  //  Ⳋ <> ⳋ 
{.upper_case = 0xe2b38c,    .lower_case = 0xe2b38d},  //  Ⳍ <> ⳍ 
{.upper_case = 0xe2b38e,    .lower_case = 0xe2b38f},  //  Ⳏ <> ⳏ 
{.upper_case = 0xe2b390,    .lower_case = 0xe2b391},  //  Ⳑ <> ⳑ 
{.upper_case = 0xe2b392,    .lower_case = 0xe2b393},  //  Ⳓ <> ⳓ 
{.upper_case = 0xe2b394,    .lower_case = 0xe2b395},  //  Ⳕ <> ⳕ 
{.upper_case = 0xe2b396,    .lower_case = 0xe2b397},  //  Ⳗ <> ⳗ 
{.upper_case = 0xe2b398,    .lower_case = 0xe2b399},  //  Ⳙ <> ⳙ 
{.upper_case = 0xe2b39a,    .lower_case = 0xe2b39b},  //  Ⳛ <> ⳛ 
{.upper_case = 0xe2b39c,    .lower_case = 0xe2b39d},  //  Ⳝ <> ⳝ 
{.upper_case = 0xe2b39e,    .lower_case = 0xe2b39f},  //  Ⳟ <> ⳟ 
{.upper_case = 0xe2b3a0,    .lower_case = 0xe2b3a1},  //  Ⳡ <> ⳡ 
{.upper_case = 0xe2b3a2,    .lower_case = 0xe2b3a3},  //  Ⳣ <> ⳣ 
{.upper_case = 0xe2b3ab,    .lower_case = 0xe2b3ac},  //  Ⳬ <> ⳬ 
{.upper_case = 0xe2b3ad,    .lower_case = 0xe2b3ae},  //  Ⳮ <> ⳮ 
{.upper_case = 0xe182a0,    .lower_case = 0xe2b480},  //  Ⴀ <> ⴀ 
{.upper_case = 0xe182a1,    .lower_case = 0xe2b481},  //  Ⴁ <> ⴁ 
{.upper_case = 0xe182a2,    .lower_case = 0xe2b482},  //  Ⴂ <> ⴂ 
{.upper_case = 0xe182a3,    .lower_case = 0xe2b483},  //  Ⴃ <> ⴃ 
{.upper_case = 0xe182a4,    .lower_case = 0xe2b484},  //  Ⴄ <> ⴄ 
{.upper_case = 0xe182a5,    .lower_case = 0xe2b485},  //  Ⴅ <> ⴅ 
{.upper_case = 0xe182a6,    .lower_case = 0xe2b486},  //  Ⴆ <> ⴆ 
{.upper_case = 0xe182a7,    .lower_case = 0xe2b487},  //  Ⴇ <> ⴇ 
{.upper_case = 0xe182a8,    .lower_case = 0xe2b488},  //  Ⴈ <> ⴈ 
{.upper_case = 0xe182a9,    .lower_case = 0xe2b489},  //  Ⴉ <> ⴉ 
{.upper_case = 0xe182aa,    .lower_case = 0xe2b48a},  //  Ⴊ <> ⴊ 
{.upper_case = 0xe182ab,    .lower_case = 0xe2b48b},  //  Ⴋ <> ⴋ 
{.upper_case = 0xe182ac,    .lower_case = 0xe2b48c},  //  Ⴌ <> ⴌ 
{.upper_case = 0xe182ad,    .lower_case = 0xe2b48d},  //  Ⴍ <> ⴍ 
{.upper_case = 0xe182ae,    .lower_case = 0xe2b48e},  //  Ⴎ <> ⴎ 
{.upper_case = 0xe182af,    .lower_case = 0xe2b48f},  //  Ⴏ <> ⴏ 
{.upper_case = 0xe182b0,    .lower_case = 0xe2b490},  //  Ⴐ <> ⴐ 
{.upper_case = 0xe182b1,    .lower_case = 0xe2b491},  //  Ⴑ <> ⴑ 
{.upper_case = 0xe182b2,    .lower_case = 0xe2b492},  //  Ⴒ <> ⴒ 
{.upper_case = 0xe182b3,    .lower_case = 0xe2b493},  //  Ⴓ <> ⴓ 
{.upper_case = 0xe182b4,    .lower_case = 0xe2b494},  //  Ⴔ <> ⴔ 
{.upper_case = 0xe182b5,    .lower_case = 0xe2b495},  //  Ⴕ <> ⴕ 
{.upper_case = 0xe182b6,    .lower_case = 0xe2b496},  //  Ⴖ <> ⴖ 
{.upper_case = 0xe182b7,    .lower_case = 0xe2b497},  //  Ⴗ <> ⴗ 
{.upper_case = 0xe182b8,    .lower_case = 0xe2b498},  //  Ⴘ <> ⴘ 
{.upper_case = 0xe182b9,    .lower_case = 0xe2b499},  //  Ⴙ <> ⴙ 
{.upper_case = 0xe182ba,    .lower_case = 0xe2b49a},  //  Ⴚ <> ⴚ 
{.upper_case = 0xe182bb,    .lower_case = 0xe2b49b},  //  Ⴛ <> ⴛ 
{.upper_case = 0xe182bc,    .lower_case = 0xe2b49c},  //  Ⴜ <> ⴜ 
{.upper_case = 0xe182bd,    .lower_case = 0xe2b49d},  //  Ⴝ <> ⴝ 
{.upper_case = 0xe182be,    .lower_case = 0xe2b49e},  //  Ⴞ <> ⴞ 
{.upper_case = 0xe182bf,    .lower_case = 0xe2b49f},  //  Ⴟ <> ⴟ 
{.upper_case = 0xe18380,    .lower_case = 0xe2b4a0},  //  Ⴠ <> ⴠ 
{.upper_case = 0xe18381,    .lower_case = 0xe2b4a1},  //  Ⴡ <> ⴡ 
{.upper_case = 0xe18382,    .lower_case = 0xe2b4a2},  //  Ⴢ <> ⴢ 
{.upper_case = 0xe18383,    .lower_case = 0xe2b4a3},  //  Ⴣ <> ⴣ 
{.upper_case = 0xe18384,    .lower_case = 0xe2b4a4},  //  Ⴤ <> ⴤ 
{.upper_case = 0xe18385,    .lower_case = 0xe2b4a5},  //  Ⴥ <> ⴥ 
{.upper_case = 0xea9980,    .lower_case = 0xea9981},  //  Ꙁ <> ꙁ 
{.upper_case = 0xea9982,    .lower_case = 0xea9983},  //  Ꙃ <> ꙃ 
{.upper_case = 0xea9984,    .lower_case = 0xea9985},  //  Ꙅ <> ꙅ 
{.upper_case = 0xea9986,    .lower_case = 0xea9987},  //  Ꙇ <> ꙇ 
{.upper_case = 0xea9988,    .lower_case = 0xea9989},  //  Ꙉ <> ꙉ 
{.upper_case = 0xea998a,    .lower_case = 0xea998b},  //  Ꙋ <> ꙋ 
{.upper_case = 0xea998c,    .lower_case = 0xea998d},  //  Ꙍ <> ꙍ 
{.upper_case = 0xea998e,    .lower_case = 0xea998f},  //  Ꙏ <> ꙏ 
{.upper_case = 0xea9990,    .lower_case = 0xea9991},  //  Ꙑ <> ꙑ 
{.upper_case = 0xea9992,    .lower_case = 0xea9993},  //  Ꙓ <> ꙓ 
{.upper_case = 0xea9994,    .lower_case = 0xea9995},  //  Ꙕ <> ꙕ 
{.upper_case = 0xea9996,    .lower_case = 0xea9997},  //  Ꙗ <> ꙗ 
{.upper_case = 0xea9998,    .lower_case = 0xea9999},  //  Ꙙ <> ꙙ 
{.upper_case = 0xea999a,    .lower_case = 0xea999b},  //  Ꙛ <> ꙛ 
{.upper_case = 0xea999c,    .lower_case = 0xea999d},  //  Ꙝ <> ꙝ 
{.upper_case = 0xea999e,    .lower_case = 0xea999f},  //  Ꙟ <> ꙟ 
{.upper_case = 0xea99a0,    .lower_case = 0xea99a1},  //  Ꙡ <> ꙡ 
{.upper_case = 0xea99a2,    .lower_case = 0xea99a3},  //  Ꙣ <> ꙣ 
{.upper_case = 0xea99a4,    .lower_case = 0xea99a5},  //  Ꙥ <> ꙥ 
{.upper_case = 0xea99a6,    .lower_case = 0xea99a7},  //  Ꙧ <> ꙧ 
{.upper_case = 0xea99a8,    .lower_case = 0xea99a9},  //  Ꙩ <> ꙩ 
{.upper_case = 0xea99aa,    .lower_case = 0xea99ab},  //  Ꙫ <> ꙫ 
{.upper_case = 0xea99ac,    .lower_case = 0xea99ad},  //  Ꙭ <> ꙭ 
{.upper_case = 0xea9a80,    .lower_case = 0xea9a81},  //  Ꚁ <> ꚁ 
{.upper_case = 0xea9a82,    .lower_case = 0xea9a83},  //  Ꚃ <> ꚃ 
{.upper_case = 0xea9a84,    .lower_case = 0xea9a85},  //  Ꚅ <> ꚅ 
{.upper_case = 0xea9a86,    .lower_case = 0xea9a87},  //  Ꚇ <> ꚇ 
{.upper_case = 0xea9a88,    .lower_case = 0xea9a89},  //  Ꚉ <> ꚉ 
{.upper_case = 0xea9a8a,    .lower_case = 0xea9a8b},  //  Ꚋ <> ꚋ 
{.upper_case = 0xea9a8c,    .lower_case = 0xea9a8d},  //  Ꚍ <> ꚍ 
{.upper_case = 0xea9a8e,    .lower_case = 0xea9a8f},  //  Ꚏ <> ꚏ 
{.upper_case = 0xea9a90,    .lower_case = 0xea9a91},  //  Ꚑ <> ꚑ 
{.upper_case = 0xea9a92,    .lower_case = 0xea9a93},  //  Ꚓ <> ꚓ 
{.upper_case = 0xea9a94,    .lower_case = 0xea9a95},  //  Ꚕ <> ꚕ 
{.upper_case = 0xea9a96,    .lower_case = 0xea9a97},  //  Ꚗ <> ꚗ 
{.upper_case = 0xea9ca2,    .lower_case = 0xea9ca3},  //  Ꜣ <> ꜣ 
{.upper_case = 0xea9ca4,    .lower_case = 0xea9ca5},  //  Ꜥ <> ꜥ 
{.upper_case = 0xea9ca6,    .lower_case = 0xea9ca7},  //  Ꜧ <> ꜧ 
{.upper_case = 0xea9ca8,    .lower_case = 0xea9ca9},  //  Ꜩ <> ꜩ 
{.upper_case = 0xea9caa,    .lower_case = 0xea9cab},  //  Ꜫ <> ꜫ 
{.upper_case = 0xea9cac,    .lower_case = 0xea9cad},  //  Ꜭ <> ꜭ 
{.upper_case = 0xea9cae,    .lower_case = 0xea9caf},  //  Ꜯ <> ꜯ 
{.upper_case = 0xea9cb2,    .lower_case = 0xea9cb3},  //  Ꜳ <> ꜳ 
{.upper_case = 0xea9cb4,    .lower_case = 0xea9cb5},  //  Ꜵ <> ꜵ 
{.upper_case = 0xea9cb6,    .lower_case = 0xea9cb7},  //  Ꜷ <> ꜷ 
{.upper_case = 0xea9cb8,    .lower_case = 0xea9cb9},  //  Ꜹ <> ꜹ 
{.upper_case = 0xea9cba,    .lower_case = 0xea9cbb},  //  Ꜻ <> ꜻ 
{.upper_case = 0xea9cbc,    .lower_case = 0xea9cbd},  //  Ꜽ <> ꜽ 
{.upper_case = 0xea9cbe,    .lower_case = 0xea9cbf},  //  Ꜿ <> ꜿ 
{.upper_case = 0xea9d80,    .lower_case = 0xea9d81},  //  Ꝁ <> ꝁ 
{.upper_case = 0xea9d82,    .lower_case = 0xea9d83},  //  Ꝃ <> ꝃ 
{.upper_case = 0xea9d84,    .lower_case = 0xea9d85},  //  Ꝅ <> ꝅ 
{.upper_case = 0xea9d86,    .lower_case = 0xea9d87},  //  Ꝇ <> ꝇ 
{.upper_case = 0xea9d88,    .lower_case = 0xea9d89},  //  Ꝉ <> ꝉ 
{.upper_case = 0xea9d8a,    .lower_case = 0xea9d8b},  //  Ꝋ <> ꝋ 
{.upper_case = 0xea9d8c,    .lower_case = 0xea9d8d},  //  Ꝍ <> ꝍ 
{.upper_case = 0xea9d8e,    .lower_case = 0xea9d8f},  //  Ꝏ <> ꝏ 
{.upper_case = 0xea9d90,    .lower_case = 0xea9d91},  //  Ꝑ <> ꝑ 
{.upper_case = 0xea9d92,    .lower_case = 0xea9d93},  //  Ꝓ <> ꝓ 
{.upper_case = 0xea9d94,    .lower_case = 0xea9d95},  //  Ꝕ <> ꝕ 
{.upper_case = 0xea9d96,    .lower_case = 0xea9d97},  //  Ꝗ <> ꝗ 
{.upper_case = 0xea9d98,    .lower_case = 0xea9d99},  //  Ꝙ <> ꝙ 
{.upper_case = 0xea9d9a,    .lower_case = 0xea9d9b},  //  Ꝛ <> ꝛ 
{.upper_case = 0xea9d9c,    .lower_case = 0xea9d9d},  //  Ꝝ <> ꝝ 
{.upper_case = 0xea9d9e,    .lower_case = 0xea9d9f},  //  Ꝟ <> ꝟ 
{.upper_case = 0xea9da0,    .lower_case = 0xea9da1},  //  Ꝡ <> ꝡ 
{.upper_case = 0xea9da2,    .lower_case = 0xea9da3},  //  Ꝣ <> ꝣ 
{.upper_case = 0xea9da4,    .lower_case = 0xea9da5},  //  Ꝥ <> ꝥ 
{.upper_case = 0xea9da6,    .lower_case = 0xea9da7},  //  Ꝧ <> ꝧ 
{.upper_case = 0xea9da8,    .lower_case = 0xea9da9},  //  Ꝩ <> ꝩ 
{.upper_case = 0xea9daa,    .lower_case = 0xea9dab},  //  Ꝫ <> ꝫ 
{.upper_case = 0xea9dac,    .lower_case = 0xea9dad},  //  Ꝭ <> ꝭ 
{.upper_case = 0xea9dae,    .lower_case = 0xea9daf},  //  Ꝯ <> ꝯ 
{.upper_case = 0xea9db9,    .lower_case = 0xea9dba},  //  Ꝺ <> ꝺ 
{.upper_case = 0xea9dbb,    .lower_case = 0xea9dbc},  //  Ꝼ <> ꝼ 
{.upper_case = 0xea9dbe,    .lower_case = 0xea9dbf},  //  Ꝿ <> ꝿ 
{.upper_case = 0xea9e80,    .lower_case = 0xea9e81},  //  Ꞁ <> ꞁ 
{.upper_case = 0xea9e82,    .lower_case = 0xea9e83},  //  Ꞃ <> ꞃ 
{.upper_case = 0xea9e84,    .lower_case = 0xea9e85},  //  Ꞅ <> ꞅ 
{.upper_case = 0xea9e86,    .lower_case = 0xea9e87},  //  Ꞇ <> ꞇ 
{.upper_case = 0xea9e8b,    .lower_case = 0xea9e8c},  //  Ꞌ <> ꞌ 
{.upper_case = 0xea9e90,    .lower_case = 0xea9e91},  //  Ꞑ <> ꞑ 
{.upper_case = 0xea9ea0,    .lower_case = 0xea9ea1},  //  Ꞡ <> ꞡ 
{.upper_case = 0xea9ea2,    .lower_case = 0xea9ea3},  //  Ꞣ <> ꞣ 
{.upper_case = 0xea9ea4,    .lower_case = 0xea9ea5},  //  Ꞥ <> ꞥ 
{.upper_case = 0xea9ea6,    .lower_case = 0xea9ea7},  //  Ꞧ <> ꞧ 
{.upper_case = 0xea9ea8,    .lower_case = 0xea9ea9},  //  Ꞩ <> ꞩ 
{.upper_case = 0xefbca1,    .lower_case = 0xefbd81},  //  Ａ <> ａ 
{.upper_case = 0xefbca2,    .lower_case = 0xefbd82},  //  Ｂ <> ｂ 
{.upper_case = 0xefbca3,    .lower_case = 0xefbd83},  //  Ｃ <> ｃ 
{.upper_case = 0xefbca4,    .lower_case = 0xefbd84},  //  Ｄ <> ｄ 
{.upper_case = 0xefbca5,    .lower_case = 0xefbd85},  //  Ｅ <> ｅ 
{.upper_case = 0xefbca6,    .lower_case = 0xefbd86},  //  Ｆ <> ｆ 
{.upper_case = 0xefbca7,    .lower_case = 0xefbd87},  //  Ｇ <> ｇ 
{.upper_case = 0xefbca8,    .lower_case = 0xefbd88},  //  Ｈ <> ｈ 
{.upper_case = 0xefbca9,    .lower_case = 0xefbd89},  //  Ｉ <> ｉ 
{.upper_case = 0xefbcaa,    .lower_case = 0xefbd8a},  //  Ｊ <> ｊ 
{.upper_case = 0xefbcab,    .lower_case = 0xefbd8b},  //  Ｋ <> ｋ 
{.upper_case = 0xefbcac,    .lower_case = 0xefbd8c},  //  Ｌ <> ｌ 
{.upper_case = 0xefbcad,    .lower_case = 0xefbd8d},  //  Ｍ <> ｍ 
{.upper_case = 0xefbcae,    .lower_case = 0xefbd8e},  //  Ｎ <> ｎ 
{.upper_case = 0xefbcaf,    .lower_case = 0xefbd8f},  //  Ｏ <> ｏ 
{.upper_case = 0xefbcb0,    .lower_case = 0xefbd90},  //  Ｐ <> ｐ 
{.upper_case = 0xefbcb1,    .lower_case = 0xefbd91},  //  Ｑ <> ｑ 
{.upper_case = 0xefbcb2,    .lower_case = 0xefbd92},  //  Ｒ <> ｒ 
{.upper_case = 0xefbcb3,    .lower_case = 0xefbd93},  //  Ｓ <> ｓ 
{.upper_case = 0xefbcb4,    .lower_case = 0xefbd94},  //  Ｔ <> ｔ 
{.upper_case = 0xefbcb5,    .lower_case = 0xefbd95},  //  Ｕ <> ｕ 
{.upper_case = 0xefbcb6,    .lower_case = 0xefbd96},  //  Ｖ <> ｖ 
{.upper_case = 0xefbcb7,    .lower_case = 0xefbd97},  //  Ｗ <> ｗ 
{.upper_case = 0xefbcb8,    .lower_case = 0xefbd98},  //  Ｘ <> ｘ 
{.upper_case = 0xefbcb9,    .lower_case = 0xefbd99},  //  Ｙ <> ｙ 
{.upper_case = 0xefbcba,    .lower_case = 0xefbd9a},  //  Ｚ <> ｚ 
#else  
/* Russian only */
{.upper_case = 0xD090, .lower_case = 0xD0B0 },   // А, а
{.upper_case = 0xD091, .lower_case = 0xD0B1 },   // Б, б
{.upper_case = 0xD092, .lower_case = 0xD0B2 },   // В, в
{.upper_case = 0xD093, .lower_case = 0xD0B3 },   // Г, г
{.upper_case = 0xD094, .lower_case = 0xD0B4 },   // Д, д
{.upper_case = 0xD095, .lower_case = 0xD0B5 },   // Е, е
{.upper_case = 0xD081, .lower_case = 0xD191 },   // Ё, ё
{.upper_case = 0xD096, .lower_case = 0xD0B6 },   // Ж, ж
{.upper_case = 0xD097, .lower_case = 0xD0B7 },   // З, з
{.upper_case = 0xD098, .lower_case = 0xD0B8 },   // И, и
{.upper_case = 0xD099, .lower_case = 0xD0B9 },   // Й, й
{.upper_case = 0xD09A, .lower_case = 0xD0BA },   // К, к
{.upper_case = 0xD09B, .lower_case = 0xD0BB },   // Л, л
{.upper_case = 0xD09C, .lower_case = 0xD0BC },   // М, м
{.upper_case = 0xD09D, .lower_case = 0xD0BD },   // Н, н
{.upper_case = 0xD09E, .lower_case = 0xD0BE },   // О, о
{.upper_case = 0xD09F, .lower_case = 0xD0BF },   // П, п
{.upper_case = 0xD0A0, .lower_case = 0xD180 },   // Р, р
{.upper_case = 0xD0A1, .lower_case = 0xD181 },   // С, с
{.upper_case = 0xD0A2, .lower_case = 0xD182 },   // Т, т
{.upper_case = 0xD0A3, .lower_case = 0xD183 },   // У, у
{.upper_case = 0xD0A4, .lower_case = 0xD184 },   // Ф, ф
{.upper_case = 0xD0A5, .lower_case = 0xD185 },   // Х, х
{.upper_case = 0xD0A6, .lower_case = 0xD186 },   // Ц, ц
{.upper_case = 0xD0A7, .lower_case = 0xD187 },   // Ч, ч
{.upper_case = 0xD0A8, .lower_case = 0xD188 },   // Ш, ш
{.upper_case = 0xD0A9, .lower_case = 0xD189 },   // Щ, щ
{.upper_case = 0xD0AA, .lower_case = 0xD18A },   // Ъ, ъ
{.upper_case = 0xD0AB, .lower_case = 0xD18B },   // Ы, ы
{.upper_case = 0xD0AC, .lower_case = 0xD18C },   // Ь, ь
{.upper_case = 0xD0AD, .lower_case = 0xD18D },   // Э, э
{.upper_case = 0xD0AE, .lower_case = 0xD18E },   // Ю, ю
{.upper_case = 0xD0AF, .lower_case = 0xD18F },   // Я, я
{.upper_case = 0xD086, .lower_case = 0xD196 },   // І, і
{.upper_case = 0xD1B2, .lower_case = 0xD1B3 },   // Ѳ, ѳ
{.upper_case = 0xD1A2, .lower_case = 0xD1A3 },   // Ѣ, ѣ
{.upper_case = 0xD1B4, .lower_case = 0xD1B5 },   // Ѵ, ѵ

#endif
};

int get_char_len(int c)
{
	if((c & 0xFF) < 0x80) return 1;
	if((c & 0xE0) == 0xC0) return 2;
	if((c & 0xF0) == 0xE0) return 3;
	if((c & 0xF8) == 0xF0) return 4;

	return 1;
}
int get_char_utf8(const char *s, int len)
{
	int c = 0;
	int i;

	c = 0xff & s[0];
	for(i = 1; i < len; i++)
	{
		c <<= 8;
		c |= 0xff & s[i];
	}
	return c;
}
int cmp_character(const void *c1, const void *c2)
{
	return (*(int *)c1) - ((case_t *)c2)->lower_case;	
}
int utf8_uppercase(int c)
{
	int i;
	case_t *ch;
	// speed up ASCII
	if(c < 0x80)
	{
		if(c >= 0x61 && c <= 0x7A)
			return (c - 0x20);
		else
			return c;

	}
#if 0
	for(i = 0; i < sizeof(character_table)/sizeof(case_t); i++)
	{
		if(c == character_table[i].lower_case)
			return character_table[i].upper_case;
	}
#else
	if((ch = bsearch(&c, character_table, sizeof(character_table)/sizeof(case_t), sizeof(case_t), cmp_character)) != NULL) {
		return ch->upper_case;
	}
#endif
	return c;
}


//royc 20091223 fix share name with utf8 char
bool utf8_compare(const char *s1, const char *s2)
{
	int				s1_len = 0;
	int				s2_len = 0;
	unsigned char	byte = 0;
	int us1, us2;
	int idx1, idx2;
	
	//input can't be a null string
	if(s1 == NULL || s2 == NULL)
		return false;
	
	//get length of string
	s1_len = strlen(s1);
	s2_len = strlen(s2);
	
	if(s1_len == 0 || s2_len == 0)
		return false;
		

	for(idx1 = 0, idx2 = 0; idx1 < s1_len && idx2 < s2_len; /*nop*/ )
	{
		byte = get_char_len((unsigned char)s1[idx1]);
		us1 = get_char_utf8(s1 + idx1, byte);
		idx1 += byte;

		byte = get_char_len((unsigned char)s2[idx2]);
		us2 = get_char_utf8(s2 + idx2, byte);
		idx2 += byte;

		if(utf8_uppercase(us1) == utf8_uppercase(us2))
			continue;
		else
			return false;
	}

	if(idx1 < s1_len || idx2 < s2_len)
	{
		return false;
	}
	
	return true;
}
//royc 20091223 end

int lp_servicenumber(const char *pszServiceName)
{
	int iService;
        fstring serviceName;
        
        if (!pszServiceName) {
        	return GLOBAL_SECTION_SNUM;
	}
        
	for (iService = iNumServices - 1; iService >= 0; iService--) {
		if (VALID(iService) && ServicePtrs[iService]->szService) {
			/*
			 * The substitution here is used to support %U is
			 * service names
			 */
			fstrcpy(serviceName, ServicePtrs[iService]->szService);
#if 0	
			int i;
	DEBUG(0,("s1(%s)\n", serviceName));
	DEBUG(0,("s2(%s)\n", pszServiceName));
	for(i=0; i<strlen(serviceName); i++)
		DEBUG(0,("(%x)", serviceName[i]));
	DEBUG(0,("\n"));	
	for(i=0; i<strlen(pszServiceName); i++)
		DEBUG(0,("<%x>", pszServiceName[i]));
	DEBUG(0,("\n"));	
#endif
			standard_sub_basic(get_current_username(),
					   current_user_info.domain,
					   serviceName,sizeof(serviceName));
			if (strequal(serviceName, pszServiceName)) {
				break;
			}
#if 1
			//royc 20091223 fix share name with utf8 char
			if (utf8_compare(serviceName, pszServiceName)) {
				break;
			}
			//royc 20091223 end
#endif
		}
	}

	if (iService >= 0 && ServicePtrs[iService]->usershare == USERSHARE_VALID) {
		struct timespec last_mod;

		if (!usershare_exists(iService, &last_mod)) {
			/* Remove the share security tdb entry for it. */
			delete_share_security(lp_servicename(iService));
			/* Remove it from the array. */
			free_service_byindex(iService);
			/* Doesn't exist anymore. */
			return GLOBAL_SECTION_SNUM;
		}

		/* Has it been modified ? If so delete and reload. */
		if (timespec_compare(&ServicePtrs[iService]->usershare_last_mod,
				     &last_mod) < 0) {
			/* Remove it from the array. */
			free_service_byindex(iService);
			/* and now reload it. */
			iService = load_usershare_service(pszServiceName);
		}
	}

	if (iService < 0) {
		DEBUG(7,("lp_servicenumber: couldn't find %s\n", pszServiceName));
		return GLOBAL_SECTION_SNUM;
	}

	return (iService);
}

bool share_defined(const char *service_name)
{
	return (lp_servicenumber(service_name) != -1);
}

struct share_params *get_share_params(TALLOC_CTX *mem_ctx,
				      const char *sharename)
{
	struct share_params *result;
	char *sname;
	int snum;

	if (!(sname = SMB_STRDUP(sharename))) {
		return NULL;
	}

	snum = find_service(sname);
	SAFE_FREE(sname);

	if (snum < 0) {
		return NULL;
	}

	if (!(result = TALLOC_P(mem_ctx, struct share_params))) {
		DEBUG(0, ("talloc failed\n"));
		return NULL;
	}

	result->service = snum;
	return result;
}

struct share_iterator *share_list_all(TALLOC_CTX *mem_ctx)
{
	struct share_iterator *result;

	if (!(result = TALLOC_P(mem_ctx, struct share_iterator))) {
		DEBUG(0, ("talloc failed\n"));
		return NULL;
	}

	result->next_id = 0;
	return result;
}

struct share_params *next_share(struct share_iterator *list)
{
	struct share_params *result;

	while (!lp_snum_ok(list->next_id) &&
	       (list->next_id < lp_numservices())) {
		list->next_id += 1;
	}

	if (list->next_id >= lp_numservices()) {
		return NULL;
	}

	if (!(result = TALLOC_P(list, struct share_params))) {
		DEBUG(0, ("talloc failed\n"));
		return NULL;
	}

	result->service = list->next_id;
	list->next_id += 1;
	return result;
}

struct share_params *next_printer(struct share_iterator *list)
{
	struct share_params *result;

	while ((result = next_share(list)) != NULL) {
		if (lp_print_ok(result->service)) {
			break;
		}
	}
	return result;
}

/*
 * This is a hack for a transition period until we transformed all code from
 * service numbers to struct share_params.
 */

struct share_params *snum2params_static(int snum)
{
	static struct share_params result;
	result.service = snum;
	return &result;
}

/*******************************************************************
 A useful volume label function. 
********************************************************************/

const char *volume_label(int snum)
{
	char *ret;
	const char *label = lp_volume(snum);
	if (!*label) {
		label = lp_servicename(snum);
	}
		
	/* This returns a 33 byte guarenteed null terminated string. */
	ret = talloc_strndup(talloc_tos(), label, 32);
	if (!ret) {
		return "";
	}		
	return ret;
}

/*******************************************************************
 Set the server type we will announce as via nmbd.
********************************************************************/

static void set_default_server_announce_type(void)
{
	default_server_announce = 0;
	default_server_announce |= SV_TYPE_WORKSTATION;
	default_server_announce |= SV_TYPE_SERVER;
	default_server_announce |= SV_TYPE_SERVER_UNIX;

	/* note that the flag should be set only if we have a 
	   printer service but nmbd doesn't actually load the 
	   services so we can't tell   --jerry */

	default_server_announce |= SV_TYPE_PRINTQ_SERVER;

	switch (lp_announce_as()) {
		case ANNOUNCE_AS_NT_SERVER:
			default_server_announce |= SV_TYPE_SERVER_NT;
			/* fall through... */
		case ANNOUNCE_AS_NT_WORKSTATION:
			default_server_announce |= SV_TYPE_NT;
			break;
		case ANNOUNCE_AS_WIN95:
			default_server_announce |= SV_TYPE_WIN95_PLUS;
			break;
		case ANNOUNCE_AS_WFW:
			default_server_announce |= SV_TYPE_WFW;
			break;
		default:
			break;
	}

	switch (lp_server_role()) {
		case ROLE_DOMAIN_MEMBER:
			default_server_announce |= SV_TYPE_DOMAIN_MEMBER;
			break;
		case ROLE_DOMAIN_PDC:
			default_server_announce |= SV_TYPE_DOMAIN_CTRL;
			break;
		case ROLE_DOMAIN_BDC:
			default_server_announce |= SV_TYPE_DOMAIN_BAKCTRL;
			break;
		case ROLE_STANDALONE:
		default:
			break;
	}
	if (lp_time_server())
		default_server_announce |= SV_TYPE_TIME_SOURCE;

	if (lp_host_msdfs())
		default_server_announce |= SV_TYPE_DFS_SERVER;
}

/***********************************************************
 returns role of Samba server
************************************************************/

int lp_server_role(void)
{
	return server_role;
}

/***********************************************************
 If we are PDC then prefer us as DMB
************************************************************/

bool lp_domain_master(void)
{
	if (Globals.iDomainMaster == Auto)
		return (lp_server_role() == ROLE_DOMAIN_PDC);

	return (bool)Globals.iDomainMaster;
}

/***********************************************************
 If we are DMB then prefer us as LMB
************************************************************/

bool lp_preferred_master(void)
{
	if (Globals.iPreferredMaster == Auto)
		return (lp_local_master() && lp_domain_master());

	return (bool)Globals.iPreferredMaster;
}

/*******************************************************************
 Remove a service.
********************************************************************/

void lp_remove_service(int snum)
{
	ServicePtrs[snum]->valid = False;
	invalid_services[num_invalid_services++] = snum;
}

/*******************************************************************
 Copy a service.
********************************************************************/

void lp_copy_service(int snum, const char *new_name)
{
	do_section(new_name, NULL);
	if (snum >= 0) {
		snum = lp_servicenumber(new_name);
		if (snum >= 0)
			lp_do_parameter(snum, "copy", lp_servicename(snum));
	}
}


/*******************************************************************
 Get the default server type we will announce as via nmbd.
********************************************************************/

int lp_default_server_announce(void)
{
	return default_server_announce;
}

/*******************************************************************
 Split the announce version into major and minor numbers.
********************************************************************/

int lp_major_announce_version(void)
{
	static bool got_major = False;
	static int major_version = DEFAULT_MAJOR_VERSION;
	char *vers;
	char *p;

	if (got_major)
		return major_version;

	got_major = True;
	if ((vers = lp_announce_version()) == NULL)
		return major_version;

	if ((p = strchr_m(vers, '.')) == 0)
		return major_version;

	*p = '\0';
	major_version = atoi(vers);
	return major_version;
}

int lp_minor_announce_version(void)
{
	static bool got_minor = False;
	static int minor_version = DEFAULT_MINOR_VERSION;
	char *vers;
	char *p;

	if (got_minor)
		return minor_version;

	got_minor = True;
	if ((vers = lp_announce_version()) == NULL)
		return minor_version;

	if ((p = strchr_m(vers, '.')) == 0)
		return minor_version;

	p++;
	minor_version = atoi(p);
	return minor_version;
}

/***********************************************************
 Set the global name resolution order (used in smbclient).
************************************************************/

void lp_set_name_resolve_order(const char *new_order)
{
	string_set(&Globals.szNameResolveOrder, new_order);
}

const char *lp_printername(int snum)
{
	const char *ret = _lp_printername(snum);
	if (ret == NULL || (ret != NULL && *ret == '\0'))
		ret = lp_const_servicename(snum);

	return ret;
}


/***********************************************************
 Allow daemons such as winbindd to fix their logfile name.
************************************************************/

void lp_set_logfile(const char *name)
{
	string_set(&Globals.szLogFile, name);
	debug_set_logfile(name);
}

/*******************************************************************
 Return the max print jobs per queue.
********************************************************************/

int lp_maxprintjobs(int snum)
{
	int maxjobs = LP_SNUM_OK(snum) ? ServicePtrs[snum]->iMaxPrintJobs : sDefault.iMaxPrintJobs;
	if (maxjobs <= 0 || maxjobs >= PRINT_MAX_JOBID)
		maxjobs = PRINT_MAX_JOBID - 1;

	return maxjobs;
}

const char *lp_printcapname(void)
{
	if ((Globals.szPrintcapname != NULL) &&
	    (Globals.szPrintcapname[0] != '\0'))
		return Globals.szPrintcapname;

	if (sDefault.iPrinting == PRINT_CUPS) {
#ifdef HAVE_CUPS
		return "cups";
#else
		return "lpstat";
#endif
	}

	if (sDefault.iPrinting == PRINT_BSD)
		return "/etc/printcap";

	return PRINTCAP_NAME;
}

static uint32 spoolss_state;

bool lp_disable_spoolss( void )
{
	if ( spoolss_state == SVCCTL_STATE_UNKNOWN )
		spoolss_state = _lp_disable_spoolss() ? SVCCTL_STOPPED : SVCCTL_RUNNING;

	return spoolss_state == SVCCTL_STOPPED ? True : False;
}

void lp_set_spoolss_state( uint32 state )
{
	SMB_ASSERT( (state == SVCCTL_STOPPED) || (state == SVCCTL_RUNNING) );

	spoolss_state = state;
}

uint32 lp_get_spoolss_state( void )
{
	return lp_disable_spoolss() ? SVCCTL_STOPPED : SVCCTL_RUNNING;
}

/*******************************************************************
 Ensure we don't use sendfile if server smb signing is active.
********************************************************************/

bool lp_use_sendfile(int snum, struct smb_signing_state *signing_state)
{
	bool sign_active = false;

	/* Using sendfile blows the brains out of any DOS or Win9x TCP stack... JRA. */
	if (get_Protocol() < PROTOCOL_NT1) {
		return false;
	}
	if (signing_state) {
		sign_active = smb_signing_is_active(signing_state);
	}
	return (_lp_use_sendfile(snum) &&
			(get_remote_arch() != RA_WIN95) &&
			!sign_active);
}

/*******************************************************************
 Turn off sendfile if we find the underlying OS doesn't support it.
********************************************************************/

void set_use_sendfile(int snum, bool val)
{
	if (LP_SNUM_OK(snum))
		ServicePtrs[snum]->bUseSendfile = val;
	else
		sDefault.bUseSendfile = val;
}

/*******************************************************************
 Turn off storing DOS attributes if this share doesn't support it.
********************************************************************/

void set_store_dos_attributes(int snum, bool val)
{
	if (!LP_SNUM_OK(snum))
		return;
	ServicePtrs[(snum)]->bStoreDosAttributes = val;
}

void lp_set_mangling_method(const char *new_method)
{
	string_set(&Globals.szManglingMethod, new_method);
}

/*******************************************************************
 Global state for POSIX pathname processing.
********************************************************************/

static bool posix_pathnames;

bool lp_posix_pathnames(void)
{
	return posix_pathnames;
}

/*******************************************************************
 Change everything needed to ensure POSIX pathname processing (currently
 not much).
********************************************************************/

void lp_set_posix_pathnames(void)
{
	posix_pathnames = True;
}

/*******************************************************************
 Global state for POSIX lock processing - CIFS unix extensions.
********************************************************************/

bool posix_default_lock_was_set;
static enum brl_flavour posix_cifsx_locktype; /* By default 0 == WINDOWS_LOCK */

enum brl_flavour lp_posix_cifsu_locktype(files_struct *fsp)
{
	if (posix_default_lock_was_set) {
		return posix_cifsx_locktype;
	} else {
		return fsp->posix_open ? POSIX_LOCK : WINDOWS_LOCK;
	}
}

/*******************************************************************
********************************************************************/

void lp_set_posix_default_cifsx_readwrite_locktype(enum brl_flavour val)
{
	posix_default_lock_was_set = True;
	posix_cifsx_locktype = val;
}

int lp_min_receive_file_size(void)
{
	if (Globals.iminreceivefile < 0) {
		return 0;
	}
	return MIN(Globals.iminreceivefile, BUFFER_SIZE);
}

/*******************************************************************
 If socket address is an empty character string, it is necessary to 
 define it as "0.0.0.0". 
********************************************************************/

const char *lp_socket_address(void)
{
	char *sock_addr = Globals.szSocketAddress;
	
	if (sock_addr[0] == '\0'){
		string_set(&Globals.szSocketAddress, "0.0.0.0");
	}
	return  Globals.szSocketAddress;
}

void lp_set_passdb_backend(const char *backend)
{
	string_set(&Globals.szPassdbBackend, backend);
}

/*******************************************************************
 Safe wide links checks.
 This helper function always verify the validity of wide links,
 even after a configuration file reload.
********************************************************************/

static bool lp_widelinks_internal(int snum)
{
	return (bool)(LP_SNUM_OK(snum)? ServicePtrs[(snum)]->bWidelinks :
			sDefault.bWidelinks);
}

void widelinks_warning(int snum)
{
	if (lp_unix_extensions() && lp_widelinks_internal(snum)) {
		DEBUG(0,("Share '%s' has wide links and unix extensions enabled. "
			"These parameters are incompatible. "
			"Wide links will be disabled for this share.\n",
			lp_servicename(snum) ));
	}
}

bool lp_widelinks(int snum)
{
	/* wide links is always incompatible with unix extensions */
	if (lp_unix_extensions()) {
		return false;
	}

	return lp_widelinks_internal(snum);
}
