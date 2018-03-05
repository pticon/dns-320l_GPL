#include "libeio/xthread.h"

#include <errno.h>

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "schmorp.h"

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <limits.h>
#include <fcntl.h>
#include <sched.h>

#if _POSIX_MEMLOCK || _POSIX_MAPPED_FILES
# include <sys/mman.h>
#endif

/* perl namespace pollution */
#undef VERSION

#ifdef _WIN32

# define EIO_STRUCT_DIRENT Direntry_t
# undef malloc
# undef free

// perl overrides all those nice win32 functions
# undef open
# undef read
# undef write
# undef send
# undef recv
# undef stat
# undef fstat
# define lstat stat
# undef truncate
# undef ftruncate
# undef open
# undef close
# undef unlink
# undef rmdir
# undef rename
# undef lseek

# define chown(a,b,c)    (errno = ENOSYS, -1)
# define fchown(a,b,c)   (errno = ENOSYS, -1)
# define fchmod(a,b)     (errno = ENOSYS, -1)
# define symlink(a,b)    (errno = ENOSYS, -1)
# define readlink(a,b,c) (errno = ENOSYS, -1)
# define mknod(a,b,c)    (errno = ENOSYS, -1)
# define truncate(a,b)   (errno = ENOSYS, -1)
# define ftruncate(fd,o) chsize ((fd), (o))
# define fsync(fd)       _commit (fd)
# define opendir(fd)     (errno = ENOSYS, 0)
# define readdir(fd)     (errno = ENOSYS, -1)
# define closedir(fd)    (errno = ENOSYS, -1)
# define mkdir(a,b)      mkdir (a)

#else

# include <sys/time.h>
# include <sys/select.h>
# include <unistd.h>
# include <utime.h>
# include <signal.h>
# define EIO_STRUCT_DIRENT struct dirent

#endif

/* perl stupidly overrides readdir and maybe others */
/* with thread-unsafe versions, imagine that :( */
#undef readdir
#undef opendir
#undef closedir

#define EIO_STRUCT_STAT Stat_t

/* use NV for 32 bit perls as it allows larger offsets */
#if IVSIZE >= 8
# define VAL64 IV
# define SvVAL64 SvIV
# define newSVval64 newSViv
#else
# define VAL64 NV
# define SvVAL64 SvNV
# define newSVval64 newSVnv
#endif

/*****************************************************************************/

#if __GNUC__ >= 3
# define expect(expr,value) __builtin_expect ((expr),(value))
#else
# define expect(expr,value) (expr)
#endif

#define expect_false(expr) expect ((expr) != 0, 0)
#define expect_true(expr)  expect ((expr) != 0, 1)

/*****************************************************************************/

typedef SV SV8; /* byte-sv, used for argument-checking */
typedef int aio_rfd; /* read file desriptor */
typedef int aio_wfd; /* write file descriptor */

static HV *aio_stash, *aio_req_stash, *aio_grp_stash;

#define EIO_REQ_MEMBERS	\
  SV *callback;		\
  SV *sv1, *sv2;	\
  STRLEN stroffset;	\
  SV *self;

#define EIO_NO_WRAPPERS 1

#include "libeio/config.h"
#include "libeio/eio.h"

#ifndef POSIX_FADV_NORMAL
# define POSIX_FADV_NORMAL 0
#endif
#ifndef POSIX_FADV_SEQUENTIAL
# define POSIX_FADV_SEQUENTIAL 0
#endif
#ifndef POSIX_FADV_RANDOM
# define POSIX_FADV_RANDOM 0
#endif
#ifndef POSIX_FADV_NOREUSE
# define POSIX_FADV_NOREUSE 0
#endif
#ifndef POSIX_FADV_WILLNEED
# define POSIX_FADV_WILLNEED 0
#endif
#ifndef POSIX_FADV_DONTNEED
# define POSIX_FADV_DONTNEED 0
#endif

#if !HAVE_POSIX_FADVISE
# define posix_fadvise(a,b,c,d) errno = ENOSYS /* also return ENOSYS */
#endif

#ifndef POSIX_MADV_NORMAL
# define POSIX_MADV_NORMAL 0
#endif
#ifndef POSIX_MADV_SEQUENTIAL
# define POSIX_MADV_SEQUENTIAL 0
#endif
#ifndef POSIX_MADV_RANDOM
# define POSIX_MADV_RANDOM 0
#endif
#ifndef POSIX_MADV_WILLNEED
# define POSIX_MADV_WILLNEED 0
#endif
#ifndef POSIX_MADV_DONTNEED
# define POSIX_MADV_DONTNEED 0
#endif

#if !HAVE_POSIX_MADVISE
# define posix_madvise(a,b,c) errno = ENOSYS /* also return ENOSYS */
#endif

#ifndef PROT_NONE
# define PROT_NONE 0
#endif
#ifndef PROT_READ
# define PROT_READ 0
#endif
#ifndef PROT_WRITE
# define PROT_READ 0
#endif
#ifndef PROT_EXEC
# define PROT_EXEC 0
#endif

#ifndef ST_NODEV
# define ST_NODEV       0
#endif
#ifndef ST_NOEXEC
# define ST_NOEXEC      0
#endif
#ifndef ST_SYNCHRONOUS
# define ST_SYNCHRONOUS 0
#endif
#ifndef ST_MANDLOCK
# define ST_MANDLOCK    0
#endif
#ifndef ST_WRITE
# define ST_WRITE       0
#endif
#ifndef ST_APPEND
# define ST_APPEND      0
#endif
#ifndef ST_IMMUTABLE
# define ST_IMMUTABLE   0
#endif
#ifndef ST_NOATIME
# define ST_NOATIME     0
#endif
#ifndef ST_NODIRATIME
# define ST_NODIRATIME  0
#endif
#ifndef ST_RELATIME
# define ST_RELATIME    0
#endif

