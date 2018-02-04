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
#include <cstdlib>

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
while(ros::ok()){

	// Assign publishing and subscribing node
	velocity_publisher = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
	pose_subscriber = nh.subscribe("/turtle1/pose", 100, poseCallback);//call poseCallback everytime the turtle pose msg is published over the /turtle1/pose topic.
	ros::Rate loop_rate(1);
	//	/turtle1/cmd_vel is the Topic name
	//	/geometry_msgs::Twist is the msg type 
	

//-----------------------------------------//
//--------------Spawn wall-----------------//
//-----------------------------------------//

	
	char count [] = { 'z', 'a', 'b', 'c', 'd', 'e', 'f','g', 'h', 'i', '\0' }; 
	
	int j;	
	cout << "Number of obstacles: " << endl;
	cin >> j;

	for(int i = 0; i < j; i++){
	int x_pose;
	int y_pose;
	cout << "Key in x-coordinate: " << endl;
	cin >> x_pose;
	cout << "Key in y-coordinate: " << endl;
	cin >> y_pose;
	
	
	ros::ServiceClient spawnClient = nh.serviceClient<turtlesim::Spawn>("spawn");
	turtlesim::Spawn::Request req;
	turtlesim::Spawn::Response resp;
	turtlesim::Spawn srv; //using Spawn.srv
 	 req.x = x_pose;
  	 req.y = y_pose;
  	 req.theta = 0;
	 req.name = count[0 + i];	
	 spawnClient.call(req,resp);
	}

/*	 
	if(success){
	ROS_INFO_STREAM("Spawned a "<<resp.name);
	} else {
	ROS_ERROR_STREAM(" Failed to spawn. ");
	;
		}
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
	

	moveGoal(goal_pose,0.02);		
	loop_rate.sleep();
	
	ros::spin();
	ros::shutdown();
	}
	
	return 0;
}

//-----------------------------------------//
//-----------END of Main code--------------//
//-----------------------------------------//



/*
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
*/
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
	turtlesim::Spawn::Request req;
	ros::Rate loop_rate(1000);
	do{
		//linear velocity 
		vel_msg.linear.x = 0.1*getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y);//0.05;
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;
		//angular velocity
		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
		vel_msg.angular.z = 0.5*(atan2(goal_pose.y - turtlesim_pose.y, goal_pose.x - turtlesim_pose.x)-turtlesim_pose.theta);
		
			if(abs(turtlesim_pose.x - req.x) < 1 || abs(turtlesim_pose.y - req.y) < 1){
			cout<<"wall detected"<<endl;
			//linear velocity 
			vel_msg.linear.x = 0.04*getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y);//0.05;
			vel_msg.linear.y = 0;
			vel_msg.linear.z = 0;
			//angular velocity
			vel_msg.angular.x = 0;
			vel_msg.angular.y = 0;
			vel_msg.angular.z = 0.9*(atan2(req.y - turtlesim_pose.y, req.x - turtlesim_pose.x) - turtlesim_pose.theta);
			}
		
		cout << "My x difference: " << abs(turtlesim_pose.x - req.x) << endl;	
		cout << "My y difference: " << abs(turtlesim_pose.y - req.y) << endl;		
		velocity_publisher.publish(vel_msg);
		
		ros::spinOnce();
		loop_rate.sleep();
		cout<<"IM here 2"<<endl;

	}while(getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y) > distance_tolerance);
	cout << "I am at (" << goal_pose.x << "," << goal_pose.y<< ")" << endl;
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

