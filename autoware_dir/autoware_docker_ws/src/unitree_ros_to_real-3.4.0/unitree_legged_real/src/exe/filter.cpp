/******************************************************************************
 *   Editer:: Hayato Nakamura 19X0118
 *   2022/12/13~
 *   on ros merodic
 *   
 * 　移動平均フィルタとデジタルフィルタを合成
 * 　設計については卒論参照
 * 
 * 
*******************************************************************************/
#include <iostream>
#include <ros/ros.h>
#include <pthread.h>
#include <string>
#include <unitree_legged_msgs/HighState.h>
#include <convert.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Twist.h>


int flag = 0;
int n = 10;
float k = 0.02; 
double data_linear_x[20] ;
double data_linear_y[20] ;
double data_linear_z[20] ;
double data_angular_z[20] ;

//Subscribe処理
void autowareCallback(const geometry_msgs::TwistStamped::ConstPtr& msg)
{
        data_linear_x[0]    = msg->twist.linear.x;
        data_linear_y[0]    = msg->twist.linear.y;
        data_linear_z[0]    = msg->twist.linear.z;
        data_angular_z[0]   = msg->twist.angular.z;
    
        flag = 1; //twist_cmdからの受信完了フラグ
    return;
}

// デジタルフィルタ
void filter(double &fil, double data, double k){
    //data0 is new
    fil = data * k + fil *(1-k);
}

//後移動平均
void Movemean(double &MM, double data0, double data_end){
    //data0 is new
    MM = MM + (data0 - data_end)/n;
}

int main(int argc, char* argv[]){

    ros::init(argc, argv, "twist_filter");
    geometry_msgs::TwistStamped twist_fil;

    double MoveMean_linear_x = 0.0;
    double MoveMean_linear_y = 0.0;
    double MoveMean_linear_z = 0.0;
    double MoveMean_angular_z = 0.0;
 
    double fil_move[4];
    fil_move[0] = 0.0;
    fil_move[1] = 0.0;
    fil_move[2] = 0.0;
    fil_move[3] = 0.0;
 
    twist_fil.twist.linear.x = 0.0;
    twist_fil.twist.linear.y = 0.0;
    twist_fil.twist.linear.z = 0.0;
    twist_fil.twist.angular.x = 0.0;
    twist_fil.twist.angular.y = 0.0;
    twist_fil.twist.angular.z = 0.0;

    ros::NodeHandle nh;
    ros::Subscriber twist_sub = nh.subscribe("/twist_cmd", 1000, autowareCallback);
    ros::Publisher pub_twist = nh.advertise<geometry_msgs::TwistStamped>("/twist_filter", 10);
    
    int motionscore = 0;
    int score = 0;
    ros::Rate loop_rate(100);

    while (ros::ok()){
        motionscore = motionscore+1;
        printf("-------------------motionscore: %d -------------------\n", motionscore);

        twist_fil.twist.linear.x = 0.0;
        twist_fil.twist.linear.y = 0.0;
        twist_fil.twist.linear.z = 0.0;
        twist_fil.twist.angular.x = 0.0;
        twist_fil.twist.angular.y = 0.0;
        twist_fil.twist.angular.z = 0.0;

        for(int i = 1; i <= n; i++){

            if (i < n){

                data_linear_x[n - i]    = data_linear_x[n - (i + 1) ];
                data_linear_y[n - i]    = data_linear_y[n - (i + 1) ];
                data_linear_z[n - i]    = data_linear_z[n - (i + 1) ];
                data_angular_z[n - i]   = data_angular_z[n - (i + 1) ];
            }

            else{
                if(flag == 1){
                    printf("Receve\n");
                }
            
                if(flag == 0){
                    data_linear_x[0]    = 0.0;
                    data_linear_y[0]    = 0.0;
                    data_linear_z[0]    = 0.0;
                    data_angular_z[0]   = 0.0;
                    printf("No comand\n");

                }
            }
        }
    

        Movemean(MoveMean_linear_x, data_linear_x[0], data_linear_x[n-1]);
        Movemean(MoveMean_linear_y, data_linear_y[0], data_linear_y[n-1]);
        Movemean(MoveMean_linear_y, data_linear_z[0], data_linear_z[n-1]);
        Movemean(MoveMean_angular_z, data_angular_z[0], data_angular_z[n-1]);

        filter(fil_move[0], MoveMean_linear_x, k);
        filter(fil_move[1], MoveMean_linear_y, k);
        filter(fil_move[2], MoveMean_linear_z, k);
        filter(fil_move[3], MoveMean_angular_z, k);

        twist_fil.twist.linear.x = fil_move[0];
        twist_fil.twist.linear.y = fil_move[1];
        twist_fil.twist.linear.z = fil_move[2];
        twist_fil.twist.angular.z = fil_move[3];

        std::cout   << "Unitree.x: " << to_string(twist_fil.twist.linear.x)  << std::endl
                    << "Unitree.y: " << to_string(twist_fil.twist.linear.y)  << std::endl
                    << "Unitree.z: " << to_string(twist_fil.twist.linear.z)     << std::endl
                    << "Unitree.z: " << to_string(twist_fil.twist.angular.z)     << std::endl;
        pub_twist.publish(twist_fil);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;

}