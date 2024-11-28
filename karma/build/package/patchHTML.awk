#
# MathEngine Canada Inc.                                     (c) 2000-2001
# Written by: Michael Chowet
#
# Filter intended for HTML files. Will filter out bits selectively by
# platform (set in the environment) using comments in the HTML code.
#
# The HTML code is expected to surround portions of text that are to
# be optionally filtered with comments of the form:
#
# <!-- PLATFORM-START [platform]* -->
# ...
# <!-- PLATFORM-END -->
#
# NOTES: 
#  - These comments must be on their own lines (though surrounding
#    whitespace is ignored).
#  - These comments are expected NOT to be nested. No guarantees
#    are made as to behaviour, should nested comments be found.


BEGIN {
  currentPlatform = ENVIRON["PLATFORM"]
  leaveItIn = 1
}


#
# <!-- PLATFORM-START plat1 plat2 -->
#
/^[ \t]*<!--[ \t]*PLATFORM-START[^-]*-->[ \t]*$/ {

  #
  # Try to adjust for no space between keyword and comment-start
  #
  if (length($1) > 4) start = 2
  else                start = 3

  #
  # Try to adjust for no space between platforms and comment-end
  #
  if ($NF != "-->") {
    $NF = substr($NF,1,length($NF)-3)
    $(NF+1) = "-->"
  }

  #
  # Now, see if we match any of the listed platforms
  #
  leaveItIn = 0
  for (count = start; count <= NF-1; count++)
    leaveItIn = leaveItIn || ( currentPlatform == $count )

  next

}

#
# Main rule! :^)
# Spit out text we're not filtering out
#
leaveItIn {  print  }

#
# <!-- PLATFORM-END -->
#
/^[ \t]*<!--[ \t]*PLATFORM-END[^-]*-->[ \t]*$/ {

  leaveItIn = 1
  next
}
