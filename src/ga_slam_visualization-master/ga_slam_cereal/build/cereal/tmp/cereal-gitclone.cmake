if("master" STREQUAL "")
  message(FATAL_ERROR "Tag for git checkout should not be empty.")
endif()

set(run 0)

if("/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal-stamp/cereal-gitinfo.txt" IS_NEWER_THAN "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal-stamp/cereal-gitclone-lastrun.txt")
  set(run 1)
endif()

if(NOT run)
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal-stamp/cereal-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal'")
endif()

# try the clone 3 times incase there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git" clone --origin "origin" "https://github.com/USCiLab/cereal" "cereal"
    WORKING_DIRECTORY "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/USCiLab/cereal'")
endif()

execute_process(
  COMMAND "/usr/bin/git" checkout master
  WORKING_DIRECTORY "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

execute_process(
  COMMAND "/usr/bin/git" submodule init 
  WORKING_DIRECTORY "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to init submodules in: '/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal'")
endif()

execute_process(
  COMMAND "/usr/bin/git" submodule update --recursive 
  WORKING_DIRECTORY "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal-stamp/cereal-gitinfo.txt"
    "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal-stamp/cereal-gitclone-lastrun.txt"
  WORKING_DIRECTORY "/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/anhtuanquang1234/Documents/Github/GA_SLAM_ARDENT/src/ga_slam_visualization-master/ga_slam_cereal/build/cereal/src/cereal-stamp/cereal-gitclone-lastrun.txt'")
endif()

