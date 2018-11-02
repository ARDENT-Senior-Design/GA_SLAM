/*
 * This file is part of GA SLAM.
 * Copyright (C) 2018 Dimitris Geromichalos,
 * Planetary Robotics Lab (PRL), European Space Agency (ESA)
 *
 * GA SLAM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GA SLAM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GA SLAM. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// GA SLAM
#include "ga_slam/TypeDefs.h"
#include "ga_slam/localization/ParticleFilter.h"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

// PCL
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

// STL
#include <vector>
#include <mutex>
#include <atomic>

namespace ga_slam {

/** Module responsible for maintaining the robot's 6-DoF pose in space.
  * The asynchronous inputs, delta pose from odometry and point clouds from
  * sensors, are handled and passed to the particle filter.
  */
class PoseEstimation {
  public:
    /// Instantiates the particle filter
    PoseEstimation(void)
            : pose_(Pose::Identity()),
              resampleCounter_(0),
              particleFilter_() {}

    /// Delete the default copy/move constructors and operators
    PoseEstimation(const PoseEstimation&) = delete;
    PoseEstimation& operator=(const PoseEstimation&) = delete;
    PoseEstimation(PoseEstimation&&) = delete;
    PoseEstimation& operator=(PoseEstimation&&) = delete;

    /// Returns the robot's estimated pose in the ground frame
    Pose getPose(void) const {
        std::lock_guard<std::mutex> guard(poseMutex_);
        return pose_;
    }

    /// Returns the array with the particles from the particle filter
    Eigen::ArrayXXd getParticlesArray(void) const {
        return particleFilter_.getParticlesArray(); }

    /// Returns the mutex protecting the pose
    std::mutex& getPoseMutex(void) { return poseMutex_; }

    /** Configures the particle filter by passing the parameters
      * @param[in] numParticles number of particles used
      * @param[in] resampleFrequency number of iterations of the particle filter
      *            before resampling
      * @param[in] initialSigmaX gaussian sigma of x for particle initialization
      * @param[in] initialSigmaY gaussian sigma of y for particle initialization
      * @param[in] initialSigmaYaw gaussian sigma of yaw for particle
      *            initialization
      * @param[in] predictSigmaX gaussian sigma of x for particle prediction
      * @param[in] predictSigmaY gaussian sigma of y for particle prediction
      * @param[in] predictSigmaYaw gaussian sigma of yaw for particle prediction
      */
    void configure(int numParticles, int resampleFrequency,
            double initialSigmaX, double initialSigmaY, double initialSigmaYaw,
            double predictSigmaX, double predictSigmaY, double predictSigmaYaw);

    /** Passes the input delta pose to the particle filter to predict the
      * particles' state and then constructs the new estimate
      * @param[in] deltaPose the input delta pose used for prediction
      */
    void predictPose(const Pose& deltaPose);

    /** Calls the update and resample steps of the particle filter
      * @param[in] rawCloud the raw point cloud used in the update step
      * @param[in] mapCloud the map point cloud used in the update step
      */
    void filterPose(
            const Cloud::ConstPtr& rawCloud,
            const Cloud::ConstPtr& mapCloud);

    /** Fuses the IMU pose with the current pose estimate by copying the
      * roll and pitch values as they are and performing gaussian fusion
      * to update the yaw value
      * @param[in] imuOrientation the input orientation as received from the IMU
      */
    void fuseImuOrientation(const Pose& imuOrientation);

  protected:
    /** Creates a pose given the translation and angle vectors
      * @param[in] translation the translation of the pose
      * @param[in] angles the euler angles of the pose
      * @return the created pose
      */
    static Pose createPose(
            const Eigen::Vector3d& translation,
            const Eigen::Vector3d& angles);

    /** Calculates the euler angles from a pose
      * @param[in] pose the input pose
      * @return the euler angles of the pose
      */
    static Eigen::Vector3d getAnglesFromPose(const Pose& pose);

  protected:
    /// Robot's 6-DoF pose in the continuous space
    Pose pose_;

    /// Mutex protecting the pose
    mutable std::mutex poseMutex_;

    /// Number of iterations of the particle filter without having resampled
    std::atomic<int> resampleCounter_;

    /// Number of iterations of the particle filter before resampling
    int resampleFrequency_;

    /// Instance of the particle filter
    ParticleFilter particleFilter_;
};

}  // namespace ga_slam

