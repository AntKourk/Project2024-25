#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <vector>
#include <utility>
#include <iostream>
#include "executable.h"

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;

int main() {
    // Initialize the Constrained Delaunay Triangulation (CDT)
    CDT cdt;

    // Get data from the executable function
    OutputData output = executable();

    // Get points
    std::vector<Point> points = output.points;

    // Get Region Boundary
    std::vector<int> region_boundary = output.region_boundary;

    // Insert points into the triangulation
    for (const Point& p : points) {  
        cdt.insert(p);
    }

    // Define and add the constrained edges (from additional_constraints)
    const auto& constraints = output.additional_constraints;

    // Insert constrained edges based on the provided indices
    for (const auto& constraint : constraints) {  
        if (constraint.size() == 2) { // Check if constraint is a pair
            int idx1 = constraint[0];
            int idx2 = constraint[1];
            if (idx1 < points.size() && idx2 < points.size()) {
                cdt.insert_constraint(points[idx1], points[idx2]);
            } else {
                std::cerr << "Invalid constraint index: " << idx1 << ", " << idx2 << std::endl;
            }
        }
    }

    // Calculate the number of points
    size_t num_points = points.size();

    // Print number of points
    std::cout << "Number of Points: " << num_points << std::endl;

    // Print X and Y coordinates of points
    std::cout << "Points X: ";
    for (const auto& p : points) {
        std::cout << p.x() << " "; 
    }
    std::cout << "\nPoints Y: ";
    for (const auto& p : points) {
        std::cout << p.y() << " ";
    }
    std::cout << std::endl;

    // Print Additional Constraints
    std::cout << "Additional Constraints: ";
    for (const auto& row : constraints) {
        std::cout << "\n";
        for (const auto& value : row) {
            std::cout << value << " ";
        }
    }
    std::cout << "\n";

    // Print Region Boundary
    std::cout << "Region Boundary: ";
    for (const auto& boundary : region_boundary) {
        std::cout << boundary << " ";
    }
    std::cout << std::endl;

    // Draw the triangulation
    CGAL::draw(cdt);

    // OpenGL error checking (make sure an OpenGL context is active)
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    return 0;
}
