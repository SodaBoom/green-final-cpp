#!/bin/bash

chmod +x atec_project/*
sudo docker build -f Dockerfile -t atec_timesharing:player-test-v1 .
sudo docker save atec_timesharing:player-test-v1 -o green_final_v1.tar && sudo chmod 0777 green_final_v1.tar
