/***********************************************************************************************
*
*	$Id: parser.hpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*
************************************************************************************************/
#ifndef _PARSER_H
#define _PARSER_H

#if defined( WIN32 )

	#include<stdio.h>
	typedef FILE* FILE_PTR;

#elif defined( PS2 )

	typedef struct {
	  int filehandle;
	  char *data;
	  int len;
	  int pos;
	}MYFILE;
	typedef MYFILE* FILE_PTR;

#endif

typedef class Parser PARSER;

typedef void PFunc(PARSER *);

typedef struct {
	char *text;
	PFunc *fptr;
} TEXT_2_PFUNC;

typedef struct {
	char *text;
	int	enum_val;
} TEXT_2_ENUM;


class Parser
{
private:
	char filename[100];
	char linebuffer[250];
	char wordbuffer[50];
	TEXT_2_PFUNC *comtable;
	int index;
	int line_number;
	FILE_PTR file_ptr;
	int GetNextEntry(char *, char *);
	int	InterpretWord();
public:
	Parser();
	~Parser();
	int		Open(const char *fname);
	void	SetComTable(TEXT_2_PFUNC *ct) { comtable = ct; }
	void	Parse();
	int		GetLine();
	char    *GetWord();
	MeReal	GetFloat();
	int		GetInt();
	int		WordToEnum(TEXT_2_ENUM *);
};

#endif //_PARSER_H
