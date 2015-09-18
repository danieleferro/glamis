#!/bin/bash

# script to be called by cron for setting
# off at 19:31 on the same day

at -M 19:31 2>/tmp/glamis_off_id <<EOF
~/glamis/glamis_server -c off
EOF

