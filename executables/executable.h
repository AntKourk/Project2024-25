#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <vector>

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;  // This defines the Point type as CGAL's Point_2 type

// Function declaration for executable
std::vector<Point> executable();  // Change the return type to std::vector<Point>