#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include "output.h"
#include "executable.h"
#include <string>

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

void output() {
    // Δημιουργία του property tree
    boost::property_tree::ptree pt;

    // Ανάγνωση από JSON αρχείο
    try {
        read_json("../input.json", pt);
    } catch (const boost::property_tree::json_parser_error &e) {
        std::cerr << "Error reading JSON: " << e.what() << std::endl;
        // return {};  // Return empty struct
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

    // Retrieve additional constraints
    std::vector<std::vector<int>> additional_constraints;
    for (const auto& row : pt.get_child("additional_constraints")) {
        std::vector<int> row_values;  // Temporary vector to store each row
        for (const auto& value : row.second) {
            row_values.push_back(value.second.get_value<int>());
        }
        additional_constraints.push_back(row_values);  // Add the row to the 2D vector
    }

    // Retrieve region_boundary
    std::vector<int> region_boundary;
    for (const auto& boundary_index : pt.get_child("region_boundary")) {
        region_boundary.push_back(boundary_index.second.get_value<int>());
    }

    // Create InputData struct and populate it
    InputData input_data;
    input_data.points = points;
    input_data.region_boundary = region_boundary;
    input_data.additional_constraints = additional_constraints;

    // This is where you'd process the input data and generate Steiner points and edges
    // For this example, I'm creating some mock Steiner points and edges

    std::vector<std::string> steiner_points_x = {"123/456", "789", "1011/1213"};
    std::vector<std::string> steiner_points_y = {"314/159", "265", "358/979"};
    std::vector<std::vector<int>> edges = {{0, 7}, {7, 8}, {8, 9}};

    // Create the output property tree
    boost::property_tree::ptree output_pt;

    // Populate the JSON structure
    output_pt.put("content_type", "CG_SHOP_2025_Solution");
    output_pt.put("instance_uid", instance_uid);

    // Steiner points x
    boost::property_tree::ptree steiner_points_x_node;
    for (const auto& x : steiner_points_x) {
        boost::property_tree::ptree child;
        child.put("", x);
        steiner_points_x_node.push_back(std::make_pair("", child));
    }
    output_pt.add_child("steiner_points_x", steiner_points_x_node);

    // Steiner points y
    boost::property_tree::ptree steiner_points_y_node;
    for (const auto& y : steiner_points_y) {
        boost::property_tree::ptree child;
        child.put("", y);
        steiner_points_y_node.push_back(std::make_pair("", child));
    }
    output_pt.add_child("steiner_points_y", steiner_points_y_node);

    // Edges
    boost::property_tree::ptree edges_node;
    for (const auto& edge : edges) {
        boost::property_tree::ptree edge_node;
        boost::property_tree::ptree edge_first, edge_second;
        edge_first.put("", edge[0]);
        edge_second.put("", edge[1]);
        edge_node.push_back(std::make_pair("", edge_first));
        edge_node.push_back(std::make_pair("", edge_second));
        edges_node.push_back(std::make_pair("", edge_node));
    }
    output_pt.add_child("edges", edges_node);

    // Write the output JSON to a file
    try {
        write_json("../output.json", output_pt);
        std::cout << "Output written to output.json" << std::endl;
    } catch (const boost::property_tree::json_parser_error &e) {
        std::cerr << "Error writing JSON: " << e.what() << std::endl;
    }

    // return input_data;  // Return the populated struct
    // return 0;
}