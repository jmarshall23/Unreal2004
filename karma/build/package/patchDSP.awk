#=================================================================
#
#  MathEngine Canada Inc                            (c) 2000-2001
#
#   Original author: Michael Chowet
#
#
#   Filter for Visual Studio 6 DSP files. Processes the include
#   and link paths, replacing anything relative by a set of
#   paths provided, passed in via the FORCED_INC and FORCED_LIB
#   variables.
#
#   As an additional side-effect, the DSP will be "cleansed" of
#   references to non-Microsoft compilation tools that might have
#   been introduced by outside tools hooking into VC (e.g. Intel
#   optimizing compiler).
#
#   Last modified by: $Author: michaelc $
#   Last modified on: $Date: 2001/06/20 16:43:00 $
#
#=================================================================

BEGIN {

  projType = "unknown"
  debug = "unknown"

  if (FORCED_INC != "") {
    FORCED_INC_DIRECTIVE = "/I \"" FORCED_INC "\""
    gsub(":", "\" /I \"", FORCED_INC_DIRECTIVE)
    gsub(";;", ":", FORCED_INC_DIRECTIVE)
  }

  if (FORCED_LIB != "") {
    FORCED_LIB_DIRECTIVE = "/libpath;;\"" FORCED_LIB "\""
    gsub(":", "\" /libpath;;\"", FORCED_LIB_DIRECTIVE)
    gsub(";;", ":", FORCED_LIB_DIRECTIVE)
  }

  if (FORCED_LIB_ROOT != "") {
  }

  if (FORCED_OUT_ROOT != "") {
  }

  if (FORCED_LIB_ADDFIRST != "") {
      ADD_LIB_DIRECTIVE = FORCED_LIB_ADDFIRST
  }

#  if (FORCED_LIB_CONFIG != "") {
#    gsub("/win32", "/"FORCED_LIB_CONFIG, FORCED_LIB_DIRECTIVE)
#  }

  if (ADD_DEFINE != "") {
    num_defs = split(ADD_DEFINE, definitions, " ,")
    for (i = 1; i <= num_defs; i++)
      ADD_DEFINE_DIRECTIVE = " /D \"" definitions[i] "\""
  }

  if (ADD_UNDEFINE != "") {
    num_defs = split(ADD_UNDEFINE, definitions, " ,")
    for (i = 1; i <= num_defs; i++)
      ADD_UNDEFINE_DIRECTIVE = " /U \"" definitions[i] "\""
  }

  if (REMOVE_DEFINE != "") {
    REMOVE_DEFINE_DIRECTIVE = " /D \"" REMOVE_DEFINE "\""
  }

  stripConfigs = (stripSuffixes != "")

}

#
# Rules for stripping out CPP configurations
#

stripConfigs && /CFG=/ { 
  sub("- Win32 Release " stripSuffixes, "- Win32 Release")
  sub("- Win32 Debug "   stripSuffixes, "- Win32 Debug")
}

stripConfigs && ( /^!MESSAGE/ || /^# Name/) {
  if ($0 ~ ("[Debug|Release] " stripSuffixes)) next
}

stripConfigs && /^!(ELSE)?IF +\"\$\(CFG\)\"/ {
  firstWord = $1
  if ($0 ~ ("[Debug|Release] " stripSuffixes)) {
    do { 
      getline NEWLINE
    } while (!(((NEWLINE ~ "^!ENDIF") ||
              ((NEWLINE ~ "^!ELSEIF") && (NEWLINE !~ "[Debug|Release] " stripSuffixes)))))
    $0 = NEWLINE
    if (firstWord ~ "^!IF" && $1 ~ "^!ELSEIF") sub("^!ELSEIF","!IF")
  }
}

#
# Figure out if we are parsing the debug section
#

/^# PROP Use_Debug_Libraries/ {
  if ($0 ~ /1/) {debug=1}
  else {debug=0}

  ADD_LIB_DIRECTIVE = FORCED_LIB_ADDFIRST
  if (debug)
     gsub("CONFIG", "dbg", ADD_LIB_DIRECTIVE)
  else
     gsub("CONFIG", "rel", ADD_LIB_DIRECTIVE) 
}

#
# Figure out which kind of project we're dealing with
#

/^# TARGTYPE/ {

  if      ($0 ~ /Application"/) { projType = "app" ; linker = "link.exe" }
  else if ($0 ~ /Library"/)     { projType = "lib" ; linker = "link.exe -lib" }
  else {
    #print "Unknown targettype!"
    #print "  -> " substr($0,12,length($0)-11-6)
    linker = "link.exe"
  }

}



#
# Make sure the tools are correct
#

/^LIB32=/ { print "LIB32=" linker     ; next }
/^BSC32=/ { print "BSC32=bscmake.exe" ; next }
/^CPP=/   { print "CPP=cl.exe"        ; next }
/^RSC=/   { print "RSC=rc.exe"        ; next }


#
# Make sure the include paths are correct
#

/^# ADD BASE CPP/ { 
  isCPP = 2 ; #print "*** IS CPP" 
}