#ifndef MAP_ANONYMOUS
# ifdef MAP_ANON
#  define MAP_ANONYMOUS MAP_ANON
# else
#  define MAP_ANONYMOUS MAP_FIXED /* and hope this fails */
# endif
#endif
#ifndef MAP_HUGETLB
# define MAP_HUGETLB    0
#endif
#ifndef MAP_LOCKED
# define MAP_LOCKED     0
#endif
#ifndef MAP_NORESERVE
# define MAP_NORESERVE  0
#endif
#ifndef MAP_POPULATE
# define MAP_POPULATE   0
#endif
#ifndef MAP_NONBLOCK
# define MAP_NONBLOCK   0
#endif

#ifndef PAGESIZE
# define PAGESIZE sysconf (_SC_PAGESIZE)
#endif

static int req_invoke    (eio_req *req);
#define EIO_FINISH(req)  req_invoke (req)
static void req_destroy  (eio_req *grp);
#define EIO_DESTROY(req) req_destroy (req)

enum {
  FLAG_SV2_RO_OFF = 0x40, /* data was set readonly */
};

#include "libeio/eio.c"

typedef eio_req *aio_req;
typedef eio_req *aio_req_ornot;

static SV *on_next_submit;
static int next_pri = EIO_PRI_DEFAULT;
static int max_outstanding;

static s_epipe respipe;

static void req_destroy (aio_req req);
static void req_cancel (aio_req req);

static void want_poll (void)
{
  /* write a dummy byte to the pipe so fh becomes ready */
  s_epipe_signal (&respipe);
}

static void done_poll (void)
{
  /* read any signals sent by the worker threads */
  s_epipe_drain (&respipe);
}

/* must be called at most once */
static SV *req_sv (aio_req req, HV *stash)
{
  if (!req->self)
    {
      req->self = (SV *)newHV ();
      sv_magic (req->self, 0, PERL_MAGIC_ext, (char *)req, 0);
    }

  return sv_2mortal (sv_bless (newRV_inc (req->self), stash));
}

static aio_req SvAIO_REQ (SV *sv)
{
  MAGIC *mg;

  if (!SvROK (sv)
      || (SvSTASH (SvRV (sv)) != aio_grp_stash
          && SvSTASH (SvRV (sv)) != aio_req_stash
          && !sv_derived_from (sv, "IO::AIO::REQ")))
    croak ("object of class IO::AIO::REQ expected");

  mg = mg_find (SvRV (sv), PERL_MAGIC_ext);

  return mg ? (aio_req)mg->mg_ptr : 0;
}

static void aio_grp_feed (aio_req grp)
{
  if (grp->sv2 && SvOK (grp->sv2))
    {
      dSP;

      ENTER;
      SAVETMPS;
      PUSHMARK (SP);
      XPUSHs (req_sv (grp, aio_grp_stash));
      PUTBACK;
      call_sv (grp->sv2, G_VOID | G_EVAL | G_KEEPERR);
      SPAGAIN;
      FREETMPS;
      LEAVE;
    }
}

static void req_submit (eio_req *req)
{
  eio_submit (req);

  if (expect_false (on_next_submit))
    {
      dSP;
      SV *cb = sv_2mortal (on_next_submit);

      on_next_submit = 0;

      PUSHMARK (SP);
      PUTBACK;
      call_sv (cb, G_DISCARD | G_EVAL);
    }
}

