#ifndef UAE_SYSDEPS_H
#define UAE_SYSDEPS_H

/*
  * UAE - The Un*x Amiga Emulator
  *
  * Try to include the right system headers and get other system-specific
  * stuff right & other collected kludges.
  *
  * If you think about modifying this, think twice. Some systems rely on
  * the exact order of the #include statements. That's also the reason
  * why everything gets included unconditionally regardless of whether
  * it's actually needed by the .c file.
  *
  * Copyright 1996, 1997 Bernd Schmidt
  */

#define ECS_DENISE

#ifdef JIT
#define NATMEM_OFFSET natmem_offset
#else
#undef NATMEM_OFFSET
#endif

#if defined __AMIGA__ || defined __amiga__
#include <devices/timer.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>

#ifndef __STDC__
#ifndef _MSC_VER
#error "Your compiler is not ANSI. Get a real one."
#endif
#endif

#include <stdarg.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_VALUES_H
#include <values.h>
#endif

#include "uae_string.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UTIME_H
#include <utime.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#ifndef stat64
#define stat64 stat
#endif
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
#else
# define dirent direct
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#ifdef HAVE_SYS_UTIME_H
# include <sys/utime.h>
#endif

#include <errno.h>
#include <assert.h>

#if EEXIST == ENOTEMPTY
#define BROKEN_OS_PROBABLY_AIX
#endif

#ifdef __NeXT__
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#define S_ISDIR(val) (S_IFDIR & val)
struct utimbuf
{
    time_t actime;
    time_t modtime;
};
#endif

#include "uae_types.h"

#include "uae_malloc.h"

#include "writelog.h"

#ifdef __GNUC__
/* While we're here, make abort more useful.  */
#ifndef __MORPHOS__
/* This fails to compile on Morphos - not sure why yet */
#define abort() \
  do { \
    write_log ("Internal error; file %s, line %d\n", __FILE__, __LINE__); \
    exit (0); \
} while (0)
#else
#define abort() exit(0)
#endif
#endif

/*
 * Porters to weird systems, look! This is the preferred way to get
 * filesys.c (and other stuff) running on your system. Define the
 * appropriate macros and implement wrappers in a machine-specific file.
 *
 * I guess the Mac port could use this (Ernesto?)
 */

#undef DONT_HAVE_POSIX
#undef DONT_HAVE_REAL_POSIX /* define if open+delete doesn't do what it should */
#undef DONT_HAVE_STDIO
#undef DONT_HAVE_MALLOC

#if defined(WARPUP)
#define DONT_HAVE_POSIX
#endif

#define FILEFLAG_DIR     0x1
#define FILEFLAG_ARCHIVE 0x2
#define FILEFLAG_WRITE   0x4
#define FILEFLAG_READ    0x8
#define FILEFLAG_EXECUTE 0x10
#define FILEFLAG_SCRIPT  0x20
#define FILEFLAG_PURE    0x40

#if defined _WIN32

#if defined __WATCOMC__

#define O_NDELAY 0
#include <direct.h>
#define dirent direct
#define mkdir(a,b) mkdir(a)
#define strcasecmp stricmp

#elif defined __MINGW32__

#define O_NDELAY 0
#define mkdir(a,b) mkdir(a)

#elif defined _MSC_VER

#ifdef HAVE_GETTIMEOFDAY
#include <winsock.h> // for 'struct timeval' definition
extern void gettimeofday( struct timeval *tv, void *blah );
#endif

#define O_NDELAY 0

#ifdef REGPARAM2
#undef REGPARAM2
#endif
#define REGPARAM2 __fastcall
#define REGPARAM3 __fastcall
#define REGPARAM

#include <io.h>
#define O_BINARY _O_BINARY
#define O_WRONLY _O_WRONLY
#define O_RDONLY _O_RDONLY
#define O_RDWR   _O_RDWR
#define O_CREAT  _O_CREAT
#define O_TRUNC  _O_TRUNC
#define strcasecmp _tcsicmp 
#define strncasecmp _tcsncicmp 
#define W_OK 0x2
#define R_OK 0x4
#define STAT struct stat
#define DIR struct DIR
struct direct
{
    char d_name[1];
};
#include <sys/utime.h>
#define utimbuf __utimbuf64
#define USE_ZFILE

