#
# $Id: vcvars32.sh,v 1.3.10.1 2002/04/11 03:45:34 jamesg Exp $
# $Name: t-stevet-RWSpre-030110 $
#
# sh version of vcvars32.bat
#
# Sets $PATH, $INCLUDE and $LIB to run Visual C++ under Cygwin.
#
# You of cource need to read this file using bash's "source" command.
#
# Note that $PATH is in cygwin format ("/", ":" and /cygdrive/c/...), but
# that the other two are in DOS format ("\\", ";" and C:/...)
#
# Adapted from Iestyn's version (i.e., really only set up for \\PANG at
# the mo.)

# Root of Visual Developer Studio Common files.
VSCommonDir=`cygpath -u 'C:\Tools\VStudio\Common'`

# Root of Visual Developer Studio installed files.
MSDevDir=`cygpath -u 'C:\Tools\VStudio\Common\msdev98'`

# Root of Visual C++ installed files.
MSVCDirWin='C:\Tools\VStudio\VC98'
MSVCDirUnix=`cygpath -u $MSVCDirWin`

# VcOsDir is used to help create either a Windows 95 or Windows NT specific path.
VcOsDir=WIN95
[ "x$OS" = xWindows_NT ] && VcOsDir=WINNT

echo 'Setting environment for using Microsoft Visual C++ tools.'

[ "x$OS" = xWindows_NT ] && PATH="$MSDevDir/BIN:$MSVCDirUnix/BIN:$VSCommonDir/TOOLS/$VcOsDir:$VSCommonDir/TOOLS:$PATH"
[ -z "$OS" ] && PATH="$MSDevDir/BIN:$MSVCDirUnix/BIN:$VSCommonDir/TOOLS/$VcOsDir:$VSCommonDir/TOOLS:$windir/SYSTEM:$PATH"

# These next two should be windows format
export INCLUDE="$MSVCDirWin\ATL\INCLUDE;$MSVCDirWin\INCLUDE;$MSVCDirWin\MFC\INCLUDE;${INCLUDE:-}"
export LIB="$MSVCDirWin\LIB;$MSVCDirWin\MFC\LIB;${LIB:-}"

unset VcOsDir
unset VSCommonDir

# Add DirectX 8 SDK
export INCLUDE="C:\Tools\sdk\DirectX8\include;$INCLUDE"
export LIB="C:\Tools\sdk\DirectX8\lib;$LIB"