/^# ADD CPP/      { 
  isCPP = 1 ; #print "*** IS CPP" 
}

isCPP {

  gsub(" /I \"\.\.[^\"]+\"", "")

  if (FORCED_INC_DIRECTIVE != "")
  {
    if (isCPP == 1)
      $0 = substr($0, 1, 10) FORCED_INC_DIRECTIVE " " substr($0, 11)
  }

  if (FORCED_LIB_CONFIG != "" )
  {
    # Should this happen to the library .dsps ?
    if (isCPP == 1)
    {
      if (FORCED_LIB_CONFIG ~/libcmt/ )
        libType = "/MT";
      else if (FORCED_LIB_CONFIG ~/libc/ )
        libType = "/ML";
      else if (FORCED_LIB_CONFIG ~/msvcrt/ )
        libType = "/MD";
      else
        libType = "/MD"; # default to msvcrt (must change if default changes)

      # d is for debug
      if ($0 ~/\"_DEBUG\"/ )
        libType = libType"d";

      # Remove all old runtime library specifications
      gsub("/MDd", "") # MSVCRTD
      gsub("/MLd", "") # LIBCD
      gsub("/MTd", "") # LIBCMTD
      gsub("/MD", "")  # MSVCRT
      gsub("/ML", "")  # LIB
      gsub("/MT", "")  # LIBCMT
      $0 = substr($0, 1, 10) libType " " substr($0, 11)

      if (REMOVE_DEFINE_DIRECTIVE != "")
      {
        gsub(REMOVE_DEFINE_DIRECTIVE, "")
      }

      if (ADD_DEFINE_DIRECTIVE != "")
      {
        $0 = $0 ADD_DEFINE_DIRECTIVE
      }

      if (ADD_UNDEFINE_DIRECTIVE != "")
      {
        $0 = $0 ADD_UNDEFINE_DIRECTIVE
      }
    }
  }

}

#
# Make sure the library paths are correct
#

/^# ADD BASE LINK32/ { 
  isLINK = 2 ; #if (DEBUG) print "*** IS LINK" 
}

/^# ADD LINK32/ {
  isLINK = 1 ; #if (DEBUG) print "*** IS LINK"
}

isLINK {

  gsub("_[Dd][Ee][Bb][Uu][Gg]\.lib", "_check\.lib")

  # should search for ..\..\lib.*\win32 => ..\..\lib.*\FORCED_LIB_CONFIG
  if (FORCED_LIB_CONFIG != "")
  {
    gsub("/win32", "/"FORCED_LIB_CONFIG)
  
    # Remove all old "no default library" statements
    gsub("/nodefaultlib:\"[Ll][Ii][Bb][Cc][Dd]?(\.[Ll][Ii][Bb])?\"", "")
    gsub("/nodefaultlib:\"[Mm][Ss][Vv][Cc][Rr][Tt][Dd]?(\.[Ll][Ii][Bb])?\"", "")
    gsub("/nodefaultlib:\"[Ll][Ii][Bb][Cc][Mm][Tt][Dd]?(\.[Ll][Ii][Bb])?\"", "")
  }

  if (FORCED_LIB != "")
    gsub(" /libpath:\"\.\.[^\"]+\"", "")

  if (FORCED_LIB_DIRECTIVE != "")
  {
    if (isLINK == 1)
      $0 = substr($0, 1, 13) FORCED_LIB_DIRECTIVE " " substr($0, 14)
  }
    
  if (FORCED_LIB_ROOT != "")
    gsub(" /libpath:\"(..\/)+", " /libpath:\""FORCED_LIB_ROOT"/")

  # After libpath root manipulation
  if (FORCED_LIB_ADDFIRST != "")
    sub(" /libpath:\"[^\"]*\"", " /libpath:\""ADD_LIB_DIRECTIVE"/\" &")
}

#
# Make sure library output paths are correct
#

/^# ADD BASE LIB32/ { 
  isLIB = 2 ; #if (DEBUG) print "*** IS LIB" 
}

/^# ADD LIB32/ { 
  isLIB = 1 ; #if (DEBUG) print "*** IS LIB" 
}

isLIB {
  # should search for ..\..\lib.*\win32 => ..\..\lib.*\FORCED_LIB_CONFIG
  if (FORCED_LIB_CONFIG != "")
    gsub("/win32", "/"FORCED_LIB_CONFIG)

  if (FORCED_OUT_ROOT != "")
    gsub(" /out:\"(..\/)+", " /out:\""FORCED_OUT_ROOT"/")
}

#
# Rules to patch links to header files that are now in a common dir
#

/^# Begin Source File$/ { inFile = 1 }
/^# End Source File$/   { inFile = 0 }

/^SOURCE=/ && /\.h$/ && inFile && ME_HEADER_DIR != "" { 
  sub("\\.\\.[\\\\/]include[\\\\/]", ME_HEADER_DIR "/")
}

#
# Default action - spew what we've (maybe) mangled
#

{ print ; isCPP = 0 ; isLINK = 0 }


END { print "" }
