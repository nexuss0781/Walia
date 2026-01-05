#!/bin/bash 
set -e 
 
# Visuals 
GREEN='\033[0;32m' 
NC='\033[0m' 
 
echo -e "${GREEN}>> INITIALIZING WALIA SOVEREIGN BUILD SEQUENCE...${NC}" 
 
# 1. Clean previous artifacts 
make clean 
 
# 2. Compile and Link 
make iso 
 
echo -e "${GREEN}>> BUILD SUCCESSFUL.${NC}" 
echo -e ">> To launch simulation: make run" 
