#!/bin/bash

function build() {
		echo docker-build.sh: Building...
		docker build -t game .
}
function run() {
		echo docker-build.sh: Running...
		docker run -e DISPLAY -v $HOME/.Xauthority:/home/developer/.Xauthority  --net=host game
}

if [ $# -ge 1 ]; then
		if [ $1 == "-r" ] || [ $1 == "--run" ]; then
				build
				run
		fi
		if [ $1 == "-ro" ] || [ $1 == "--run-only" ]; then
				run
		fi
else
		build
fi
