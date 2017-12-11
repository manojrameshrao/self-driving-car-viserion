#!/bin/bash

# 
# This file executes from the build directory by the Eclipse "Pre Build" step
#

python ../../dbc_parse.py -i ../../Viserion.dbc -s BT -a > ../_can_dbc/generated_Viserion.h