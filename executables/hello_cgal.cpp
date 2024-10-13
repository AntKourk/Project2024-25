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

using namespace std;

int main() {
    // Initialize the Constrained Delaunay Triangulation (CDT)
    CDT cdt;

    // Get data from the executable function
    OutputData output = executable();

    // Get points
    vector<Point> points = output.points;

    // Get Region Boundary
    vector<int> region_boundary = output.region_boundary;

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
                cerr << "Invalid constraint index: " << idx1 << ", " << idx2 << endl;
            }
        }
    }

    // Calculate the number of points
    size_t num_points = points.size();

    // Print number of points
    cout << "Number of Points: " << num_points << endl;

    // Print X and Y coordinates of points
    cout << "Points X: ";
    for (const auto& p : points) {
        cout << p.x() << " "; 
    }
    cout << "\nPoints Y: ";
    for (const auto& p : points) {
        cout << p.y() << " ";
    }
    cout << endl;

    // Print Additional Constraints
    cout << "Additional Constraints: ";
    for (const auto& row : constraints) {
        cout << "\n";
        for (const auto& value : row) {
            cout << value << " ";
        }
    }
    cout << "\n";

    // Print Region Boundary
    cout << "Region Boundary: ";
    for (const auto& boundary : region_boundary) {
        cout << boundary << " ";
    }
    cout << endl;

    // Draw the triangulation
    CGAL::draw(cdt);

    // OpenGL error checking (make sure an OpenGL context is active)
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cerr << "OpenGL error: " << err << endl;
    }

    return 0;
}
