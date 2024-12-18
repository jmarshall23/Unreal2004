#!/bin/sh

##
# Open a terminal window
#
# Wilfredo Sanchez | wsanchez@mit.edu
# Copyright (c) 2002 Wilfredo Sanchez Vega.
# All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for
# any purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHORS BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
##

##
# Functions
##

tell ()
{
  app=$1; shift;

  osascript -e "tell application \"${app}\" $*"
}

tell_queue ()
{
  app=$1; shift;

  if [ -z "${TellQueueFile}" ] || [ ! -f "${TellQueueFile}" ]; then
    if ! TellQueueFile=$(mktemp -t terminal_script_queue); then
      # Can't create temp file, so do it the slow way.
      tell "${app}" "$@";
      return;
    else
      echo "ignoring application responses" >> "${TellQueueFile}";
    fi;
  fi;

  echo 'tell application "'"${app}"'" '"$*" >> "${TellQueueFile}";
}
TellQueueFile="";

do_tell_queue ()
{
  if [ -f "${TellQueueFile}" ]; then
    echo "end ignoring" >> "${TellQueueFile}";
    osascript "${TellQueueFile}";
    rm -f "${TellQueueFile}";
  fi;
}

set_bool ()
{
  variable="$1"; bool="$2";

  if ! ( [ "${bool}" = "true" ] || [ "${bool}" = "false" ] ); then
    usage "Invalid boolean: ${bool}";
    exit 1;
  fi;

  eval "${variable}='${bool}'";
}

set_value ()
{
  variable="$1"; value="$2";

  if [ -z "${value}" ]; then
    usage "Invalid value: ${value}";
    exit 1;
  fi;

  eval "${variable}='${value}'";
}

##
# Handle command line
##

usage ()
{
  program=$(basename "$0");

  if [ $# != 0 ]; then echo "$@"; echo ""; fi;

  echo "${program}: usage:";
  echo "    ${program} [-options] [-e command args]";
  echo "";
  echo "options:";
  echo "    --help               print this help";
  echo "    --version            print Terminal's version number";
  echo "    --isfrontmost        print whether Terminal is the frontmost app";
  echo "    --activate           activate Terminal (bring to front)";
  echo "    --size WxH           set window size in pixels";
  echo "    --textsize WxH       set window size in characters";
  echo "    --position XxY       set window screen position relative to top left corner";
  echo "    --origin XxY         set window screen position relative to bottom left corner";
  echo "    --miniaturized bool  miniaturize window";
  echo "    --title name         set window title; implies \"--showtitle true\"";
  echo "    --bgcolor color      set window background color";
  echo "    --textcolor color    set window text color";
  echo "    --boldcolor color    set window bold text color";
  echo "    --cursorcolor color  set window cursor color";
  echo "    --showdevice bool    show device name in window title bar";
  echo "    --showshell bool     show shell in window title bar";
  echo "    --showsize bool      show size in window title bar";
  echo "    --showfile bool      show file in window title bar";
  echo "    --showtitle bool     show custom title in window title bar";
}

# Initialize variables:
       T_Activate="NO";
        T_Command="";
          T_Width="";
         T_Height="";
        T_Columns="";
           T_Rows="";
   T_WhereFromTop="";
T_WhereFromBottom="";
  T_WhereFromLeft="";
     T_ShowDevice="";
      T_ShowShell="";
       T_ShowSize="";
       T_ShowFile="";
      T_ShowTitle="";
          T_Title="";
T_BackgroundColor="";
      T_TextColor="";
  T_BoldTextColor="";
    T_CursorColor="";

# Process arguments
while [ $# != 0 ]; do
  case $1 in
    --help|-h)
      usage;
      exit 0;
      ;;
    --version)
      tell Terminal to get its version;
      exit 0;
      ;;
    --isfrontmost)
      tell Terminal to get frontmost;
      exit 0;
      ;;
    --activate|-a)
      T_Activate="YES"; shift;
      ;;
    --size)
      size="$2"; shift 2;
        T_Width=$(echo "${size}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\1/');
       T_Height=$(echo "${size}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\2/');
      T_Columns="";
         T_Rows="";
      if [ ! "${T_Width}" -gt 0 ] || [ ! "${T_Height}" -gt 0 ]; then
        usage "Invalid size: ${T_Width} x ${T_Height}";
        exit 1;
      fi 2>/dev/null;
      ;;
    --textsize)
      size="$2"; shift 2;
        T_Width="";
       T_Height="";
      T_Columns=$(echo "${size}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\1/');
         T_Rows=$(echo "${size}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\2/');
      if [ ! "${T_Columns}" -gt 0 ] || [ ! "${T_Rows}" -gt 0 ]; then
        usage "Invalid text size: ${T_Columns} x ${T_Rows}";
        exit 1;
      fi 2>/dev/null;
      ;;
    --position)
      where="$2"; shift 2;
         T_WhereFromTop=$(echo "${where}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\1/');
      T_WhereFromBottom="";
        T_WhereFromLeft=$(echo "${where}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\2/');
      if ! ( [ "${T_WhereFromTop}"  -ge 0 ] || [ "${T_WhereFromTop}"  -lt 0 ] ) &&
         ! ( [ "${T_WhereFromLeft}" -ge 0 ] || [ "${T_WhereFromLeft}" -lt 0 ] ); then
        usage "Invalid position: ${T_WhereFromTop} x ${T_WhereFromLeft}";
        exit 1;
      fi 2>/dev/null;
      ;;
    --origin)
      where="$2"; shift 2;
         T_WhereFromTop="";
      T_WhereFromBottom=$(echo "${where}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\1/');
        T_WhereFromLeft=$(echo "${where}" | sed 's/^\([0-9]*\)x\([0-9]*\).*$/\2/');
      if ! ( [ "${T_WhereFromBottom}" -ge 0 ] || [ "${T_WhereFromBottom}" -lt 0 ] ) &&
         ! ( [ "${T_WhereFromLeft}"   -ge 0 ] || [ "${T_WhereFromLeft}"   -lt 0 ] ); then
        usage "Invalid position: ${T_WhereFromBottom} x ${T_WhereFromLeft}";
        exit 1;
      fi 2>/dev/null;
      ;;
    --title|-t)
      shift; if [ $# = 0 ]; then usage "No title."; exit 1; fi;
      T_Title="$1"; shift;
      T_ShowTitle="true";
      ;;
    --bgcolor)      set_value T_BackgroundColor "$2"; shift 2; ;;
    --textcolor)    set_value T_TextColor       "$2"; shift 2; ;;
    --boldcolor)    set_value T_BoldTextColor   "$2"; shift 2; ;;
    --cursorcolor)  set_value T_CursorColor     "$2"; shift 2; ;;
    --showdevice)   set_bool  T_ShowDevice      "$2"; shift 2; ;;
    --showshell)    set_bool  T_ShowShell       "$2"; shift 2; ;;
    --showsize)     set_bool  T_ShowSize        "$2"; shift 2; ;;
    --showfile)     set_bool  T_ShowFile        "$2"; shift 2; ;;
    --showtitle)    set_bool  T_ShowTitle       "$2"; shift 2; ;;
    --miniaturized) set_bool  T_Miniaturized    "$2"; shift 2; ;;
    -e)
      shift; if [ $# = 0 ]; then usage "No command."; exit 1; fi;
      T_Command=$(echo "$@" | sed 's/"/\\"/g');
      break;
      ;;
    --tell)
      shift; if [ $# = 0 ]; then usage "No tell."; exit 1; fi;
      tell Terminal to "$@";
      exit 0;
      ;;
    *)
      usage "Invalid argument: $1"; shift;
      exit 1;
      ;;
  esac;
