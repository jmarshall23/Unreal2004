BEGIN {

  badFlags[1]=" ?/MLd? ?"
  badFlags[2]=" ?/MDd? ?"
  badFlags[3]=" ?/MTd? ?"

  if (CType=="STATIC")    flagPrefix = "/ML"
  if (CType=="STATICMT")  flagPrefix = "/MT"
  if (CType=="DLLMT")     flagPrefix = "/MD"
  if (CType=="")          flagPrefix = "/ML"
}

/\# ADD CPP/ && / \/D \"NDEBUG\"/ {
  for (i = 1; i <= 3 ; i++) gsub(badFlags[i]," ")
  print $0 " " flagPrefix ; next
}

/\# ADD CPP/ && / \/D \"_DEBUG\"/ {
  for (i = 1; i <= 3 ; i++) gsub(badFlags[i]," ")
  print $0 " " flagPrefix "d" ; next
}

{ print }
