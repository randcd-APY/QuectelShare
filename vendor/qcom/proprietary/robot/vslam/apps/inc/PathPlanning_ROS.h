/***************************************************************************//**
@copyright
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef VSLAM_APPS_INC_PATHPLANNING_ROS_H_
#define VSLAM_APPS_INC_PATHPLANNING_ROS_H_

#include "PathPlanning.h"
#include "Visualization.h"

#ifdef ROS_BASED
#include <image_transport/image_transport.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseStamped.h>
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "nav_msgs/MapMetaData.h"
#include "nav_msgs/OccupancyGrid.h"
#include "std_msgs/String.h"
#include <kobuki_msgs/BumperEvent.h>
#include <sensor_msgs/Imu.h>
#endif

#include <cmath>

#define ENABLE_TOF

#ifndef WIN32
#define DEBUG_PP

#ifdef DEBUG_PP
#define PRINT_PP(x,...) printf(x,##__VA_ARGS__)
#else
#define PRINT_PP(x,...)
#endif
#else
#define PRINT_PP(x,...) printf(x,__VA_ARGS__)
#define M_PI 3.1415926f
#endif

typedef enum {
   MOTION_MOVE_CLEAN,
   MOTION_PRE_NAVI,
   MOTION_MOVE_NAVI_P2P,
   MOTION_MOVE_NAVI_SEARCH,
   MOTION_FINISH_P2P,
   MOTION_MOVE_TO_CHARGER,
   MOTION_FINISH_ALL
}MOTION_STATUS;

typedef enum{
   MOVE_FOR_CLEAN,
   MOVE_FOR_NAVI_REQ,
   MOVE_FOR_FINISH,  //navi to charger
}MOTION_MOTIVATION;

typedef enum {
   ALONG_0_TURN_LEFT,
   ALONG_90_TURN_LEFT,
   ALONG_0_TURN_RIGHT,
   ALONG_90_TURN_RIGHT,
}MOTION_DIRECTION;

typedef enum {
   SQUARE_PATH_PHASE_1,
   SQUARE_PATH_PHASE_2,
   SQUARE_PATH_PHASE_3,
   SQUARE_PATH_PHASE_4,
}SQUARE_PATH_PHASE;

#ifndef ROS_BASED
namespace geometry_msgs
{
   typedef const int * PoseStampedConstPtr;
   struct Twist
   {
       struct LinearS
       {
          double x, y, z;
       } linear, angular;
   };
};
namespace kobuki_msgs
{
   typedef const int * BumperEventConstPtr;
};
#endif
typedef  void( *f_naviCmdCB )(const geometry_msgs::PoseStampedConstPtr &msg);
typedef  void( *f_bumperCB )(const kobuki_msgs::BumperEventConstPtr msg);


/**------------------------------------------------------------------------------
@brief
Pose information including the location of (x, y) in meter and the orientation of yaw in degree [0, 360)
------------------------------------------------------------------------------**/
struct Pose3D
{
   float32_t x;   // the location of axis x
   float32_t y;   // the location of axis y
   float32_t yaw;  // the orientation of yaw in degree [0, 360)
};



/**------------------------------------------------------------------------------
@brief
Velocity of the robot including linarly speed x m/s and angualar speed degree/s
------------------------------------------------------------------------------**/
struct Velocity
{
   float32_t x;   // speed of x direction in m/s
   float32_t yaw;  // speed of yaw direction in rad/s
};

/**------------------------------------------------------------------------------
@brief
Substraction between two poses: pose1 - pose2
@param pose1
   Pose for subtractor
@param pose2
   Pose for minuend
------------------------------------------------------------------------------**/
inline Pose3D operator - ( const Pose3D& pose1, const Pose3D& pose2 )
{
   Pose3D outputPose;

   outputPose.x = pose1.x - pose2.x;
   outputPose.y = pose1.y - pose2.y;
   outputPose.yaw = pose1.yaw - pose2.yaw;

   if( outputPose.yaw < 0.0 )
      outputPose.yaw += 360;

   return outputPose;
}