static int req_invoke (eio_req *req)
{
  if (req->flags & FLAG_SV2_RO_OFF)
    SvREADONLY_off (req->sv2);

  if (!EIO_CANCELLED (req) && req->callback)
    {
      dSP;
      static SV *sv_result_cache; /* caches the result integer SV */
      SV *sv_result;

      ENTER;
      SAVETMPS;
      PUSHMARK (SP);
      EXTEND (SP, 1);

      /* do not recreate the result IV from scratch each time */
      if (expect_true (sv_result_cache))
        {
          sv_result = sv_result_cache; sv_result_cache = 0;
          SvIV_set (sv_result, req->result);
          SvIOK_only (sv_result);
        }
      else
        {
          sv_result = newSViv (req->result);
          SvREADONLY_on (sv_result);
        }

      switch (req->type)
        {
          case EIO_READDIR:
            {
              SV *rv = &PL_sv_undef;

              if (req->result >= 0)
                {
                  int i;
                  char *names = (char *)req->ptr2;
                  eio_dirent *ent = (eio_dirent *)req->ptr1; /* might be 0 */
                  AV *av = newAV ();

                  av_extend (av, req->result - 1);

                  for (i = 0; i < req->result; ++i)
                    {
                      if (req->int1 & EIO_READDIR_DENTS)
                        {
                          SV *namesv = newSVpvn (names + ent->nameofs, ent->namelen);

                          if (req->int1 & EIO_READDIR_CUSTOM2)
                            {
                              static SV *sv_type [EIO_DT_MAX + 1]; /* type sv cache */
                              AV *avent = newAV ();

                              av_extend (avent, 2);

                              if (!sv_type [ent->type])
                                {
                                  sv_type [ent->type] = newSViv (ent->type);
                                  SvREADONLY_on (sv_type [ent->type]);
                                }

                              av_store (avent, 0, namesv);
                              av_store (avent, 1, SvREFCNT_inc (sv_type [ent->type]));
                              av_store (avent, 2, IVSIZE >= 8 ? newSVuv (ent->inode) : newSVnv (ent->inode));

                              av_store (av, i, newRV_noinc ((SV *)avent));
                            }
                          else
                            av_store (av, i, namesv);

                          ++ent;
                        }
                      else
                        {
                          SV *name = newSVpv (names, 0);
                          av_store (av, i, name);
                          names += SvCUR (name) + 1;
                        }
                    }

                  rv = sv_2mortal (newRV_noinc ((SV *)av));
                }

              PUSHs (rv);

              if (req->int1 & EIO_READDIR_CUSTOM1)
                XPUSHs (sv_2mortal (newSViv (req->int1 & ~(EIO_READDIR_CUSTOM1 | EIO_READDIR_CUSTOM2))));
            }
            break;

          case EIO_OPEN:
            {
              /* convert fd to fh */
              SV *fh = &PL_sv_undef;

              if (req->result >= 0)
                {
                  GV *gv = (GV *)sv_newmortal ();
                  int flags = req->int1 & (O_RDONLY | O_WRONLY | O_RDWR);
                  char sym [64];
                  int symlen;
                  
                  symlen = snprintf (sym, sizeof (sym), "fd#%d", (int)req->result);
                  gv_init (gv, aio_stash, sym, symlen, 0);

                  symlen = snprintf (
                     sym,
                     sizeof (sym),
                     "%s&=%d",
                     flags == O_RDONLY ? "<" : flags == O_WRONLY ? ">" : "+<",
                     (int)req->result
                  );

                  if (do_open (gv, sym, symlen, 0, 0, 0, 0))
                    fh = (SV *)gv;
                }

              PUSHs (fh);
            }
            break;

          case EIO_STATVFS:
          case EIO_FSTATVFS:
            {
              SV *rv = &PL_sv_undef;
             
              if (req->result >= 0)
                {
                  EIO_STRUCT_STATVFS *f = EIO_STATVFS_BUF (req);
                  HV *hv = newHV ();

                  rv = sv_2mortal (newRV_noinc ((SV *)hv));

                  hv_store (hv, "bsize"  , sizeof ("bsize"  ) - 1, newSVval64 (f->f_bsize  ), 0);
                  hv_store (hv, "frsize" , sizeof ("frsize" ) - 1, newSVval64 (f->f_frsize ), 0);
                  hv_store (hv, "blocks" , sizeof ("blocks" ) - 1, newSVval64 (f->f_blocks ), 0);
                  hv_store (hv, "bfree"  , sizeof ("bfree"  ) - 1, newSVval64 (f->f_bfree  ), 0);
                  hv_store (hv, "bavail" , sizeof ("bavail" ) - 1, newSVval64 (f->f_bavail ), 0);
                  hv_store (hv, "files"  , sizeof ("files"  ) - 1, newSVval64 (f->f_files  ), 0);
                  hv_store (hv, "ffree"  , sizeof ("ffree"  ) - 1, newSVval64 (f->f_ffree  ), 0);
                  hv_store (hv, "favail" , sizeof ("favail" ) - 1, newSVval64 (f->f_favail ), 0);
                  hv_store (hv, "fsid"   , sizeof ("fsid"   ) - 1, newSVval64 (f->f_fsid   ), 0);
                  hv_store (hv, "flag"   , sizeof ("flag"   ) - 1, newSVval64 (f->f_flag   ), 0);
                  hv_store (hv, "namemax", sizeof ("namemax") - 1, newSVval64 (f->f_namemax), 0);
                }

              PUSHs (rv);
            }

            break;

          case EIO_GROUP:
            req->int1 = 2; /* mark group as finished */

            if (req->sv1)
              {
                int i;
                AV *av = (AV *)req->sv1;

                EXTEND (SP, AvFILL (av) + 1);
                for (i = 0; i <= AvFILL (av); ++i)
                  PUSHs (*av_fetch (av, i, 0));
              }
            break;

          case EIO_NOP:
          case EIO_BUSY:
            break;

          case EIO_READLINK:
            if (req->result > 0)
              PUSHs (sv_2mortal (newSVpvn (req->ptr2, req->result)));
            break;

          case EIO_STAT:
          case EIO_LSTAT:
          case EIO_FSTAT:
            PL_laststype   = req->type == EIO_LSTAT ? OP_LSTAT : OP_STAT;
            PL_laststatval = req->result;
            PL_statcache   = *(EIO_STRUCT_STAT *)(req->ptr2);
            PUSHs (sv_result);
            break;

          case EIO_READ:
            {
              SvCUR_set (req->sv2, req->stroffset + (req->result > 0 ? req->result : 0));
              *SvEND (req->sv2) = 0;
              SvPOK_only (req->sv2);
              SvSETMAGIC (req->sv2);
              PUSHs (sv_result);
            }
            break;

          case EIO_DUP2: /* EIO_DUP2 actually means aio_close(), so fudge result value */
            if (req->result > 0)
              SvIV_set (sv_result, 0);
            /* FALLTHROUGH */

          default:
            PUSHs (sv_result);
            break;
        }

      errno = req->errorno;

      PUTBACK;
      call_sv (req->callback, G_VOID | G_EVAL | G_DISCARD);
      SPAGAIN;

      if (expect_false (SvREFCNT (sv_result) != 1 || sv_result_cache))
        SvREFCNT_dec (sv_result);
      else
        sv_result_cache = sv_result;

      FREETMPS;
      LEAVE;

      PUTBACK;
    }

  return !!SvTRUE (ERRSV);
}

static void req_destroy (aio_req req)
{
  if (req->self)
    {
      sv_unmagic (req->self, PERL_MAGIC_ext);
      SvREFCNT_dec (req->self);
    }

  SvREFCNT_dec (req->sv1);
  SvREFCNT_dec (req->sv2);
  SvREFCNT_dec (req->callback);

  Safefree (req);
}

static void req_cancel_subs (aio_req grp)
{
  aio_req sub;

  if (grp->type != EIO_GROUP)
    return;

  SvREFCNT_dec (grp->sv2);
  grp->sv2 = 0;

  eio_grp_cancel (grp);
}

static void
create_respipe (void)
{
  if (s_epipe_renew (&respipe))
    croak ("IO::AIO: unable to initialize result pipe");
}

