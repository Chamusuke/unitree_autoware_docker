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
 * 
 * 　テストコントロールでは、キーボード制御＋フィルターから０〜１の範囲で制御値が届く
 * 　autowareに接続しないプログラムなためバック機能にも対応
 * 　
 * 　キーボードコントロール用　合わせてFilter.cppも起動させる
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
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>

using namespace UNITREE_LEGGED_SDK;
geometry_msgs::Twist autoware_cmd;
int flag = 0;
//const float speed_offset = 0.03f; //m/s　　offset　
const float yaw_offset   = 0.03f; //rad/s　offset　
float speed = 0.3f;    //m/s
float yawspeed = 0.8f; //rad/s

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

int unback(double &msg)
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
    autoware_cmd.linear.y = msg->twist.linear.y;
    autoware_cmd.linear.z = 0.0;
    autoware_cmd.angular.x = 0.0;
    autoware_cmd.angular.y = 0.0;
    autoware_cmd.angular.z = msg->twist.angular.z;
    /*
    std::cout   << "linear.x : " << to_string(autoware_cmd.twist.linear.x)  << std::endl
                << "linear.y : " << to_string(autoware_cmd.twist.linear.y)  << std::endl
                << "angular.z: " << to_string(autoware_cmd.twist.angular.z) << std::endl;
    */
    flag = 1; //twist_cmdからの受信完了フラグ
    
    return;
}

int main(int argc, char* argv[]){

    std::cout   << "Warning: Set control level to High level."                  <<std::endl
                << "You must make sure thr robot is standing on the ground."    <<std::endl
                << "Have you arleady started? OK??."                            <<std::endl
                << "Press ENTER!! On the Auoware, You can start to control the Unitree. " <<std::endl;
    std::cin.ignore();

    /*ros*/
    ros::init(argc, argv, "manual_control");
    autoware_cmd.linear.x = 0.0;
    autoware_cmd.linear.y = 0.0;
    autoware_cmd.linear.z = 0.0;
    autoware_cmd.angular.x = 0.0;
    autoware_cmd.angular.y = 0.0;
    autoware_cmd.angular.z = 0.0;

    ros::NodeHandle nh;
    ros::Subscriber twist_sub = nh.subscribe("/twist_filter", 1000, autowareCallback);
    UNITREE_LEGGED_SDK::LCM roslcm(UNITREE_LEGGED_SDK::HIGHLEVEL); 
    
    int motionscore = 0;
    int score = 0;
    

    ros::Rate loop_rate(100);//500
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

    while (ros::ok()){
        motionscore = motionscore+1;
        printf("-------------------motionscore: %d -------------------\n", motionscore);

        //Unitree内部のデータを受信
        roslcm.Get(RecvHighLCM);
        RecvHighROS = ToRos(RecvHighLCM);
            /**
             * 詳細はunitree_legged_msgsを参照
             * HighCmd data
             * IMU, foot speed & footpsition(relative to body)など 
             */
        std::cout   << "Unitree.quanternion.w: " << to_string(RecvHighROS.imu.quaternion[0])  << std::endl
                    << "Unitree.quanternion.x: " << to_string(RecvHighROS.imu.quaternion[1])  << std::endl
                    << "Unitree.quanternion.y: " << to_string(RecvHighROS.imu.quaternion[2])  << std::endl
                    << "Unitree.quanternion.z: " << to_string(RecvHighROS.imu.quaternion[3])  << std::endl << std::endl

                    << "Unitree.gyroscope.x: " << to_string(RecvHighROS.imu.gyroscope[0])  << std::endl
                    << "Unitree.gyroscope.y: " << to_string(RecvHighROS.imu.gyroscope[1])  << std::endl
                    << "Unitree.gyroscope.z: " << to_string(RecvHighROS.imu.gyroscope[2])  << std::endl << std::endl

                    << "Unitree.accelerometer.x: " << to_string(RecvHighROS.imu.accelerometer[0])  << std::endl
                    << "Unitree.accelerometer.y: " << to_string(RecvHighROS.imu.accelerometer[1])  << std::endl
                    << "Unitree.accelerometer.z: " << to_string(RecvHighROS.imu.accelerometer[2])  << std::endl << std::endl

                    << "Unitree.x: " << to_string(SendHighROS.velocity[0])  << std::endl
                    << "Unitree.y: " << to_string(SendHighROS.velocity[1])  << std::endl
                    << "Unitree.yaw_z: " << to_string(SendHighROS.yawSpeed)     << std::endl << std::endl;

        //Unitreeに制御信号の送信
        if (flag == 1){
            SendHighROS.mode = 0;
            /**
             * 詳細はunitree_legged_msgsもしくはSDK内のinclude comm.hを参照   
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
            SendHighROS.bodyHeight = 0.0f;      //(unit:m) float32 defalt -> +0.2 m
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
             * 
             * キーボード制御での直進時、左にそれる現象があったため、角速度にオフセットを挟む
             * autoware_cmd.linear.x は、0~1なのでこれで良い
             **/
            SendHighROS.mode = 2;
            SendHighROS.gaitType = 1;
            SendHighROS.velocity[0] =  (float)autoware_cmd.linear.x * speed; 
            SendHighROS.velocity[1] = (float)autoware_cmd.linear.y * speed;
            SendHighROS.yawSpeed = (float)autoware_cmd.angular.z * yawspeed + ((float)autoware_cmd.linear.x * yaw_offset); 
            SendHighROS.footRaiseHeight = -0.2f;  //足の動作を優しくする

            printf("Completed to send");
            flag = 0;
            score = 0;
        }

        else{
            if(score < 100){
                printf("No data but sent to unitree. count %d \n", score);
            
                score = score+1;
                flag = 0;
            }

            if(score > 100){
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
                printf("No,data, Unitree's sent idling comand");
                flag = 0;
                score = score + 1;
            }
        }
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
