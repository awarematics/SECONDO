#!/bin/sh
# install.bash - untar files and run make for
# various software packages
#
# 04/19/05 M. Spiekermann
# 04/22/09 M. Spiekermann, error handling for starting xterm &
# 05/16/02 M. Spiekermann, MSYS-Mingw and Linux script merged into this version
# 05/12/10 M. Spiekermann, Compilation of 3d party packages improved and uninstall function added  
# 05/18/10 M. Spiekermann, Code restructured into many new functions. Easier to test and maintain.   
# 06/27/09 M. Spiekermann, tools version check implemented and new packages
#                          added in order to support Mac-OSX.   
# 07/02/04 C. Duentgen,    Added Package GSL - GNU Scientific Library, which is needed by the new
#                          GSL-Algebra

startDir=$PWD

# include function definitions
libFile="./scripts/bin/libutil.sh"
if ! source $libFile; then 
  printf "%s\n" "This script needs routines from the file $libFile"
  exit 1; 
fi

if win32Host; then
  shProfile="./scripts/home/profile"
  if [ -f $shProfile ]; then
    source $shProfile
  fi  
fi  

function showBashrcMsg {

  printx "\n"
  showMsg "em" "Note: Before compiling SECONDO run \"source ~/.secondorc\" \n\
otherwise some environment variables are not set to their correct values \n\
and make will abort with an error."
}

# The next two function are used to set checkpoints
# for sucessful installation steps

# $1 name
function checkPoint {
  
  local file="$sdk/_DONE_$1"
  if [ -e "$file" ]; then
    return 0
  fi
  return 1
}

# $1 name
function setCheckPoint {

  local file="$sdk/_DONE_$1"
  checkCmd touch $file
  showMsg "info" "-> OK!"
}

