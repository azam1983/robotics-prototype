# Flask Prototype
This is the SpaceConcordia's Robotic web GUI using Flask.

To view the previous GUI please refer to the `AsimovOperation.py` file located [here.](https://github.com/space-concordia-robotics/robotics-prototype/tree/asimov-operation-gui-flask-prototype-42/robot/archives/gtk)

## Dependencies
Use pip to install flask and flask-bootstrap

`pip install flask`

`pip install flask-bootstrap`

## How to use
Run the app with `python app.py`

You will see: `Running on http://127.0.0.1:5000`

Visit this link in your browser to see the GUI.

# ROS Dummy simulation
For details on code used for the ros dummy simulation see [this](http://wiki.ros.org/ros3djs/Tutorials/VisualizingAURDF)

To be able to run the simulation in a browser the following ROS pakages need to be installed using the following commands:
`sudo apt-get install ros-kinetic-pr2-description`
`sudo apt-get install ros-kinetic-pr2-common`
`sudo apt-get install ros-kinetic-tf2-web-republisher`
`sudo apt-get install ros-kinetic-tf2-web-republisher`

Once the above are installted, the simulation can be opened up in a browser once the following ros nodes have been run in separate terminal windows (accept for the `rosparam set use_gui true` which can be run the same time as the command right after it):

> roslaunch pr2_description upload_pr2.launch
> rosrun robot_state_publisher robot_state_publisher
> rosparam set use_gui true
> rosrun joint_state_publisher joint_state_publisher
> rosrun tf2_web_republisher tf2_web_republisher
> roslaunch rosbridge_server rosbridge_websocket.launch

**Note:** Currently when running the working GUI the simulation does not display. It is in the processes of being fixed.


