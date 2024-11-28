
if [ -z "${SELECTED_PLATFORM}" -o "${SELECTED_PLATFORM}" = "none" ] ; then
  echo 'fatal error: ${PLATFORM} unset, or incorrect, unable to continue'
else
  if [ -z "${GEN_ROOT}" ] ; then
    echo 'fatal error: ${GEN_ROOT} not defined, unable to continue'
  else 
    if [ -z "${GEN_SOURCE}" ] ; then
      echo 'warning: ${GEN_SOURCE} not defined, defaulting to "."'
      GEN_SOURCE=`pwd`
    fi

    generate_image
    if [ ! -z "${TO_GENERATE}" ] ; then
      for curr_tgt in ${TO_GENERATE} ; do
        gen_command_${curr_tgt}
      done
    fi
  fi
fi
