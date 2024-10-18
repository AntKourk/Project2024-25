#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <vector>
#include <utility>
#include <iostream>
#include "executable.h"
#include "flipEdges.h"
#include "output.h"
#include "steiner_points.h"

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;

using namespace std;

int main() {
    // Initialize the Constrained Delaunay Triangulation (CDT)
    CDT cdt;

    // Get data from the executable function
    InputData input = executable();

    // Get points
    vector<Point> points = input.points;

    // Get Region Boundary
    vector<int> region_boundary = input.region_boundary;

    // Insert points into the triangulation
    for (const Point& p : points) {
        cdt.insert(p);
    }

    // Insert the region boundary as a constrained polygon
    std::vector<Point> polygon;
    for (int idx : region_boundary) {
        if (idx < points.size()) {
            polygon.push_back(points[idx]);
        } else {
            cerr << "Invalid index in region_boundary: " << idx << endl;
        }
    }

    // Append the first point again to close the polygon
    if (!region_boundary.empty()) {
        int first_idx = region_boundary[0]; // Get the first index from region_boundary
        if (first_idx < points.size()) {
            polygon.push_back(points[first_idx]); // Append the first point to close the polygon
        } else {
            cerr << "Invalid first index in region_boundary: " << first_idx << endl;
        }
    }

    // Check if the polygon is valid and insert the constraint
    if (polygon.size() > 2) {
        cdt.insert_constraint(polygon.begin(), polygon.end());
    } else {
        cerr << "Not enough points to form a boundary." << endl;
    }

    // Define and add the constrained edges (from additional_constraints)
    const auto& constraints = input.additional_constraints;

    // Insert constrained edges based on the provided indices
    for (const auto& constraint : constraints) {  
        if (constraint.size() == 2) { // Check if constraint is a pair
            int idx1 = constraint[0];
            int idx2 = constraint[1];
            if (idx1 < points.size() && idx2 < points.size()) {
                cdt.insert_constraint(points[idx1], points[idx2]);
            } else {
                cerr << "Invalid constraint index: " << idx1 << ", " << idx2 << endl;
            }
        }
    }

    // Calculate the number of points
    size_t num_points = points.size();

    // Print number of points
    // cout << "Number of Points: " << num_points << endl;

    // Print X and Y coordinates of points
    // cout << "Points X: ";
    // for (const auto& p : points) {
    //     cout << p.x() << " "; 
    // }
    // cout << "\nPoints Y: ";
    // for (const auto& p : points) {
    //     cout << p.y() << " ";
    // }
    // cout << endl;

    // Print Additional Constraints
    // cout << "Additional Constraints: ";
    // for (const auto& row : constraints) {
    //     cout << "\n";
    //     for (const auto& value : row) {
    //         cout << value << " ";
    //     }
    // }
    // cout << "\n";

    // Print Region Boundary
    // cout << "Region Boundary: ";
    // for (const auto& boundary : region_boundary) {
    //     cout << boundary << " ";
    // }
    // cout << endl;

    // Draw the triangulation
    // CGAL::draw(cdt);

    // output();

    // flip_edges();

     steiner_points();

    return 0;
}
