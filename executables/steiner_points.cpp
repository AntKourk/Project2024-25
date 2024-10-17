#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <cmath> // For angle calculations

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> DT;  // Plain Delaunay triangulation
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
template <typename FaceHandle>
bool is_obtuse_triangle(const FaceHandle& face) {
    double angle1 = angle_between(face->vertex(0)->point(), face->vertex(1)->point(), face->vertex(2)->point());
    double angle2 = angle_between(face->vertex(1)->point(), face->vertex(2)->point(), face->vertex(0)->point());
    double angle3 = angle_between(face->vertex(2)->point(), face->vertex(0)->point(), face->vertex(1)->point());
    
    // Check if any angle is greater than 90 degrees
    return (angle1 > 90.0 || angle2 > 90.0 || angle3 > 90.0);
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

// Function to flip the diagonal if there are obtuse triangles
template <typename DT>
void flip_if_obtuse(DT& dt) {
    for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge) {
        FaceHandle face1 = edge->first; // One triangle
        FaceHandle face2 = face1->neighbor(edge->second); // Neighboring triangle
        
        // Check if any of the triangles is obtuse
        if (is_obtuse_triangle(face1) || is_obtuse_triangle(face2)) {
            // Try flipping the diagonal
            if (dt.is_flipable(edge->first, edge->second)) {
                dt.flip(edge->first, edge->second); // Flip the diagonal if possible
                std::cout << "Flipping edge between triangles\n";
                print_edges(dt); // Print edges after flip
            }
        }
    }
}

int steiner_points() {
    // Initialize plain Delaunay triangulation (no constraints)
    DT dt;

    // Example points that will generate obtuse triangles
    std::vector<Point> points = {
        Point(0, 0),    // Bottom left
        Point(5, 0),    // Bottom right
        // Point(2.5, 5),  // Top middle
        Point(1, 2),    // Internal point
        Point(4, 2)     // Internal point
    };

    // Insert points into the triangulation
    for (const Point& p : points) {
        dt.insert(p);
    }

    // Print points and edges before flipping
    std::cout << "Before flipping edges:\n";
    print_points(dt);
    print_edges(dt);

    CGAL::draw(dt);
    // Flip obtuse edges if possible
    flip_if_obtuse(dt);

    // Print edges after flipping
    std::cout << "After flipping edges:\n";
    print_edges(dt);

    CGAL::draw(dt);

    return 0;
}