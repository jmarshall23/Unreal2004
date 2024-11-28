BEGIN {

  if (PREFIX != "")
    prefixLength = length(PREFIX)

  line = "###############################################################################\n"
  current = 0
  
}

/CFG=/{
  split($0, ARR, ":")
  if (prefixLength > 0 &&
      substr(ARR[1],1,prefixLength) == PREFIX)
    ARR[1] = "." substr(ARR[1], prefixLength+1)

  current++
  gsub("/", "\\", ARR[1])

  DSPfile    [current] = ARR[1]
  cfgBaseName[current] = substr(ARR[2],5,index(ARR[2], " ")-5)
  libsNeeded [current] = " "
  libsAvailable[cfgBaseName[current]] = 1
}

/ADD LINK32/ {
  
  for (i = 3; i <= NF; i++) {
    # Known library prefixes: Mst Mcd Me Mdt
    if (toupper($i) ~ /MST.*\.LIB/ ||
        toupper($i) ~ /MDT.*\.LIB/ ||
        toupper($i) ~ /MCD.*\.LIB/ ||
        toupper($i) ~  /ME.+\.LIB/  ) {
      libname = substr($i, 1, length($i)-4)
      gsub("_check\|_debug","",libname)
      if (! index(libsNeeded[current], " "libname" "))
        libsNeeded[current] = libsNeeded[current] libname " "
    }
  }
}


END {

  print "Microsoft Developer Studio Workspace File, Format Version 6.00"
  print "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!\n"

  for (i = 1; i <= current; i++) {
    print line "\nProject: \"" cfgBaseName[i] "\"=\"" DSPfile[i] "\" - Package Owner=<4>\n"
    print "Package=<5>\n{{{\n}}}\n"
    print "Package=<4>\n{{{"

    n = split(libsNeeded[i], libsNeededArr, " ")
    for (j = 1; j <= n; j++) {
      if (libsNeededArr[j] != "" && (libsNeededArr[j] in libsAvailable) ) {
        print "    Begin Project Dependency"
        print "    Project_Dep_Name " libsNeededArr[j]
        print "    End Project Dependency"
      }
    }

    print "}}}\n"
  }

  print line "\nGlobal:\n"
  print "Package=<5>\n{{{\n}}}\n"
  print "Package=<3>\n{{{\n}}}\n\n" line
}