done;

##
# Do The Right Thing
##

## Redirect stdout because osascript clears the screen for kicks.
#tell Terminal to do script \"${T_Command}\" >/dev/null; #"
#CommandWindow=$(tell Terminal to get its front window);
#
# Use tell_queue for speed.  But then we reply on the front window staying in front.
tell_queue Terminal to do script \"${T_Command}\" #"
CommandWindow="its front window";

if [ -z "${CommandWindow}" ]; then
  echo "Error finding front window.  Aborting.";
  exit 1;
fi;

set_window () { tell_queue Terminal to tell "${CommandWindow}" to set its "$@"; }

if [ -n "${T_Width}" ] && [ -n "${T_Height}" ]; then
  set_window size to '(' "${T_Width}" , "${T_Height}" ')';
fi;

if [ -n "${T_Columns}" ] && [ -n "${T_Rows}" ]; then
  set_window number of columns to "${T_Columns}";
  set_window number of rows    to "${T_Rows}";
fi;

if [ -n "${T_WhereFromTop}" ] && [ -n "${T_WhereFromLeft}" ]; then
  set_window position to '(' "${T_WhereFromTop}" , "${T_WhereFromLeft}" ')';
fi;

if [ -n "${T_WhereFromBottom}" ] && [ -n "${T_WhereFromLeft}" ]; then
  set_window origin to '(' "${T_WhereFromBottom}" , "${T_WhereFromLeft}" ')';
fi;

[ -n "${T_Title}" ] && set_window custom title to \""${T_Title}"\"; #"

[ -n "${T_BackgroundColor}" ] && set_window background  color to \""${T_BackgroundColor}"\"; #"
[ -n "${T_TextColor}"       ] && set_window normal text color to \""${T_TextColor}"\"      ; #"
[ -n "${T_BoldTextColor}"   ] && set_window bold text   color to \""${T_BoldTextColor}"\"  ; #"
[ -n "${T_CursorColor}"     ] && set_window cursor      color to \""${T_CursorColor}"\"    ; #"

[ -n "${T_ShowDevice}" ] && set_window title displays device name  to "${T_ShowDevice}";
[ -n "${T_ShowShell}"  ] && set_window title displays shell path   to "${T_ShowShell}" ;
[ -n "${T_ShowSize}"   ] && set_window title displays window size  to "${T_ShowSize}"  ;
[ -n "${T_ShowFile}"   ] && set_window title displays file name    to "${T_ShowFile}"  ;
[ -n "${T_ShowTitle}"  ] && set_window title displays custom title to "${T_ShowTitle}" ;

# Has to be last because in the tell_queue case, we don't know the window ID,
# so we need it to be the front window.  Should be first for performance.
[ -n "${T_Miniaturized}" ]&& set_window miniaturized to "${T_Miniaturized}";

if [ "${T_Activate}" = "YES" ]; then tell_queue Terminal to activate; fi;

do_tell_queue;

exit 0;


