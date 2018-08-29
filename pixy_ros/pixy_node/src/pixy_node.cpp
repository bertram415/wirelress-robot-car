/*
 * pixy_node is part of the pixy_ros package for interfacing with
 * a CMUcam5 pixy with ROS.
 * Copyright (C) 2014 Justin Eskesen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
 
//#include <iostream>
//#include <string>
#include <ros/ros.h>
//#include <angles/angles.h>
//#include <pixy_msgs/PixyData.h>
#include <sstream>
#include "std_msgs/String.h"
#include "pixy.h"
//#include <tf/transform_broadcaster.h>
//#include <pixy_msgs/PixyBlock.h>
//#include <pixy_msgs/Servo.h>

using namespace std;

#define BLOCK_BUFFER_SIZE 100

class PixyNode
{
public:
	PixyNode();

	void spin();

private:
	void update();
	//void setServo(const pixy_msgs::Servo& msg) {pixy_rcs_set_position(msg.channel, msg.position);}

	ros::NodeHandle node_handle_;
	ros::NodeHandle private_node_handle_;
	
	ros::Rate rate_;
	/**tf::TransformBroadcaster tf_broadcaster_;
	//bool use_servos_;
	//ros::Publisher publisher_;
	//ros::Subscriber servo_subscriber_; */
	
	ros::Publisher object_;
	/**ros::Publisher chatter_pub;
	//ros::Subscriber chatter_;*/
	
	//std::string frame_id;

};


PixyNode::PixyNode() :
		node_handle_(),
		private_node_handle_("~"),
		rate_(1.0)
{

	//private_node_handle_.param<std::string>(std::string("frame_id"), frame_id,
	//		std::string("pixy_frame"));

	double rate;
	
	private_node_handle_.param("rate", rate, 10.0);
	rate_=ros::Rate(rate);

	int ret = pixy_init();
	if (ret != 0)
	{
		ROS_FATAL("PixyNode - %s - Failed to open with the USB error %d!",
				__FUNCTION__, ret);
		ROS_BREAK();
	}

	//object_ = node_handle_.advertise<pixy_msgs::PixyData>("object", 2.0);  // Publish object status to /object topic
	object_ = node_handle_.advertise<std_msgs::String>("object", 2.0);
}



void PixyNode::update()
{
	// Pixy Block buffer //
	struct Block blocks[BLOCK_BUFFER_SIZE];

	// Get blocks from Pixy //
	int blocks_copied = pixy_get_blocks(BLOCK_BUFFER_SIZE, blocks);

	//pixy_msgs::PixyData data;
	
    std_msgs::String msg;   
    std::stringstream ss;

	if (blocks_copied > 0)
	{
		ss << "true";
		msg.data = ss.str();	
		object_.publish(msg); // publish object found as true
	}
	else if (blocks_copied < 0)
	{
		ROS_INFO("Pixy read error.");
		return;
	}
	else //if (blocks_copied <= 0)
	{
		ss << "false";
		msg.data = ss.str();	
		object_.publish(msg); // publish object found as false
	}

}


void PixyNode::spin()
{

	while (node_handle_.ok())
	{
		update();

		ros::spinOnce();
		rate_.sleep();
	}
}

int main(int argc, char** argv)
{
		
	ros::init(argc, argv, "pixy_node");
	//ROS_INFO("PixyNode for ROS");

	PixyNode myPixy;
	myPixy.spin();

	return (0);
}

// EOF
