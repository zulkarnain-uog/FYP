//============================================================================
// Name        : movebot.cpp
// Author      : Nain
// Version     : 
// Copyright   : 
// Description : moving and spawning robot
//============================================================================

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include <sstream>
#include <turtlesim/Spawn.h>

using namespace std;

ros::Publisher velocity_publisher;	//Publishing velocity topic
ros::Subscriber pose_subscriber;	//Subcribe to pose from turtlesim
turtlesim::Pose turtlesim_pose;

const double PI = 3.14159265359;

void move(double speed, double distance, bool isForward);
void rotate(double angular_speed, double angle, bool cloclwise); //rotate from header direction
double degrees2radians(double angle_in_degrees);		
double setDesiredOrientation(double desired_angle_radians);	//Rotate the turtle at an absolute angle, whatever its current angle is
void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);	//Callback fn everytime the turtle pose msg is published over the /turtle1/pose topic.
void moveGoal(turtlesim::Pose goal_pose, double distance_tolerance);	//this will move robot to goal
double getDistance(double x1, double y1, double x2, double y2);

//-----------------------------------------//
//--------------Main code------------------//
//-----------------------------------------//

int main(int argc, char **argv) {
	
	// Initiate new ROS node named "talker"
	ros::init(argc, argv, "turtlesim_cleaner");
	ros::NodeHandle nh;

	// Initialize parameters
	double speed, angular_speed;
	double distance, angle;
	bool isForward, clockwise;

	// Assign publishing and subscribing node
	velocity_publisher = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
	pose_subscriber = nh.subscribe("/turtle1/pose", 100, poseCallback);//call poseCallback everytime the turtle pose msg is published over the /turtle1/pose topic.
	ros::Rate loop_rate(0.5);
	//	/turtle1/cmd_vel is the Topic name
	//	/geometry_msgs::Twist is the msg type 
	

//-----------------------------------------//
//--------------Spawn wall-----------------//
//-----------------------------------------//

/*	 ros::ServiceClient spawnClient = nh.serviceClient<turtlesim::Spawn>("spawn") ;
	 turtlesim::Spawn::Request req ;
	 turtlesim::Spawn::Response resp ;

	//Spawn turtle
	 turtlesim::Spawn srv; //using Spawn.srv
 	 srv.request.x = 2.0;
  	 srv.request.y = 2.0;
  	 srv.request.theta = 0;
	 srv.request.name="wall1.";	
	 bool success = spawnClient.call(req,resp);

	if(success){
	ROS_INFO_STREAM("Spawned a turtle named "<<resp.name);
	} else {
	ROS_ERROR_STREAM(" Failed to spawn. ");
	
		}
*/

//-----------------------------------------//
//--------------START TEST-----------------//
//-----------------------------------------//

	ROS_INFO("\n\n\n ********START TESTING*********\n");

	turtlesim::Pose goal_pose;
	cout<<"Enter desired location (x direction): ";
	cin>>goal_pose.x;
	cout<<"Enter desired location (y direction): ";
	cin>>goal_pose.y;
	goal_pose.theta = 0;
	cout<<"I'm here 1"<<endl;
	
	if(turtlesim_pose.x == 2 && turtlesim_pose.y == 2){
	
	geometry_msgs::Twist vel_msg;
	   
	//set a random linear velocity in the x-axis
	vel_msg.linear.x = 0;
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;
	//set a random angular velocity in the y-axis
	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;
	vel_msg.angular.z = 2;
	
	loop_rate.sleep();
		
	} else {
	
		moveGoal(goal_pose,0.02);
		loop_rate.sleep();
	
		ros::spin();
		ros::shutdown();
		return 0;
		}

	moveGoal(goal_pose,0.02);		
	loop_rate.sleep();
	
	ros::spin();
	ros::shutdown();
	return 0;
}

//-----------------------------------------//
//-----------END of Main code--------------//
//-----------------------------------------//




//-----------------------------------------//
//-------------Moving object---------------//
//-----------------------------------------//
void move(double speed, double distance, bool isForward){
	
	geometry_msgs::Twist vel_msg;	//calling message type
	
	//set a random linear velocity in the x-axis   
	if (isForward)
	   vel_msg.linear.x =abs(speed);
   	else
	   vel_msg.linear.x =-abs(speed);

   	vel_msg.linear.y = 0;
   	vel_msg.linear.z = 0;
   	//set a random angular velocity in the y-axis
   	vel_msg.angular.x = 0;
   	vel_msg.angular.y = 0;
 	vel_msg.angular.z = 0;
	
	 double t0 = ros::Time::now().toSec();
  	 double current_distance = 0.0;
  	 ros::Rate loop_rate(100);

	do{
	   velocity_publisher.publish(vel_msg);
	   double t1 = ros::Time::now().toSec();
	   current_distance = speed * (t1-t0);

	   ros::spinOnce();

	   loop_rate.sleep();

	   
   		}while(current_distance < distance);
   			vel_msg.linear.x = 0;
   			velocity_publisher.publish(vel_msg);
			

}

