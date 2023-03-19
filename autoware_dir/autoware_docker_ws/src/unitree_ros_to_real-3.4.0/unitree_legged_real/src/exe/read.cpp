/******************************************************************************
 *   Editer:: Hayato Nakamura 19X0118
 *   2022/12/13~
 *   on ros merodic
 *   
 * 　プログラムチェック用
 * 　Subscriubeプログラム
 * 
*******************************************************************************/


#include <ros/ros.h>
#include <geometry_msgs/TwistStamped.h>

//Subscribu処理
void cmd_callback(const geometry_msgs::TwistStamped& msg_cmd){
    printf("vel_linear_x = %3.2f\n",msg_cmd.twist.linear.x);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "read_cmd");
    ros::NodeHandle nh;
    ros::Subscriber cmd_sub = nh.subscribe("/twist_cmd", 10, cmd_callback);

    ros::spin();
    return 0;
}