#undef S_ISDIR
#undef S_IWUSR
#undef S_IRUSR
#undef S_IXUSR
#define S_ISDIR(a) (a&FILEFLAG_DIR)
#define S_ISARC(a) (a&FILEFLAG_ARCHIVE)
#define S_IWUSR FILEFLAG_WRITE
#define S_IRUSR FILEFLAG_READ
#define S_IXUSR FILEFLAG_EXECUTE

/* These are prototypes for functions from the Win32 posixemu file */
extern void get_time (time_t t, long* days, long* mins, long* ticks);
extern time_t put_time (long days, long mins, long ticks);

/* #define DONT_HAVE_POSIX - don't need all of Mathias' posixemu_functions, just a subset (below) */
#define chmod(a,b) posixemu_chmod ((a), (b))
extern int posixemu_chmod (const char *, int);
#define stat(a,b) posixemu_stat ((a), (b))
extern int posixemu_stat (const char *, struct stat *);
#define mkdir(x,y) mkdir(x)
#define truncate posixemu_truncate
extern int posixemu_truncate (const char *, long int);
#define utime posixemu_utime
extern int posixemu_utime (const char *, struct utimbuf *);
#define opendir posixemu_opendir
extern DIR * posixemu_opendir (const char *);
#define readdir posixemu_readdir
extern struct dirent* posixemu_readdir (DIR *);
#define closedir posixemu_closedir
extern void posixemu_closedir (DIR *);

#endif

#endif /* _WIN32 */

#ifdef DONT_HAVE_POSIX

#define access posixemu_access
extern int posixemu_access (const char *, int);
#define open posixemu_open
extern int posixemu_open (const char *, int, int);
#define close posixemu_close
extern void posixemu_close (int);
#define read posixemu_read
extern int posixemu_read (int, char *, int);
#define write posixemu_write
extern int posixemu_write (int, const char *, int);
#undef lseek
#define lseek posixemu_seek
extern int posixemu_seek (int, int, int);
#define stat(a,b) posixemu_stat ((a), (b))
extern int posixemu_stat (const char *, STAT *);
#define mkdir posixemu_mkdir
extern int mkdir (const char *, int);
#define rmdir posixemu_rmdir
extern int posixemu_rmdir (const char *);
#define unlink posixemu_unlink
extern int posixemu_unlink (const char *);
#define truncate posixemu_truncate
extern int posixemu_truncate (const char *, long int);
#define rename posixemu_rename
extern int posixemu_rename (const char *, const char *);
#define chmod posixemu_chmod
extern int posixemu_chmod (const char *, int);
#define tmpnam posixemu_tmpnam
extern void posixemu_tmpnam (char *);
#define utime posixemu_utime
extern int posixemu_utime (const char *, struct utimbuf *);
#define opendir posixemu_opendir
extern DIR * posixemu_opendir (const char *);
#define readdir posixemu_readdir
extern struct dirent* readdir (DIR *);
#define closedir posixemu_closedir
extern void closedir (DIR *);

/* This isn't the best place for this, but it fits reasonably well. The logic
 * is that you probably don't have POSIX errnos if you don't have the above
 * functions. */
extern long dos_errno (void);

#endif

#ifdef DONT_HAVE_STDIO

extern FILE *stdioemu_fopen (const char *, const char *);
#define fopen(a,b) stdioemu_fopen(a, b)
extern int stdioemu_fseek (FILE *, int, int);
#define fseek(a,b,c) stdioemu_fseek(a, b, c)
extern int stdioemu_fread (char *, int, int, FILE *);
#define fread(a,b,c,d) stdioemu_fread(a, b, c, d)
extern int stdioemu_fwrite (const char *, int, int, FILE *);
#define fwrite(a,b,c,d) stdioemu_fwrite(a, b, c, d)
extern int stdioemu_ftell (FILE *);
#define ftell(a) stdioemu_ftell(a)
extern int stdioemu_fclose (FILE *);
#define fclose(a) stdioemu_fclose(a)

