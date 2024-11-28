:

SELECTED_PLATFORM=none

if [ "${PLATFORM}" = "win32" ] ; then
	SELECTED_PLATFORM=${PLATFORM}
	LIB_PREFIX=
	LIB_SUFFIX=.lib
	EXE_SUFFIX=.exe
	DO_STRIP=0
	STRIP='rebase'
fi
if [ "${PLATFORM}" = "xbox" ] ; then
        SELECTED_PLATFORM=${PLATFORM}
        LIB_PREFIX=
        LIB_SUFFIX=.lib
        EXE_SUFFIX=.xbe
        DO_STRIP=0
        STRIP='rebase'
fi

if [ "${PLATFORM}" = "ps2" ] ; then
	SELECTED_PLATFORM=${PLATFORM}
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=.elf
	DO_STRIP=1
	STRIP='ee-strip -s'
fi


if [ "${PLATFORM}" = "irix" ] ; then
	SELECTED_PLATFORM=${PLATFORM}
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=
	DO_STRIP=1
	STRIP='strip -s'
fi

if [ "${PLATFORM}" = "linux" ] ; then
	SELECTED_PLATFORM=${PLATFORM}
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=
	DO_STRIP=1
	STRIP='strip -s'
fi

if [ "${PLATFORM}" = "linux_hx" ] ; then
	SELECTED_PLATFORM=linux
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=
	DO_STRIP=1
	STRIP='strip -s'
fi

if [ "${PLATFORM}" = "linux_hx_cc" ] ; then
	SELECTED_PLATFORM=linux
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=
	DO_STRIP=1
	STRIP='strip -s'
fi

if [ "${PLATFORM}" = "macos" ] ; then
	SELECTED_PLATFORM=${PLATFORM}
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=
	DO_STRIP=0
	STRIP='strip -s'
fi

if [ "${PLATFORM}" = "ngc" ] ; then
	SELECTED_PLATFORM=${PLATFORM}
	LIB_PREFIX=lib
	LIB_SUFFIX=.a
	EXE_SUFFIX=.elf
	DO_STRIP=0
	STRIP='strip -s'
fi

echo Selected platform = ${SELECTED_PLATFORM}
