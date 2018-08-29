
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

class RadioChannel
{
public:
	int car_flag, obstacle, obstacle_counter;
	bool position1, position2;
	bool object, car1_availability, car2_availability, car1_problem, car2_problem;	

	RadioChannel()
	{
		/** Topic publisher  */
		chatter_pub_ = n_.advertise<std_msgs::String>("chatter", 5.0);
		servo_pub_ = n_.advertise<std_msgs::String>("servo", 1.0);
		position_pub_ = n_.advertise<std_msgs::String>("position", 1.0);
		schedule_pub_ = n_.advertise<std_msgs::String>("/recognizer/output", 10.0);
		/** Topic subscriber */
		chatter_sub_ = n_.subscribe("chatter", 100, &RadioChannel::chatterCallback, this);	
		position_sub_ = n_.subscribe("position", 500, &RadioChannel::positionCallback, this);
		object_sub_ = n_.subscribe("object", 100, &RadioChannel::objectCallback, this);
	}

	std::string data;
	void spin();	
    
	void objectCallback(const std_msgs::String::ConstPtr& omsg)
	{
		
		this->data = omsg->data.c_str();	
		if (this->data == "true")
			object = true;
		else 
			object = false;				
	}
	
	void positionCallback(const std_msgs::String::ConstPtr& pmsg)
	{
		int option;
		this->data = pmsg->data.c_str();
		position1 == false;
		position2 == false; 
		
		if (this->data == "Car 1 is in position.")
			option = 1;
		else if (this->data == "Car 1 is back to origin.")
			option = 2;			
		else if (this->data == "Car 2 is in position.")
			option = 3;
		else if (this->data == "Car 2 is back to origin.")
			option = 4;
		    
		switch(option)
		{
			case 1:
				position1 = true;
				//ROS_INFO("Position 1 true");
				break;
			case 2:
				position1 = false;	
				//ROS_INFO("Position 1 @ origin");	
				break;	
			case 3:
				position2 = true;
				//ROS_INFO("Position 2 false");	
				break;							
			case 4:
				position2 = false;
				//ROS_INFO("Position 2 @ origin");	
				break;							
			default:
				//position1 = false;
				//position2 = false;
				car1_availability = true;
				car2_availability = true;
				//ROS_INFO("Position 1 false");				
				//ROS_INFO("Position 2 false");
				break;
		}		
	}
	
	void chatterCallback(const std_msgs::String::ConstPtr& cmsg)
	{
    
		this->data = cmsg->data.c_str();	
			
		int case_a = 0;
		int case_b = 0;
		//int count = 0;		
		car1_problem = false; 
		car2_problem = false;
		
		// Filter which car is problematic
		if (this->data == "1:ERROR")
		{
			car1_problem = true;
		}
		else if (this->data == "2:ERROR")
		{
			car2_problem = true;
		}		
		
		// Use 'obstacle' variable increment to delay a certain time before car react
		// Car 1 
		if ((car1_problem == true))
		{
			if ((obstacle_counter == 25))// && (car2_problem == true))
			{	
				obstacle = 1;
				car1_availability = false;
				obstacle_counter++;
			}				
			else if (obstacle_counter >= 150)
			{
				obstacle = 2;
				car1_availability = false;
				obstacle_counter++;
			}

			else 
				obstacle_counter++;
		}		
		// Car 2
		else if ((car2_problem == true))
		{			
			if ((obstacle_counter == 25))// && (car2_problem == true))
			{	
				obstacle = 3;
				car2_availability = false;
				obstacle_counter++;				
			}				
			else if (obstacle_counter >= 150)
			{
				obstacle = 4;
				car2_availability = false;
				obstacle_counter++;				
			}

			else 
				obstacle_counter++;
		}
		// 
		/*if (car1_problem == true)
		{
			case_a = 1;			
			count = 1;
			if (car2_problem == true)
			{
				case_b = 1;
				count = 2;
			}
			else
			{
				case_b = 0;
				count = 1;
			}
		}
		else 	
			case_a = 0;		
		if (car2_problem == true)
		{
			case_b = 1;			
			count = 3;
			if (car1_problem == true)
			{
				case_a = 1;
				count = 4;
			}
			else
			{
				case_a = 0;
				count = 3;
			}
		}		
		else 	
			case_b = 0;	*/	

		//printf("case_a = %d\n", case_a);
		//printf("case_b = %d\n", case_b);
		//printf("car_flag = %d\n", car_flag);  
		//printf("count = %d\n", count); 
		/*		
		switch(count)
		{
			case 0:  /// 00
				car_flag = 0;
				car1_availability = true;
				car2_availability = true;
				break;
			case 1:  ///01
				car_flag = 1;
				car1_availability = false;
				//car2_availability = true;
				//ROS_INFO("Car1 false.\n");
				break;
			case 2:  ///02
				car_flag = 2;
				car1_availability = false;
				car2_availability = false;
				break;
			case 3:  ///03
				car_flag = 3;
				//car1 = true;
				car2 = false;
				break;
			case 4:  ///04
				car_flag = 3;
				car1 = false;
				car2 = false;
				break;				
			default: 
				car_flag = 0;
				//case_a = 0;
				//case_b = 0;
				car1 = true;
				car2 = true;
				break;
		}*/
		
		

		printf("car1 problem = %s\n", car1_problem ? "true" : "false");		
		printf("car2 problem = %s\n", car2_problem ? "true" : "false");	
		printf("car1 = %s\n", car1_availability ? "true" : "false");		
		printf("car2 = %s\n", car2_availability ? "true" : "false");			
		printf("Obstacle = %d\n", obstacle);		
		printf("Obstacle_counter = %d\n", obstacle_counter);		
		printf("\n");				
	}  	

private:
	void update();
	ros::NodeHandle n_; 
	
