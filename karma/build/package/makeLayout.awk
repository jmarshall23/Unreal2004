#!/bin/awk

BEGIN {

  split("", directory, ":")
  currentDir = "."

  if (OUTFILE == "")      OUTFILE = "/dev/stdout"
  if (ERRFILE == "")      ERRFILE = "/dev/stderr"
  if (MODE != "APPEND")   printf "" > OUTFILE
  if (SCRATCH_FILE == "") SCRATCH_FILE = "_exec_SCRATCH.@@@"

  printf "" > SCRATCH_FILE

  print "generate_image() {" >> OUTFILE

}

###########################################################
#
# inConditional is a state variable:
#  0 - not in a conditional
#  1 - in a conditional, before any other lines
#  2 - in a conditional, after having seen a line of code

###########################################################
# Swallow blank lines and comments
NF<1                  { next }
substr($1,1,1)==";"   { next }


{

#  print ""
#  print " > " $0

  isLibrary    = 0
  isDirectory  = 0
  isExecutable = 0

  shift = 0  

  originalLine = $0
  $1 = $1
  line = $0
  directive = $0

  source = ""
  target = ""

}

/^[ \t]*[%]generate/ {

# Send output to SCRATCH_FILE, so it can be appended as
# actions at the end of execution, since some tools may
# require the rest of the image to already be in place.

  sub("^[ \t]*","",originalLine)
  savedLine1 = originalLine
  sub("^[%]generate[ \t]*","", originalLine)

  getline   # Do we want to put this in a loop, allowing multiple
            # %command statements constituting a larger amount of 
            # work to create the file?
  if ($1 != "%command") {

    signalError("E", NR, "found %generate without associated %command; ignoring")

  } else {

    # Split up the %generate line into target and precedents

    eq = index(originalLine, "=")

    if (eq == 0) {
      _B[1] = originalLine
      _B[2] = ""
    } 
    else if (eq > 0) {
      _B[1] = substr(originalLine, 1, eq-1)
      _B[2] = substr(originalLine, eq+1)
    }

    sub("^[ \t]+|[ \t]+$","",_B[1])
    gsub("^| "," ${GEN_SOURCE}/",_B[2])
    _Bminus[1] = _B[1]
    _B[1] = "${GEN_TARGET}/" _B[1]
    sub("^[ \t]+|[ \t]+$","",_B[2])


    # Calculate the command we've passed in, expanding variables
    commandLine = $0
    sub("^[ \t]*","",commandLine)
    savedLine2 = commandLine
    sub("^[%]command[ \t]*","", commandLine)

    print "# " commandLine >> OUTFILE

    gsub("\\$@", _B[1], commandLine)
    gsub("\\$<", _B[2], commandLine)
    gsub("\\$\\+", _B[2], commandLine)
    gsub("\\$\\(@D\\)",currentDir, commandLine)

    # Now, spit out the code we've found into the files,
    # and hook in execution of the function we're adding

    print "# In [" currentDir "]" >> SCRATCH_FILE
    print "#  -: " savedLine1     >> SCRATCH_FILE
    print "#  -: " savedLine2     >> SCRATCH_FILE

    generateIndex++
    printf("%sTO_GENERATE=\"${TO_GENERATE} %04d\"\n",
           indent, generateIndex) >> OUTFILE
    printf("gen_command_%04d () {\n\n (\n", generateIndex) >> SCRATCH_FILE
    #print "  cd " currentDir >> SCRATCH_FILE
    print  "  GEN_TARGET="currentDir >> SCRATCH_FILE
    print  "  echo Generating " _B[1] >> SCRATCH_FILE
    print  "  " commandLine "\n )\n\n}\n" >> SCRATCH_FILE

    eq = index("%command[ \t]*", $0)
    theCommand = substr($0, RSTART+RLENGTH)

    next
  }

}


###########################################################
# Process conditional sections

