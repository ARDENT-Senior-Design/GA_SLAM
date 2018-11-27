#pragma once

#include "ga_slam_cereal/EigenCereal.h"

#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/unordered_map.hpp"

#include "grid_map_core/GridMap.hpp"

void savePose(const Eigen::Affine3d& pose, const std::string& filename);

void loadPose(Eigen::Affine3d& pose, const std::string& filename);

void saveArray(const Eigen::ArrayXXd& array, const std::string& filename);

void loadArray(Eigen::ArrayXXd& array, const std::string& filename);

void saveGridMap(const grid_map::GridMap& map, const std::string& filename);

void loadGridMap(grid_map::GridMap& map, const std::string& filename);

namespace cereal {

template <class Archive>
void save(Archive& archive, const grid_map::GridMap& map) {
    std::string frameId = map.getFrameId();
    grid_map::Time timestamp = map.getTimestamp();
    std::vector<std::string> layers = map.getLayers();
    std::vector<std::string> basicLayers = map.getBasicLayers();
    grid_map::Length length = map.getLength();
    double resolution = map.getResolution();
    grid_map::Position position = map.getPosition();
    grid_map::Index startIndex = map.getStartIndex();

    std::unordered_map<std::string, grid_map::Matrix> data;
    for (const auto& layer : layers)
        data[layer] = map.get(layer);

    archive(frameId);
    archive(timestamp);
    archive(data);
    archive(layers);
    archive(basicLayers);
    archive(resolution);
    archive(length);
    archive(position);
    archive(startIndex);
}

template <class Archive>
void load(Archive& archive, grid_map::GridMap& map) {
    std::string frameId;
    grid_map::Time timestamp;
    std::unordered_map<std::string, grid_map::Matrix> data;
    std::vector<std::string> layers;
    std::vector<std::string> basicLayers;
    grid_map::Length length;
    double resolution;
    grid_map::Position position;
    grid_map::Index startIndex;

    archive(frameId);
    archive(timestamp);
    archive(data);
    archive(layers);
    archive(basicLayers);
    archive(resolution);
    archive(length);
    archive(position);
    archive(startIndex);

    map.setFrameId(frameId);
    map.setTimestamp(timestamp);
    map.setGeometry(length, resolution, position);

    for (const auto& layerData : data)
        map.add(layerData.first, layerData.second);

    map.setBasicLayers(basicLayers);
    map.setStartIndex(startIndex);
}

}  // namespace cereal

