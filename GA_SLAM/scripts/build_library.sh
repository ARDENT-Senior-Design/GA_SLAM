#!/bin/sh

# This file is part of GA SLAM.
# Copyright (C) 2018 Dimitris Geromichalos,
# Planetary Robotics Lab (PRL), European Space Agency (ESA)
#
# GA SLAM is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GA SLAM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GA SLAM. If not, see <http://www.gnu.org/licenses/>.

set -o errexit
set -o verbose

cd ga_slam
mkdir build
cd build

export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/opt/ros/kinetic
export CTEST_OUTPUT_ON_FAILURE=1
cmake .. -DENABLE_TESTS=ON \
         -DENABLE_COVERAGE=ON

make -j1
make test
sudo make install

cd ..
coveralls --root . --exclude 'test' --extension 'cc'

