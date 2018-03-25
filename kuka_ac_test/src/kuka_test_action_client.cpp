// example_trajectory_action_client: 
// see complementary server, "example_trajectory_action_server"
// this simple node populates a trajectory message and sends it to the trajectory action server for execution
// Run this together with minimal robot; start-up minimal robot with:   roslaunch minimal_robot_description minimal_robot.launch 



#include<ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <control_msgs/FollowJointTrajectoryAction.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <trajectory_msgs/JointTrajectoryPoint.h>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>

int g_done_count = 0;

// This function will be called once when the goal completes
// this is optional, but it is a convenient way to get access to the "result" message sent by the server
void doneCb(const actionlib::SimpleClientGoalState& state,
        const control_msgs::FollowJointTrajectoryResultConstPtr& result) {
    ROS_INFO(" doneCb: server responded with state [%s]", state.toString().c_str());
    g_done_count++;
}

int main(int argc, char** argv) {
        ros::init(argc, argv, "demo_trajectory_client_node"); // name this node 
        ros::NodeHandle nh; //standard ros node handle        

        control_msgs::FollowJointTrajectoryGoal robot_goal;
        //instantiate a goal message compatible with robot action server
        trajectory_msgs::JointTrajectory des_trajectory;
    //instantiate an action client of the robot-arm motion  action server:
    actionlib::SimpleActionClient<control_msgs::FollowJointTrajectoryAction>
            robot_motion_action_client("/ariac/arm/follow_joint_trajectory", true);

    // attempt to connect to the server:
    ROS_INFO("waiting for arm server: ");
    bool server_exists = robot_motion_action_client.waitForServer(ros::Duration(1.0));
    while (!server_exists) {
        ROS_WARN("waiting on arm server...");
        ros::spinOnce();
        ros::Duration(1.0).sleep();
        server_exists = robot_motion_action_client.waitForServer(ros::Duration(1.0));
    }
    ROS_INFO("connected to arm action server"); // if here, then we connected to the server;  

     //populate a goal message
        Eigen::VectorXd q_goal;
        q_goal.resize(8);
        q_goal<<0,0,0,0,0,0,0,0.5;

        trajectory_msgs::JointTrajectoryPoint trajectory_point1;
        trajectory_point1.positions.clear();
        trajectory_point1.positions.resize(8);
        
        des_trajectory.points.clear(); // can clear components, but not entire trajectory_msgs
        des_trajectory.joint_names.clear();
    //[iiwa_joint_1, iiwa_joint_2, iiwa_joint_3, iiwa_joint_4, iiwa_joint_5, iiwa_joint_6,
    //iiwa_joint_7, linear_arm_actuator_joint
    des_trajectory.joint_names.push_back("iiwa_joint_1");
    des_trajectory.joint_names.push_back("iiwa_joint_2");
    des_trajectory.joint_names.push_back("iiwa_joint_3");
    des_trajectory.joint_names.push_back("iiwa_joint_4");
    des_trajectory.joint_names.push_back("iiwa_joint_5");
    des_trajectory.joint_names.push_back("iiwa_joint_6");
    des_trajectory.joint_names.push_back("iiwa_joint_7");
    des_trajectory.joint_names.push_back("linear_arm_actuator_joint");


    for (int i = 0; i < 8; i++) { //copy over the joint-command values
        trajectory_point1.positions[i] = q_goal[i];
    }

    //set arrival time:
    trajectory_point1.time_from_start = ros::Duration(1.0);
    des_trajectory.points.push_back(trajectory_point1);

    //put traj in goal message
    robot_goal.trajectory = des_trajectory;


        
    ROS_INFO("sending goal to arm: ");
    robot_motion_action_client.sendGoal(robot_goal, &doneCb);

    while (g_done_count < 1) {
        ROS_INFO("waiting to finish pre-pose..");
        ros::Duration(1.0).sleep();
    }
    ROS_INFO("done w/ test");

    return 0;
}
