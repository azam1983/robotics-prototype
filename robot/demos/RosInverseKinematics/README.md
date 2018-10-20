# ROS Inverse Kinematics Demo

## Setup

Make sure you have `joint-state-publisher` package installed.

If you have already installed `urdf-tutorial` with `apt-get`, then it should already be installed.

To install the urdf-tutorial package, run:

- `sudo apt-get install ros-kinetic-urdf-tutorial` to install the urdf-tutorial package.

## Examining URDF files with Rviz

To view a URDF file with Rviz, use: `roslaunch urdf_tutorial display.launch model:=quick_bot.urdf`.

This command:

- Loads the specified model into the parameter server
- Runs nodes to publish `sensor_msgs/JointState` and transforms
- Starts Rviz with a configuration file

Note that the relative file path to the model is passed to argument `model:=`.

Alternatively, use the following command which will work from any directory and uses the example from the `urdf_tutorial` package):

- `roslaunch urdf_tutorial display.launch model:='$(find urdf_tutorial)/urdf/01-myfirst.urdf'
