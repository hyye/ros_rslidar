// #define PCL_NO_PRECOMPILE

#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
// #include <pcl/filters/passthrough.h>
#include "rawdata.h"

static ros::Publisher pub_filtered_cloud;
static int ring_to_filter = -1;

// pcl::PassThrough<RSPoint> pass;

void PointCloudHandler(const sensor_msgs::PointCloud2ConstPtr &raw_points_msg) {
  sensor_msgs::PointCloud2 filtered_points_msg;

  pcl::PointCloud<RSPoint>::Ptr raw_points_ptr(new pcl::PointCloud<RSPoint>);
  pcl::PointCloud<RSPoint> filtered_points;
  pcl::fromROSMsg(*raw_points_msg, *raw_points_ptr);

  for (int i = 0; i < raw_points_ptr->size(); ++i) {
    const RSPoint &p = raw_points_ptr->at(i);
    if (p.ring != ring_to_filter) {
      filtered_points.push_back(p);
    }
  }
//   pass.setInputCloud(raw_points_ptr);
//   pass.setFilterFieldName("ring");
//   pass.setFilterLimits(ring_to_filter, ring_to_filter);
//   pass.setFilterLimitsNegative(true);
//   pass.filter(filtered_points);

  pcl::toROSMsg(filtered_points, filtered_points_msg);
  filtered_points_msg.header = raw_points_msg->header;

  pub_filtered_cloud.publish(filtered_points_msg);
}

int main(int argc, char **argv) {

  ros::init(argc, argv, "ring_filter");
  ros::NodeHandle nh("~");

  ros::Subscriber sub_raw_points = nh.subscribe<sensor_msgs::PointCloud2>
      ("/raw_points", 2, PointCloudHandler);
  nh.param("ring_to_filter", ring_to_filter, 10);

  pub_filtered_cloud = nh.advertise<sensor_msgs::PointCloud2>("/filtered_points", 2);

  ros::Rate r(500);

  while (ros::ok()) {
    ros::spinOnce();
    r.sleep();
  }
}