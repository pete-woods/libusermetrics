#!/bin/bash

if [ -e "~/.cache/infographic-service/visualizers/${APP_ID}" ] ; then
	initctl set-env "APP_EXEC=echo Visualizer ${APP_ID} started"
else
	echo "No visualizer for ${APP_ID}"
	exit -1
fi
