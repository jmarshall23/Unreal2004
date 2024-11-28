/\#line/ {
    gsub("\\\\\\\\","/")
    ARR[$3] = 1
}

END {
  print DEP " " TGT " : \\"
  for (f in ARR)
    if (index(toupper(f),"MSDEV") == 0)
      print "  "f" \\"
  print ""
}