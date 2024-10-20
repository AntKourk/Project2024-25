// //shmeia steiner sto meso thw pleuras apenanti apo thn ambleia

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <cmath> // For angle calculations
#include "center.h"

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> DT;  // Constrained Delaunay triangulation
typedef DT::Point Point;
typedef DT::Edge Edge;
typedef DT::Face_handle FaceHandle;

// Function to calculate the angle between two points and a common vertex
template <typename P>
double angle_between(const P& p1, const P& p2, const P& p3) {
    double a = std::sqrt(CGAL::squared_distance(p2, p3));
    double b = std::sqrt(CGAL::squared_distance(p1, p3));
    double c = std::sqrt(CGAL::squared_distance(p1, p2));

    // Cosine rule to calculate the angle
    double cos_angle = (b * b + c * c - a * a) / (2 * b * c);
    return std::acos(cos_angle) * 180.0 / M_PI; // Return angle in degrees
}

// Function to check if a triangle is obtuse (has an angle > 90 degrees)
// Returns the index of the obtuse angle's vertex or -1 if no obtuse angle is found
template <typename FaceHandle>
int obtuse_vertex_index(const FaceHandle& face) {
    double angle1 = angle_between(face->vertex(0)->point(), face->vertex(1)->point(), face->vertex(2)->point());
    double angle2 = angle_between(face->vertex(1)->point(), face->vertex(2)->point(), face->vertex(0)->point());
    double angle3 = angle_between(face->vertex(2)->point(), face->vertex(0)->point(), face->vertex(1)->point());
    
    if (angle1 > 90.0) return 0;
    if (angle2 > 90.0) return 1;
    if (angle3 > 90.0) return 2;
    return -1; // No obtuse angle
}

// Function to print the edges of the triangulation
template <typename DT>
void print_edges(const DT& dt) {
    std::cout << "Edges:\n";
    for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge) {
        auto v1 = edge->first->vertex((edge->second + 1) % 3)->point();
        auto v2 = edge->first->vertex((edge->second + 2) % 3)->point();
        std::cout << "(" << v1.x() << ", " << v1.y() << ") - (" 
                  << v2.x() << ", " << v2.y() << ")\n";
    }
}

// Function to print the points of the triangulation
template <typename DT>
void print_points(const DT& dt) {
    std::cout << "Points:\n";
    for (auto vertex = dt.finite_vertices_begin(); vertex != dt.finite_vertices_end(); ++vertex) {
        std::cout << "(" << vertex->point().x() << ", " << vertex->point().y() << ")\n";
    }
}

// Function to add Steiner points instead of flipping edges when a triangle is obtuse
template <typename DT>
void add_steiner_if_obtuse_center(DT& dt) {
    bool added_steiner = false;
    std::vector<Point> steiner_points;

    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        int obtuse_vertex = obtuse_vertex_index(face);
        if (obtuse_vertex != -1) {
            // Get the vertices of the obtuse triangle
            Point p1 = face->vertex((obtuse_vertex + 1) % 3)->point();
            Point p2 = face->vertex((obtuse_vertex + 2) % 3)->point();

            // Calculate the midpoint of the edge opposite the obtuse angle
            Point midpoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);

            // Add the Steiner point to the list
            steiner_points.push_back(midpoint);
            std::cout << "Adding Steiner point at (" << midpoint.x() << ", " << midpoint.y() << ")\n";
            added_steiner = true;
        }
    }

    // Insert Steiner points into the triangulation and re-triangulate
    for (const Point& p : steiner_points) {
        dt.insert(p);
    }

    if (added_steiner) {
        std::cout << "After adding Steiner points:\n";
        print_points(dt);
        print_edges(dt);
    } else {
        std::cout << "No obtuse triangles found.\n";
    }
}

int center_steiner_points(std::vector<Point> points) {
    // Initialize Delaunay triangulation
    DT dt;

    // Example points that will generate obtuse triangles
    // std::vector<Point> points = {
    //     Point(0, 0),    // Bottom left
    //     Point(5, 0),    // Bottom right
    //     Point(1, 2),    // Internal point
    //     Point(4, 2)     // Internal point
    // };

    // Insert points into the triangulation
    for (const Point& p : points) {
        dt.insert(p);
    }

    // Print points and edges before adding Steiner points
    std::cout << "Before adding Steiner points:\n";
    print_points(dt);
    print_edges(dt);

    CGAL::draw(dt);
    // Add Steiner points at the midpoint of the edge opposite obtuse angles
    add_steiner_if_obtuse_center(dt);

    // Print edges after adding Steiner points
    std::cout << "After adding Steiner points:\n";
    print_edges(dt);

    CGAL::draw(dt);

    return 0;
    
}