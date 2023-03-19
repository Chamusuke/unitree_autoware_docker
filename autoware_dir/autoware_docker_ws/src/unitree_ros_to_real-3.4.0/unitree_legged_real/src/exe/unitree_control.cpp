/******************************************************************************
 *   Editer:: Hayato Nakamura 19X0118
 *   2022/12/13~
 *   on ros merodic
 *
 *   you can control unitreee with autoware.
 *   The subscribed twist_cmd(geometry_msgs/Twist) of autoware is
 *   transformed and Sent to Unitree
 *  
 *   I'm soryy for you.
 *   I wrote most comment in English.
 *   Becouse I care that Japanese text is garbled and you can't read a thing, 
 *   If you dont't use the format "UTF-8", these japanese text will be garbled.
 * 　コメントは日本語でも大丈夫　pintfなど表示系は文字化けするので基本英語で
 * 
 * 　
 * 　
 * 　Autoware自動運転用　バックを制限
*******************************************************************************/

#include <iostream>
#include <ros/ros.h>
#include <pthread.h>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <unitree_legged_msgs/HighCmd.h>
#include <unitree_legged_msgs/HighState.h>
#include <convert.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Twist.h>


using namespace UNITREE_LEGGED_SDK;
geometry_msgs::Twist autoware_cmd;
int flag = 0;

//const float speed = 0.5f; //gain
const float yaw   = 1.5f; 
//gain 実験の結果、角速度はゲインを挟むと追従性が良くなった

/*わからん パラメータを何かに格納している
あとでNULLチェックしているので、何かを参照している*/
template<typename TLCM>
void* update_loop(void* param) 
{
    TLCM *data = (TLCM *)param;//paramをvoid*型に変換している。
    while(ros::ok)
    {
        data->Recv();
        usleep(2000);
    }
}

//Autowareではバック機能がないので（たぶん）
int unback(float &msg)
{
 msg = abs(msg);
 return 0;  
}

//Subscribu処理
void autowareCallback(const geometry_msgs::TwistStamped::ConstPtr& msg)
{
    //twist_cmd on autoware -->geometry_msgs::TwistStamped  
    //geometry_msgs::TwistStamped --> header + twist (geometry_msgs::Twist)
    //大半のロボットがgeometry_msgs::Twistなので今後なにかに使いやすく（リファレンス）
    autoware_cmd.linear.x = msg->twist.linear.x;
    autoware_cmd.linear.y = 0.0;
    autoware_cmd.linear.z = 0.0;
    autoware_cmd.angular.x = 0.0;
    autoware_cmd.angular.y = 0.0;
    autoware_cmd.angular.z = msg->twist.angular.z;

    std::cout   << "linear.x : " << to_string(autoware_cmd.linear.x)  << std::endl
                //<< "linear.y : " << to_string(autoware_cmd.linear.y)  << std::endl
                << "angular.z: " << to_string(autoware_cmd.angular.z) << std::endl;
    flag = 1; //twist_cmdからの受信完了フラグ
    
    return;
}


