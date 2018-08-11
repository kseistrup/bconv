/******************************************************************************
 * Copyright (C) 1996 Klaus Alexander Seistrup @ Magnetic Ink, Dianalund, DK
 *
 * Author : 1996 Klaus Alexander Seistrup
 * Created: Wed Nov 06 15:50:09 MET 1996
 * $Header: /home/kas/src/b/bconv/RCS/bconv.c,v 1.7 1996/11/21 23:42:08 kas Exp kas $
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITH-
 * OUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 ******************************************************************************/

#include <getopt.h>
#include <locale.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#define PGMNAME		"bconv"
#define PGMVER		"0"
#define PGMREV		"1"
#define PGMPL		"3"
#define PGMDATE		"21.XI.1996"
#define ENDOPTS		{NULL,0,NULL,0}
#define STREQ(a,b)	(strncasecmp((a), (b), 3) == 0)

#ifndef MSDOS
#define DIR_SEP	'/'
#else
#define DIR_SEP	'\\'
#endif

#ifndef LONG_LONG_MAX
#define LONG_LONG_MAX	0x7FFFFFFFFFFFFFFFULL
#endif

#define MakeID(name,ver,rev,pl,date) \
"@(#)" name " " ver "." rev "." pl " (" date ")\n"

#define USAGE	\
"Usage: %s [OPTION(S)] NUMBER ...\n"\
"  options are:\n"\
"    -iIBASE, --input-base=IBASE .... input is in base IBASE  (2..36)\n"\
"    -oOBASE, --output-base=OBASE ... output is in base OBASE (2..36)\n"\
"    -u, --upper .................... output result in uppercase\n"\
"    -l, --lower .................... output result in lowercase  (*)\n"\
"    -c, --copyright ................ copying policy\n"\
"    -h, --help ..................... this help text\n"\
"    -V, --version .................. output version information\n"\
"Input base is auto-detected if in octal, decimal or hexadecimal; see\n"\
"man page for strtol(3).  Default output base is 10 (decimal).\n"\
"For convenience, input and output bases can be specified either as a\n"\
"number or as BINary (2), OCTal (8), DECimal (10) or HEXadecimal (16).\n"\
"NUMBER should be in range 0-%Lu (0x%LX).\n"

#define MINIGPL	\
"Copyright (C) 1996 Klaus A Seistrup @ Magnetic Ink, Dianalund, DENMARK.\n"\
"\n"\
"This program is free software; you can redistribute it and/or modify it\n"\
"under the terms of the GNU General Public License as published by the Free\n"\
"Software Foundation; either version 2 of the License, or (at your option)\n"\
"any later version.\n"\
"\n"\
"This program is distributed in the hope that it will be useful, but WITH-\n"\
"OUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n"\
"FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for\n"\
"more details.\n"\
"\n"\
"You should have received a copy of the GNU General Public License along\n"\
"with this program; if not, write to the Free Software Foundation, Inc.,\n"\
"675 Mass Ave, Cambridge, MA 02139, USA.\n"

static const char PgmID[] = MakeID (PGMNAME, PGMVER, PGMREV, PGMPL, PGMDATE);
static const char RCSid[] = "$Id: bconv.c,v 1.7 1996/11/21 23:42:08 kas Exp kas $";

/**/
char *
getprogramname (char *path)
{
  static char *__program_name = NULL;

  if (__program_name == NULL)
    {
      if (path == NULL)
	path = PGMNAME;

      __program_name = strrchr (path, DIR_SEP);

      if (__program_name)
	++__program_name;
      else
	__program_name = path;
    }
  return __program_name;
}

/**/
int
getbase (char *str, char *type)
{
  unsigned int base = ~0;

  if (str)			/* NULL-pointer paranoia */
    {
      if (STREQ ("BIN", str))
	base = 2;
      else if (STREQ ("OCT", str))
	base = 8;
      else if (STREQ ("DEC", str))
	base = 10;
      else if (STREQ ("HEX", str))
	base = 16;
      else
	{
	  char *endptr = NULL;

	  base = (unsigned int) strtoul (str, &endptr, 0);

	  if (*endptr != '\0')
	    {
	      errno = EDOM;
	      perror (str);
	      exit (EXIT_FAILURE);
	    }
	}
    }

  if (base < 2 || base > 36)
    {
      (void) fprintf (stderr, "%s: %sput base should be in range 2..36\n", getprogramname (NULL), type);
      exit (EXIT_FAILURE);
    }
  return base;
}

