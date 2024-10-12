#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

std::vector<Point> executable()
{
    // Δημιουργία του property tree
    boost::property_tree::ptree pt;

    // Ανάγνωση από JSON αρχείο
    try {
        read_json("input.json", pt);
    } catch (const boost::property_tree::json_parser_error &e) {
        std::cerr << "Error reading JSON: " << e.what() << std::endl;
        return {};
    }

    // Ανάκτηση δεδομένων από το property tree
    std::string instance_uid = pt.get<std::string>("instance_uid");
    int num_points = pt.get<int>("num_points");

    // Ανάκτηση σημείων
    std::vector<int> points_x;
    for (const auto& point : pt.get_child("points_x")) {
        points_x.push_back(point.second.get_value<int>());
    }

    std::vector<int> points_y;
    for (const auto& point : pt.get_child("points_y")) {
        points_y.push_back(point.second.get_value<int>());
    }

    // Create CGAL Point objects from the x and y coordinates
    std::vector<Point> points;
    for (int i = 0; i < num_points; ++i) {
        points.push_back(Point(points_x[i], points_y[i]));  // Constructing CGAL Point
    }

    // Εκτύπωση δεδομένων
    std::cout << "Instance UID: " << instance_uid << std::endl;
    std::cout << "Number of Points: " << num_points << std::endl;
    std::cout << "Points X: ";
    for (const auto& x : points_x) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Points Y: ";
    for (const auto& y : points_y) {
        std::cout << y << " ";
    }
    std::cout << std::endl;

    return points;
}
