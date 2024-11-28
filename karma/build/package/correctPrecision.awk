#################################################################
#
# MathEngine Canada Inc.                                (c) 2001
#
# by: Michael Chowet
#
#
# This script mangles the content of MePrecision.h. It looks for:
#
# "#if 0" or "#if 1" 
#
# followed in quick succession by: 
#
# "#ifndef _ME_API_DOUBLE" of "#ifndef _ME_API_DOUBLE"
# (which actually preceeds "#define _ME_API_SINGLE" or "#define _ME_API_DOUBLE")
#
# It then tries to flip the test so that the correct setting will be
# left behind to agree with either the passed-in value of PRECISION
# or the environment variable DEF_PRECISION (for variants of AWK that
# correctly support ENVIRON[]).
#

/^#if [01]/ { seenAtLine = NR ;  stored = $0 ; next }

((NR-seenAtLine)==1) && /^#ifndef _ME_API_/ {
  specified = substr($0, 17)

  if (specified != "" &&
      (PRECISION == specified || ENVIRON["DEF_PRECISION"] == specified)) {
    stored = "#if 1"
  } else {
    stored = "#if 0"
  }
}



{
  if (stored != "") {
    print stored ; stored = "" ; seenAtLine = 0
  }
  print
}
