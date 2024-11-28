/* -*- mode: C; -*- */

/*
Copyright (c) 1997-2002 MathEngine PLC

$Name: t-stevet-RWSpre-030110 $
  
Date: $Date: 2002/04/04 15:29:40 $ - Revision: $Revision: 1.8.2.2 $

This software and its accompanying manuals have been developed
by Mathengine PLC ("MathEngine") and the copyright and all other
intellectual property rights in them belong to MathEngine. All
rights conferred by law (including rights under international
copyright conventions) are reserved to MathEngine. This software
may also incorporate information which is confidential to
MathEngine.

Save to the extent permitted by law, or as otherwise expressly
permitted by MathEngine, this software and the manuals must not
be copied (in whole or in part), re-arranged, altered or adapted
in any way without the prior written consent of the Company. In
addition, the information contained in the software may not be
disseminated without the prior written consent of MathEngine.
        
*/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <MeXMLParser.h>
#include <MeMemory.h>


/* --------------- parsing functions for individual types ----------------- */

static MeXMLError readToNextTag(MeXMLInput *fi, char *dest, int size);


/* returns 0 if successfully consumed a comma and got to a non-comma,
or if there was only whitespace, zero.
There shouldn't be any comments in here by this point
*/

int MEAPI MeXMLParseComma(char *in, char **out) {
    while(isspace(*in)) in++;
    *out = in;
    switch(*in) {
    case 0:
        return 0;
    case ',':
        for(in++;isspace(*in);in++);
        *out=in;
        return !*in;
    default:
        return 1;
    }
}