static void
initconv (char *tab, char *fmt, unsigned int ob, int up)
{
  char ch;

  for (ch = '0'; ch <= '9'; ++ch)
    {
      *tab = ch;
      ++tab;
    }

  for (ch = 'a'; ch <= 'z'; ++ch)
    {
      *tab = up ? toupper (ch) : ch;
      ++tab;
    }
  *tab = 0;

  fmt[0] = '%';
  fmt[1] = 'L';
  fmt[2] = 'u';
  fmt[3] = '\n';
  fmt[4] = '\0';		/* AKA strcpy(fmt,"%Lu\n"); */

  switch (ob)
    {
    case 8:
      fmt[2] = 'o';
      break;
    case 16:
      fmt[2] = up ? 'X' : 'x';
      break;
    default:
      /* fmt[2] = 'u'; */
      /* break; */
    }
}

/**/
int
bconv (int ac, char **av, unsigned int ib, unsigned int ob, int up)
{
  char base_tab[48], base_fmt[8];
  unsigned long long obase = (unsigned long long) ob;
  int rc = 0;

  initconv (base_tab, base_fmt, ob, up);

  while (ac)
    {
      unsigned long long result;
      char *endptr = NULL, *ptr = *av;

      errno = 0;
      result = strtouq (ptr, &endptr, ib);

      if ((long long) result < 0)
	{
	  errno = ERANGE;
	  result = ~0;
	}

      if (*endptr != 0 || errno)
	{
	  char *pgm = getprogramname(NULL);
	  char *msg = alloca (strlen (pgm) + strlen (ptr) + 4);

	  (void) sprintf (msg, "%s(%s)", pgm, ptr);

	  if (errno == 0)
	    errno = EDOM;

	  perror (msg);
	  errno = 0;
	  ++rc;
	}
      else if (ob == 10 || ob == 16 || ob == 8)
	{
	  (void) fprintf (stdout, base_fmt, result);
	}
      else
	{
	  char buf[(_TYPEBITS(long long) + 4)];
	  char *ptr = buf;

	  do
	    {
	      *ptr = base_tab[result%obase];
	      ++ptr;
	      result /= obase;
	    }
	  while (result);

	  while (--ptr >= buf)
	    (void) putc (*ptr, stdout);

	  (void) putc ('\n', stdout);
	}
      --ac;
      ++av;
    }
  return rc;
}

/**/
int
main (int argc, char **argv)
{
  static const struct option long_opts[] =
  {
    {"ibase",           1, NULL, 'i'},
    {"input-base",      1, NULL, 'i'},
    {"obase",           1, NULL, 'o'},
    {"output-base",     1, NULL, 'o'},
    {"uppercase",       0, NULL, 'u'},
    {"upper-case",      0, NULL, 'u'},
    {"lowercase",       0, NULL, 'l'},
    {"lower-case",      0, NULL, 'l'},
    {"copying-policy",  0, NULL, 'c'},
    {"copyleft",        0, NULL, 'c'},
    {"copyright",       0, NULL, 'c'},
    {"help",            0, NULL, 'h'},
    {"version",         0, NULL, 'V'},
    ENDOPTS
  };
  int opt, idx;
  unsigned int ibase, obase, upper;
  char *program_name = getprogramname (argv[0]);

  obase = 10;
  idx = ibase = upper = 0;
  (void) setlocale (LC_ALL, "");

  while ((opt = getopt_long (argc, argv, "i:o:ulchV", long_opts, &idx)) != EOF)
    {
      switch (opt)
	{
	case 'i':
	  ibase = getbase (optarg, "in");
	  break;
	case 'o':
	  obase = getbase (optarg, "out");
	  break;
	case 'u':
	  upper = 1;
	  break;
	case 'l':
	  upper = 0;
	  break;
	case 'h':
	  (void) fprintf (stdout, USAGE, program_name, LONG_LONG_MAX, LONG_LONG_MAX);
	  exit (EXIT_SUCCESS);
	  /* break; */
	case 'V':
	  (void) fputs (PgmID + 4, stdout);
	  exit (EXIT_SUCCESS);
	  /* break; */
	case 'c':
	  (void) fputs (MINIGPL, stdout);
	  exit (EXIT_SUCCESS);
	  /* break; */
	default:
	  (void) fprintf (stderr, "Try `%s --help' for more information.\n", program_name);
	  exit (EXIT_FAILURE);
	  /* break; */
	}
    }

  if ((argc -= optind))
    idx = bconv (argc, argv + optind, ibase, obase, upper);
  else
    (void) fprintf (stderr, "%s: nothing to do?\n", program_name);

  exit (argc ? EXIT_FAILURE : idx);
  /* NOTREACHED */
  return EXIT_SUCCESS;
}

/* EOF */