/**------------------------------------------------------------------------------
@brief
Addition between two poses: pose1 + pose2
@param pose1
   Pose for one addend
@param pose2
   Pose for another addend
------------------------------------------------------------------------------**/
inline Pose3D operator + ( const Pose3D& pose1, const Pose3D& pose2 )
{
   Pose3D outputPose;

   outputPose.x = pose1.x + pose2.x;
   outputPose.y = pose1.y + pose2.y;
   outputPose.yaw = pose1.yaw + pose2.yaw;

   if( outputPose.yaw > 360.0 )
      outputPose.yaw -= 360;

   return outputPose;
}


/**------------------------------------------------------------------------------
@brief
Computer the orientation different between two poses
@param pose1
   Pose whose orientation is taken as subtractor
@param pose2
   Pose whose orientation is taken as minuend
------------------------------------------------------------------------------**/
inline float32_t getDiffAngle( const Pose3D& nextGoal, const Pose3D& curPose )
{
   float32_t tmpDiffAngle = nextGoal.yaw - curPose.yaw;
   float32_t diffAngle = tmpDiffAngle > 0 ? tmpDiffAngle : -1 * tmpDiffAngle;

   if( diffAngle > 180 )
      diffAngle = 360 - diffAngle;

   return diffAngle;
}


/**------------------------------------------------------------------------------
@brief
Computer the translation different between two poses
@param pose1
   Pose whose location is taken as subtractor
@param pose2
   Pose whose location is taken as minuend
------------------------------------------------------------------------------**/
inline float32_t getDistSquare( Pose3D nextGoal, Pose3D curPose )
{
   float32_t distSquare = (nextGoal.x - curPose.x) * (nextGoal.x - curPose.x)
      + (nextGoal.y - curPose.y) * (nextGoal.y - curPose.y);

   return distSquare;
}

/*
 * obstacle marking
 */
struct BumperPosition
{
   Pose3D left;
   Pose3D center;
   Pose3D right;
};

/*
 * map description
 */
typedef enum{
   UNKNOWN = 0,
   CURRENT,
   OCCUPIED,
   FREE,
   ORIGIN,
}GIRD_STATUS;

typedef enum{
   NOT_CLEANED = 0,
   CLEANED,
}CLEAN_STATUS;

typedef enum{
   NOT_COV = 0,
   COV,
}VSLAM_STATUS;


struct MapElement
{
   signed char x;   // note: x/scale is the real position
   signed char y;
   GIRD_STATUS status;
   CLEAN_STATUS cs;
   VSLAM_STATUS vs;
   VSLAM_STATUS rs;
};

struct LandMark
{
   struct MapElement e;
};


#define X_LEN 10 //note: enlarge this number pls change MapElement members type to avoid overflow
#define Y_LEN 10
#define GRID_LEN 0.1f
#define GRID_SCALE 10 // (1.0/GRID_LEN)
#define GRID_NUM (4 * (X_LEN * GRID_SCALE) * ((X_LEN * GRID_SCALE)))
#define LANDMARK_CHECK_LEN	5
#define SHORT_EDGE_LEN 0.25f


#define ANGLE_THRESHOLD  (2.0)
#define FEATURE_THRESHOLD (500) //feature detect threshold

#define TOF_SENSOR1 "/sys/devices/virtual/input/input1/"
#define TOF_SENSOR2 "/sys/devices/virtual/input/input2/"

#define TOF_MARK_OBSTACLE_LEN  0.4f
#define TOF_STOP_FORWARD_LEN 0.2f
#define LOCAL_AREA_WIDTH 0.4f


struct Map
{
   struct MapElement rawData[GRID_NUM];
   struct Pose3D cleanMark;
   struct Pose3D origin;
};

struct InitAngle
{
   float wall_angle;
   float maxfeature_yaw;
   int maxfeature_num;
};

class PathPlanning_ROS: public PathPlanning
{
public:
   PathPlanning_ROS(f_naviCmdCB f1, f_bumperCB f2);
   virtual ~PathPlanning_ROS();