static void poll_wait (void)
{
  while (eio_nreqs ())
    {
      int size;

      X_LOCK   (reslock);
      size = res_queue.size;
      X_UNLOCK (reslock);

      if (size)
        return;

      etp_maybe_start_thread ();

      s_epipe_wait (&respipe);
    }
}

static int poll_cb (void)
{
  for (;;)
    {
      int res = eio_poll ();

      if (res > 0)
        croak (0);

      if (!max_outstanding || max_outstanding > eio_nreqs ())
        return res;

      poll_wait ();
    }
}

static void atfork_child (void)
{
  create_respipe ();
}

/*****************************************************************************/

#if !_POSIX_MAPPED_FILES
# define mmap(addr,length,prot,flags,fd,offs) (errno = ENOSYS, -1)
# define munmap(addr,length)                  (errno = ENOSYS, -1)
# define mprotect(addr,len,prot)              (errno = ENOSYS, -1)
# define PROT_NONE   0
# define PROT_WRITE  0
# define MAP_PRIVATE 0
# define MAP_SHARED  0
# define MAP_FIXED   0
#endif

#define MMAP_MAGIC PERL_MAGIC_ext

static int
mmap_free (pTHX_ SV *sv, MAGIC *mg)
{
  int old_errno = errno;
  munmap (mg->mg_ptr, (size_t)mg->mg_obj);
  errno = old_errno;

  mg->mg_obj = 0; /* just in case */

  SvREADONLY_off (sv);

  if (SvPVX (sv) != mg->mg_ptr)
    croak ("ERROR: IO::AIO::mmap-mapped scalar changed location, detected");

  SvCUR_set (sv, 0);
  SvPVX (sv) = 0;
  SvOK_off (sv);

  return 0;
}

static MGVTBL mmap_vtbl = {
  0, 0, 0, 0, mmap_free
};

/*****************************************************************************/

static SV *
get_cb (SV *cb_sv)
{
  SvGETMAGIC (cb_sv);
  return SvOK (cb_sv) ? s_get_cv_croak (cb_sv) : 0;
}

#define dREQ							\
  SV *cb_cv;							\
  aio_req req;							\
  int req_pri = next_pri;					\
  next_pri = EIO_PRI_DEFAULT;					\
								\
  cb_cv = get_cb (callback);					\
								\
  Newz (0, req, 1, eio_req);					\
  if (!req)							\
    croak ("out of memory during eio_req allocation");		\
								\
  req->callback = SvREFCNT_inc (cb_cv);				\
  req->pri = req_pri

#define REQ_SEND						\
  PUTBACK;							\
  req_submit (req);						\
  SPAGAIN;							\
								\
  if (GIMME_V != G_VOID)					\
    XPUSHs (req_sv (req, aio_req_stash));

MODULE = IO::AIO                PACKAGE = IO::AIO

PROTOTYPES: ENABLE

BOOT:
{
  static const struct {
    const char *name;
    IV iv;
  } *civ, const_iv[] = {
#   define const_niv(name, value) { # name, (IV) value },
#   define const_iv(name) { # name, (IV) name },
#   define const_eio(name) { # name, (IV) EIO_ ## name },
    const_iv (EXDEV)
    const_iv (ENOSYS)
    const_iv (O_RDONLY)
    const_iv (O_WRONLY)
    const_iv (O_RDWR)
    const_iv (O_CREAT)
    const_iv (O_TRUNC)
    const_iv (O_EXCL)
    const_iv (O_APPEND)
#ifndef _WIN32
    const_iv (S_IFIFO)
#endif
    const_niv (FADV_NORMAL    , POSIX_FADV_NORMAL)
    const_niv (FADV_SEQUENTIAL, POSIX_FADV_SEQUENTIAL)
    const_niv (FADV_RANDOM    , POSIX_FADV_RANDOM)
    const_niv (FADV_NOREUSE   , POSIX_FADV_NOREUSE)
    const_niv (FADV_WILLNEED  , POSIX_FADV_WILLNEED)
    const_niv (FADV_DONTNEED  , POSIX_FADV_DONTNEED)

    const_niv (MADV_NORMAL    , POSIX_MADV_NORMAL)
    const_niv (MADV_SEQUENTIAL, POSIX_MADV_SEQUENTIAL)
    const_niv (MADV_RANDOM    , POSIX_MADV_RANDOM)
    const_niv (MADV_WILLNEED  , POSIX_MADV_WILLNEED)
    const_niv (MADV_DONTNEED  , POSIX_MADV_DONTNEED)

    const_iv (ST_RDONLY)
    const_iv (ST_NOSUID)
    const_iv (ST_NODEV)
    const_iv (ST_NOEXEC)
    const_iv (ST_SYNCHRONOUS)
    const_iv (ST_MANDLOCK)
    const_iv (ST_WRITE)
    const_iv (ST_APPEND)
    const_iv (ST_IMMUTABLE)
    const_iv (ST_NOATIME)
    const_iv (ST_NODIRATIME)
    const_iv (ST_RELATIME)

    const_iv (PROT_NONE)
    const_iv (PROT_EXEC)
    const_iv (PROT_READ)
    const_iv (PROT_WRITE)

    /*const_iv (MAP_FIXED)*/
    const_iv (MAP_PRIVATE)
    const_iv (MAP_SHARED)
    const_iv (MAP_ANONYMOUS)

    /* linuxish */
    const_iv (MAP_HUGETLB)
    const_iv (MAP_LOCKED)
    const_iv (MAP_NORESERVE)
    const_iv (MAP_POPULATE)
    const_iv (MAP_NONBLOCK)

    const_eio (MCL_FUTURE)
    const_eio (MCL_CURRENT)

    const_eio (MS_ASYNC)
    const_eio (MS_INVALIDATE)
    const_eio (MS_SYNC)

    const_eio (MT_MODIFY)

    const_eio (SYNC_FILE_RANGE_WAIT_BEFORE)
    const_eio (SYNC_FILE_RANGE_WRITE)
    const_eio (SYNC_FILE_RANGE_WAIT_AFTER)

    const_eio (READDIR_DENTS)
    const_eio (READDIR_DIRS_FIRST)
    const_eio (READDIR_STAT_ORDER)
    const_eio (READDIR_FOUND_UNKNOWN)

    const_eio (DT_UNKNOWN)
    const_eio (DT_FIFO)
    const_eio (DT_CHR)
    const_eio (DT_DIR)
    const_eio (DT_BLK)
    const_eio (DT_REG)
    const_eio (DT_LNK)
    const_eio (DT_SOCK)
    const_eio (DT_WHT)
  };

  aio_stash     = gv_stashpv ("IO::AIO"     , 1);
  aio_req_stash = gv_stashpv ("IO::AIO::REQ", 1);
  aio_grp_stash = gv_stashpv ("IO::AIO::GRP", 1);

  for (civ = const_iv + sizeof (const_iv) / sizeof (const_iv [0]); civ-- > const_iv; )
    newCONSTSUB (aio_stash, (char *)civ->name, newSViv (civ->iv));

  newCONSTSUB (aio_stash, "PAGESIZE", newSViv (PAGESIZE));

  create_respipe ();

  if (eio_init (want_poll, done_poll) < 0)
    croak ("IO::AIO: unable to initialise eio library");

  /* atfork child called in fifo order, so before eio's handler */
  X_THREAD_ATFORK (0, 0, atfork_child);
}

