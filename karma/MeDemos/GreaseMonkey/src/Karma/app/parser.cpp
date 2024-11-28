/***********************************************************************************************
*
*	$Id: parser.cpp,v 1.1.2.2 2002/03/06 18:16:32 ianmt Exp $
*
************************************************************************************************/
#include<ctype.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include"MePrecision.h"
#include"parser.hpp"

#if defined( WIN32 )

#elif defined( PS2 )
	#include<sifdev.h>
#endif

int line_count;

/***********************************************************************************************
*
*
*
************************************************************************************************/
static FILE_PTR FileOpenReadOnly(const char *fname)
{
#if defined( WIN32 )

	return(fopen(fname, "r"));

#elif defined( PS2 )
	char temp[255] = "host:";
	int read, i;
	MYFILE *f=0;


	f = (MYFILE *)malloc(sizeof(MYFILE));


	if(f) {
		strcat(temp,fname);

		f->filehandle = sceOpen(temp, SCE_RDONLY);
		f->len = sceLseek(f->filehandle, 0, SCE_SEEK_END);
		f->data = (char *)malloc(f->len);
		memset(f->data,0,f->len);
		f->pos = 0;
		sceLseek(f->filehandle, 0, SCE_SEEK_SET);
		read = sceRead(f->filehandle, f->data, f->len);
	}

	return(f);

#endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void FileClose(FILE_PTR f)
{
#if defined( WIN32 )

	fclose(f);

#elif defined( PS2 )

	sceClose(f->filehandle);
	free(f->data);
	free(f);

#endif

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static char *FileReadNextLine(char *buffer, int size, FILE_PTR f)
{
  line_count = 0;

#if defined( WIN32 )
	char *first_entry;

	do{
		first_entry = fgets(buffer,size,f);
		line_count++;
	} while(first_entry && *first_entry == '#');

	return(first_entry);
#elif defined( PS2 )
	int i = 0, comment = 0;

	if(f->pos >= f->len) return 0;

	while(f->pos <= f->len && i < size)
	{
		f->pos++;
		if(f->data[f->pos-1] != '\n')
		{
			if(f->data[f->pos-1] == '#') comment = 1;//# hashes used to comment out line of script	

			if(!comment) {
				buffer[i] = f->data[f->pos-1];
				i++;
			}
		}
		else if(comment)
		  {
		    comment = 0;
		    line_count++;
		  }
		else
		  {
		    line_count++;
		    if(i > 0) break;
		  }
	}

	buffer[i]=0;

	return buffer;

#endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
Parser::Parser()
{
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int Parser::Open(const char *fname)
{
	file_ptr = FileOpenReadOnly(fname);

	if(file_ptr)
	{
		strcpy(filename, fname);
		line_number = 0;
		return(1);
	}

	return(0);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Parser::Parse()
{
	while(GetLine())
		{

			GetWord();

			if(!InterpretWord())
			  {
			    printf("Failed to interpret %s on line %d in %s\n",wordbuffer,line_number,filename);
			  }
		}
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
Parser::~Parser()
{
	if(file_ptr) FileClose(file_ptr);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int Parser::GetLine()
{

	if(FileReadNextLine(linebuffer, 250, file_ptr))
	{
		index = 0;
		line_number += line_count;
		return(1);
	}

	return(0);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int Parser::GetNextEntry(char *ibuffer, char *obuffer)
{
	int read = 0, count = 0;

	*obuffer = 0;

	while(*ibuffer)
	{
		if(isalnum(*ibuffer) //valid characters
			|| *ibuffer == '.'
			|| *ibuffer == '-'
			|| *ibuffer == '_'
			|| *ibuffer == '/'
			|| *ibuffer == '\\') {
			{
				obuffer[read] = *ibuffer;
				obuffer[read+1] = 0;
				read++;
			}
		} else if(read) {
			return(count+1);
		}
		ibuffer++;
		count++;
	}
	return(count);
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
int Parser::InterpretWord()
{
	int i = 0;

	if(!strlen(wordbuffer)) return 1; //nothing to do on zero length string

	while(comtable[i].text)
	{
		if(!stricmp(comtable[i].text, wordbuffer))
		{
			comtable[i].fptr(this);
			return(1);
		}
		i++;
	}

	return 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int Parser::WordToEnum(TEXT_2_ENUM *table)
{
	int i = 0;

	while(table[i].text)
	{
		if(!stricmp(table[i].text, wordbuffer))
		{
			return(table[i].enum_val);
		}
		i++;
	}

	return(-1);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
char * Parser::GetWord()
{
	int count;

	count = GetNextEntry(&linebuffer[index], wordbuffer);

	index += count;

	return wordbuffer; //this just makes life easier

}


/***********************************************************************************************
*
*
*
************************************************************************************************/
int Parser::GetInt() {
	int count;

	count = GetNextEntry(&linebuffer[index], wordbuffer);

	index += count;

	return(atoi(wordbuffer));
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static MeReal MyAtoF(char *string)
{
#if defined( WIN32 )

	return (MeReal)atof(string); //doesn't seem to work on PS2 ??

#elif defined( PS2 )
	MeReal divisor = 0, temp1; //get the number whole number part
	int temp2 = 0, i, exit = 0, len = 0;
	MeReal ret = 0, sign = 1.0f;

	if(*string == '-')
	  {
	    sign = -1.0f;
	    string++;
	  }

	temp1 =(MeReal)(atoi(string));

	while(*string && !exit)
	{
		if(*string == '.')
		{
			exit = 1;
			string++; //want the bit after decimal point
			temp2 = atoi(string);
			len = strlen(string);
		}
		else
		{
			string++;
		}
	}

	ret = (MeReal)temp2;
	for(i=0;i<len;i++) ret *= 0.1f;
	ret += temp1;
	ret *= sign;

	return(ret);
#endif
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
MeReal Parser::GetFloat() {
	int count;
	MeReal res;

	count = GetNextEntry(&linebuffer[index], wordbuffer);

	index += count;


	res = MyAtoF(wordbuffer);

	return(res);
}

