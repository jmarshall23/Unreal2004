;= $Header: /proj/Toolkits/build/makerules/configs/Readme.txt,v 1.1.10.1 2002/04/11 03:45:35 jamesg Exp $
;= $Name: t-stevet-RWSpre-030110 $

These files are part of cowpat make.

Each file defines a canned configuration. These basically compress cowpat
command lines a bit. There are personal ones (you can do what you like)
and project-official ones (which you shouldn't edit without authorisation
in triplicate). See the comments in each file.

Note: These files should simply declare aliases for groups of standard cowpat
command-line variables. Ideally they should depend on no-other cowpat
files. They are provided as a simple convenience. (You can achieve just
about the same effect with multiple "-f" options to make.)

Cowpat allows multiple comma-separated configs to be chained togeter:

	make config:=mothstd,idw

Thus, you may have to think carefully about using "?=", ":=" or "=".
