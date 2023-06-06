# unitree_autoware_docker

## Introduction
　This software is intreface of Unitree Go1 EDU on Autoware and Autoware.AI.
 
　This implements the control interface of UnitreeGO1EDU in the automatic operation software (Autoware), which enables the actual operation of the device by Autoware.

　unitree_ros_to_real-3.4.0 and unitree_legged_real-3.4.2 are inclueded in software.
 
![system_phot](https://github.com/Chamusuke/unitree_autoware_docker/assets/120120108/e511fb36-024c-4a65-8d37-2588dda5a49d)
![Auto_pilot_8](https://github.com/Chamusuke/unitree_autoware_docker/assets/120120108/e3c4bc2e-8948-4379-9074-b921d219fc3e)

 
## Dependencies
- Linux PC (Ubuntu18.04 or 20.04 is checked to run on this software)
- docker
- unitree Go1 EDU
- LiDAR veldyne VLP-16


## Install && Software start
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


## build
 You must need to build Ros program.
 ROS compiler's in docker container.
```
cd shared_dir/autoware_docker_ws && catkin_make
```

## Hardware setting
 You need to connect Uintree to PC and LiDAR via Ethernet.
 In addition, setting IP address on PC and LiDAR.

## Autoware start 
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

## unitree control
### Manual keybord control

![localization_system](https://github.com/Chamusuke/unitree_autoware_docker/assets/120120108/1721a68d-544a-4769-88eb-1519988e03d6)

If it does not start, you must enter the commands one at a time.

```
cd ~/shared_dir/launch_sh/manual_control
bash unitree_LCM.sh
bash imu.sh
bash filter_control.sh
bash keybord_control.sh
```

### Autoware control 

![Auto_pilot](https://github.com/Chamusuke/unitree_autoware_docker/assets/120120108/5c10b394-ba5a-4da4-bb59-39241cf9bd52)

In order to avoid sudden start,
When you run safety.sh, you need to push "b". 

```
cd ~/shared_dir/launch_sh/auto_pilot
bash unitree_LCM.sh
bash imu.sh
bash safety.sh
bash autoware_control.sh
```

# mapping
![pcd_data_no2](https://github.com/Chamusuke/unitree_autoware_docker/assets/120120108/a6fec5b7-9fef-44a2-8ff2-02923bc7e804)

