#!/bin/bash
# $Header: /home/cvsroot/secondo/Optimizer/MemoryAllocation/nrwtest/testnrw3.sh,v 1.1 2012/11/28 20:05:29 nvkempen Exp $
# @author Nikolai van Kempen
#

#old: P=http://download.geofabrik.de/osm/europe/germany/
P=http://download.geofabrik.de/openstreetmap/europe/germany/
F=nordrhein-westfalen.shp.zip
PF=$P$F

T=$SECONDO_BUILD_DIR/bin/nrw/
[ -d $T ] || mkdir $T || return 2

cd $T || return 4

if [ ! -f "$F" ]; then
	wget "$PF" || return 1
fi

unzip $F #&& rm $F

echo "Using now SecondoBDB, please make sure no secondo server proccess is running and press return."
read trash

cd $SECONDO_BUILD_DIR/bin
SecondoBDB <<<"delete database nrw3;"

# The author of this script is unkown
SCMDF=$SECONDO_BUILD_DIR/Optimizer/MemoryAllocation/nrwtest/nrw3ImportShape.SEC
SecondoBDB < $SCMDF

SecondoBDB <<<" 
open database nrw3

let Roads800k = Roads feed head[800000] consume

let Roads700k = Roads feed head[700000] consume

let Roads600k = Roads feed head[600000] consume

let Roads500k = Roads feed head[500000] consume

let Roads400k = Roads feed head[400000] consume

let Roads300k = Roads feed head[300000] consume

let Roads200k = Roads feed head[200000] consume

let Buildings800k = Buildings feed head[800000] consume

let Buildings700k = Buildings feed head[700000] consume

let Buildings600k = Buildings feed head[600000] consume

let Buildings500k = Buildings feed head[500000] consume

let Buildings400k = Buildings feed head[400000] consume

let Buildings300k = Buildings feed head[300000] consume

let Buildings200k = Buildings feed head[200000] consume

"

# eof
