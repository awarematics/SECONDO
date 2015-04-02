# list of command aliases
#
# Feb 2005, M. Spiekermann

if [ "$OSTYPE" == "msys" ]; then
   prefix=/c
   platform="win32"
else 
   prefix=$HOME
   platform="linux"
fi

# set default values
${SECONDO_SDK:=$prefix/secondo-sdk}
${SECONDO_BUILD_DIR:=$prefix/secondo}

# export variables used by make
export SECONDO_SDK
export SECONDO_BUILD_DIR
export SECONDO_PLATFORM=$platform

# function newAlias
#
# $1 alias name
# $2 command name
#
# Adds path information before the command name $2
function newAlias() {

if [ -z "$SECONDO_BUILD_DIR" ]; then
  alias ${1}="$2"
else
  alias ${1}="${SECONDO_BUILD_DIR}/${cmdDir}/$2"
fi

}
###########################################################
##
## Start of configurable part
##
## Please add new shell scripts here. $cmdDir defines
## a path relative to $SECONDO_BUILD_DIR.
##
###########################################################

cmdDir=CM-Scripts
newAlias backup backup.sh
newAlias runtests run-tests.sh
newAlias cvstest cvs-make.sh

cmdDir=Tools/Generators/TPC-H/secondo
newAlias tpcgen tpcgen.sh
newAlias tpcrun tpcrun.sh

cmdDir=bin
newAlias SecondoTTY SecondoTTYBDB
newAlias SecondoMonitor "SecondoMonitorBDB -s"

# Envrionment for SECONDO 
alias setvar="source ${SECONDO_SDK}/bin/setvar.bash" 
alias catvar="${SECONDO_SDK}/bin/catvar.sh" 
alias secinit="source $HOME/.secondorc"
alias secenv="source ${SECONDO_SDK}/bin/setvar.bash"
alias secroot="${SECONDO_SDK}/bin/catvar.sh"

# CVS
alias jcvs='java -jar ${SECONDO_SDK}/jCVS-5.2.2/jars/jcvsii.jar'
alias cvs-info='cvs -nq update | grep "^[A-Z]"'
alias cvs-mod='cvs -nq update | grep "^[MAC]"'
