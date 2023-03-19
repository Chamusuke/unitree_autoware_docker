/******************************************************************************
 *   Editer:: Hayato Nakamura 19X0118
 *   2022/12/13~
 *   on ros merodic
 *   
 *   Autoware自動運転時の安全装置
 *   自動運転時にはこのプログラムで発進と停止を行う
 * 
 * 
 * 　
 * 　キーボード入力による割り込みプログラムを追加
 * 　”ｂ”を押してAutowareからUnitreeへの制御信号をポーズする．
 *   以前は緊急停止は、プログラムを強制終了させていた．
 * 　しかしこれでは、物理的切断であり一部のプログラムが宙吊りとなり危険であった．
 * 　
 * 　システムにポーズ機能を入れることで、プログラムを宙吊りにさせることなく
 * 　制御信号のみをとめることができ、安全性が高まる．
 * 
 * 　またCtrl+Cによる終了を禁止にし、"ｂ"を押して必ず一時停止したあと
 *   続けて"q"を入力しEnterキーを押すことでプログラムを終了できる．
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
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

char key = (' ');
int flag = 0;
geometry_msgs::TwistStamped twist_safe;

//Subscribu処理
void autowareCallback(const geometry_msgs::TwistStamped::ConstPtr& msg)
{
        twist_safe.twist.linear.x    = msg->twist.linear.x;
        twist_safe.twist.linear.y    = msg->twist.linear.y;
        twist_safe.twist.linear.z    = msg->twist.linear.z;
        twist_safe.twist.angular.z   = msg->twist.angular.z;

        printf("callback:%f \n", twist_safe.twist.linear.x);

        flag = 1; //twist_cmdからの受信完了フラグ
    return;
}

//キーボード入力処理
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

//キーボード入力割り込みイベント
int isInterrupt()
{
    if ( kbhit() )
    {
        if ( getchar() == 'b' )
        {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]){

    // 基本設定
    ros::init(argc, argv, "safety_filter");
 

    twist_safe.twist.linear.x = 0.0;
    twist_safe.twist.linear.y = 0.0;
    twist_safe.twist.linear.z = 0.0;
    twist_safe.twist.angular.x = 0.0;
    twist_safe.twist.angular.y = 0.0;
    twist_safe.twist.angular.z = 0.0;

    ros::NodeHandle nh;
    ros::Subscriber twist_sub = nh.subscribe("/twist_cmd", 1000, autowareCallback);
    ros::Publisher pub_twist = nh.advertise<geometry_msgs::TwistStamped>("/safety_filter", 10);
    
    int motionscore = 0;
    int score = 0;
    ros::Rate loop_rate(100);

    //ループ処理 ros::ok()による
    while (true){

        // Get the pressed key
        motionscore = motionscore + 1;
        printf("-------------------motionscore: %d -------------------\n", motionscore);


        //一時停止とプログラムの終了（ループからの脱出）
        if(isInterrupt()){
            printf("Program stop 'q' and 'Enter', Pose off 'Enter' \n");
            key = getchar();
            // ｑを押せばプログラムを終了できる
            if (key == 'q'){
                break;
            }
        }

        if(flag == 1 ){
            printf("Rceive\n");
            score = 0;
            flag = 0;
        }

        if(flag == 0){
            if(score < 100){
                printf("Recover data :%d \n", score);
            }

            else{
                printf("NO.COMAND :%d \n", score);
                twist_safe.twist.linear.x = 0.0;
                twist_safe.twist.linear.y = 0.0;
                twist_safe.twist.linear.z = 0.0;
                twist_safe.twist.angular.x = 0.0;
                twist_safe.twist.angular.y = 0.0;
                twist_safe.twist.angular.z = 0.0;                
                    
            }

            score = score + 1;
            flag = 0;
        }

        std::cout   << "Unitree.x: " << to_string(twist_safe.twist.linear.x)  << std::endl
                    << "Unitree.y: " << to_string(twist_safe.twist.linear.y)  << std::endl
                    << "Unitree.z: " << to_string(twist_safe.twist.linear.z)     << std::endl
                    << "Unitree.z: " << to_string(twist_safe.twist.angular.z)     << std::endl;

        printf("Stop command 'b'. Last command: %c \n", key);

        pub_twist.publish(twist_safe);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;

}