#ifndef BUS_STOP_PANEL_H
#define BUS_STOP_PANEL_H

#include <py_trees_msgs/Behaviour.h>
#include <ros/ros.h>

#include <QFileDialog>

#include <QTextEdit>
#include <QLineEdit>

#include <QThread>
#include <QFrame>
#include <QSlider>
#include <QMouseEvent>
#include <QSvgWidget>
#include <QSvgRenderer>

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

#include "QBusStopScene.h"


#include <geometry_msgs/PoseStamped.h>

#include <h264_bag_playback/h264_bag_playback.hpp>


class DatasetThread : public QThread
{


  Q_OBJECT





  void run() {
        QString result;
        /* expensive or blocking operation  */
        ros::NodeHandle("~").setParam("bag_file", file_name);

        bag_playback.init_playback();
        bag_playback.limit_playback_speed = false;

        bag_playback.OpenBags();

        current_state = PLAYBACK_PAUSE;

        do
        {
            while(current_state == PLAYBACK_PAUSE) {
            // Spin once so that any other ros controls/pub/sub can be actioned
              ros::spinOnce();
            }

            if (!ros::ok() || current_state == PLAYBACK_STOP) {
              break;
            }

            if (current_state == PLAYBACK_SEEK) {
              if (seek_time < bag_playback.last_packet_time) {
                // need to reload the data as the iterator only goes one way
                bag_playback.init_playback();

                for (auto video: bag_playback.videos) {
                  video.second.frame_counter = 0;
                }

                bag_playback.OpenBags();
              }
              bag_playback.SeekTime(seek_time);
              current_state = PLAYBACK_START;
            }


        } while (bag_playback.ReadNextPacket());

        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);

public:
    std::string file_name;

    ros::Time seek_time;

    typedef enum PLAYBACK_CONTROL {
      PLAYBACK_LOADING,
      PLAYBACK_STOP,
      PLAYBACK_START,
      PLAYBACK_PAUSE,
      PLAYBACK_SEEK
    } PLAYBACK_CONTROL;

    PLAYBACK_CONTROL current_state;

    dataset_toolkit::h264_bag_playback bag_playback;

    DatasetThread() {current_state = PLAYBACK_LOADING;}

};


class BusStopPanel: public QFrame
{
    Q_OBJECT
public:
    BusStopPanel( QWidget* parent = 0 );

    DatasetThread *workerThread;

protected Q_SLOTS:

    // slot for when the start button is pressed
    void startPressed();
    void stopPressed();
    void pausePressed();

    void sliderPressed();
    void sliderMoved(int new_value);
    void sliderReleased();


    // Request the set of trajectories from the service and update the list
    void RefreshTrajectoryList();

    void BusMapPressed(QString stop_name);

    void PollROS();


    void selectBagFile();


protected:

    // Monitor the state machine status so we can change button colours, etc
    void StatusCallback(const py_trees_msgs::BehaviourConstPtr& msg);

    QPushButton* start_button_;
    QPushButton* pause_button_;
    QPushButton* stop_button_;
    QPushButton* file_select_button_;

    QTextEdit *statusText;
    QLineEdit *currentTimeText;

    QSlider *slider;

    QBusStopView* graphics_view;
    QBusStopScene* graphics_scene;

    // The ROS node handle.
    ros::NodeHandle nh_;
    ros::Publisher start_msg_pub;
    ros::Publisher stop_msg_pub;
    ros::Publisher bus_stop_name_pub;
    ros::Subscriber bus_stop_state_machine_sub;
    ros::ServiceClient bus_stops_service_client_;

    std::map<std::string, geometry_msgs::PoseStamped> destination_map;
    std::vector<std::pair<std::string, std::string>> connections;


};


#endif // BUS_STOP_PANEL_H
