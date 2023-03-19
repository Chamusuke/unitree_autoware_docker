cclear
resize -s 40 130 
cat text/nakamura.txt
cat cpu_run_autoware.sh
#"Editer::  Nakamura Hayato 19X0118"
#"2022-12 ~~~"
# ------------------------------------------------------
docker run -it --rm --name unitree \
	--volume=/tmp/.X11-unix:/tmp/.X11-unix:rw \
	--volume=$HOME/.Xauthority:$HOME/.Xauthority:rw \
	--volume=$HOME/unitree_autoware_docker/autoware_dir:/home/autoware/shared_dir:rw \
	--env=XAUTHORITY=$HOME/.Xauthority \
	--env=DISPLAY=$DISPLAY \
	--env=USER_ID=$(id -u) \
	--privileged --net=host \
       	unitree/autoware:0.0
