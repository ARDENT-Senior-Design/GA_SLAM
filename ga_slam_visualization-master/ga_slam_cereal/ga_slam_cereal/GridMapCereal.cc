#include "ga_slam_cereal/GridMapCereal.h"

#include "cereal/archives/binary.hpp"

#include <fstream>

void savePose(const Eigen::Affine3d& pose, const std::string& filename) {
    std::ofstream streamOut(filename, std::ios::binary);
    cereal::BinaryOutputArchive archiveOut(streamOut);

    archiveOut(pose);
}

void loadPose(Eigen::Affine3d& pose, const std::string& filename) {
    try {
        std::ifstream streamIn(filename, std::ios::binary);
        cereal::BinaryInputArchive archiveIn(streamIn);

        archiveIn(pose);
    } catch (const cereal::Exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

void saveArray(const Eigen::ArrayXXd& array, const std::string& filename) {
    std::ofstream streamOut(filename, std::ios::binary);
    cereal::BinaryOutputArchive archiveOut(streamOut);

    archiveOut(array);
}

void loadArray(Eigen::ArrayXXd& array, const std::string& filename) {
    try {
        std::ifstream streamIn(filename, std::ios::binary);
        cereal::BinaryInputArchive archiveIn(streamIn);

        archiveIn(array);
    } catch (const cereal::Exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

void saveGridMap(const grid_map::GridMap& map, const std::string& filename) {
    std::ofstream streamOut(filename, std::ios::binary);
    cereal::BinaryOutputArchive archiveOut(streamOut);

    archiveOut(map);
}

void loadGridMap(grid_map::GridMap& map, const std::string& filename) {
    try {
        std::ifstream streamIn(filename, std::ios::binary);
        cereal::BinaryInputArchive archiveIn(streamIn);

        archiveIn(map);
    } catch (const cereal::Exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}

