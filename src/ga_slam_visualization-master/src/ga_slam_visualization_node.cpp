#include <fstream>

#include <Eigen/Core>

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseArray.h>
#include <nav_msgs/Path.h>
#include <eigen_conversions/eigen_msg.h>
#include <tf/transform_broadcaster.h>
#include <tf_conversions/tf_eigen.h>

#include <grid_map_core/grid_map_core.hpp>
#include <grid_map_ros/grid_map_ros.hpp>
#include <grid_map_msgs/GridMap.h>


#include "/ga_slam_cm_cereal/ga_slam_cereal/GridMapCereal.h"

int main(int argc, char **argv) {
    ros::init(argc, argv, "ga_slam_visualization");
    ros::NodeHandle nodeHandle("~");
    ros::Rate rate(1);
    ros::Publisher rawMapPublisher = nodeHandle.
            advertise<grid_map_msgs::GridMap>("raw_map", 1, true);
    ros::Publisher globalMapPublisher = nodeHandle.
            advertise<grid_map_msgs::GridMap>("global_map", 1, true);
    ros::Publisher slamPosePublisher = nodeHandle.
            advertise<geometry_msgs::PoseStamped>("slam_pose", 1, true);
    ros::Publisher globalPosePublisher = nodeHandle.
            advertise<geometry_msgs::PoseStamped>("global_pose", 1, true);
    ros::Publisher slamPosePathPublisher = nodeHandle.
            advertise<nav_msgs::Path>("slam_pose_path", 1, true);
    ros::Publisher globalPosePathPublisher = nodeHandle.
            advertise<nav_msgs::Path>("global_pose_path", 1, true);
    ros::Publisher particlesArrayPublisher = nodeHandle.
            advertise<geometry_msgs::PoseArray>("particles_array", 1, true);
    tf::TransformBroadcaster tfBroadcaster;

    const std::string mapFrameId = "map";
    const std::string bodyFrameId = "body_base";

    Eigen::Affine3d slamPose, globalPose;
    geometry_msgs::Pose poseMessage;
    geometry_msgs::PoseStamped poseStampedMessage;
    nav_msgs::Path slamPosePathMessage, globalPosePathMessage;
    tf::Transform bodyToMapTF;

    Eigen::ArrayXXd particlesArray;
    geometry_msgs::PoseArray arrayMessage;

    grid_map::GridMap rawMap, globalMap;
    grid_map_msgs::GridMap mapMessage;
    grid_map::Time lastRawMapStamp, currentRawMapStamp;
    grid_map::Time lastGlobalMapStamp, currentGlobalMapStamp;

    lastRawMapStamp = 0;
    lastGlobalMapStamp = 0;

    while (ros::ok()) {
        //loadPose(slamPose, "/tmp/ga_slam_slam_pose.cereal");
        //loadPose(globalPose, "/tmp/ga_slam_global_pose.cereal");
       // loadArray(particlesArray, "/tmp/ga_slam_particles_array.cereal");

        //loadGridMap(rawMap, "/tmp/ga_slam_local_map.cereal");
        rawMap.setFrameId(mapFrameId);
        currentRawMapStamp = rawMap.getTimestamp();

        if (currentRawMapStamp != lastRawMapStamp) {
            grid_map::GridMapRosConverter::toMessage(rawMap, mapMessage);
            rawMapPublisher.publish(mapMessage);

            tf::poseEigenToMsg(globalPose, poseMessage);
            poseStampedMessage.pose = poseMessage;
            poseStampedMessage.header.stamp.fromNSec(currentRawMapStamp);
            poseStampedMessage.header.frame_id = mapFrameId;
            globalPosePublisher.publish(poseStampedMessage);

            if (currentRawMapStamp < lastRawMapStamp)
                globalPosePathMessage.poses.clear();

            globalPosePathMessage.header.stamp.fromNSec(currentRawMapStamp);
            globalPosePathMessage.header.frame_id = mapFrameId;
            globalPosePathMessage.poses.push_back(poseStampedMessage);
            globalPosePathPublisher.publish(globalPosePathMessage);

            tf::poseEigenToMsg(slamPose, poseMessage);
            poseStampedMessage.pose = poseMessage;
            poseStampedMessage.header.stamp.fromNSec(currentRawMapStamp);
            poseStampedMessage.header.frame_id = mapFrameId;
            slamPosePublisher.publish(poseStampedMessage);

            if (currentRawMapStamp < lastRawMapStamp)
                slamPosePathMessage.poses.clear();

            slamPosePathMessage.header.stamp.fromNSec(currentRawMapStamp);
            slamPosePathMessage.header.frame_id = mapFrameId;
            slamPosePathMessage.poses.push_back(poseStampedMessage);
            slamPosePathPublisher.publish(slamPosePathMessage);

            tf::poseEigenToTF(slamPose, bodyToMapTF);
            tfBroadcaster.sendTransform(tf::StampedTransform(
                    bodyToMapTF, ros::Time::now(), mapFrameId, bodyFrameId));

            arrayMessage.header.stamp.fromNSec(currentRawMapStamp);
            arrayMessage.header.frame_id = mapFrameId;
            arrayMessage.poses.resize(particlesArray.rows());

            for (auto i = 0; i < particlesArray.rows(); ++i) {
                const auto quaternion = tf::createQuaternionFromYaw(
                        particlesArray(i, 2));
                const auto position = tf::Vector3(
                        particlesArray(i, 0),
                        particlesArray(i, 1),
                        slamPose.translation().z());
                const auto particlePose = tf::Pose(quaternion, position);
                tf::poseTFToMsg(particlePose, arrayMessage.poses[i]);
            }

            particlesArrayPublisher.publish(arrayMessage);

            lastRawMapStamp = currentRawMapStamp;
        }

       loadGridMap(globalMap, "/tmp/ga_slam_global_map.cereal");
        globalMap.setFrameId(mapFrameId);
        currentGlobalMapStamp = globalMap.getTimestamp();

        if (currentGlobalMapStamp != lastGlobalMapStamp) {
            grid_map::GridMapRosConverter::toMessage(globalMap, mapMessage);
            globalMapPublisher.publish(mapMessage);

            lastGlobalMapStamp = currentGlobalMapStamp;
        }

        rate.sleep();
    }

    return 0;
}

