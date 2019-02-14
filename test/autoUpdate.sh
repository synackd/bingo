#!/bin/bash
cd ..
git pull
cd test/

make client

make server
