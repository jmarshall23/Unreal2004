//
// $Id: tou.cpp,v 1.2.2.1 2002/04/04 15:29:41 richardm Exp $
// $Name: t-stevet-RWSpre-030110 $
//
// Convert line-endings on ascii files in place.
//
// tou:	Ensure all lines end in "\n" only
// topc: Ensure all lines end in "\r\n" only
//

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#pragma comment (exestr, "$Id: tou.cpp,v 1.2.2.1 2002/04/04 15:29:41 richardm Exp $")

char* progname = "";

//************************************************************************************

void
PutbackWithErrorReport (int c, FILE* f, const char* str)
{
	if (ungetc (c, f) != c)
	{
		static int putpackerror = 0;
		if (! putpackerror)
		{
			putpackerror = 1;
			fprintf (stderr, "%s: %s\n", str, strerror (errno));
		} // if
	} // if
} // PutbackWithErrorReport (int, FILE*, const char*)

//************************************************************************************

bool
MakeUnixFile (FILE* src, FILE* dst, const char* fname)
{
	int in = 0;
	int out = 0;
	int eaten = 0;
	int swapped = 0;
#define WRITE(n) do {fputc(n, dst); ++out;} while (0)
	for (int c = fgetc (src); EOF != c; c = fgetc (src))
	{
		if (EOF == c) break;
		++in;

		if ('\r' == c)
		{
			WRITE ('\n');

			// Eat the next character if it's a '\n'
			c = fgetc (src);
			if (EOF == c)
			{
				++swapped;
				fprintf (stderr, "%s: trailing '\\r'\n", fname);
				break;
			} // if
			else if ('\n' == c)
			{
				// eat
				++in;
				++eaten;
			}
			else
			{
				++swapped;
				PutbackWithErrorReport (c, src, fname);
			} // if
		}
		else
		{
			WRITE (c);
		} // else
	} // while

	if (swapped + eaten)
	{
		fprintf (stderr, "%s: %d char%s read, %d written (%d swapped, %d eaten)\n", fname, in, in == 1? "": "s", out, swapped, eaten);
	}

	return swapped + eaten != 0;
} // MakeUnixFile (FILE*, FILE*, const char*)

//************************************************************************************

bool
MakeDosFile (FILE* src, FILE* dst, const char* fname)
{
	int in = 0;
	int out = 0;
	int added = 0;
#define WRITE(n) do {fputc(n, dst); ++out;} while (0)
	for (int c = fgetc (src); EOF != c; c = fgetc (src))
	{
		if (EOF == c) break;
		++in;

		if ('\n' == c)
		{
			WRITE ('\r');
			++added;
		} // if

		WRITE (c);

		if ('\r' == c)
		{
			// Add '\n' unless it's already there
			c = fgetc (src);
			switch (c)
			{
			case EOF:
					WRITE ('\n');
					++added;
					break;

			case '\n':
					// Do nothing extra
					++in;
					WRITE (c);
					break;

			default:
				WRITE ('\n');
				++added;
				PutbackWithErrorReport (c, src, fname);
				break;
			} // switch
		} // if
	} // while

	if (added)
	{
		fprintf (stderr, "%s: %d char%s read, %d written (%d added)\n", fname, in, in == 1? "": "s", out, added);
	} // if

	return added != 0;
} // MakeDosFile (const char* fname)

//************************************************************************************

typedef bool (*ConversionFunc) (FILE*, FILE*, const char*);

int
TransliterateFile (const char* fname, ConversionFunc f)
{
	// Don't convert directories, dummy!
	struct _stat stat;
	if (_stat (fname, &stat))
	{
		fprintf (stderr, "%s: Can't stat [%s]\n", fname, strerror (errno));
		return 0;	// E A R L Y   R E T U R N
	}
	else if (!(stat.st_mode & _S_IFREG))
	{
		fprintf (stderr, "%s: not a regular file\n", fname);
		return 0;	// E A R L Y   R E T U R N
	} // else

	char dbuff [40];
	strcpy (dbuff, "touXXXXXX");
	char* sparename = _mktemp (dbuff);
	if (! sparename)
	{
		fprintf (stderr, "Cannot make temp name for [%s]: %s\n", fname, strerror (errno));
		return 1;
	} // if

	if (rename (fname, sparename))
	{
		if (ENOENT == errno)
			fprintf (stderr, "[%s]: %s\n", fname, strerror (errno));
		else
			fprintf (stderr, "Couldn't rename [%s]: %s\n", fname, strerror (errno));
		return 2;
	}

	FILE* dst = fopen (fname, "wb");
	if (! dst)
	{
		fprintf (stderr, "Cannot open [%s]\n", fname);
		if (! rename (sparename, fname))
		{
			fprintf (stderr, "Couldn't put back [%s]: %s\n", fname, strerror (errno));
			return 3;
		}
		return 4;
	} // if

	FILE* src = fopen (sparename, "rb");
	if (! src)
	{
		fprintf (stderr, "%s: Cannot open \"%s\" [%s]\n", fname, sparename, strerror (errno));
		fclose (dst);
		if (! rename (sparename, fname))
		{
			fprintf (stderr, "Couldn't put back [%s]: %s\n", fname, strerror (errno));
			return 3;
		}
		return 5;
	} // if

	const bool Changed = f (src, dst, fname);

	fclose (src);
	fclose (dst);

	if (Changed)
	{
		if (remove (sparename))
		{
			fprintf (stderr, "%s: Couldn't remove temp file [%s] for [%s]: %s\n", progname, sparename, fname, strerror (errno));
		} // if
	}
	else
	{
		// No change. -- don't change input file
		fprintf (stderr, "%s: no change\n", fname);
		if (remove (fname))
		{
			fprintf (stderr, "%s: Couln't remove temp file [%s] (original file is in [%s]): %s\n", progname, fname, sparename, strerror (errno));
		}
		else if (rename (sparename, fname))
		{
			if (ENOENT == errno)
				fprintf (stderr, "[%s]: %s\n", fname, strerror (errno));
			else
				fprintf (stderr, "Couldn't rename [%s]: %s\n", fname, strerror (errno));

			return 2;
		} // elif
	} // else

	return 0;
} // TransliterateFile (const char*, ConversionFunc)