toupper($1) == "!IFEQ" || toupper($1) == "!IFNEQ" {
  conditionals++
  conditionalsPending++
  inConditional=1

  if (NF != 3) {
  
    signalError("E", NR, "incorrectly formed test; replacing with tautology")
    line = "!IFNEQ 'malformed' 'statement'"
    print "\n" indent "if [ ! 'malformed' = 'statement' ] ; then" >> OUTFILE

  }

  else {

    # Save the test, in case we need it later
    sub("^[ \t]+", "", line)
    sub("[ \t]+$", "", line)
    condition[conditionalsPending] = line
    

    # Clean up the value, making sure of quoting
    sub("^[\"']", "'", $3)
    sub("[\"']$", "'", $3)

    _1 = substr($3,1,1)
    _2 = substr($3,length($3),1)
    $3 = ((_1 == "'") ? "" : "'") $3 ((_2 == "'") ? "" : "'")
        
    printf("\n%sif [%s \"${%s}\" = %s ] ; then\n",
           indent,
           (toupper($1) == "!IFNEQ") ? " !" : "",
           $2,
           $3) >> OUTFILE
              
  }
  indent = "  " indent
  next

}

toupper($1) == "!ELSE" {

  if (conditionalsPending < 1)
    signalError("E", NR, "encountered !ELSE outside conditional clause; ignoring")
  else if (NF > 1)
    signalError("W", NR, "!ELSE takes no parameters; ignoring")
  else {
    if (inConditional == 1)
      print indent ":" >> OUTFILE
    inConditional = 1
    print substr(indent,3) "else" >> OUTFILE
  }
  next

}

toupper($1) == "!ENDIF" {

  if (! conditionalsPending > 0) {
    signalError("E", NR, "encountered !ENDIF outside conditional clause; ignoring")
  }
  else {
    if (inConditional == 1)
      print indent ":" >> OUTFILE
    inConditional = 0
    conditionalsPending--
    indent = substr(indent,3)
    print indent "fi\n" >> OUTFILE
  }
  next
}


############################################################
# Process a directory section header
substr($1,1,1)=="[" && substr($0,length($0),1)=="]" {

  currentDir = substr($1,2,length($1)-2)
  setOutputPath(currentDir)
  directory[currentDir] = 1

  next
}

############################################################
# Catch keyword rules
substr($1,1,1) == "%" {

  if ($1 == "%library")
    isLibrary = 1
  else if ($1 == "%directory")
    isDirectory = 1
  else if ($1 == "%executable")
    isExecutable = 1
  else
    signalError("W", NR, "ignoring unknown directive type: " $1)

  shift = 1
  line = substr(line, length($1)+1)

}


(NF - shift) < 1 { 
  signalError("W", NR, "too few parameters; ignoring line") ; next
}

(NF - shift) > 3 {
  signalError("W", NR, "too many parameters; ignoring extras")
}

index(line, "=") != 0 {

  loc = index(line, "=")
  target = substr(line, 1, loc-1)
  source = substr(line, loc+1)
  
  sub("^[ \t]+", "", source)
  sub("[ \t]+$", "", source)

  sub("^[ \t]+", "", target)
  sub("[ \t]+$", "", target)

  if (loc = index(target, "/") != 0) {
    signalError("W", NR, "path in target is illegal; stripping path info")
    while (loc > 0) {
      target = substr(target, loc+1)
      loc = index(target, "/")
    }
  }

  if ((loc = index(target, " ")) != 0) {
    signalError("W", NR, "spaces in target are illegal; ignoring beyond first blank")
    target = substr(target, 1, loc-1)
  }

  if ((loc = index(source, " ")) != 0) {
    source = substr(source, 1, loc-1)
    signalError("W", NR, "spaces in source are illegal; ignoring beyond first blank")
  }

#  print "SRC = '" source "' ==> TGT = '" target "'"
  
}


index(line, "=") == 0 {

  source = line
  sub("^[ \t]+", "", source)
  sub("[ \t]+$", "", source)

  if ((loc = index(source, " ")) != 0) {
    signalError("W", NR, "spaces in source are illegal; replacing with placeholder")
    gsub("[ \t]", "_", source)
  }

  target = source
  if (loc = index(target, "/") != 0) {
    while (loc > 0) {
      target = substr(target, loc+1)
      loc = index(target, "/")
    }
  }
}


