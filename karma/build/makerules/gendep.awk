/^\#line/ {
  gsub("\\\\\\\\","/")
  gsub("[\r\"\f]","")
  ARR[$3] = 1
}

END {
  print DEP " " TGT " : \\"
  for (f in ARR)
    if (index(f,":") == 0)
      print "  "f" \\"
  print ""
}