//************************************************************************************

int
M1akeUnixFile (const char* fname)
{
	char dbuff [40];
	strcpy (dbuff, "touXXXXXX");
	char* sparename = _mktemp (dbuff);
	if (! sparename)
	{
		fprintf (stderr, "Cannot make temp name for [%s]: %s\n", fname, strerror (errno));
		return 1;
	} // if

	if (rename (fname, sparename))
	{
		if (ENOENT == errno)
			fprintf (stderr, "[%s]: %s\n", fname, strerror (errno));
		else
			fprintf (stderr, "Couldn't rename [%s]: %s\n", fname, strerror (errno));
		return 2;
	}

	FILE* dst = fopen (fname, "wb");
	if (! dst)
	{
		fprintf (stderr, "Cannot open [%s]\n", fname);
		if (! rename (sparename, fname))
		{
			fprintf (stderr, "Couldn't put back [%s]: %s\n", fname, strerror (errno));
			return 3;
		}
		return 4;
	} // if

	FILE* src = fopen (sparename, "rb");
	if (! src)
	{
		fprintf (stderr, "%s: Cannot open \"%s\" [%s]\n", fname, sparename, strerror (errno));
		fclose (dst);
		if (! rename (sparename, fname))
		{
			fprintf (stderr, "Couldn't put back [%s]: %s\n", fname, strerror (errno));
			return 3;
		}
		return 5;
	} // if

	int in = 0;
	int out = 0;
	int eaten = 0;
	int swapped = 0;
#define WRITE(n) do {fputc(n, dst); ++out;} while (0)
	for (int c = fgetc (src); EOF != c; c = fgetc (src))
	{
		if (EOF == c) break;
		++in;

		if ('\r' == c)
		{
			WRITE ('\n');

			// Eat the next character if it's '\n'
			c = fgetc (src);
			if (EOF == c)
			{
				++swapped;
				fprintf (stderr, "%s: trailing '\\r'\n", fname);
				break;
			} // if
			else if ('\n' == c)
			{
				// eat
				++in;
				++eaten;
			}
			else
			{
				++swapped;
				if (ungetc (c, src) != c)
				{
					static int putpackerror = 0;
					if (! putpackerror)
					{
						putpackerror = 1;
						fprintf (stderr, "%s: %s\n", fname, strerror (errno));
					} // if
				} // if
			} // if
		}
		else
		{
			WRITE (c);
		} // else
	} // while

	fclose (src);
	fclose (dst);

	if (swapped + eaten)
	{
		fprintf (stderr, "%s: %d char%s read, %d written (%d swapped, %d eaten)\n", fname, in, in == 1? "": "s", out, swapped, eaten);
		if (remove (sparename))
		{
			fprintf (stderr, "%s: Couln't remove temp file [%s] for [%s]: %s\n", progname, sparename, fname, strerror (errno));
		} // if
	}
	else
	{
		// No change. -- don't change input file
		fprintf (stderr, "%s: no change\n", fname);
		if (remove (fname))
		{
			fprintf (stderr, "%s: Couln't remove temp file [%s] (original file is in [%s]): %s\n", progname, fname, sparename, strerror (errno));
		}
		else if (rename (sparename, fname))
		{
			if (ENOENT == errno)
				fprintf (stderr, "[%s]: %s\n", fname, strerror (errno));
			else
				fprintf (stderr, "Couldn't rename [%s]: %s\n", fname, strerror (errno));

			return 2;
		} // elif
	} // else

	return 0;
} // MakeDosFile (const char*)

//************************************************************************************

