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

#include "ga_slam/localization/ParticleFilter.h"

// GA SLAM
#include "ga_slam/TypeDefs.h"
#include "ga_slam/processing/CloudProcessing.h"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

// PCL
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/transforms.h>

// STL
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <mutex>
#include <limits>

namespace ga_slam {

void ParticleFilter::configure(
        int numParticles,
        double initialSigmaX, double initialSigmaY, double initialSigmaYaw,
        double predictSigmaX, double predictSigmaY, double predictSigmaYaw) {
    numParticles_ = numParticles;
    initialSigmaX_ = initialSigmaX;
    initialSigmaY_ = initialSigmaY;
    initialSigmaYaw_ = initialSigmaYaw;
    predictSigmaX_ = predictSigmaX;
    predictSigmaY_ = predictSigmaY;
    predictSigmaYaw_ = predictSigmaYaw;

    std::lock_guard<std::mutex> guard(particlesMutex_);
    particles_.clear();
    particles_.resize(numParticles_);
}

void ParticleFilter::initialize(
        double initialX,
        double initialY,
        double initialYaw) {
    std::lock_guard<std::mutex> guard(particlesMutex_);

    for (auto& particle : particles_) {
        particle.x = sampleGaussian(initialX, initialSigmaX_);
        particle.y = sampleGaussian(initialY, initialSigmaY_);
        particle.yaw = sampleGaussian(initialYaw, initialSigmaYaw_);
    }
}

void ParticleFilter::predict(
        double deltaX,
        double deltaY,
        double deltaYaw) {
    std::lock_guard<std::mutex> guard(particlesMutex_);

    double sigmaX = 0., sigmaY = 0., sigmaYaw = 0.;

    if (weightsUpdated_) {
        weightsUpdated_ = false;
        sigmaX = predictSigmaX_;
        sigmaY = predictSigmaY_;
        sigmaYaw = predictSigmaYaw_;
    }

    for (auto& particle : particles_) {
        particle.x = sampleGaussian(particle.x + deltaX, sigmaX);
        particle.y = sampleGaussian(particle.y + deltaY, sigmaY);
        particle.yaw = sampleGaussian(particle.yaw + deltaYaw, sigmaYaw);
    }
}

void ParticleFilter::update(
        const Pose& lastPose,
        const Cloud::ConstPtr& rawCloud,
        const Cloud::ConstPtr& mapCloud) {
    if (mapCloud->empty()) return;

    std::unique_lock<std::mutex> guard(particlesMutex_);
    std::vector<Particle> particlesCopy = particles_;
    guard.unlock();

    for (auto& particle : particlesCopy) {
        Cloud::Ptr particleCloud(new Cloud);
        const auto deltaPose = getDeltaPoseFromParticle(particle, lastPose);

        pcl::transformPointCloud(*mapCloud, *particleCloud, deltaPose);
        double score = CloudProcessing::matchClouds(rawCloud, particleCloud);

        if (score == 0.) score = std::numeric_limits<double>::min();
        particle.weight = 1. / score;
    }

    guard.lock();
    for (auto i = 0; i < numParticles_; i++)
        particles_[i].weight = particlesCopy[i].weight;
    guard.unlock();

    weightsUpdated_ = true;
}

void ParticleFilter::resample(void) {
    std::lock_guard<std::mutex> guard(particlesMutex_);

    std::vector<Particle> newParticles;
    std::vector<double> weights;

    std::transform(
            particles_.begin(), particles_.end(), std::back_inserter(weights),
            [] (Particle particle) -> double { return particle.weight; });

    std::discrete_distribution<> distribution(weights.begin(), weights.end());

    for (auto i = 0; i < numParticles_; i++) {
        const auto& newParticle = particles_[distribution(generator_)];
        newParticles.push_back(newParticle);
    }

    particles_ = newParticles;
}

void ParticleFilter::getEstimate(
        double& estimateX,
        double& estimateY,
        double& estimateYaw) const {
    std::lock_guard<std::mutex> guard(particlesMutex_);

    const auto bestParticle = getBestParticle();

    estimateX = bestParticle.x;
    estimateY = bestParticle.y;
    estimateYaw = bestParticle.yaw;
}

Particle ParticleFilter::getBestParticle(void) const {
    auto bestParticle = particles_[0];

    for (const auto& particle : particles_)
        if (particle.weight > bestParticle.weight) bestParticle = particle;

    return bestParticle;
}

double ParticleFilter::sampleGaussian(double mean, double sigma) {
    std::normal_distribution<> distribution(mean, sigma);

    return distribution(generator_);
}

Pose ParticleFilter::getDeltaPoseFromParticle(
        const Particle& particle,
        const Pose& pose) {
    Pose deltaPose;

    deltaPose = Eigen::Translation3d(
            particle.x - pose.translation().x(),
            particle.y - pose.translation().y(),
            0.);

    deltaPose.rotate(Eigen::AngleAxisd(
            particle.yaw - pose.linear().eulerAngles(2, 1, 0)[0],
            Eigen::Vector3d::UnitZ()));

    return deltaPose;
}

Eigen::ArrayXXd ParticleFilter::getParticlesArray(void) const {
    Eigen::ArrayXXd particlesArray(numParticles_, 4);

    std::lock_guard<std::mutex> guard(particlesMutex_);

    for (auto i = 0; i < numParticles_; ++i)
        particlesArray.row(i) << particles_[i].x, particles_[i].y,
                particles_[i].yaw, particles_[i].weight;

    return particlesArray;
}

}  // namespace ga_slam