//-----------------------------------------//
//-----------Rotating object---------------//
//-----------------------------------------//

void rotate (double angular_speed, double relative_angle, bool clockwise){

	geometry_msgs::Twist vel_msg;
	   
	//set a random linear velocity in the x-axis
	vel_msg.linear.x = 0;
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;
	//set a random angular velocity in the y-axis
	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;
	if (clockwise)
	   vel_msg.angular.z =-abs(angular_speed);
	   
	else
	   vel_msg.angular.z =abs(angular_speed);

	double t0 = ros::Time::now().toSec();
	double current_angle = 0.0;

	ros::Rate loop_rate(100);


	do{
	velocity_publisher.publish(vel_msg);
	double t1 = ros::Time::now().toSec();
	current_angle = angular_speed * (t1-t0);

	ros::spinOnce();

	loop_rate.sleep();
		   
	   }while(current_angle < relative_angle);
	   vel_msg.angular.z =0;
	   velocity_publisher.publish(vel_msg);
}

//-----------------------------------------//
//------------Deg to Radians---------------//
//-----------------------------------------//

double degrees2radians(double angle_in_degrees){
	return angle_in_degrees *PI /180.0;
}

//-----------------------------------------//
//---------Desired orientation-------------//
//-----------------------------------------//

double setDesiredOrientation(double desired_angle_radians)
{	
	double relative_angle_radians = desired_angle_radians - turtlesim_pose.theta;
	//if we want to turn at a perticular orientation, we subtract the current orientation from it
	bool clockwise = ((relative_angle_radians<0)?true:false);
	//cout<<desired_angle_radians <<","<<turtlesim_pose.theta<<","<<relative_angle_radians<<","<<clockwise<<endl;
	rotate (abs(relative_angle_radians), abs(relative_angle_radians), clockwise);
}

//-----------------------------------------//
//------------Update position--------------//
//-----------------------------------------//

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message){
	int i=0;	
	turtlesim_pose.x=pose_message->x;
	turtlesim_pose.y=pose_message->y;
	turtlesim_pose.theta=pose_message->theta;
	if(turtlesim_pose.x < 0 || turtlesim_pose.x > 11.0 || turtlesim_pose.y < 0 || turtlesim_pose.y > 11.0){					//hitting the wall
		if(i<5){
			cout<<"You are hitting the wall!"<<endl;
			i++;
		}
	cout<<"Shutting down"<<endl;	
	}	
	
}

//-----------------------------------------//
//-------------Move to goal----------------//
//-----------------------------------------//

void moveGoal(turtlesim::Pose goal_pose, double distance_tolerance){
	//We implement a Proportional Controller. We need to go from (x,y) to (x',y'). Then, linear velocity v' = K ((x'-x)^2 + (y'-y)^2)^0.5 where K is the constant and ((x'-x)^2 + (y'-y)^2)^0.5 is the Euclidian distance. The steering angle theta = tan^-1(y'-y)/(x'-x) is the angle between these 2 points.
	geometry_msgs::Twist vel_msg;

	ros::Rate loop_rate(100);
	do{
		//linear velocity 
		vel_msg.linear.x = 1.4*getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y);
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;
		//angular velocity
		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
		vel_msg.angular.z = 3*(atan2(goal_pose.y - turtlesim_pose.y, goal_pose.x - turtlesim_pose.x)-turtlesim_pose.theta);

		velocity_publisher.publish(vel_msg);

		ros::spinOnce();
		loop_rate.sleep();
		cout<<"IM here 2"<<endl;

	}while(getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y)>distance_tolerance);
	cout<<"Reached goal"<<endl;
	vel_msg.linear.x = 0;
	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);
	
}

//-----------------------------------------//
//-----Distance between point and goal-----//
//-----------------------------------------//

double getDistance(double x1, double y1, double x2, double y2){
	return sqrt(pow((x2-x1),2) + pow((y2-y1),2));
}


