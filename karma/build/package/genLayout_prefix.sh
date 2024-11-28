:

# Function to copy over a single file from $1 to $2
copy_file() {
  if [ $# -eq 2 ] ; then
    if [ ! -f $1 ] ; then
      echo "error (copy_file): non-existent source [$1]" ${ERR_LOG_REDIRECT}
      return 1
    else
      cp -f -p $1 $2
      rc=$?
      if [ ${rc} -ne 0 ] ; then
        echo "error (copy_file): error copying [$1, $2]" ${ERR_LOG_REDIRECT}
        return 1
      fi
    fi
  else
    return 1
  fi
}

# Function to copy over an executable (or set of executables) and
# mangle them as appropriate (e.g.: stripping out symbols)
copy_exec() {
  copy_file $1${EXE_SUFFIX} $2${EXE_SUFFIX}
  if [ ! -z "${DO_STRIP}" ] ; then
    if [ ! "${DO_STRIP}" = "0" ] ; then
      if [ ! -z "${STRIP}" ] ; then
        ${STRIP} $2${EXE_SUFFIX}
      fi
    fi
  fi
}

# Function to make sure directory $1 exists by checking, then creating
# it if it wasn't found to exist already
make_dir() {
  if [ $# -eq 1 ] ; then
    if [ -d "$1" ] ; then
      :
      #echo "warning (make_dir): re-using directory that exists already [$1]" ${WARN_LOG_REDIRECT}
    else
      mkdir -p $1
      rc=$?
      if [ ${rc} -ne 0 ] ; then
        echo "error (make_dir): error making directory [$1]" ${ERR_LOG_REDIRECT}
        return 1
      fi
      if [ ! -d "$1" ] ; then
        echo "error (make_dir): resulting directory missing [$1]" ${ERR_LOG_REDIRECT}
        return 1
      fi
    fi
  else
    return 1
  fi
}

# Function to create what is expected to be a new directory $1
# with complaint if it already existed
make_new_dir() {
  if [ $# -eq 1 ] ; then
    if [ -d $1 ] ; then
      echo "error (make_new_dir): directory already exists [$1]" ${ERR_LOG_REDIRECT}
    else
      mkdir -p $1
      rc=$?
      if [ ${rc} -ne 0 ] ; then
        echo "error (make_new_dir): error making directory [$1]" ${ERR_LOG_REDIRECT}
        return 1
      fi
    fi
    if [ ! -d $1 ] ; then
      echo "error (make_new_dir): resulting directory missing [$1]" ${ERR_LOG_REDIRECT}
      return 1
    fi
  else
    return 1
  fi
}

# Function to copy a directory over from $1 to $2
copy_dir() {
  if [ $# -ne 2 ] ; then
    echo "error (copy_dir): incorrect number of parameters [$1]" ${ERR_LOG_REDIRECT}
  else
    if [ ! -d "$1" ] ; then
      echo "error (copy_dir): missing source directory [$1]" ${ERR_LOG_REDIRECT}
    else
      if [ -d $2 ] ; then
        rm -rf $2
      fi
      mkdir $2
      cp -p -R  $1/* $2
    fi
  fi
}

# Tweak this to reflect the platform and options we want
# e.g. single vs. double, Vanilla vs. SSE, etc.
copy_lib_core() {

  _err=0

  if [ $# -eq 3 ] ; then

    _FILE="${LIB_PREFIX}$3${LIB_SUFFIX}"

    for _DIR in $1 ; do

      if [ ! -f ${_DIR}/${_FILE} ] ; then
        echo "error (copy_lib): non-existent source [${_DIR}/${_FILE}]" ${ERR_LOG_REDIRECT}
        _err=1
      else
        cp -f -p ${_DIR}/${_FILE} $2/${_FILE}
        rc=$?
        if [ ${rc} -ne 0 ] ; then
          echo "error (copy_lib): error copying [${_FILE} from $1 to $2]" ${ERR_LOG_REDIRECT}
          _err=1
        fi
      fi

    done
    return ${_err}

  else
    return 1
  fi

}

copy_lib() {

  if [ $# -eq 3 ] ; then

    copy_lib_core $1 $2 $3
#    if [ ! -z "${DEBUG_TOO}" ] ; then
#      copy_lib_core $1 $2 $3_debug
#    fi

  else
    return 1
  fi

}