{

  if (isDirectory) {
    copyDirectory(source, target)
    if (inConditional) inConditional = 2
  }
  else if (isLibrary) {
    copyLibrary(source, target)
    if (inConditional) inConditional = 2
  }
  else if (isExecutable) {
    copyExecutable(source, target)
    if (inConditional) inConditional = 2
  }
  else {
    copyFile(source, target)
    if (inConditional) inConditional = 2
  }

}

###########################################################################
#
# Bring it all home...

END {

  message = ""
  
  if (warnsFound > 0) {
    message = warnsFound " warning" ((warnsFound == 1)? "" : "s")
  }

  if (errorsFound > 0) {
    if (message != "") message = message " and"
    message = message " " errorsFound " error" ((errorsFound == 1)? "" : "s")
  }

  if (message != "") {
    message = "\n# " message " encountered during generation of this script"
    print message >> OUTFILE
    exit -1
  }

  print "}\n" >> OUTFILE

  close(SCRATCH_FILE)
  while ((getline text < SCRATCH_FILE) == 1) {
    print text >> OUTFILE
  }
  close(SCRATCH_FILE)
  system("rm -f " SCRATCH_FILE)

  print "" >> OUTFILE
  
}

###########################################################################
#
# Utility functions
#

function signalError(errtype, lineno, text) 
{

  if (toupper(errtype) == "W") {
    print "WARNING (" lineno "): " text >> ERRFILE
    warnsFound++
  }
  else if (toupper(errtype) == "E") {
    print "ERROR (" lineno "): " text >> ERRFILE
    errorsFound++
  }
  else {
    print errtype " (" lineno "): " text >> ERRFILE
    abnormsFound++
  }
  
#  print " > " $0

}


function setOutputPath(path) {
  print  "\n" indent "# " directive >> OUTFILE
  path = "${GEN_ROOT}/" path ; gsub("//", "/", path)
  sub("/$", "", path)
  currentDir = path

  printf indent "GEN_TARGET=" path "\n" >> OUTFILE
  if (path in directory)
    printf indent "make_dir     ${GEN_TARGET}\n" >> OUTFILE
  else
#   printf indent "make_new_dir ${GEN_TARGET}\n" >> OUTFILE
    printf indent "make_dir     ${GEN_TARGET}\n" >> OUTFILE
}  


function OUT(command, arg1, arg2) {
  count = length(command) + length(arg1) + 1
  printf("%s %s", command, arg1) >> OUTFILE
  if (count + length(arg2) > 75)            # try to limit lines to 75 chars
    printf(" \\\n%"length(command)"s", "") >> OUTFILE
  printf " " arg2 "\n" >> OUTFILE
}

function copyDirectory(src, tgt) {
  src = "${GEN_SOURCE}/"src ; gsub("//", "/", src)
  tgt = "${GEN_TARGET}/"tgt ; gsub("//", "/", tgt)
  OUT(indent "copy_dir    ", src, tgt)
}

function copyLibrary(src, tgt) {
  src = "${GEN_SOURCE}/"src ; gsub("//", "/", src)
  tgt = "${GEN_TARGET} "tgt ; gsub("//", "/", tgt)
  OUT(indent "copy_lib    ", src, tgt)
}

function copyFile(src, tgt) {
  src = "${GEN_SOURCE}/"src ; gsub("//", "/", src)
  tgt = "${GEN_TARGET}/"tgt ; gsub("//", "/", tgt)
  OUT(indent "copy_file   ", src, tgt)
}

function copyExecutable(src, tgt) {
  src = "${GEN_SOURCE}/"src ; gsub("//", "/", src)
  tgt = "${GEN_TARGET}/"tgt ; gsub("//", "/", tgt)
  OUT(indent "copy_exec   ", src, tgt)
}

