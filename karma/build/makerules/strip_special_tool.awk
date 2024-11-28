/^# Begin Special Build Tool/ 	{ inSpecial=1 }
/^# End Special Build Tool/ 	{ inSpecial=0; next }
inSpecial                       { next }
                                { print }
