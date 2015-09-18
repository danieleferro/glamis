#!/bin/bash

# script to be called by cron for setting 
# on at 19:01 on the same day

at -M 19:01 2>/tmp/glamis_on_id <<EOF
/home/daniele/glamis/glamis_server -c on
EOF