# $1 = package info
# $2 = package file 
# $3 = package dir 
# $4 = make targets
# $5 = configure options
# $6 = optional configure cmd
#
function installPackage {

  local pckgInfo=$1
  local subDir=$2
  local pckgFiles=$3
  local compileDir=$4
  local targets=$5
  local confOpt=$6
  local confCmd=$7
  
  local conf="./configure"
  
  # There's a posix fix in bash 3.0 which breaks older script code.
  # bash 2.05 accepts command "trap 0", but version 3.0 needs
  # "trap - 0". As a result some configure scripts stop with
  # an error, e.g. for gcc 3.2.3. A solution is to start the
  # configure script with bash which turns off posix compatibility mode.
  if ! win32Host; then
    local bashCmd="bash"
  fi

  if [ "$confCmd" != "" ]; then
    conf=$confCmd
  fi
  
  printx "%s\n" "Installing package \"$pckgInfo\" ..."

  local file=${pckgFiles##*/}
  local name="PCKG_$file"
  if ! checkPoint "$name"; then
    
    # extract files for package
    printx "%s\n" "Uncompressing $pckgFiles"
    uncompressFiles $temp $pckgFiles
    if [ $? -ne 0 ]; then
      return 1
    fi

    # compile package if necessary
    if [ "$compileDir" != "" ]; then
      printx "%s\n" "Compiling package ..."
      printf "%s\n" "cd $compileDir"
      assert cd $compileDir
      printf "%s\n" "$bashCmd $conf --prefix=$sdk/$subDir $confOpt --disable-nls $configureFlags"
      checkCmd $bashCmd $conf --prefix=$sdk/$subDir $confOpt --disable-nls $configureFlags
      if [ $? -ne 0 ]; then
        return 1;
      fi
      for target in $targets; do
        makeCmd="make"
        if [ "$target" != "all" ]; then
          makeCmd="make $target"
        fi  
        printf "%s\n" "$makeCmd"
        checkCmd make $target
        rc=$?
        if [ $rc -ne 0 ]; then
          return 1;
        fi
      done
    fi
    setCheckPoint "$name" 
  else
    showMsg "info" "-> already done!"
  fi

  return 0
}


# $1 package info
# $2 mode = [ dir, files, file ]
# $3 [ $* ] files or dir  
function uncompressPackage {

  if [ $# -le 2 ]; then
    return 1;
  fi

  local pckgInfo=$1
  local mode=$2
  shift
  shift

  printx "%s\n" "Installing package \"$pckgInfo\" ..."
  
  # uncompress all zip files of a given directory
  if [ "$mode" == "dir" ]; then
    local dir="$*"
    local folder=${dir##*/}
    local name="UNCOMP_DIR_$folder"
    if ! checkPoint "$name"; then
      uncompressFolders $dir
      if [ $? -eq 0 ]; then
        setCheckPoint "$name"
        return 0
      else
        return 1
      fi 
    else
     showMsg "info" "-> already done"
    fi
    return 0
  fi

  # uncompress all given files
  if [ "$mode" == "files" ]; then
    local file=${1##*/}
    local name="UNCOMP_FILES_$file"
    if ! checkPoint "$name"; then
      uncompressFiles $sdk $*
      if [ $? -eq 0 ]; then
        setCheckPoint "$name"
        return 0
      else
        return 1
      fi 
    else
     showMsg "info" "-> already done!"
    fi
    return 0
  fi

  # uncompress a single file
  if [ "$mode" == "file" ]; then
    local file=${1##*/}
    local name="UNCOMP_FILE_$file"
    if ! checkPoint "$name"; then
      uncompressFiles $sdk $*
      if [ $? -eq 0 ]; then
        setCheckPoint "$name"
	return 0
      else
	return 1
      fi
    else
      showMsg "info" "-> already done"
    fi
    return 0
  else
    showMsg "err" "uncompressPackge - Unknown mode \"$mode\""
    return 1
  fi

}


function unInstall {

    local dirs="$sdk $temp $build"
    if win32Host; then
      showMsg "warn" "We assume that you have removed MinGW by using windows' system \n\
control software installation utility and SWI-Prolog by its \n\
uninstall tool unwise.exe!"
      if [ -e $mingwdir ]; then
        dirs="$dirs $mingwdir"
      fi
    fi 
    printx "%s\n" "About to delete the following directories and files:"
    local delDir=""
    for xdir in $dirs $HOME/.secondo*; do
      printx "%s\n" "  $xdir"
      delDir="$delDir $xdir"
    done

    local opt1="Delete"
    local opt2="Abort"
    select choice in "$opt1" "$opt2"; do
      if [ "$choice" == "$opt1" ]; then
        break
      else
        abort
      fi
    done

    for xdir in $delDir; do
      printf "%s\n" "Deleting $xdir ..."
      rm -rf $xdir
      LU_LOG_INIT=""
    done

    if win32Host; then
      printx "\n" 
      showMsg "em" "If Java 2 has been installed remove it with it's uninstall program or \n\
window's system control."
      showMsg "em" "Now you can remove the MSYS installation by using window's system \n\
control. If the directory /c/msys will not be deleted, \n\
delete it with a file manager."
    fi

}


function abort {

  if isRunning $xtermPID; then
    killProcess $xtermPID
  fi
  if isRunning $j2XtermPID; then
    printx "\n%s\n" "Waiting for child process $j2XtermPID"
    wait $j2XtermPID
  fi
  exit $?
}

function copyConfigFiles {

  printSep "Copying configuration files"
  local check="COPY_RCFILES"
  assert cd $cdpath/scripts
  let LU_ERRORS=0
  if ! checkPoint "$check"; then
    
    printx "%s\n" "Creating \$sdk/secondo* files"
    local cmdfiles="secondorc secondo.*"
    if win32Host; then
      cmdfiles="$cmdfiles profile"
    fi 
    for file in $cmdfiles; do
      checkCmd cp -b $file $sdk
    done 
  else
    showMsg "info" "-> already done!"
    return 0
  fi

  if [ $LU_ERRORS -eq 0 ]; then
    setCheckPoint "$check"
    return 0
  fi
  return 1 
}

# $1 directory
function checkInstDir {

  if [ ! -d "$1" ]; then
    showMsg "warn" "Recommended installation directory \"$1\" \n\
not found. You need to modify file \"$sdk/secondo.config\"."
    return 1
  fi
  return 0
}

function installJava {

  if win32Host; then
    assert cd $j2dir
    j2sdk*windows*.exe
    checkInstDir $javaInstDir
  else
    assert cd $sdk
    local j2file=$j2dir/j2sdk*.bin
    startupXterm "JAVA 2 SDK Installation" $j2file
    if [ $? -ne 0 ]; then
     printx "Running $j2file directly"
     $j2file
    else
     j2XtermPID=$LU_xPID
    fi
  fi
}


function finish {

  copyConfigFiles
  showBashrcMsg
  abort
}

##########################################################
###
### Start of the Script
###
##########################################################

#default options
testMode="false"
sdkName="secondo-sdk"

declare -i numOfArgs=$#
let numOfArgs++
opt_platform=""

while [ $numOfArgs -ne $OPTIND ]; do

  getopts "d:p:ht" optKey
  if [ "$optKey" == "?" ]; then
    optKey="h"
  fi

  case $optKey in

   h) showGPL
      printf "\n%s\n" "Usage of ${0##*/}:" 
      printf "%s\n"   "  -h print this message and exit."
      printf "%s\n"   "  -t test mode installing below directory /tmp"
      printf "%s\n"   "  -d name of the the sdk root directory [$sdkName]"
      printf "%s\n"   "  -p platfomrm name"
      printf "%s\n"   "The script installs or compiles all 3rd party tools"
      printf "%s\n\n" "needed to compile SECONDO."
      exit 0;;
  
   d) sdkName=$OPTARG;;
     
   p) opt_platform=$OPTARG;; 
      
   t) testMode="true"

  esac

done

if [ "$opt_platform" != "linux" ]; then
  if [ "$opt_platform" != "mac_osx" ]; then
    if [ "$opt_platform" != "win32" ]; then
      printf "%s\n" "Error: -p must be set to linux, mac_osx or win32!"
      exit 1
    fi
  fi
fi 


printf "\n"
showGPL


if [ "$testMode" == "true" ]; then
  printf "\n"
  showMsg "info" "Running in test mode: \$HOME set to \"$HOME\""
  mkdir -p $HOME
  mkdir -p $instpath
fi


# init log file
temp=$LU_TMP/secondo-tmp-$USER/$sdkName
mkdir -p $temp
logfile="$temp/installsdk.log"
initLogFile $logfile
if [ $? -ne 0 ]; then
  showMsg "err" "Could not create log file. Giving up!"
  abort
fi
logfile=$LU_LOG

# check if $USER is defined
if [ "$USER" == "" ]; then
  if [ "$LOGNAME" == "" ]; then
    showMsg "warn" "The variable \$USER and \$LOGNAME are empty. \$USER will be set to \"nobody\"."
    USER="nobody"
  else
    USER="$LOGNAME"
  fi  
fi  

# check if $HOME exists
if [ ! -w "$HOME" ]; then
  showMsg "err" "Directory \$HOME=\"$HOME\" does not exist or you have no write access."
  HOME2=/home/$USER
  if [ "$HOME2" != "$HOME" ]; then
    showMsg "warn" "Trying to create \$HOME=\"$HOME2\""
    assert mkdir -p $HOME2
    HOME=$HOME2
  else
    abort
  fi  
fi

# Set up variables for important directories
# Do some OS-specific settings
if [ "$opt_platform" == "win32" ]; then

  instpath=/c
  if [ "$testMode" == "true" ]; then
    instpath=$temp/C
    msysdir="$instpath/msys/1.0"
    HOME=$msysdir/home/$USER
  fi
  msysdir="$instpath/msys/1.0"
  mingwdir="$instpath/mingw"
  platformdir=$startDir/win32
  encoding="CP1252"
 
else

  if [ "$testMode" == "true" ]; then
    HOME=/tmp/installsdk/$USER
  fi
  instpath=$HOME
  platformdir=$startDir/linux
  encoding="LAT1"

fi

# set variables for important directories
cdpath=$startDir
sdk=$instpath/$sdkName
build=$HOME/secondo
prologdir=$sdk/pl




printx "\n%s\n" "*** Installation of the SECONDO DEVELOPMENT TOOLKIT ***" 
printx "\n%s\n" "    Installation source: $cdpath"
printx "%s\n"   "    Target for tools   : $sdk"
printx "%s\n"   "    Target for SECONDO : $build"
printx "%s\n"   "    Temporary directory: $temp"
printx "%s\n\n" "    Used platform      : $opt_platform"

for xdir in "$sdk" "$prologdir" "$sdk/bin" "$build"; do
  if [ -d "$xdir" ]; then
    showMsg "warn" "Directory $xdir already exists."
  fi 
done

printx "\n%s\n" "This procedure will install various 3rd party tools on your computer."
printx "%s\n"   "We assume that you have read the installation guide. What would you like to do now?"

opt1="Install"
opt2="Uninstall"
opt3="Abort"
select choice in "$opt1" "$opt2" "$opt3"; do

  if [ "$choice" == "$opt1" ]; then
    break;
  else 
    if [ "$choice" == "$opt2"  ]; then
      unInstall;
      exit $?
    else 
      exit $?;
    fi 
  fi 
done

showMsg "info" "Starting installation! Log information will be written to \n\
$logfile"

if [ "$testMode" == "true" ]; then
  assert mkdir -p $HOME
  if win32Host; then
    assert mkdir -p $instpath
  fi
fi

# create sdk directory
assert mkdir -p $sdk/bin

# On windows we need to install unzip first
if win32Host; then
  export PATH="$sdk/bin:$sdk/lib:/c/mingw/bin:$PATH"

  if [ ! -e $sdk/bin/unzip.exe ]; then
    printSep "Installing unzip ..."
    assert cd $sdk/bin
    checkCmd "$platformdir/non-gnu/unzip/unz550xN.exe" 
    checkCmd "unzip -q -o $platformdir/non-gnu/unzip/zip23xN.zip"
  fi
fi

printSep "Installing SECONDO's source files"
if [ ! -d $HOME/secondo ]; then
  printx "%s\n" "Uncompressing source files ..."
  srcfile=$cdpath/secondo-*${encoding}.*
  if [ ! -e $srcfile ]; then
    showMsg "warn" "Can't extract Secondo's sources. Please download them from \n\
\"www.informatik.fernuni-hagen.de/secondo\" and extract the zip or tar.gz archive \n\
into directory \$HOME/secondo."
  else 
    if uncompress $srcfile $HOME; then
      showMsg "info" "-> OK!"
    fi  
  fi
else
  showMsg "info" "-> Source directory is already present!"
fi

printSep "JAVA 2 SDK Version 1.4.2"
if win32Host; then
  javaInstDir="$sdk/j2sdk1.4.2"
else  
  javaInstDir="$sdk/j2sdk1.4.2_01"
fi

if [ ! -d $javaInstDir ]; then

  j2dir=$platformdir/j2sdk
  if [ ! -e $j2dir ]; then
    showMsg "warn" "The script needs Sun's J2SDK installation kit in directory \n\
  \"$j2dir\" \n\
But this directory is not present. Hence this script will not install \n\
a JAVA-SDK. Please install it later manually. Depending on which version \n\
will be installed adjust the variable \$J2SDK_ROOT in the file \n\
\$sdk/secondo.config"
  else
    printx "%s\n" "Installing Java SDK. If you don't want to install it since "
    printx "%s\n" "you have already a SDK of this version or higher omit this step."
    opt1="Install Java 2 SDK"
    opt2="Don't install"
    
    select choice in "$opt1" "$opt2"; do
    if [ "$choice" == "$opt1" ]; then
      installJava
      break
    else
      break
    fi
    done
  fi
else  
  showMsg "info" "-> J2SDK seems to be already installed!"
fi

check="UNCOMP_JCVS"
if [ -e $cdpath/extras/jcvs/jcvs*.*gz ]; then
  printx "%s\n" "Uncompressing JCVS, a java cvs client ... "
  if ! checkPoint "$check"; then
    assert cd $sdk
    checkCmd tar -xzf $cdpath/extras/jcvs/jcvs*.*gz
    if [ $? -eq 0 ]; then
      setCheckPoint "$check"
    fi
  else
    showMsg "info" "-> already done!"
  fi
fi

##
## WINDOWS - INSTALLATION
##

if win32Host; then

  printSep "MinGW (GCC 3.2 windows port) Installation"
  assert cd $platformdir/mingw
  check="INST_MINGW"
  if ! checkPoint "$check"; then 
    checkCmd Min*.exe
    if checkInstDir "$mingwdir"; then
      setCheckPoint "$check"
    fi
  else
    showMsg "info" "-> MinGW seems to be already installed!"
  fi

   
  printSep  "SWI-Prolog Installation"
  assert cd $platformdir/prolog
  check="INST_SWIPROLOG"
  if ! checkPoint "$check"; then
    checkCmd w32pl*.exe
    if checkInstDir "$prologdir"; then
      setCheckPoint "$check"
    fi
  else
    showMsg "info" "-> SWI-Prolog seems to be already installed!"
  fi


  printSep "Installation of Tools from the gnuwin32 project ..."
  assert cd $sdk
  uncompressPackage "GNU Tools: bison, etc." "dir"   $platformdir/gnu
  uncompressPackage "JPEG-Library"           "files" $platformdir/non-gnu/jpeg-*
  uncompressPackage "Flex"                   "file"  $platformdir/non-gnu/flex-*
  uncompressPackage "CVS-Client"             "file"  $platformdir/non-gnu/cvs-*

  if ! startupXterm "Messages from make" tail -f $logfile; then
    printx "%s\n" "Messages from make are kept in $logfile"
  else
    xtermPID=$LU_xPID
  fi

  printSep "Installation of Berkeley-DB"
  installPackage "Berkeley-DB" bdb $platformdir/non-gnu/db-* $temp/db-*/build_unix install "--enable-cxx --enable-mingw" ../dist/configure

  finish

fi

##
## LINUX / UNIX  - INSTALLATION
##

if ! startupXterm "Messages from make" tail -f $logfile; then
  printx "%s\n" "Messages from make are kept in $logfile"
else
  xtermPID=$LU_xPID
fi

#
# GCC installation
#

# define lists of include, lib and bin paths

cFlags=""
ldFlags=""
binPaths=""
libPaths=""
for dir in gcc aux bison flex; do
  cFlags=$cFlags"-I$sdk/$dir/include"
  ldFlags=$ldFlags"-I$sdk/$dir/lib"
  binPaths=$binPaths"$sdk/$dir/bin:"
  libPaths=$libPaths"$sdk/$dir/lib:"
done 

export PATH=".:$binPaths:$PATH"
export LD_LIBRARY_PATH=".:$libPaths:$LD_LIBRARY_PATH"

printSep "Installation of GCC"
echo "PATH: $PATH" >> $logfile
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH" >> $logfile
checkCmd "gcc --version >> $logfile"
checkCmd "gcc --print-search-dirs >> $logfile"
checkCmd "flex --version >> $logfile"
checkCmd "bison --version >> $logfile"

#install gcc if necessary
gccCoreFile=$(ls $platformdir/gnu/gcc-core-*)
if ! checkVersion "gcc --version" $gccCoreFile; then
  gccfiles=$platformdir/gnu/gcc-*
  objDir=$temp/gcc.objects
  assert mkdir -p $objDir
  installPackage "GCC with C++ support" gcc "$gccfiles" $objDir "bootstrap install" --enable-bootstrap ../gcc-*/configure
  assert hash -r
  if ! checkVersion "gcc --version" $gccCoreFile; then
    showMsg "err" "Something went wrong! gcc --version: ($LU_Version1 < \
 $LU_Version2) does not report the version of the package file 
 \"$gccCoreFile\""
    abort
  fi
else
 showMsg "info" "gcc --version: $LU_Version1 >= $LU_Version2! We will don't
 install $gccCoreFile"
fi



configureFlags="CFLAGS=$cFlags LDFLAGS=$ldFlags"

printSep "Compiling other packages ..."

if [ "$opt_platform" == "mac_osx" ]; then
 installPackage "Findutils" aux $platformdir/gnu/findutils-* $temp/findutils-* install
fi 

installPackage "Lib curses"   aux $platformdir/gnu/ncurses-*     $temp/ncurses-*  "all install"
installPackage "Lib readline" aux $platformdir/gnu/readline-*    $temp/readline-* "all install" --with-curses
installPackage "Lib jpeg"     aux $platformdir/non-gnu/jpeg*     $temp/jpeg*      "all install install-lib" 
installPackage "Lib gsl"      aux $platformdir/gnu/gsl-*         $temp/gsl-*      "all install"
installPackage "Berkeley-DB"  bdb $platformdir/non-gnu/db-*      $temp/db-*/build_unix "all install" --enable-cxx ../dist/configure
installPackage "SWI-Prolog"   swi $platformdir/prolog/pl-*       $temp/pl-*       "all install"

bisonFile=$(ls $platformdir/gnu/bison-*)
if ! checkVersion "bison --version" $bisonFile; then
  installPackage "Bison, a parser generator" bison $bisonFile $temp/bison-* install
else
 showMsg "info" "bison --version: $LU_Version1 >= $LU_Version2! We will don't
 install $bisonFile"
fi

flexFile=$(ls $platformdir/non-gnu/flex-*)
if ! checkVersion "flex --version" $flexFile; then
  installPackage "Flex, a scanner generator" flex $flexFile $temp/flex-* install
else
 showMsg "info" "flex --version: $LU_Version1 >= $LU_Version2! We will don't
 install $flexFile"
fi


finish
