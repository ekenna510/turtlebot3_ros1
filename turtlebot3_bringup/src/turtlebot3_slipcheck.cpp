/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: ed kenna

need to check angular velocity to see if it is 0

*/

#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Twist.h>
#include <string>
#include <std_msgs/Float64.h>
#include <turtlebot3_msgs/Sound.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

double cmdlinear=0, cmdangular=0;
//actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> move_base_ac;
//actionlib_msgs::GoalID cancelgoal;
bool Moving=false;

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
MoveBaseClient ac("move_base", true);

ros::Publisher sound_pub;
turtlebot3_msgs::Sound sound_;
//ros::Publisher tb3_version_info_pub;
//ros::Publisher tb3_diagnostics_pub;

//diagnostic_msgs::DiagnosticStatus imu_state;
//diagnostic_msgs::DiagnosticStatus motor_state;
//diagnostic_msgs::DiagnosticStatus LDS_state;
//diagnostic_msgs::DiagnosticStatus battery_state;




void imuMsgCallback(const sensor_msgs::Imu::ConstPtr &msg)
{
double ox,oy,oz,ow,ax,ay,az,lx,ly,lz;

  ox = msg->orientation.x;
  oy = msg->orientation.y;
  oz = msg->orientation.z;
  ow=msg->orientation.w;

  ax=msg->angular_velocity.x;
  ay=msg->angular_velocity.y;
  az=msg->angular_velocity.z;

  lx=msg->linear_acceleration.x;
  ly=msg->linear_acceleration.y;
  lz=msg->linear_acceleration.z;
  if (cmdlinear != 0.0 || cmdangular != 0.0 )
  {
 
  if (ax  == 0.0 && ay == 0.0 && az == 0.0)
  {
  ROS_INFO("imuMsgCallback: %lf %lf %lf %lf %lf %lf %lf %lf %lf",ox,oy,oz,ax,ay,az,lx,ly,lz);
  ROS_INFO("should be moving but we are not.");
  sound_pub.publish(sound_);
  // Cancel all active goals
  ac.cancelAllGoals(); 
  }
  
  }
  
 

 
  

  
  //setIMUDiagnosis(diagnostic_msgs::DiagnosticStatus::OK, "Good Condition");
}
void twistMsgCallback(const geometry_msgs::Twist::ConstPtr &msg)
{
  double x,w;
  x=msg->linear.x;
  w=msg->angular.z;
  if (cmdlinear != x)
    {
    cmdlinear = x;
    ROS_INFO("Updated cmdlinear %lf %lf",x,w );
    }
  if (cmdangular != w)
    {
    cmdangular = w;
    ROS_INFO("Updated cmdangular %lf %lf",x,w  );
    }
}



int main(int argc, char **argv)
{
  ros::init(argc, argv, "turtlebot3_slipcheck");
  ros::NodeHandle nh;

    sound_pub = nh.advertise<turtlebot3_msgs::Sound>("sound",5,true);
    sound_.value = 3;

  //tb3_diagnostics_pub  = nh.advertise<diagnostic_msgs::DiagnosticArray>("diagnostics", 10);
  //tb3_version_info_pub = nh.advertise<turtlebot3_msgs::VersionInfo>("version_info", 10);

  ros::Subscriber imu         = nh.subscribe("imu", 10, imuMsgCallback);
  ros::Subscriber twist       = nh.subscribe("cmd_vel",10, twistMsgCallback);

  ros::Rate loop_rate(1);

  while (ros::ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}
