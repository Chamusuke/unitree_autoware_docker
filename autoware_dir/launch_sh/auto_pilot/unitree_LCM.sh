gnome-terminal -- bash -c \
 "source  ~/shared_dir/autoware_docker_ws/devel/setup.bash; \
roslaunch unitree_legged_real real.launch ctrl_level:=highlevel; bash"