   void init(void){}
   void initVSLAM(void);
   void initWheel(void);
   float angleTranslation(float);
   InitAngle selectDirection(int flag);
   Pose3D getCurrentPose();
   void setCurrentPose(Pose3D p);
   //cleaning path pattern
   void work( void);
   bool isReachGoal(Pose3D goalPose);
   void naviP2P(Pose3D goal);
   void updateGoalForFinish(Pose3D &goal); //update goal after finish in cleaning path
   void updateGoalForBumper(Pose3D &goal); //update goal after finish in cleaning path
   void computeVelocity(Pose3D goalPose);
   void computeLeastRotationPath(Pose3D srcPose, Pose3D goalPose, float32_t &angle2midPose, float32_t &angle2goalPose );
   void publishMotionCmd(float32_t xSpeed, float32_t yawSpeed);
   void publishMotionCmd(void);
   bool isLocalDone(void);
   bool isCleanMarkDone(void);
   bool updateCleanMark(void);
   bool isANewCleanMark(signed char x, signed char y, MOTION_DIRECTION d);
   bool findNewArea(Pose3D &goal);
   void rotate(float32_t yaw);
   void backward();
 
   void searchPath(Pose3D curPose, Pose3D goal);
   void publishMapImage( int64_t t );
	void calCoverage(void);
   bool isLineOfSight(Pose3D pose1, Pose3D pose2);
   bool isLineOfSight(signed char x1, signed char y1, signed char x2, signed char y2);

   float getTofDataRight(void);
   float getTofLocalRight(void);
   void closeTofRight(void);
   float getTofDataFront(void);  //front tof, isNearObstacle
   float getLocalTofFront(void);
   void closeTofFront(void);
   int initTof( std::string  TOF_path );
   void markObstacleByFrontTof(void);
   void markByRightTof(void);

   Pose3D currentPose;
   uint64_t timestampUs;
   Pose3D naviPose;   //navi req
   Pose3D startWE;   //wheel pose of when robot start to move
   Pose3D vslamWE;   //wheel pose of when vslam init is done
   Velocity robotSpeed;      // robot speed during movement
   float32_t rotateSpeed[4] = {5.0, 10.0, 15.0, 20.0}; // dynamic rotate speed
   Velocity cmdVel;         // velocity of the robot for the current movement
   float32_t thresAngle;  // accept orientation error during movement
   float32_t thresDist;   // accept translation error during movement
   MOTION_DIRECTION direction;
   MOTION_DIRECTION last_direction; //step4
   SQUARE_PATH_PHASE squPhase;
   float32_t vslamCov = 0;
   float32_t ppCov = 0;

   struct Map map;
   geometry_msgs::Twist speed;

   //callback ptr
   f_naviCmdCB pf_naviCmdCB;
   f_bumperCB pf_bumperCB;

   bool restart;  //vslam need to restart

   typedef enum {
      SEARCH=0,
      WOD,
      FEATURE_SELECT,
   }INITSTATUS;

   INITSTATUS initStatus;
   INITSTATUS getInitStatus()
   {
      return initStatus;
   }
   void setInitStatus( INITSTATUS status )
   {
      initStatus = status;
   }

private:
#ifdef ENABLE_TOF
   FILE* fd_tof_front;
   FILE* fd_tof_right;
   int tofDataFront;
   int tofDataRight;
#endif

   float min_x;
   float min_y;
   float max_x;
   float max_y;

   std::vector<struct Pose3D> landmark; //used to search path

#ifdef ROS_BASED
   ros::Rate *loopRate;

   ros::Publisher pubPath;
   ros::Publisher gridMapPub;
   ros::Publisher markerPub;
   image_transport::Publisher mapImagePub;

   //ros::Subscriber subOdom;
   ros::Subscriber subBumper;
   ros::Subscriber subNaviCmd;
#else
   class pub
   {
   public:
      void publish( const geometry_msgs::Twist &) {}
   } pubPath;
#endif
};

#endif /* VSLAM_APPS_INC_PATHPLANNING_ROS_H_ */


