#!/bin/sh
export LD_LIBRARY_PATH=.:/usr/local/lib:/lib:$LD_LIBRARY_PATH
exec java -cp dist/Quantis.jar com.idquantique.quantisextensions.QuantisExtractorDemo "$@"
