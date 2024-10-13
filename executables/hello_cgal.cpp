// #include <CGAL/draw_triangulation_2.h>
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Constrained_Delaunay_triangulation_2.h>
// #include <iostream>

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef CGAL::Exact_predicates_tag Itag;
// typedef CGAL::Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;
// typedef CDT::Point Point;
// typedef CDT::Edge Edge;

// int main() {
//     CDT cdt;
//     std::cout << "Inserting a grid of 5x5 constraints " << std::endl;

//     // Corrected the for-loop syntax and the condition check
//     for (int i = 1; i < 6; ++i) {
//         cdt.insert_constraint(Point(0, i), Point(6, i));
//     }
    
//     for (int j = 1; j < 6; ++j) {
//         cdt.insert_constraint(Point(j, 0), Point(j, 6));
//     }

//     // Ensure the triangulation is valid
//     assert(cdt.is_valid());

//     int count = 0;
    
//     // Loop over edges and count the constrained edges
//     for (const Edge& e : cdt.finite_edges()) {
//         if (cdt.is_constrained(e)) {
//             ++count;
//         }
//     }

//     std::cout << "The number of resulting constrained edges is: " << count << std::endl;

//     // Drawing the triangulation
//     CGAL::draw(cdt);

//     return 0;
// }

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <vector>
#include <utility> // For std::pair
#include "executable.h"

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;

int main()
{
    // Initialize the Constrained Delaunay Triangulation (CDT)
    CDT cdt;

    // Get points from the executable function
    std::vector<Point> points = executable();

    // Insert points into the triangulation
    for (const Point& p : points) {  
        cdt.insert(p);
    }

    // Define and add the constrained edges (from additional_constraints)
    std::vector<std::pair<int, int>> constraints = { 
        {3, 4}, {5, 6}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14},
        {14, 15}, {15, 16}, {18, 19}
    };

    // Insert constrained edges based on the provided indices
    for (const auto& constraint : constraints) {  
        if (constraint.first < points.size() && constraint.second < points.size()) {
            cdt.insert_constraint(points[constraint.first], points[constraint.second]);
        } else {
            std::cerr << "Invalid constraint index: " << constraint.first << ", " << constraint.second << std::endl;
        }
    }

    // Calculate the number of points
    size_t num_points = points.size();

    // Print number of points
    std::cout << "Number of Points: " << num_points << std::endl;

    // Print X and Y coordinates of points
    std::cout << "Points X: ";
    for (const auto& p : points) {
        std::cout << p.x() << " "; // Accessing x-coordinate
    }
    std::cout << "\nPoints Y: ";
    for (const auto& p : points) {
        std::cout << p.y() << " "; // Accessing y-coordinate
    }
    std::cout << std::endl;

    // Uncomment this if you want to draw the triangulation
    // CGAL::draw(cdt);

    // OpenGL error checking
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    return 0;
}
