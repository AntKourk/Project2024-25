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

    // Define the points from the PSLG (x, y coordinates)  
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
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);  
    }

    // Draw the triangulation using CGAL's draw function  
    CGAL::draw(cdt);

    return 0;
}
