/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.20.6.3 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
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

#include <MeCommandLine.h>
#include <string.h>

/**
    Create a MeCommandLineOptions struct. This maintains a list of pointers
    to command line arguments. Note that argv will never get changed itself.
*/
MeCommandLineOptions * MEAPI MeCommandLineOptionsCreate(const int argc, const char ** argv) {
    int i;
    MeCommandLineOptions* options = MeMemoryAPI.create(sizeof(MeCommandLineOptions));

    if (!options)
        MeFatalError(0, "Unable to allocate memory for command line options");
    options->m_argc = argc;
    options->p_argv = MeMemoryAPI.create(sizeof(char*) * (argc + 1));
    if (!(options->p_argv))
        MeFatalError(0, "Unable to allocate memory for command line options");

    for(i=0; i!=argc; i++)
        options->p_argv[i] = argv[i];

    return options;
}

/**
    Clean up command line options.
*/
void MEAPI MeCommandLineOptionsDestroy(MeCommandLineOptions* options) {
    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsDestroy: You must pass in a valid MeCommandLineOptions*");

    MeMemoryAPI.destroy((char *)options->p_argv);
    MeMemoryAPI.destroy(options);
}

/**
    Eats command line arguments.
*/
void MEAPI MeCommandLineOptionsEat(MeCommandLineOptions* options, const int* eat, const int num_eat) {
    int i;
    int b_eat;
    int newargc = 0;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsEat: You must pass in a valid MeCommandLineOptions*");

    if (num_eat == 0)
        return;

    for(i = 0; i != options->m_argc; i++) {
        int j;
        b_eat = 0;

        for(j = 0; (j != num_eat) && (!b_eat); j++)
            if(eat[j] == i)
                b_eat = 1;

        if(!b_eat)
            options->p_argv[(newargc)++] = options->p_argv[i];
    }
    options->m_argc -= num_eat;
}

/**
    Returns 1 if command line argument was present, 0 if not.
*/
MeBool MEAPI MeCommandLineOptionsCheckFor(MeCommandLineOptions* options, const char * arg, const MeBool eat) {

    int i;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsGetPos: You must pass in a valid MeCommandLineOptions*");

    if (!arg)
        MeFatalError(0
            , "MeCommandLineOptionsGetPos: You must pass in a valid char*");

    i = MeCommandLineOptionsGetPos(options, arg);

    if(i == -1)
        return 0;
    else {
        if(eat)
            MeCommandLineOptionsEat(options, &i, 1);
        return 1;
    }
}

/**
    Returns 1 if any of command line arguments in list was present, 0 if not.
    N.B. List must be null-terminated.
*/
MeBool MEAPI MeCommandLineOptionsCheckForList(MeCommandLineOptions* options, const char * arglist[], const MeBool eat) {

    MeBool found = MEFALSE;
    int j;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsCheckForList: You must pass in a valid MeCommandLineOptions*");

    if (!arglist)
        MeFatalError(0
            , "MeCommandLineOptionsCheckForList: You must pass in a valid char*[]");

    for(j=0; arglist[j]; j++)
        if(MeCommandLineOptionsCheckFor(options, arglist[j], eat))
            found = !MEFALSE;

    return found;
}

/**
    Returns position of command line argument or -1 if not found.
*/
int MEAPI MeCommandLineOptionsGetPos(MeCommandLineOptions* options, const char * arg) {

    int i;
    MeBool found = 0;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsGetPos: You must pass in a valid MeCommandLineOptions*");

    for(i=0; (i != options->m_argc) && !found; i++)
        if(!strcmp(arg, options->p_argv[i]))
            found = 1;

    if(!found)
        return -1;
    else
        return i - 1;
}

