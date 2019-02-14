#!/bin/bash
cd ..
git pull
cd /test/
g++ -o client client.cpp
g++ -o server server.cpp
