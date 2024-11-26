#!/bin/bash

docker run -it -v $(pwd):/home/tracker -v /tmp/.X11-unix:/tmp/.X11-unix --privileged  -e DISPLAY=unix$DISPLAY --network host tracker:latest