int main(int argc, char* argv[]){

    std::cout   << "Warning: Set control level to High level."                  <<std::endl
                << "You must make sure thr robot is standing on the ground."    <<std::endl
                << "Have you arleady started? OK??."                            <<std::endl
                << "Press ENTER!! On the Auoware, You can start to control the Unitree. " <<std::endl;
    std::cin.ignore();

    /*rosの基本設定*/
    ros::init(argc, argv, "autoware_control");
    autoware_cmd.linear.x = 0.0;
    autoware_cmd.linear.y = 0.0;
    autoware_cmd.linear.z = 0.0;
    autoware_cmd.angular.x = 0.0;
    autoware_cmd.angular.y = 0.0;
    autoware_cmd.angular.z = 0.0;

    ros::NodeHandle nh;
    ros::Subscriber twist_sub = nh.subscribe("/safety_filter", 1000, autowareCallback);
    UNITREE_LEGGED_SDK::LCM roslcm(UNITREE_LEGGED_SDK::HIGHLEVEL); 
    
    int motionscore = 0;
    int score = 0;
    ros::Rate loop_rate(500);//500
    // SetLevel(HIGHLEVEL);
    UNITREE_LEGGED_SDK::HighCmd SendHighLCM = {0};
    UNITREE_LEGGED_SDK::HighState RecvHighLCM = {0};
    unitree_legged_msgs::HighCmd SendHighROS;
    unitree_legged_msgs::HighState RecvHighROS;

    roslcm.SubscribeState();

    pthread_t tid;
    pthread_create(&tid, NULL, update_loop<UNITREE_LEGGED_SDK::LCM>, &roslcm);
        
    SendHighROS.mode = 0;      
    SendHighROS.gaitType = 0;
    SendHighROS.speedLevel = 0;
    SendHighROS.footRaiseHeight = 0;
    SendHighROS.bodyHeight = 0;
    SendHighROS.euler[0]  = 0;
    SendHighROS.euler[1] = 0;
    SendHighROS.euler[2] = 0;
    SendHighROS.velocity[0] = 0.0f;
    SendHighROS.velocity[1] = 0.0f;
    SendHighROS.yawSpeed = 0.0f;
    SendHighROS.reserve = 0;


    //以下ループ処理
    while (ros::ok()){
        motionscore = motionscore+1;
        printf("-------------------motionscore: %d -------------------\n", motionscore);
        
        roslcm.Get(RecvHighLCM);
        RecvHighROS = ToRos(RecvHighLCM);
        /**
        * 詳細はunitree_legged_msgsを参照
        * HighCmd data
        * IMU, foot speed & footpsition(relative to body)など 
        */

        if (flag == 1){

            SendHighROS.mode = 0;
            /**
            * 詳細はunitree_legged_msgsを参照   
            * 0. idle, default stand;
            * 1. force stand (controlled by bBodyHeight + ypr)
            * 2. target velocity walking (controlled by velocity + yawspeed)
             * 3. target position walking (controlled by position + ypr[0])
             * 4. >< path mode walking (reserve for furture reelase)
             * 5. position stand down
             * 6. position stand up
             * 7. damping mode
             * 8. recovery stand
             * 9. backflip
             * 10. jump yaw
             * 11. straight hand
             * 12. dance1
             * 13, dance2
             * 14.tow leg stand
             **/ 
            SendHighROS.gaitType = 0;        //0. idle, 1.trot, 2.trot runing, 3.climb stair
            SendHighROS.speedLevel = 0;      //0. defalut low speed, 1.medium, 2.high, 
            SendHighROS.footRaiseHeight = 0.0f; //(unit:m),float32:foot up height while walking
            SendHighROS.bodyHeight = 0.0f;      //(unit:m) float32
            SendHighROS.euler[0] = 0.0f;        //(unit:rad) float32 roll pitch yaw in stand mode
            SendHighROS.euler[1] = 0.0f;        
            SendHighROS.euler[2] = 0.0f;
            SendHighROS.velocity[0] = 0.0f;  //(unit:m/s)forward speed
            SendHighROS.velocity[1] = 0.0f;  //(unit:m/s)side speed
            SendHighROS.yawSpeed = 0.0f;     //(unit:rad/s)rotate speed
            SendHighROS.reserve = 0;         //old version Aliengo doesn't have
        

            /*速度制御モードにし、データの代入*/

            /**-------geometry/Twist-------------
             * geometry_msg/Twist transed into unitree
             * Check!! Which's speed m/s or km/h ??　
             * I'd check !! ----->aoutoware(m/s, rad/s) and Uniree(m/s, rad/s)
             * geometry_msgs/Vector3 linear
             * float64 x, y, z -->C++ double
             * geometry_msgs/Vector3 angular
             * float64 x, y, z -->C++ double
             **/
            SendHighROS.mode = 2;
            SendHighROS.gaitType = 1;
            float x_vel = (float)autoware_cmd.linear.x;
            unback(x_vel);
            SendHighROS.velocity[0]  = x_vel;  //aoutowareにはバック機能はない
            SendHighROS.velocity[1] = (float)autoware_cmd.linear.y;
            SendHighROS.yawSpeed = (float)autoware_cmd.angular.z * yaw ;
            SendHighROS.footRaiseHeight = -0.2f;

            printf("Completed to send \n");
            flag = 0;
            score = 0;
        }

        else{
               
            if(score < 100){
                printf("No data but sent to unitree. count %d \n", score);
                score = score+1;
                flag = 0;
            }

            else{
                SendHighROS.mode = 0;      
                SendHighROS.gaitType = 0;
                SendHighROS.speedLevel = 0;
                SendHighROS.footRaiseHeight = 0.0f;
                SendHighROS.bodyHeight = 0.0f;
                SendHighROS.euler[0]  = 0;
                SendHighROS.euler[1] = 0;
                SendHighROS.euler[2] = 0;
                SendHighROS.velocity[0] = 0.0f;
                SendHighROS.velocity[1] = 0.0f;
                SendHighROS.yawSpeed = 0.0f;
                SendHighROS.reserve = 0;
                printf("No data, Sent idling comand to Unitree \n");
                flag = 0;
                score = score + 1;
            }
            
        }

        std::cout   << "Unitree.x: " << to_string(SendHighROS.velocity[0])  << std::endl
                    << "Unitree.y: " << to_string(SendHighROS.velocity[1])  << std::endl
                    << "Unitree.z: " << to_string(SendHighROS.yawSpeed)     << std::endl;


        //Cmd msg sent to Unitree
        /**
         * 受信 --> roslcm.Get(RecvHighLCM);
         *          RecvHighROS = ToRos(RecvHighLCM);
         * 
         * 送信 --> SendHighLCM = ToLcm(SendHighROS, SendHighLCM);
         *          roslcm.Send(SendHighLCM);
        */
        SendHighLCM = ToLcm(SendHighROS, SendHighLCM);
        roslcm.Send(SendHighLCM);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