void
max_poll_reqs (int nreqs)
	PROTOTYPE: $
        CODE:
        eio_set_max_poll_reqs (nreqs);

void
max_poll_time (double nseconds)
	PROTOTYPE: $
        CODE:
        eio_set_max_poll_time (nseconds);

void
min_parallel (int nthreads)
	PROTOTYPE: $
        CODE:
        eio_set_min_parallel (nthreads);

void
max_parallel (int nthreads)
	PROTOTYPE: $
        CODE:
        eio_set_max_parallel (nthreads);

void
max_idle (int nthreads)
	PROTOTYPE: $
        CODE:
        eio_set_max_idle (nthreads);

void
max_outstanding (int maxreqs)
	PROTOTYPE: $
        CODE:
        max_outstanding = maxreqs;

void
aio_open (SV8 *pathname, int flags, int mode, SV *callback=&PL_sv_undef)
	PPCODE:
{
        dREQ;

        req->type = EIO_OPEN;
        req->sv1  = newSVsv (pathname);
        req->ptr1 = SvPVbyte_nolen (req->sv1);
        req->int1 = flags;
        req->int2 = mode;

        REQ_SEND;
}

void
aio_fsync (SV *fh, SV *callback=&PL_sv_undef)
        ALIAS:
           aio_fsync     = EIO_FSYNC
           aio_fdatasync = EIO_FDATASYNC
	PPCODE:
{
  	int fd = s_fileno_croak (fh, 0);
        dREQ;

        req->type = ix;
        req->sv1  = newSVsv (fh);
        req->int1 = fd;

        REQ_SEND (req);
}

void
aio_sync_file_range (SV *fh, off_t offset, size_t nbytes, UV flags, SV *callback=&PL_sv_undef)
	PPCODE:
{
  	int fd = s_fileno_croak (fh, 0);
        dREQ;

        req->type = EIO_SYNC_FILE_RANGE;
        req->sv1  = newSVsv (fh);
        req->int1 = fd;
        req->offs = offset;
        req->size = nbytes;
        req->int2 = flags;

        REQ_SEND (req);
}

void
aio_close (SV *fh, SV *callback=&PL_sv_undef)
	PPCODE:
{
        static int close_pipe = -1; /* dummy fd to close fds via dup2 */
  	int fd = s_fileno_croak (fh, 0);
        dREQ;

        if (close_pipe < 0)
          {
            int pipefd [2];

            if (pipe (pipefd) < 0
                || close (pipefd [1]) < 0
                || fcntl (pipefd [0], F_SETFD, FD_CLOEXEC) < 0)
              abort (); /*D*/

            close_pipe = pipefd [0];
          }

        req->type = EIO_DUP2;
        req->int1 = close_pipe;
        req->sv2  = newSVsv (fh);
        req->int2 = fd;

        REQ_SEND (req);
}

void
aio_read (SV *fh, SV *offset, SV *length, SV8 *data, IV dataoffset, SV *callback=&PL_sv_undef)
        ALIAS:
           aio_read  = EIO_READ
           aio_write = EIO_WRITE
        PPCODE:
{
        STRLEN svlen;
        int fd = s_fileno_croak (fh, ix == EIO_WRITE);
        char *svptr = SvPVbyte (data, svlen);
        UV len = SvUV (length);

        if (dataoffset < 0)
          dataoffset += svlen;

        if (dataoffset < 0 || dataoffset > svlen)
          croak ("dataoffset outside of data scalar");

        if (ix == EIO_WRITE)
          {
            /* write: check length and adjust. */
            if (!SvOK (length) || len + dataoffset > svlen)
              len = svlen - dataoffset;
          }
        else
          {
            /* read: check type and grow scalar as necessary */
            SvUPGRADE (data, SVt_PV);
            svptr = SvGROW (data, len + dataoffset + 1);
          }

        {
          dREQ;

          req->type = ix;
          req->sv1  = newSVsv (fh);
          req->int1 = fd;
          req->offs = SvOK (offset) ? SvVAL64 (offset) : -1;
          req->size = len;
          req->sv2  = SvREFCNT_inc (data);
          req->ptr2 = (char *)svptr + dataoffset;
          req->stroffset = dataoffset;

          if (!SvREADONLY (data))
            {
              SvREADONLY_on (data);
              req->flags |= FLAG_SV2_RO_OFF;
            }

          REQ_SEND;
        }
}

