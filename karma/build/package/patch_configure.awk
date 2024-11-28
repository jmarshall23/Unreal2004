BEGIN {
  if (FORCED_INC == "") FORCED_INC="."
  if (FORCED_LIB == "") FORCED_LIB="."
}


/test -z \"\${ME_INC_PATH}\" / {
  print $0
  print "ME_INC_PATH=\"${ME_INC_PATH}" FORCED_INC "\"" 
  print "ME_LIB_PATH=\"${ME_LIB_PATH}" FORCED_LIB "\"" 
  next
}

/^ME_INC_PATH=/          { next  }
/^ME_LIB_PATH=/          { next  }
                         { print }