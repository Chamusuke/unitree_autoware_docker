# unitree_autoware_docker

# Introduction
　This software is intreface of Unitree Go1 EDU on Autoware and Autoware.AI.
 
　This implements the control interface of UnitreeGO1EDU in the automatic operation software (Autoware), which enables the actual operation of the device by Autoware.

　unitree_ros_to_real-3.4.0 and unitree_legged_real-3.4.2 are inclueded in software.
 
# Dependencies
- Linux PC (Ubuntu18.04 or 20.04 is checked to run on this software)
- docker
- unitree Go1 EDU
- LiDAR veldyne VLP-16


# Install && Software start
 You need to pull docker image from DockerHub．
```
docker pull chamusuke/unitree_autoware:0.0
```
 You can start software.
```
docker run chamusuke/unitree_autoware:0.0
```


# build
 You must need to build Ros program.
```
cd shared_dir/autoware_docker_ws && catkin_make
```

# Hardware setting
 You need to connect Uintree to PC and LiDAR via Ethernet
 In addition, setting IP address on PC and LiDAR
 

# Software setting
