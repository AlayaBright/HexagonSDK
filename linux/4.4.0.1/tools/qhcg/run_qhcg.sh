#! /bin/bash
cd setup && source ./setup.sh && cd ..
if [ -n $PY ]; then
	echo Start QHL Approximation Tool with arguments:
	echo $*
	$PY src/qhcg.py $*
fi