	ros::Publisher chatter_pub_;
	ros::Publisher servo_pub_;
	ros::Publisher position_pub_;
	ros::Publisher schedule_pub_;

	ros::Subscriber chatter_sub_;
	ros::Subscriber position_sub_;
	ros::Subscriber object_sub_;
};//End of RadioChannel class

void RadioChannel::update()
{
	//int place = 0;
	int servo_angle;
	int servo_case;
	//int obstacle = 0;
	std_msgs::String msg;  
	std_msgs::String cmsg;
	std_msgs::String pmsg;

    std::stringstream servo1;
    std::stringstream servo2;
    std::stringstream car_state;
    std::stringstream pos1;
    std::stringstream pos2;

	std_msgs::String task_to_voice;	
	std::stringstream scheduled_car;		
	
	//obstacle_counter = 0;
	//obstacle = 0;
	if ((obstacle_counter > 0) && (car1_problem == false))
	{
		car1_availability = true;
	}
	if ((obstacle_counter > 0) && (car2_problem == false))
	{
		car2_availability = true;
	}

	if ((obstacle_counter > 1000) && (car1_problem == false))
	{
		obstacle_counter = 0;					
	}
	
	if ((obstacle_counter > 1000) && (car2_problem == false))
	{
		obstacle_counter = 0;					
	}
	
	
    if (position1 == true)
	{
		if (object == true)
		{	
			//if (car1_availability == true)	
				servo_case = 1;
		}
	}
	else if (position2 == true)
	{
		if (object == true)
		{	
			//if (car2_availability == true)
				servo_case = 2;
		}
	}	
	else if (position1 == false)
		servo_case = 3;
	else if (position2 == false)
		servo_case = 4;
		
	printf("Servo_case = %d\n", servo_case);	
	
	switch (servo_case)
	{
		case 1:
			servo1 << "1:60";
			msg.data = servo1.str();
			servo_pub_.publish(msg);
			//printf("servo1\n");
			break;
		case 2:
			servo2 << "2:60";
			msg.data = servo2.str();
			servo_pub_.publish(msg);
			//printf("servo2\n");					
			break;
	    /*case 3:
			servo1 << "1:30";
			msg.data = servo1.str();
			servo_pub_.publish(msg);				
			break;
		case 4:
			servo2 << "2:30";
			msg.data = servo2.str();
			servo_pub_.publish(msg);				
			break;*/
		default:
			break;
	}	
	
	/*if (position1 == true && object == true)
	{
		servo1 << "1:60";
		msg.data = servo1.str();
		servo_pub_.publish(msg);
	}
	else if (position2 == true && object == true)
	{
		servo2 << "2:60";
	    msg.data = servo2.str();
		servo_pub_.publish(msg);
	}	
	else if (position1 == false)
	{
		servo1 << "1:30";
	    msg.data = servo1.str();
		servo_pub_.publish(msg);
	}	
	else if (position2 == false)
	{
		servo2 << "2:30";
		msg.data = servo2.str();
		servo_pub_.publish(msg);
	}	*/
	
	/**if (object == true)
	{
		ROS_INFO("Cargo has arrived, arranging robot tasks...");
	}
	else if (object == false) 
	{
		ROS_INFO("Waiting for cargo to arrive.");
	}**/
	
	
	/*switch (car_flag)
	{
		case 0:
			car_state << "Waiting for car to report status. Flag = " << car_flag;
			cmsg.data = car_state.str();		
			chatter_pub_.publish(cmsg);
			car_flag = 0;		
			break;
			
		case 1:
			car_state << "Car 1 error. Flag = " << car_flag;
			cmsg.data = car_state.str();
			chatter_pub_.publish(cmsg);

			break;
			
		case 2:
			car_state << "Both cars fail, maintenance required. Flag = " << car_flag;
			cmsg.data = car_state.str();		
			chatter_pub_.publish(cmsg);
			
			break;
			
		case 3:
			car_state << "Car 2 error. Flag = " << car_flag;
			cmsg.data = car_state.str();		
			chatter_pub_.publish(cmsg);
			
			break;
			
		default:
			car_state << "Waiting for car to report status. Flag = " << car_flag;
			cmsg.data = car_state.str();		
			chatter_pub_.publish(cmsg);
			car_flag = 0;		
			break;		
	}

	if (position1 == true)
	{	
		pos1 << "Robot (Car 1) has arrived.";
		pmsg.data = pos1.str();
		position_pub_.publish(pmsg); 
	}
	if (position2 == true)
	{	
		pos2 << "Robot (Car 2) has arrived.";
		pmsg.data = pos2.str();
		position_pub_.publish(pmsg); 
	}	
	*/
	
	switch (obstacle)
	{
		case 1:
			scheduled_car << "2:work";
			task_to_voice.data = scheduled_car.str();
			schedule_pub_.publish(task_to_voice);			
			break;
		case 2:
			ROS_INFO("PLEASE ARRANGE PERSONNEL TO RESOLVE CAR 1 PROBLEM.");
			break;
		case 3:
			scheduled_car << "1:work";
			task_to_voice.data = scheduled_car.str();
			schedule_pub_.publish(task_to_voice);		
			break;
		case 4:
			ROS_INFO("PLEASE ARRANGE PERSONNEL TO RESOLVE CAR 2 PROBLEM.");		
			break;
		default:
			break;
	}
	/*if (car1_availability == false && car2_availability == true)
	{
		scheduled_car << "2:work";
		task_to_voice.data = scheduled_car.str();
		schedule_pub_.publish(task_to_voice);
	}
	else if (car2 == false && car1 == true)
	{
		scheduled_car << "1:work";
		task_to_voice.data = scheduled_car.str();
		schedule_pub_.publish(task_to_voice);				
	}*/					
	//printf("Position 1 = %s\n", position1 ? "true" : "false");
	//printf("Position 2 = %s\n", position2 ? "true" : "false");
	printf("Car 1 available = %s\n", car1_availability ? "true" : "false");
	printf("Car 2 available = %s\n", car2_availability ? "true" : "false");
	printf("Obstacle = %d\n", obstacle);
	printf("\n");
	//printf("Servo_case = %d\n", servo_case);
	//printf("Object = %s\n", object ? "true" : "false");  	


}

void RadioChannel::spin()
{

	while (n_.ok())
	{
		ros::Rate rate_(2.0);
		update();

		ros::spinOnce();
		rate_.sleep();
	}
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "CarRadio");  

  RadioChannel carRadio;
  carRadio.obstacle = 0;
  carRadio.obstacle_counter = 0;
  carRadio.position1 = false; 
  carRadio.position2 = false; 
  carRadio.object = false;    
  carRadio.car1_availability = true;
  carRadio.car2_availability = true;    
  //int count = 0;
  carRadio.spin();

  return 0;
}