/**
    Gets a numeric value from the command line. If the command line contained
    -number 10 the call MeCommandLineOptionsGetNumeric(o,"-number",1) would
    return 10 and eat -number and 10.
*/
int MEAPI MeCommandLineOptionsGetNumeric(MeCommandLineOptions* options, const char * arg, const MeBool eat) {
    int checker;
    int parameter;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsGetNumeric: You must pass in a valid MeCommandLineOptions*");

    if((checker = MeCommandLineOptionsGetPos(options, arg)) != -1) {
        if (checker < (options->m_argc - 1)) {
            parameter = atoi(options->p_argv[checker+1]);
            if (parameter || !strcmp(options->p_argv[checker+1], "0")) {
                /* We have a valid number. Eat argument and the number. */
                if(eat) {
                    int eats[2];
                    eats[0] = checker, eats[1] = checker + 1;
                    MeCommandLineOptionsEat(options, eats, 2);
                }
            } else {
                /* We don't, but take the parameter out anyway */
                if(eat)
                    MeCommandLineOptionsEat(options, &checker, 1);
                MeInfo(0,"Numerical parameter that should follow %s is missing! Defaulting to 0.", arg);
                return 0;
            }
        } else {
            MeCommandLineOptionsEat(options, &checker, 1);
            MeInfo(0,"Numerical parameter that should follow %s is missing! Defaulting to 0.", arg);
            return 0;
        }
        return parameter;
    } else
        /* Parameter simply not there */
        return 0;
}

/**
    Gets a floating point value from the command line.
*/
double MEAPI MeCommandLineOptionsGetFloat(MeCommandLineOptions* options, const char * arg, const MeBool eat) {
    int checker;
    double parameter;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsGetFloat: You must pass in a valid MeCommandLineOptions*");

    if((checker = MeCommandLineOptionsGetPos(options, arg)) != -1) {
        if (checker < (options->m_argc - 1)) {
            parameter = atof(options->p_argv[checker+1]);
            if (parameter || !strcmp(options->p_argv[checker+1], "0")) {
                /* We have a valid number. Eat argument and the number. */
                if(eat) {
                    int eats[2];
                    eats[0] = checker, eats[1] = checker + 1;
                    MeCommandLineOptionsEat(options, eats, 2);
                }
            } else {
                /* We don't, but take the parameter out anyway */
                if(eat)
                    MeCommandLineOptionsEat(options, &checker, 1);
                MeInfo(0,"Floating point parameter that should follow %s is missing! Defaulting to 0.", arg);
                return 0;
            }
        } else {
            MeCommandLineOptionsEat(options, &checker, 1);
            MeInfo(0,"Floating point parameter that should follow %s is missing! Defaulting to 0.", arg);
            return 0;
        }
        return parameter;
    } else
        /* Parameter simply not there */
        return 0;
}

/**
    Gets a string value from the command line. If the command line contained
    -string hello the call MeCommandLineOptionsGetNumeric(o,"-string",1) would
    return "hello" and eat -string and "hello".
*/
char * MEAPI MeCommandLineOptionsGetString(MeCommandLineOptions* options, const char * arg, const MeBool eat) {
    int checker;
    const char* parameter;

    if (!options)
        MeFatalError(0
            , "MeCommandLineOptionsGetNumeric: You must pass in a valid MeCommandLineOptions*");

    if((checker = MeCommandLineOptionsGetPos(options, arg)) != -1) {
        if (checker < (options->m_argc - 1)) {
            parameter = options->p_argv[checker+1];
            if (parameter[0] != '-') {
                /* We have a valid number. Eat argument and parameter. */
                if(eat) {
                    int eats[2];
                    eats[0] = checker, eats[1] = checker + 1;
                    MeCommandLineOptionsEat(options, eats, 2);
                }
            } else {
                /* We don't, but take the parameter out anyway */
                if(eat)
                    MeCommandLineOptionsEat(options, &checker, 1);
                MeInfo(0,"String parameter that should follow %s is missing! Returning null pointer.", arg);
                MeInfo(0,"String parameters aren't allowed to start with `-'.", arg);
                return 0;
            }
        } else {
            MeCommandLineOptionsEat(options, &checker, 1);
            MeInfo(0,"String parameter that should follow %s is missing! Returning null pointer.", arg);
            MeInfo(0,"String parameters aren't allowed to start with `-'.", arg);
            return 0;
        }
        return (char *)parameter;
    } else
        /* Parameter simply not there */
        return 0;
}