#endif

#ifdef DONT_HAVE_MALLOC

#define malloc(a) mallocemu_malloc(a)
extern void *mallocemu_malloc (int size);
#define free(a) mallocemu_free(a)
extern void mallocemu_free (void *ptr);

#endif

#ifdef X86_ASSEMBLY
#define ASM_SYM_FOR_FUNC(a) __asm__(a)
#else
#define ASM_SYM_FOR_FUNC(a)
#endif

#include "target.h"
#if !defined(RECUR)
#include "machdep/machdep.h"
#include "gfxdep/gfx.h"
#endif

extern void console_out (const char *, ...);
extern void console_flush (void);
extern int  console_get (char *, int);
extern void f_out (void *, const char *, ...);
extern void gui_message (const char *,...);
extern int gui_message_multibutton (int flags, const char *format,...);
#define write_log_err write_log

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef STATIC_INLINE
#if __GNUC__ - 1 > 1 && __GNUC_MINOR__ - 1 >= 0
#define STATIC_INLINE static __inline__ __attribute__ ((always_inline))
#define NOINLINE __attribute__ ((noinline))
#define NORETURN __attribute__ ((noreturn))
#elif _MSC_VER
#define STATIC_INLINE static __forceinline
#define NOINLINE __declspec(noinline)
#define NORETURN __declspec(noreturn)
#else
#define STATIC_INLINE static __inline__
#define NOINLINE
#define NORETURN
#endif
#endif

#ifndef MAX_PATH
#define MAX_PATH	512
#endif
#ifndef MAX_DPATH
#define MAX_DPATH	512
#endif

#ifndef __cplusplus

#define xmalloc(T, N) malloc(sizeof (T) * (N))
#define xcalloc(T, N) calloc(sizeof (T), N)
#define xfree(T) free(T)
#define xrealloc(T, TP, N) realloc(TP, sizeof (T) * (N))

#if 0
extern void *xmalloc (size_t);
extern void *xcalloc (size_t, size_t);
extern void xfree (const void*);
#endif

#else

#define xmalloc(T, N) static_cast<T*>(malloc (sizeof (T) * (N)))
#define xcalloc(T, N) static_cast<T*>(calloc (sizeof (T), N))
#define xrealloc(T, TP, N) static_cast<T*>(realloc (TP, sizeof (T) * (N)))
#define xfree(T) free(T)

#endif

#define DBLEQU(f, i) (abs ((f) - (i)) < 0.000001)

#define TCHAR char
#define REGPARAM3
#define uae_char char
#define _tcslen strlen
#define _tcscpy strcpy
#define _tcscmp strcmp
#define _tcsncat strncat
#define _tcsncmp strncmp
#define _tstol atol
#define _totupper toupper
#define _stprintf sprintf
#define _tcscat strcat
#define _tcsicmp strcasecmp
#define _tcsnicmp strncasecmp
#define _tcsstr strstr
#define _tcsrchr strrchr
#define _tcsncpy strncpy
#define _tcschr strchr
#define _tstof atof
#define _istdigit isdigit
#define _istspace isspace
#define _tstoi atoi
#define _tcstol strtol
#define _wunlink unlink
#define _tcsftime strftime
#define vsntprintf vsnprint
#define _tcsspn strspn
#define _istupper isupper
#define _totlower tolower
#define _tcstok strtok
#define _wunlink unlink
#define _tfopen fopen
#define bool _Bool
#define true 1
#define false 0
#define _vsntprintf vsnprintf
#define max(a,b) ((a) > (b) ? (a) : (b))
#define _tcstod strtod
#define _T
#define sleep_millis uae_msleep
#define _istalnum iswalnum
#define offsetof(type, member)  __builtin_offsetof (type, member)
#define ULONG unsigned long
#define _strtoui64 strtoul
#define _tcscspn wcscspn
#define _stat64 stat64
#endif