int
M1akeDosFile (const char* fname)
{
	char dbuff [40];
	strcpy (dbuff, "touXXXXXX");
	char* sparename = _mktemp (dbuff);
	if (! sparename)
	{
		fprintf (stderr, "Cannot make temp name for [%s]: %s\n", fname, strerror (errno));
		return 1;
	} // if

	if (rename (fname, sparename))
	{
		if (ENOENT == errno)
			fprintf (stderr, "[%s]: %s\n", fname, strerror (errno));
		else
			fprintf (stderr, "Couldn't rename [%s]: %s\n", fname, strerror (errno));
		return 2;
	}

	FILE* dst = fopen (fname, "wb");
	if (! dst)
	{
		fprintf (stderr, "Cannot open [%s]\n", fname);
		if (! rename (sparename, fname))
		{
			fprintf (stderr, "Couldn't put back [%s]: %s\n", fname, strerror (errno));
			return 3;
		}
		return 4;
	} // if

	FILE* src = fopen (sparename, "rb");
	if (! src)
	{
		fprintf (stderr, "%s: Cannot open \"%s\" [%s]\n", fname, sparename, strerror (errno));
		fclose (dst);
		if (! rename (sparename, fname))
		{
			fprintf (stderr, "Couldn't put back [%s]: %s\n", fname, strerror (errno));
			return 3;
		}
		return 5;
	} // if

	int in = 0;
	int out = 0;
	int added = 0;
#define WRITE(n) do {fputc(n, dst); ++out;} while (0)
	for (int c = fgetc (src); EOF != c; c = fgetc (src))
	{
		if (EOF == c) break;
		++in;

		if ('\n' == c)
		{
			WRITE ('\r');
			++added;
		} // if

		WRITE (c);

		if ('\r' == c)
		{
			// Add '\n' unless it's already there
			c = fgetc (src);
			switch (c)
			{
			case EOF:
					WRITE ('\n');
					++added;
					break;

			case '\n':
					// Do nothing extra
					++in;
					WRITE (c);
					break;

			default:
				WRITE ('\n');
				++added;
				if (ungetc (c, src) != c)
				{
					static int putpackerror = 0;
					if (! putpackerror)
					{
						putpackerror = 1;
						fprintf (stderr, "%s: %s\n", fname, strerror (errno));
					} // if
				} // if
				break;
			} // switch
		} // if
	} // while

	fclose (src);
	fclose (dst);

	if (added)
	{
		fprintf (stderr, "%s: %d char%s read, %d written (%d added)\n", fname, in, in == 1? "": "s", out, added);
		if (remove (sparename))
		{
			fprintf (stderr, "%s: Couln't remove temp file [%s] for [%s]: %s\n", progname, sparename, fname, strerror (errno));
		} // if
	}
	else
	{
		// No change. -- don't change input file
		fprintf (stderr, "%s: no change\n", fname);
		if (remove (fname))
		{
			fprintf (stderr, "%s: Couln't remove temp file [%s] (original file is in [%s]): %s\n", progname, fname, sparename, strerror (errno));
		}
		else if (rename (sparename, fname))
		{
			if (ENOENT == errno)
				fprintf (stderr, "[%s]: %s\n", fname, strerror (errno));
			else
				fprintf (stderr, "Couldn't rename [%s]: %s\n", fname, strerror (errno));

			return 2;
		} // elif
	} // else

	return 0;
} // M1akeDosFile (const char* fname)

//************************************************************************************

void
Usage()	// Doesn't return
{
		fprintf (stderr, "Usage:\n%s files...\n", progname);
		exit (1);
} // Usage ();

//************************************************************************************

//extern "C" void
//_setenvp()
//{
//}

//void __cdecl
//_setargv ()
//{
//        __setargv();
//}

//************************************************************************************

int
main(int argc, char* argv[])
{
	sscanf ("", argv[0], &progname);
	char* sl0 = strrchr (argv[0], '\\')? strrchr (argv[0], '\\') + 1: argv[0];
	char* sl1 = strrchr (argv[0], '/')? strrchr (argv[0], '/') + 1: argv[0];
	progname = sl0 > sl1? sl0: sl1;

	const ConversionFunc F = strnicmp ("topc", progname, 4)? MakeUnixFile: MakeDosFile;

	++argv;
	--argc;

	if (argc <= 0) Usage();

//	if (0 == strcmp ("-R", *argv))
//	{
//		if (2 != argc) Usage();
//		// Recursive
//		struct _finddata_t f = {0};
//		long h = _findfirst ("*", &f);
//		while (0 == _findnext (h, &f))
//		{
//			printf( " %-12s %.24s  %9ld\n", f.name, "hello" /* ctime( &( f.time_write ) )*/, f.size );
//		}
//		_findclose (h);
//		return 0;	
//	}

	while (argc--) TransliterateFile (*argv++, F);

	return 0;
} // main(int, char* [])