void
aio_readlink (SV8 *path, SV *callback=&PL_sv_undef)
        PPCODE:
{
	SV *data;
        dREQ;

        req->type = EIO_READLINK;
        req->sv1  = newSVsv (path);
        req->ptr1 = SvPVbyte_nolen (req->sv1);

        REQ_SEND;
}

void
aio_sendfile (SV *out_fh, SV *in_fh, off_t in_offset, size_t length, SV *callback=&PL_sv_undef)
        PPCODE:
{
  	int ifd = s_fileno_croak (in_fh , 0);
  	int ofd = s_fileno_croak (out_fh, 1);
	dREQ;

        req->type = EIO_SENDFILE;
        req->sv1  = newSVsv (out_fh);
        req->int1 = ofd;
        req->sv2  = newSVsv (in_fh);
        req->int2 = ifd;
        req->offs = in_offset;
        req->size = length;

        REQ_SEND;
}

void
aio_readahead (SV *fh, off_t offset, size_t length, SV *callback=&PL_sv_undef)
        PPCODE:
{
  	int fd = s_fileno_croak (fh, 0);
	dREQ;

        req->type = EIO_READAHEAD;
        req->sv1  = newSVsv (fh);
        req->int1 = fd;
        req->offs = offset;
        req->size = length;

        REQ_SEND;
}

void
aio_stat (SV8 *fh_or_path, SV *callback=&PL_sv_undef)
        ALIAS:
           aio_stat    = EIO_STAT
           aio_lstat   = EIO_LSTAT
           aio_statvfs = EIO_STATVFS
	PPCODE:
{
	dREQ;

        req->sv1 = newSVsv (fh_or_path);

        if (SvPOK (req->sv1))
          {
            req->type = ix;
            req->ptr1 = SvPVbyte_nolen (req->sv1);
          }
        else
          {
            req->type = ix == EIO_STATVFS ? EIO_FSTATVFS : EIO_FSTAT;
            req->int1 = PerlIO_fileno (IoIFP (sv_2io (fh_or_path)));
          }

        REQ_SEND;
}

void
aio_utime (SV8 *fh_or_path, SV *atime, SV *mtime, SV *callback=&PL_sv_undef)
	PPCODE:
{
	dREQ;

        req->nv1 = SvOK (atime) ? SvNV (atime) : -1.;
        req->nv2 = SvOK (mtime) ? SvNV (mtime) : -1.;
        req->sv1 = newSVsv (fh_or_path);

        if (SvPOK (req->sv1))
          {
            req->type = EIO_UTIME;
            req->ptr1 = SvPVbyte_nolen (req->sv1);
          }
        else
          {
            req->type = EIO_FUTIME;
            req->int1 = PerlIO_fileno (IoIFP (sv_2io (fh_or_path)));
          }

        REQ_SEND;
}

void
aio_truncate (SV8 *fh_or_path, SV *offset, SV *callback=&PL_sv_undef)
	PPCODE:
{
	dREQ;

        req->sv1  = newSVsv (fh_or_path);
        req->offs = SvOK (offset) ? SvVAL64 (offset) : -1;

        if (SvPOK (req->sv1))
          {
            req->type = EIO_TRUNCATE;
            req->ptr1 = SvPVbyte_nolen (req->sv1);
          }
        else
          {
            req->type = EIO_FTRUNCATE;
            req->int1 = PerlIO_fileno (IoIFP (sv_2io (fh_or_path)));
          }

        REQ_SEND;
}

void
aio_chmod (SV8 *fh_or_path, int mode, SV *callback=&PL_sv_undef)
	ALIAS:
        aio_chmod  = EIO_CHMOD
        aio_mkdir  = EIO_MKDIR
	PPCODE:
{
	dREQ;

        req->int2 = mode;
        req->sv1  = newSVsv (fh_or_path);

        if (SvPOK (req->sv1))
          {
            req->type = ix;
            req->ptr1 = SvPVbyte_nolen (req->sv1);
          }
        else
          {
            req->type = EIO_FCHMOD;
            req->int1 = PerlIO_fileno (IoIFP (sv_2io (fh_or_path)));
          }

        REQ_SEND;
}

void
aio_chown (SV8 *fh_or_path, SV *uid, SV *gid, SV *callback=&PL_sv_undef)
	PPCODE:
{
	dREQ;

        req->int2 = SvOK (uid) ? SvIV (uid) : -1;
        req->int3 = SvOK (gid) ? SvIV (gid) : -1;
        req->sv1  = newSVsv (fh_or_path);

        if (SvPOK (req->sv1))
          {
            req->type = EIO_CHOWN;
            req->ptr1 = SvPVbyte_nolen (req->sv1);
          }
        else
          {
            req->type = EIO_FCHOWN;
            req->int1 = PerlIO_fileno (IoIFP (sv_2io (fh_or_path)));
          }

        REQ_SEND;
}

void
aio_readdirx (SV8 *pathname, IV flags, SV *callback=&PL_sv_undef)
	PPCODE:
{
	dREQ;
	
        req->type = EIO_READDIR;
	req->sv1  = newSVsv (pathname);
	req->ptr1 = SvPVbyte_nolen (req->sv1);
        req->int1 = flags | EIO_READDIR_DENTS | EIO_READDIR_CUSTOM1;

        if (flags & EIO_READDIR_DENTS)
          req->int1 |= EIO_READDIR_CUSTOM2;

	REQ_SEND;
}

