#!/bin/bash

# kill all containers
docker kill $(sudo docker ps -q)
