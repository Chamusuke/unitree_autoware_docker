/******************************************************************************
 *   Editer:: Hayato Nakamura 19X0118
 *   2022/12/13~
 *   on ros merodic
 * 
 *   参照-->
 *         unitree_legged_sdk-3.4.2/inculde/unitree_legged_sdk//comm.h
 *         unitree_ros_to_real-3.4.0/unitree_msgs/  *.msg
 * 
 * 　Unitree内部情報の通信リファレンス　IMUなどを取得できる
 * 　UnitreeからIMUデータを受信し、ROS規格のIMUメッセージに変換する
*******************************************************************************/

#include <iostream>
#include <ros/ros.h>
#include <pthread.h>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <unitree_legged_msgs/HighState.h>
#include <unitree_legged_msgs/IMU.h>
#include <sensor_msgs/Imu.h>
#include <convert.h>

using namespace UNITREE_LEGGED_SDK;
int flag = 0;

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

int main(int argc, char **argv){

    /*ros*/
    ros::init(argc, argv, "Unitree_IMU");

    ros::NodeHandle nh;
    ros::Publisher pub_imu_raw = nh.advertise<sensor_msgs::Imu>("imu_raw", 10);
    //ros::Publisher pub_imu_unitree = nh.advertise<unitree_legged_msgs::IMU>("Unitree_IMU", 10);
    
    UNITREE_LEGGED_SDK::LCM roslcm(UNITREE_LEGGED_SDK::HIGHLEVEL); 
    
    int motionscore = 0;
    ros::Rate loop_rate(10);

    
    // SetLevel(HIGHLEVEL);
    UNITREE_LEGGED_SDK::HighState RecvHighLCM = {0};
    unitree_legged_msgs::HighState RecvHighROS;

    //Unitree用のメッセージ型
    unitree_legged_msgs::IMU RecvIMU;
    /*
    * quaternion[0~4], normalized, (w,x,y,z)
	* float gyroscope[0~3], angular velocity (x,y,z)（unit: rad/s)    (raw data)
    * float accelerometer[0~3]   (x,y,z) m/(s2)                       (raw data)
    */

    //ROSでの規格化されたIMUのメッセージ形式
    sensor_msgs::Imu unitree_imu;
    /*
    * std_msgs/Header header
    * geometry_msgs/Quaternion orientation
    * float64[9] orientation_covariance      ----->センサー値なので共分散　デフォルトで0.0
    * geometry_msgs/Vector3 angular_velocity
    * float64[9] angular_velocity_covariance
    * geometry_msgs/Vector3 linear_acceleration
    * float64[9] linear_acceleration_covariance
    */



    roslcm.SubscribeState();

    pthread_t tid;
    pthread_create(&tid, NULL, update_loop<UNITREE_LEGGED_SDK::LCM>, &roslcm);
        /*
        RecvHighROS.levelFlag;                       //uint8
        RecvHighROS.commVersion;                     //uint16: Old version Aliengo does not have
        RecvHighROS.robotID;                         //uint16 Old version Aliengo does not have
        RecvHighROS.SN;                              //uint32 Old version Aliengo does not have
        RecvHighROS.bandWidth;                       //uint16 Old version Aliengo does not have;
        RecvHighROS.mode;                            //uint8
        RecvHighROS.progress;                        //folat32 new on Go1, reserve
        RecvHighROS.imu;                             //unitree_msgs/Imu
        RecvHighROS.gaitType;                        //uint8 new on Go1, 0.idle  1.trot  2.trot running  3.climb stair
        RecvHighROS.footRaiseHeight;                 //foalt32 (unit: m, default: 0.08m), foot up height while walking
        RecvHighROS.position;                        //foalt32 (unit: m), from own odometry in inertial frame, usually drift
        RecvHighROS.bodyHeight;                      //foalt32 (unit: m, default: 0.28m)
        RecvHighROS.velocity;                        //foalt32 (unit: m/s), forwardSpeed, sideSpeed, rotateSpeed in body frame
        RecvHighROS.yawSpeed;                        //foalt32 (unit: rad/s), rotateSpeed in body frame        
        RecvHighROS.footPosition2Body;               //unitree_msds/Ceterian[4] foot position relative to body
        RecvHighROS.footSpeed2Body;                  //unitree_msgs/Ceterian[4] foot speed relative to body
        RecvHighROS.bms;                             //unitree_msgs/BmsState
        RecvHighROS.footForce;                       //int16[4] Old version Aliengo is different
        RecvHighROS.footForceEst;                    //int16[4] Old version Aliengo does not have
        RecvHighROS.wirelessRemote;                  //unit8[40]
        RecvHighROS.reserve;                         //unit32 Old version Aliengo does not have
        RecvHighROS.crc;                             //unit32
        */

    while (ros::ok()){
        motionscore = motionscore+1;
        printf("-------------------motionscore: %d -------------------\n", motionscore);

        roslcm.Get(RecvHighLCM);
        RecvHighROS = ToRos(RecvHighLCM);
        //RecvHighROS.imu;
        std::cout       << "Unitree.quanternion.w: "    << to_string(RecvHighROS.imu.quaternion[0])  << std::endl
                        << "Unitree.quanternion.x: "    << to_string(RecvHighROS.imu.quaternion[1])  << std::endl
                        << "Unitree.quanternion.y: "    << to_string(RecvHighROS.imu.quaternion[2])  << std::endl
                        << "Unitree.quanternion.z: "    << to_string(RecvHighROS.imu.quaternion[3])  << std::endl << std::endl

                        << "Unitree.gyroscope.x: "      << to_string(RecvHighROS.imu.gyroscope[0])  << std::endl
                        << "Unitree.gyroscope.y: "      << to_string(RecvHighROS.imu.gyroscope[1])  << std::endl
                        << "Unitree.gyroscope.z: "      << to_string(RecvHighROS.imu.gyroscope[2])  << std::endl << std::endl
                        
                        << "Unitree.accelerometer.x: "  << to_string(RecvHighROS.imu.accelerometer[0])  << std::endl
                        << "Unitree.accelerometer.y: "  << to_string(RecvHighROS.imu.accelerometer[1])  << std::endl
                        << "Unitree.accelerometer.z: "  << to_string(RecvHighROS.imu.accelerometer[2])  << std::endl << std::endl;

        //チェック用
        //RecvIMU = RecvHighROS.imu;

        unitree_imu.orientation.w = RecvHighROS.imu.quaternion[0];
        unitree_imu.orientation.x = RecvHighROS.imu.quaternion[1];
        unitree_imu.orientation.y = RecvHighROS.imu.quaternion[2];
        unitree_imu.orientation.z = RecvHighROS.imu.quaternion[3];

        /*チェック用
        unitree_imu.angular_velocity.x = RecvHighROS.imu.gyroscope[0];
        unitree_imu.angular_velocity.y = RecvHighROS.imu.gyroscope[1];
        unitree_imu.angular_velocity.z = RecvHighROS.imu.gyroscope[2];

        unitree_imu.linear_acceleration.x = RecvHighROS.imu.accelerometer[0] + 0.12f;
        unitree_imu.linear_acceleration.y = RecvHighROS.imu.accelerometer[1] + 0.29f;
        unitree_imu.linear_acceleration.z = RecvHighROS.imu.accelerometer[2];
        */

        pub_imu_raw.publish(unitree_imu);
        //pub_imu_unitree.publish(RecvIMU);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
