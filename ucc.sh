#!/bin/sh
#
# ucc startup script
#

# Function to find the real directory a program resides in.
# Feb. 17, 2000 - Sam Lantinga, Loki Entertainment Software
FindPath()
{
    fullpath="`echo $1 | grep /`"
    if [ "$fullpath" = "" ]; then
        oIFS="$IFS"
        IFS=:
        for path in $PATH
        do if [ -x "$path/$1" ]; then
               if [ "$path" = "" ]; then
                   path="."
               fi
               fullpath="$path/$1"
               break
           fi
        done
        IFS="$oIFS"
    fi
    if [ "$fullpath" = "" ]; then
        fullpath="$1"
    fi
    # Is the awk/ls magic portable?
    if [ -L "$fullpath" ]; then
        fullpath="`ls -l "$fullpath" | awk '{print $11}'`"
    fi
    dirname $fullpath
}

# Set the home if not already set.
if [ "${UT2003_DATA_PATH}" = "" ]; then
    UT2003_DATA_PATH="`FindPath $0`/System"
fi

LD_LIBRARY_PATH=.:${UT2003_DATA_PATH}:${LD_LIBRARY_PATH}

export LD_LIBRARY_PATH

# Let's boogie!
if [ -x "${UT2003_DATA_PATH}/ucc-bin" ]
then
	cd "${UT2003_DATA_PATH}/"
	exec "./ucc-bin" $*
fi
echo "Couldn't run ucc (ucc-bin). Is UT2003_DATA_PATH set?"
exit 1

# end of ucc ...
