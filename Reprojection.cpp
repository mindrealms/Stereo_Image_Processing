#include "Reprojection.h"
#include <open3d/geometry/PointCloud.h>
//#include <open3d/visualization/visualizer/Visualizer.h>;
#include <open3d/visualization/utility/DrawGeometry.h>
#include <open3d/io/PointCloudIO.h>
#include <iostream>

Reprojection::Reprojection() {

    open3d::geometry::PointCloud cloud;
    //I also tried:
        // open3d::geometry::PointCloud cloud();
        // open3d::geometry::PointCloud cloud = open3d::geometry::PointCloud();
    //all gave the same result

//    cloud.colors_; //compiles until here

    //but when i add this line....

//    cloud.Clear(); //... it doesn't compile (throws 'symbols not found for architecture x85_64')

    //(clear() is just an example of a function. No matter what I declare, it works as long as
    //what i'm accessing is eg. a member variable, but doesn't work for any functions.

}













//    open3d::io::ReadPointCloud("/Users/marky/Downloads/california_office_chair_ply/California \office \chair.ply", cloud);

//    open3d::visualization::Visualizer vis();
//    vis.CreateVisualizerWindow()
//    open3d::geometry::PointCloud cloud = open3d::geometry::PointCloud();
//    cloud.points_ = open3d::utility::Vector3uint8_allocator()
//                o3d.utility.Vector3dVector(cam_coords.T[:, :3])

//    open3d::visualization::DrawGeometries()

//    open3d::visualization::Visualizer vis;
//    _vis.CreateVisualizerWindow("asd",234,234);
