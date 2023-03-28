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
cd ~/unitree_autoware_docker
bash cpu_run_autoware.sh
```

If you use NVIDIA GPU, you can run CUDA.
```
bash nvidia_run_autoware.sh
```


# build
 You must need to build Ros program.
 ROS compiler's in docker container.
```
cd shared_dir/autoware_docker_ws && catkin_make
```

# Hardware setting
 You need to connect Uintree to PC and LiDAR via Ethernet.
 In addition, setting IP address on PC and LiDAR.

# Autoware start 
 You need to enter the following command in the container.
 and make subterminal.
```
cd ~/shared_dir/launch_sh
gnome-terminal
```
 You can start autoware with this command.
```
bash runtime_manager.sh
```

# unitree control
-Manual keybord control
If it does not start, you must enter the commands one at a time.

```
cd ~/shared_dir/launch_sh/manual_control
bash unitree_LCM.sh
bash imu.sh
bash filter_control.sh
bash keybord_control.sh
```

-Autoware control 
```
cd ~/shared_dir/launch_sh/auto_pilot
bash unitree_LCM.sh
bash imu.sh
bash autoware_control.sh
bash safety.sh
```
