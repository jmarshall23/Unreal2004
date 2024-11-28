BEGIN { inTopBlk = 0 ; inDefBlk = 0 ; slurp = 0 }

/^CFG=/ 				{ gsub("Debug","Release") ; inTopBlk=1 }
/^!MESSAGE NMAKE \/f/ 			{ gsub("Debug","Release") }
/^!MESSAGE Possible choices for/ 	{ inTopBlk=2 }
inTopBlk && /Debug/ 			{ next }
inTopBlk && /^$/ 			{ inTopBlk=0 }
/^# Name / && /Debug"$/ 		{ next }

# Slurp!
/^!IF/					{ inDefBlk=1 }
inDefBlk && /^!IF/ && !/Release"$/	{ slurp=1 }
inDefBlk && /^!ELSEIF/ && !/Release"$/	{ slurp=1 }
inDefBlk && /^!ELSEIF/ && /Release"$/	{ $1 = "!IF" ; slurp=0 }
inDefBlk && /^!ENDIF/			{ inDefBlk=0 ; slurp=0 }
inDefBlk && slurp			{ next }

/^SOURCE=\.\.\\src\\/			{ gsub("\\.\\.\\\\src\\\\",".\\") }

{ print }