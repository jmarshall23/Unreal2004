#!/bin/sh
EXE=$1


spit_suffix() {
if [ ! -z "$theBMPfiles$theMEfiles" ] ; then
  echo "[bin/Resources]"
#  RESfiles=`echo $RESfiles | tr \ \\n | sort | uniq`
  if [ ! -z "$theBMPfiles" ]; then
    theBMPfiles2=`echo $theBMPfiles | awk '{for(i=1;i<NF;i++)print$i}' | sort | uniq`
#    echo ""
#    echo "$theBMPfiles"
    echo ""
    for l in $theBMPfiles2 ; do
      echo "    $l"
    done
  fi

  if [ ! -z "$theMEfiles" ]; then
    echo ""
    newMEfiles="$theMEfiles"
    newMEfiles="`echo $theMEfiles | awk '{for(i=1;i<=NF;i++){x[$i]++;if (x[$i]==1) printf("%s ", $i)}}'`"
    for l in $newMEfiles ; do
      echo "    $l"
    done
  fi

  if [ ! -z "$theASEfiles" ]; then
    echo ""
    newASEfiles="$theASEfiles"
    newASEfiles="`echo $theASEfiles | awk '{for(i=1;i<=NF;i++){x[$i]++;if (x[$i]==1) printf("%s ", $i)}}'`"
    for l in $newASEfiles ; do
      echo "    Resources/$l"
    done
  fi


fi
echo ""
echo "[src/examples]"
echo "    MeSamples/makefile"
echo "    %generate examples.def"
echo "    %command  echo \"EXAMPLES := $allExamples\" > \$@"
echo ""
echo "[src/tutorials]"
echo "    MeTutorials/makefile"
echo "    %generate examples.def"
echo "    %command  echo \"EXAMPLES := $allTutorials\" > \$@"
echo ""
}


spit_dependencies() {
  
SRC=$1
DEST=$2
EXE=$3

theBMPfiles="$theBMPfiles Resources/font.bmp Resources/perfticks.meg"

for j in ${SRC}/${EXE}/*.[ch] ; do

  # Find references to ME & ASE files
  MEFiles=`grep MeStreamOpenWithSearch $j | cut -s -d\" -f2`
  ASEfiles=`grep RGraphicLoadASE $j | cut -s -d\" -f2`
  if [ ! -z "$ASEfiles" ] ; then
    echo "Found an RGraphicLoadASE: $j - $ASEFiles"
    theASEfiles="$theASEfiles $ASEfiles"
  fi
  for k in ${MEFiles} ; do
    theMEfiles="$theMEfiles Resources/me_files/$k"
    ASEfiles=`grep -i \\.ase Resources/me_files/$k | awk -v FS=\< '{for(i=1;i<NF;i++)if($i~"^FILE>")print" "substr($i,6)}'`
    if [ ! -z "$ASEfiles" ] ; then
      theASEfiles="$theASEfiles $ASEfiles"
    fi
  done

  # Find references to textures files
  BMPFiles1=`grep RGraphicSetTexture   $j | cut -s -d\" -f2`
  BMPFiles2=`grep RRenderSkydomeCreate $j | cut -s -d\" -f2`
  BMPFiles3=`grep HeightFieldFromBMP   $j | cut -s -d\" -f2`
  for k in ${BMPFiles1} ${BMPFiles2} ${BMPFiles3} ; do
    theBMPfiles="$theBMPfiles Resources/$k.bmp"
  done

done

}


spit() {
SRC=$1
DEST=$2
EXE=$3

echo ""
echo "[bin/${DEST}]"
echo "    %executable ${EXE} = ${SRC}/bin.rel/\${PLATFORM}\${DIR_MODIFIER}/${EXE}"
echo ""
echo "[src/${DEST}/${EXE}]"
for j in ${SRC}/${EXE}/*.[ch] ; do
  if [ "`basename $j | cut -c1-11`" != "PREFIX_PS2_" ] ; then
    echo "    ${SRC}/${EXE}/`basename $j`"
  fi
done
echo ""
echo "    %generate makefile = ${SRC}/${EXE}/makefile"
echo "    %command sed -e 's/..\/..\/build\/makerules/..\/..\/components\/build\/makerules/g' < \$+ > \$@"
echo ""
echo "    !IFEQ PLATFORM win32"
echo "      %generate ${EXE}.dsp = ${SRC}/${EXE}/${EXE}.dsp"
echo "      %command awk -v FORCED_INC=\"../../../include:../../../3rdParty/glut\" -v FORCED_LIB_ROOT=\"../../..\" -v FORCED_LIB_CONFIG=\"\${PLATFORM}\${DIR_MODIFIER}\" -v FORCED_LIB_ADDFIRST=\"../../components/lib.CONFIG/\${PLATFORM}\${DIR_MODIFIER}\" -f patchDSP.awk < \$+ > \$@"
echo "    !ENDIF"
spit_dependencies $1 $2 $3
echo ""
}

spit_tutorial() {
  echo "; TUTORIAL - ${EXE}"
  spit $1 tutorials $2
}

spit_example() {
  echo "; EXAMPLE - ${EXE}"
  spit $1 examples $2
}


(while read exeType exeName ; do

  if [ ! -z "$exeName" ] ; then

      typeSuff=
      source=
      EXE=$exeName

      if [ -e MeTutorials/$exeName/$exeName.c ] ; then
        source=MeTutorials
      else
        if [ -e MeSamples/$exeName/$exeName.c ] ; then
          source=MeSamples
        fi
      fi

      if [ "$exeType" = "e" ] ; then 
        typeSuff=example
      else
        if [ "$exeType" = "t" ] ; then 
          typeSuff=tutorial
        fi
      fi

      if [ -z "${typeSuff}" ] ; then
        echo "# Error [unrecognized type]: type:$exeType Name:$exeName"
      else
        if [ -z "${source}" ] ; then
          echo "# Error [unknown source]: type:$exeType Name:$exeName"
        else
          spit_${typeSuff} ${source} $exeName
          if [ "${typeSuff}" = "example" ] ; then
            allExamples="$allExamples $exeName"
          fi
          if [ "${typeSuff}" = "tutorial" ] ; then
            allTutorials="$allTutorials $exeName"
          fi
        fi
      fi

  fi
    
done ; spit_suffix) << END_OF_LIST

t BallHitsWall1
t BallHitsWall2
t Bounce
t Drop
t LoadTutorial1
t LoadTutorial2
t LoadTutorial3
t SaveTutorial1
e CarTerrain
e Chair 
e FixedPath
e HiSpeed
e Hinge
e Prismatic
e TriangleList 

e BallMan
e ConvexStairs
e Crane
e Cubes
e Friction 
e ManyPendulums
e RainbowChain
e SceneLoader 
e Tank
e Topple

END_OF_LIST
