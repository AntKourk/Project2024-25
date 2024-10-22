//στεινερ στο εσωτερικου κυρτου πολυγωνου που σχηματιζουν αμβλυγωνια τριγωνα

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/convex_hull_2.h>
#include <vector>
#include <iostream>
#include <cmath> // For angle calculations

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> DT;  // Constrained Delaunay triangulation
typedef DT::Point Point;
typedef DT::Edge Edge;
typedef DT::Face_handle FaceHandle;
typedef K::FT FT;

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

Point calculate_centroid(const Point& p1, const Point& p2, const Point& p3) {
    FT cx = (p1.x() + p2.x() + p3.x()) / 3;
    FT cy = (p1.y() + p2.y() + p3.y()) / 3;
    return Point(cx, cy);
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
    // std::vector<Point> polygon;
    // for (int idx : region_boundary) {
    //     if (idx < points.size()) {
    //         polygon.push_back(points[idx]);
    //     } else {
    //         cerr << "Invalid index in region_boundary: " << idx << endl;
    //     }
    // }
}

// Function to print the points of the triangulation
template <typename DT>
void print_points(const DT& dt) {
    std::cout << "Points:\n";
    for (auto vertex = dt.finite_vertices_begin(); vertex != dt.finite_vertices_end(); ++vertex) {
        std::cout << "(" << vertex->point().x() << ", " << vertex->point().y() << ")\n";
    }
}

// Function to add Steiner points at the circumcenters of obtuse triangles inside a convex polygon
template <typename DT>
std::vector<Point> add_steiner_in_centroid(DT& dt, std::vector<Point> steiner_points) {
    bool added_steiner = false;
    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        int obtuse_vertex = obtuse_vertex_index(face);
        if (obtuse_vertex != -1) {
            // Get the vertices of the obtuse triangle
            Point p1 = face->vertex(0)->point();
            Point p2 = face->vertex(1)->point();
            Point p3 = face->vertex(2)->point();

            // Calculate the centroid of the triangle
            Point centroid_point = calculate_centroid(p1, p2, p3);

            // Add the Steiner point (centroid) to the list
            steiner_points.push_back(centroid_point);
            std::cout << "Adding Steiner point at (" << centroid_point.x() << ", " 
                    << centroid_point.y() << ")\n";
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
        return steiner_points;
    } else {
        std::cout << "No obtuse triangles found.\n";
        return {};
    }
}

int centroid_steiner_points(std::vector<Point> points) {
    // Initialize Delaunay triangulation
    DT dt;
    std::vector<Point> steiner_points;

    // Example points that form a convex polygon
    // std::vector<Point> points = {
    //     Point(0, 0),    // Bottom left
    //     Point(6, 0),    // Bottom right
    //     Point(4, 5),    // Top right
    //     Point(1, 4),    // Top left
    //     Point(3, 2)     // Internal point
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

    // Add Steiner points in obtuse triangles within the convex polygon
    steiner_points = add_steiner_in_centroid(dt, steiner_points);

    std::cout << "\nSteiner Points X:\n";
    for (const Point& p : steiner_points) {
        std::cout << p.x() << "\n"; 
    }

    std::cout << "Steiner Points Y:\n";
    for (const Point& p : steiner_points) {
        std::cout << p.y() << "\n"; 
    }

    // Print edges after adding Steiner points
    std::cout << "\nAfter adding Steiner points:\n";
    print_edges(dt);
    print_points(dt);

    CGAL::draw(dt);

    return 0;
}