void
aio_unlink (SV8 *pathname, SV *callback=&PL_sv_undef)
        ALIAS:
           aio_unlink  = EIO_UNLINK
           aio_rmdir   = EIO_RMDIR
           aio_readdir = EIO_READDIR
	PPCODE:
{
	dREQ;
	
        req->type = ix;
	req->sv1  = newSVsv (pathname);
	req->ptr1 = SvPVbyte_nolen (req->sv1);

	REQ_SEND;
}

void
aio_link (SV8 *oldpath, SV8 *newpath, SV *callback=&PL_sv_undef)
        ALIAS:
           aio_link    = EIO_LINK
           aio_symlink = EIO_SYMLINK
           aio_rename  = EIO_RENAME
	PPCODE:
{
	dREQ;
	
        req->type = ix;
	req->sv1  = newSVsv (oldpath);
	req->ptr1 = SvPVbyte_nolen (req->sv1);
	req->sv2  = newSVsv (newpath);
	req->ptr2 = SvPVbyte_nolen (req->sv2);
	
	REQ_SEND;
}

void
aio_mknod (SV8 *pathname, int mode, UV dev, SV *callback=&PL_sv_undef)
	PPCODE:
{
	dREQ;
	
        req->type = EIO_MKNOD;
	req->sv1  = newSVsv (pathname);
	req->ptr1 = SvPVbyte_nolen (req->sv1);
        req->int2 = (mode_t)mode;
        req->offs = dev;
	
	REQ_SEND;
}

void
aio_mtouch (SV8 *data, IV offset = 0, SV *length = &PL_sv_undef, int flags = 0, SV *callback=&PL_sv_undef)
        ALIAS:
           aio_mtouch = EIO_MTOUCH
           aio_msync  = EIO_MSYNC
        PPCODE:
{
        STRLEN svlen;
        char *svptr = SvPVbyte (data, svlen);
        UV len = SvUV (length);

        if (offset < 0)
          offset += svlen;

        if (offset < 0 || offset > svlen)
          croak ("offset outside of scalar");

        if (!SvOK (length) || len + offset > svlen)
          len = svlen - offset;

        {
          dREQ;

          req->type = ix;
          req->sv2  = SvREFCNT_inc (data);
          req->ptr2 = (char *)svptr + offset;
          req->size = len;
          req->int1 = flags;

          REQ_SEND;
        }
}

void
aio_mlock (SV8 *data, IV offset = 0, SV *length = &PL_sv_undef, SV *callback=&PL_sv_undef)
        PPCODE:
{
        STRLEN svlen;
        char *svptr = SvPVbyte (data, svlen);
        UV len = SvUV (length);

        if (offset < 0)
          offset += svlen;

        if (offset < 0 || offset > svlen)
          croak ("offset outside of scalar");

        if (!SvOK (length) || len + offset > svlen)
          len = svlen - offset;

        {
          dREQ;

          req->type = EIO_MLOCK;
          req->sv2  = SvREFCNT_inc (data);
          req->ptr2 = (char *)svptr + offset;
          req->size = len;

          REQ_SEND;
        }
}

void
aio_mlockall (IV flags, SV *callback=&PL_sv_undef)
        PPCODE:
{
        dREQ;

        req->type = EIO_MLOCKALL;
        req->int1 = flags;

        REQ_SEND;
}

void
aio_busy (double delay, SV *callback=&PL_sv_undef)
	PPCODE:
{
	dREQ;

        req->type = EIO_BUSY;
        req->nv1  = delay < 0. ? 0. : delay;

	REQ_SEND;
}

void
aio_group (SV *callback=&PL_sv_undef)
        PPCODE:
{
	dREQ;

        req->type = EIO_GROUP;

        req_submit (req);
        XPUSHs (req_sv (req, aio_grp_stash));
}

void
aio_nop (SV *callback=&PL_sv_undef)
	ALIAS:
           aio_nop  = EIO_NOP
           aio_sync = EIO_SYNC
	PPCODE:
{
	dREQ;

        req->type = ix;

	REQ_SEND;
}

int
aioreq_pri (int pri = 0)
	CODE:
	RETVAL = next_pri;
	if (items > 0)
	  {
	    if (pri < EIO_PRI_MIN) pri = EIO_PRI_MIN;
	    if (pri > EIO_PRI_MAX) pri = EIO_PRI_MAX;
	    next_pri = pri;
	  }
	OUTPUT:
	RETVAL

void
aioreq_nice (int nice = 0)
	CODE:
	nice = next_pri - nice;
	if (nice < EIO_PRI_MIN) nice = EIO_PRI_MIN;
	if (nice > EIO_PRI_MAX) nice = EIO_PRI_MAX;
	next_pri = nice;

void
flush ()
	CODE:
        while (eio_nreqs ())
          {
            poll_wait ();
            poll_cb ();
          }

int
poll ()
	CODE:
        poll_wait ();
        RETVAL = poll_cb ();
	OUTPUT:
	RETVAL

int
poll_fileno ()
	CODE:
        RETVAL = s_epipe_fd (&respipe);
	OUTPUT:
	RETVAL

int
poll_cb (...)
	PROTOTYPE:
	CODE:
        RETVAL = poll_cb ();
	OUTPUT:
	RETVAL

void
poll_wait ()
	CODE:
        poll_wait ();

int
nreqs ()
	CODE:
        RETVAL = eio_nreqs ();
	OUTPUT:
	RETVAL

int
nready ()
	CODE:
        RETVAL = eio_nready ();
	OUTPUT:
	RETVAL

int
npending ()
	CODE:
        RETVAL = eio_npending ();
	OUTPUT:
	RETVAL

int
nthreads ()
	CODE:
        RETVAL = eio_nthreads ();
	OUTPUT:
	RETVAL

int
fadvise (aio_rfd fh, off_t offset, off_t length, IV advice)
        CODE:
        RETVAL = posix_fadvise (fh, offset, length, advice);
	OUTPUT:
        RETVAL

