clear
resize -s 40 130 
cat text/nakamura.txt
cat nvidia_run_autoware.sh
#"Editer::  Nakamura Hayato 19X0118"
#"2022-12 ~~~"
# ------------------------------------------------------
docker run -it --rm --name unitree -e DISPLAY=$DISPLAY \
	--volume=/tmp/.X11-unix:/tmp/.X11-unix:rw \
	--volume=$HOME/.Xauthority:$HOME/.Xauthority:rw \
	--volume=$HOME/unitree_autoware_docker/autoware_dir:/home/autoware/shared_dir:rw \
	--env=XAUTHORITY=$HOME/.Xauthority \
	--env=USER_ID=$(id -u) \
	--privileged --net=host \
	--runtime=nvidia \
	-e NVIDIA_VISIBLE_DEVICES=all \
	-e NVIDIA_DRIVER_CAPABILITIES=all \
       	chamusuke/unitree_autoware:0.0