MeXMLError MEAPI
MeXMLParseUInt(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c;
    unsigned int *dest = (unsigned int *)((char *)data+action->offset);
    int line = file->line, posn = file->posn;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    *dest = strtol(x,&c,0);
    if(c==x) {
        sprintf(file->error,
            "line %d, char %d: Expected unsigned int, found %s\n",
            line, posn, x);
        return MeXMLErrorInvalidValue;
    }
    while(isspace(*c)) c++;
    if(*c) {
        sprintf(file->error,
            "line %d, char %d: found garbage after data\n",
            line, posn);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}

MeXMLError MEAPI
MeXMLParseUIntArray(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c = x, *d;
    int line = file->line, posn = file->posn;
    unsigned int *dest = (unsigned int *)((char *)data+action->offset);
    unsigned int i = 0;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    while(*c && i<action->max) {
        dest[i++] = strtol(c,&d,0);
        if(c==d || MeXMLParseComma(d,&c)) {
            sprintf(file->error,
                "line %d, char %d: unsigned int array data invalid\n",
                line, posn);
            return MeXMLErrorInvalidValue;
        }
    };
    
    if(i<action->max) {
        sprintf(file->error,
            "line %d, char %d: expected %d unsigned ints, found %d\n",
            line, posn, action->max, i);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}


MeXMLError MEAPI
MeXMLParseInt(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c;
    int *dest = (int *)((char *)data+action->offset);
    int line = file->line, posn = file->posn;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    *dest = strtol(x,&c,0);
    if(c==x) {
        sprintf(file->error,
            "line %d, char %d: Expected int, found %s\n",
            line, posn, x);
        return MeXMLErrorInvalidValue;
    }
    while(isspace(*c)) c++;
    if(*c) {
        sprintf(file->error,
            "line %d, char %d: found garbage after data\n",
            line, posn);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}

MeXMLError MEAPI
MeXMLParseIntArray(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c = x, *d;
    int line = file->line, posn = file->posn;
    int *dest = (int *)((char *)data+action->offset);
    unsigned int i = 0;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    while(*c && i<action->max) {
        dest[i++] = strtol(c,&d,0);
        if(c==d || MeXMLParseComma(d,&c)) {
            sprintf(file->error,
                "line %d, char %d: int array data invalid\n",
                line, posn);
            return MeXMLErrorInvalidValue;
        }
    };
    
    if(i<action->max) {
        sprintf(file->error,
            "line %d, char %d: expected %d ints, found %d\n",
            line, posn, action->max, i);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}


MeXMLError MEAPI
MeXMLParseFloat(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c;
    float *dest = (float *)((char *)data+action->offset);
    int line = file->line, posn = file->posn;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    *dest = (float)strtod(x,&c);
    if(c==x) {
        sprintf(file->error,
            "line %d, char %d: Expected float, found %s\n",
            line, posn, x);
        return MeXMLErrorInvalidValue;
    }
    while(isspace(*c)) c++;
    if(*c) {
        sprintf(file->error,
            "line %d, char %d: found garbage after data\n",
            line, posn);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}

MeXMLError MEAPI
MeXMLParseFloatArray(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c = x, *d;
    int line = file->line, posn = file->posn;
    float *dest = (float *)((char *)data+action->offset);
    unsigned int i = 0;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    while(*c && i<action->max) {
        dest[i++] = (float)strtod(c,&d);
        if(c==d || MeXMLParseComma(d,&c)) {
            sprintf(file->error,
                "line %d, char %d: float array data invalid\n",
                line, posn);
            return MeXMLErrorInvalidValue;
        }
    };
    
    if(i<action->max) {
        sprintf(file->error,
            "line %d, char %d: expected %d floats, found %d\n",
            line, posn, action->max, i);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}

MeXMLError MEAPI
MeXMLParseMeReal(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c;
    MeReal *dest = (MeReal *)((char *)data+action->offset);
    int line = file->line, posn = file->posn;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    *dest = (MeReal)strtod(x,&c);
    if(c==x) {
        sprintf(file->error,
            "line %d, char %d: Expected MeReal, found %s\n",
            line, posn, x);
        return MeXMLErrorInvalidValue;
    }
    while(isspace(*c)) c++;
    if(*c) {
        sprintf(file->error,
            "line %d, char %d: found garbage after data\n",
            line, posn);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}

MeXMLError MEAPI
MeXMLParseMeRealArray(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c = x, *d;
    int line = file->line, posn = file->posn;
    MeReal *dest = (MeReal *)((char *)data+action->offset);
    unsigned int i = 0;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    while(*c && i<action->max)    {
        dest[i++] = (MeReal)strtod(c,&d);
        if(c==d || MeXMLParseComma(d,&c)) {
            sprintf(file->error,
                "line %d, char %d: MeReal array data invalid\n",
                line, posn);
            return MeXMLErrorInvalidValue;
        }
    };
    
    if(i<action->max) {
        sprintf(file->error,
            "line %d, char %d: expected %d MeReals, found %d\n",
            line, posn, action->max, i);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}


MeXMLError MEAPI
MeXMLParseDouble(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c;
    double *dest = (double *)((char *)data+action->offset);
    int line = file->line, posn = file->posn;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    *dest = strtod(x,&c);
    if(c==x) {
        sprintf(file->error,
            "line %d, char %d: Expected double, found %s\n",
            line, posn, x);
        return MeXMLErrorInvalidValue;
    }
    while(isspace(*c)) c++;
    if(*c) {
        sprintf(file->error,
            "line %d, char %d: found garbage after data\n",
            line, posn);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}

MeXMLError MEAPI
MeXMLParseDoubleArray(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    char x[ME_XML_DATA_BUFFER_SIZE], *c = x, *d;
    int line = file->line, posn = file->posn;
    double *dest = (double *)((char *)data+action->offset);
    unsigned int i = 0;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    while(*c && i<action->max) {
        dest[i++] = strtod(c,&d);
        if(c==d || MeXMLParseComma(d,&c)) {
            sprintf(file->error,
                "line %d, char %d: double array data invalid\n",
                line, posn);
            return MeXMLErrorInvalidValue;
        }
    };
    
    if(i<action->max) {
        sprintf(file->error,
            "line %d, char %d: expected %d doubles, found %d\n",
            line, posn, action->max, i);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
}


MeXMLError MEAPI
MeXMLParseString(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    
    MeXMLError err = readToNextTag(file, (char *)data+action->offset, action->max);
    if (err) return err;
    
    return MeXMLErrorNone;
    
}

MeXMLError MEAPI
MeXMLParseStringArray(MeXMLInput *file, const MeXMLHandler *action, void *data) {
    char x[ME_XML_DATA_BUFFER_SIZE], *c = x;
    int line = file->line, posn = file->posn;
    char *dest = (char*)((char *)data+action->offset);
    unsigned int i = 0;
    
    MeXMLError err = readToNextTag(file,x,ME_XML_DATA_BUFFER_SIZE);
    if(err) return err;
    
    while(*c && i<action->max) {
        strtok(c,",");
        if (strlen(c) > action->maxstr) {
            sprintf(file->error,
                "line %d, char %d: string data greater than max of %d\n",
                line, posn,action->maxstr);
            return MeXMLErrorInvalidValue;
        }
        strcpy(dest,c);
        c += strlen(c)+1;
        dest += action->maxstr;
        i++;
    };
    
    if(i<action->max) {
        sprintf(file->error,
            "line %d, char %d: expected %d strings, found %d\n",
            line, posn, action->max, i);
        return MeXMLErrorInvalidValue;
    }
    
    return MeXMLErrorNone;
    
}

/* --------------- stream parsing functions ----------------- */

static char XMLChomp(MeXMLInput *fi) {
    char c;
    
    if(fi->eof) return 0;
    
    if(fi->top) {
        c = fi->stack[--fi->top];
    } else if(fi->bufptr>=fi->bufmax) {
        fi->bufptr = 1;
        fi->bufmax = MeStreamRead(fi->buffer,1,4096,fi->stream);
        if(fi->bufmax)
            c = fi->buffer[0];
        else {
            c = 0;
            fi->eof = 1;
        }
    } else
        c = fi->buffer[fi->bufptr++];
    
    if(c == '\n') {
        fi->line++;
        fi->posn = 1;
    } else fi->posn++;
    
    return c;
}

static void XMLSpit(MeXMLInput *fi, char c) {
    if(fi->top<4) fi->stack[fi->top++]=c;
    if(fi->eof) fi->eof = 0;
    
    if(c=='\n') {
        fi->line--;
        fi->posn=0x7fff0000;
    } else fi->posn--;
}


/* read next tag, skipping proc, !DOCTYPE and comment tags */

static MeXMLError readNextTag(MeXMLInput *fi, MeXMLElement *e, int size) {
    char c, *dest = e->name;
    
    if (!dest) {
        sprintf(fi->error,
            "line %d, character %d: Nowhere to put parsed data!\n",
            fi->line, fi->posn);
        return MeXMLErrorParseFail;
    }
    
    while(1) {
        do {
            c=XMLChomp(fi);
        } while(!fi->eof && c!='<');
        
        if(fi->eof)
            return MeXMLErrorEOF;
        
        c=XMLChomp(fi);
        switch(c) {
        case '!':
            c = XMLChomp(fi);
            if (c == '-') {       /* its a comment */
                while(1) {
                    do {
                        c=XMLChomp(fi);
                    } while(!fi->eof && c!='-');
                    if(fi->eof) {
                        sprintf(fi->error,
                            "line %d, character %d: found EOF in comment\n",
                            fi->line, fi->posn);
                        return MeXMLErrorMalformed;
                    }
                    if(XMLChomp(fi)=='-') {
                        if(XMLChomp(fi) != '>') {
                            sprintf(fi->error,
                                "line %d, character %d: found illegal '--' in comment\n",
                                fi->line, fi->posn);
                            return MeXMLErrorMalformed;
                        }
                        break;
                    }
                }
            }
            else {                /* its !DOCTYPE, so skip it */
                do {
                    c=XMLChomp(fi);
                } while(!fi->eof && c!='>');
            }
            break;
            
        default:
            do {
                *dest++=c;
                if(dest==e->name+size-1) {
                    sprintf(fi->error,
                        "line %d, character %d: internal error: "
                        "buffer overflow (tag too long)\n",
                        fi->line, fi->posn);
                    return MeXMLErrorParseFail;
                }
                
                c = XMLChomp(fi);
                if(fi->eof) {
                    sprintf(fi->error,
                        "line %d, character %d: found EOF inside tag\n",
                        fi->line, fi->posn);
                    return MeXMLErrorMalformed;
                }
            } while(c != '>');
            *dest=0;
            return MeXMLErrorNone;
        }
    }
}


/* read up to next delimiter tag, ignoring comment tags */

static MeXMLError readToNextTag(MeXMLInput *fi, char *dest, int size) {
    char c,*d=dest;
    
    if (!dest) {
        sprintf(fi->error,
            "line %d, character %d: Nowhere to put parsed data!\n",
            fi->line, fi->posn);
        return MeXMLErrorParseFail;
    }
    
    while(1) {
        c=XMLChomp(fi);
        
        while(!fi->eof && c!='<') {
            *d++=c;
            if(c=='>') {
                sprintf(fi->error,
                    "line %d, character %d: unexpected '>'\n",
                    fi->line, fi->posn);
                return MeXMLErrorMalformed;
            }
            
            if(d==dest+size-1) {
                sprintf(fi->error,
                    "line %d, character %d: internal error: "
                    "buffer overflow (data too long)\n",
                    fi->line, fi->posn);
                return MeXMLErrorParseFail;
            }
            c = XMLChomp(fi);
        }
        
        
        c=XMLChomp(fi);
        
        if(fi->eof) {
            sprintf(fi->error,
                "line %d, character %d: found EOF inside tag\n",
                fi->line, fi->posn);
            return MeXMLErrorMalformed;
        }
        
        if(c=='!') {
            XMLChomp(fi);
            while(1) {
                do {
                    c=XMLChomp(fi);
                }
                while(!fi->eof && c != '-');
                if(XMLChomp(fi)=='-') {
                    if(XMLChomp(fi) == '>')
                        break;
                    sprintf(fi->error,
                        "line %d, character %d: found illegal '--' in comment\n",
                        fi->line, fi->posn);
                    return MeXMLErrorMalformed;
                }
            }
        } else {
            XMLSpit(fi,c);
            XMLSpit(fi,'<');
            *d=0;
            return MeXMLErrorNone;
        }
    }
}

/**
Processes an element.

  Requires an array of handlers in the following form:
  
    MeXMLHandler handlers[] = {
    ME_XML_ELEMENT_HANDLER("SOME_ELEMENT",Handle_SomeElement),
    ME_XML_INT_HANDLER("SOME_INT",My_XML_element,some_int,callback),
    ME_XML_STRING_HANDLER("SOME_STRING",My_XML_element,some_string,30),
    ME_XML_FLOAT_HANDLER("SOME_FLOAT",My_XML_element,some_float,callback)
    ME_XML_HANDLER_END
    };
    
*/
MeXMLError MEAPI
MeXMLElementProcess(MeXMLElement *terminator, MeXMLHandler *actions, void *data, void *userdata) {
    
    MeXMLHandler skipAction[1] = {ME_XML_HANDLER_END};
    MeXMLInput *file = terminator->fi;
    int line = file->line, posn = file->posn;
    MeXMLElement start, end;
    MeXMLError err;
    MeXMLHandler *t;
    
    while(1) {
        err = readNextTag(file, &start, ME_XML_TAG_BUFFER_SIZE);
        if(err==MeXMLErrorEOF && terminator->level==0) return MeXMLErrorNone;
        
        if(err!=MeXMLErrorNone) return err;
        if(start.name[0]=='/') {
            if(terminator->level==0) {
                sprintf(file->error,
                    "line %d, char %d: unexpected closing tag, found %s\n",
                    line, posn, start.name);
                return MeXMLErrorMalformed;
            }
            else  if(strcmp(start.name+1, terminator->name)) {
                sprintf(file->error,
                    "line %d, char %d: closing tag %s "
                    "doesn't match opening tag %s\n",
                    line, posn, terminator->name, start.name);
                return MeXMLErrorMalformed;
            }
            return MeXMLErrorNone;
        }
        
        if(start.name[0]=='?')
            continue;
        
        start.fi = file;
        start.level = terminator->level+1;
        if(start.attr=strchr(start.name,' ')) {
            *start.attr++=0;
            while(isspace(*start.attr)) start.attr++;
        }
        
        for(t=actions;t->type!=MeXMLActionEnd && strcmp(start.name,t->name);t++);
        switch(t->type) {
        case MeXMLActionEnd: /* unrecognised tag */
            err = MeXMLElementProcess(&start,skipAction,0,userdata);
            break;
        case MeXMLActionCallback:
            t->called = 1;
            err = ((MeXMLCallback)t->fn)(&start,userdata);
            break;
        default:
            t->called = 1;
            err = ((MeXMLParseFn)t->fn)(file, t, data);
            if (err) break;
            if (t->cb) if (err = t->cb(&start,data,userdata)) break;
            
            if(err = readNextTag(file, &end, ME_XML_TAG_BUFFER_SIZE)) break;
            if(end.name[0]!='/' || strcmp(end.name+1,start.name)) {
                sprintf(file->error,
                    "line %d, char %d: closing tag %s "
                    "does not match opening tag %s",
                    line, posn, end.name, start.name);
                err = MeXMLErrorMalformed;
            }
            break;
        }
        if(err!=MeXMLErrorNone) return err;
    }
}

/* --------------------------------------------- Input Stuff */

/**
Creates an XML input process.
*/
MeXMLInput * MEAPI MeXMLInputCreate(MeStream stream) {
    MeXMLInput *fi = (MeXMLInput *)MeMemoryAPI.create(sizeof(MeXMLInput));
    fi->bufptr = 4096;
    fi->bufmax = 4096;
    fi->top = 0;
    fi->stream = stream;
    fi->line = 1;
    fi->posn = 1;
    fi->eof = 0;
    strcpy(fi->error, "No error found\n");
    fi->userdata = 0;
    return fi;
}

/**
Destroys the input data at the end of the parse loop.
*/
void MEAPI MeXMLInputDestroy(MeXMLInput * input) {
    MeMemoryAPI.destroy(input);
}

/**
Set the user data associated with this process.
*/
void MEAPI MeXMLInputSetUserData(MeXMLInput * input, void *userData) {
    input->userdata = userData;
}

/**
Returns the user data associated with this process.
*/
void * MEAPI MeXMLInputGetUserData(const MeXMLInput * input) {
    return input->userdata;
}

/**
Returns the error string for the process.
*/
const char * MEAPI MeXMLInputGetErrorString(const MeXMLInput * input) {
    return input->error;
}

/**
Sets the error string.
*/
void MEAPI MeXMLInputSetErrorString(MeXMLInput *input, char *error, ...)
{
    va_list args;
    
    va_start(args, error);
    vsprintf(input->error, error, args);
    va_end(args);
}

/**
Starts the XML parse loop.
*/
MeXMLError MEAPI
MeXMLInputProcess(MeXMLInput * input, MeXMLHandler handlers[], void *userdata) {
    MeXMLElement e = { "", NULL, 0};
    e.fi = input ;
    return MeXMLElementProcess(&e,handlers,0,userdata);
}

/**
Given an element, returns the XML process input data.
*/
MeXMLInput * MEAPI MeXMLElementGetInput(MeXMLElement *elem) {
    return elem->fi;
}

/**
Returns 1 if a handler was called and 0 if it wasn't. This is the same as
saying "Was this XML tag present in the XML file".
*/
MeBool MEAPI
MeXMLHandlerWasCalled(MeXMLHandler handlers[],char *name)
{
    
    while(handlers->name)
    {
        if (strcmp(name,handlers->name) == 0)
            return handlers->called;
        handlers++;
    }
    return 0;
}

/**
Dynamically creates an element handler. This is useful if you don't
know in advance which handlers you will need.
*/
void MEAPI
MeXMLElementHandlerCreate(MeXMLHandler *handler,char *name,void *fn)
{
    handler->name = (char*)MeMemoryAPI.create(strlen(name)+1);
    strcpy(handler->name,name);
    handler->fn = fn;
    handler->type = MeXMLActionCallback;
    handler->called = 0;
    handler->max = 0;
    handler->maxstr = 0;
    handler->offset = 0;
}

/**
Destroy an element handler.
*/
void MEAPI
MeXMLElementHandlerDestroy(MeXMLHandler *handler)
{
    if (handler->name) MeMemoryAPI.destroy(handler->name);
}