ssize_t
sendfile (aio_wfd ofh, aio_rfd ifh, off_t offset, size_t count)
        CODE:
        RETVAL = eio_sendfile_sync (ofh, ifh, offset, count);
	OUTPUT:
        RETVAL

void
mmap (SV *scalar, size_t length, int prot, int flags, SV *fh, off_t offset = 0)
	PPCODE:
        sv_unmagic (scalar, MMAP_MAGIC);
{
        int fd = SvOK (fh) ? s_fileno_croak (fh, flags & PROT_WRITE) : -1;
        void *addr = (void *)mmap (0, length, prot, flags, fd, offset);
	if (addr == (void *)-1)
	  XSRETURN_NO;

        sv_force_normal (scalar);

        /* we store the length in mg_obj, as namlen is I32 :/ */
        sv_magicext (scalar, 0, MMAP_MAGIC, &mmap_vtbl, (char *)addr, 0)
          ->mg_obj = (SV *)length;

	SvUPGRADE (scalar, SVt_PV); /* nop... */

	if (!(prot & PROT_WRITE))
	  SvREADONLY_on (scalar);

        if (SvLEN (scalar))
          Safefree (SvPVX (scalar));

	SvPVX (scalar) = (char *)addr;
	SvCUR_set (scalar, length);
	SvLEN_set (scalar, 0);
	SvPOK_only (scalar);

        XSRETURN_YES;
}

void
munmap (SV *scalar)
	CODE:
        sv_unmagic (scalar, MMAP_MAGIC);

int
madvise (SV *scalar, off_t offset = 0, SV *length = &PL_sv_undef, IV advice_or_prot)
	ALIAS:
        mprotect = 1
        CODE:
{
	STRLEN svlen;
  	void *addr = SvPVbyte (scalar, svlen);
        size_t len = SvUV (length);

        if (offset < 0)
          offset += svlen;

        if (offset < 0 || offset > svlen)
          croak ("offset outside of scalar");

        if (!SvOK (length) || len + offset > svlen)
          len = svlen - offset;

        addr = (void *)(((intptr_t)addr) + offset);
        eio_page_align (&addr, &len);

        switch (ix)
          {
            case 0: RETVAL = posix_madvise (addr, len, advice_or_prot); break;
            case 1: RETVAL = mprotect      (addr, len, advice_or_prot); break;
          }
}
	OUTPUT:
        RETVAL

int
munlock (SV *scalar, off_t offset = 0, SV *length = &PL_sv_undef)
        CODE:
{
	STRLEN svlen;
  	void *addr = SvPVbyte (scalar, svlen);
        size_t len = SvUV (length);

        if (offset < 0)
          offset += svlen;

        if (offset < 0 || offset > svlen)
          croak ("offset outside of scalar");

        if (!SvOK (length) || len + offset > svlen)
          len = svlen - offset;

        addr = (void *)(((intptr_t)addr) + offset);
        eio_page_align (&addr, &len);
#if _POSIX_MEMLOCK
        RETVAL = munlock (addr, len);
#else
        RETVAL = ((errno = ENOSYS), -1);
#endif
}
        OUTPUT:
        RETVAL

int
munlockall ()
        CODE:
#if _POSIX_MEMLOCK
        munlockall ();
#else
        RETVAL = -1;
        errno = ENOSYS;
#endif
        OUTPUT:
        RETVAL

void _on_next_submit (SV *cb)
	CODE:
        SvREFCNT_dec (on_next_submit);
        on_next_submit = SvOK (cb) ? newSVsv (cb) : 0;

PROTOTYPES: DISABLE

MODULE = IO::AIO                PACKAGE = IO::AIO::REQ

void
cancel (aio_req_ornot req)
	CODE:
        eio_cancel (req);

void
cb (aio_req_ornot req, SV *callback=&PL_sv_undef)
	PPCODE:
{
        if (GIMME_V != G_VOID)
          XPUSHs (req->callback ? sv_2mortal (newRV_inc (req->callback)) : &PL_sv_undef);

        if (items > 1)
          {
            SV *cb_cv =get_cb (callback);

            SvREFCNT_dec (req->callback);
            req->callback = SvREFCNT_inc (cb_cv);
          }
}

MODULE = IO::AIO                PACKAGE = IO::AIO::GRP

void
add (aio_req grp, ...)
        PPCODE:
{
	int i;

        if (grp->int1 == 2)
          croak ("cannot add requests to IO::AIO::GRP after the group finished");

	for (i = 1; i < items; ++i )
          {
            aio_req req;

            if (GIMME_V != G_VOID)
              XPUSHs (sv_2mortal (newSVsv (ST (i))));

            req = SvAIO_REQ (ST (i));

            if (req)
              eio_grp_add (grp, req);
          }
}

void
cancel_subs (aio_req_ornot req)
	CODE:
        req_cancel_subs (req);

void
result (aio_req grp, ...)
        CODE:
{
        int i;
        AV *av;

        grp->errorno = errno;

        av = newAV ();
        av_extend (av, items - 1);

        for (i = 1; i < items; ++i )
          av_push (av, newSVsv (ST (i)));

        SvREFCNT_dec (grp->sv1);
        grp->sv1 = (SV *)av;
}

void
errno (aio_req grp, int errorno = errno)
        CODE:
        grp->errorno = errorno;

void
limit (aio_req grp, int limit)
	CODE:
        eio_grp_limit (grp, limit);

void
feed (aio_req grp, SV *callback=&PL_sv_undef)
	CODE:
{
        SvREFCNT_dec (grp->sv2);
        grp->sv2  = newSVsv (callback);
        grp->feed = aio_grp_feed;

        if (grp->int2 <= 0)
          grp->int2 = 2;

        eio_grp_limit (grp, grp->int2);
}

