//στεινερ στο εσωτερικου κυρτου πολυγωνου που σχηματιζουν αμβλυγωνια τριγωνα

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/convex_hull_2.h>
#include <vector>
#include <iostream>
#include <cmath> // For angle calculations
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>

// Define CGAL types
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> DT;  // Constrained Delaunay triangulation
typedef DT::Point Point;
typedef DT::Edge Edge;
typedef DT::Face_handle FaceHandle;
typedef K::FT FT;
typedef CGAL::Polygon_2<K> Polygon;


// Συνάρτηση για έλεγχο αν ένα σημείο είναι εντός του κυρτού περιβλήματος
bool is_within_convex_hull(const Point& p, const std::vector<Point>& convex_hull) {
    Polygon hull_polygon(convex_hull.begin(), convex_hull.end());
    return CGAL::bounded_side_2(hull_polygon.vertices_begin(), hull_polygon.vertices_end(), p, K()) 
           != CGAL::ON_UNBOUNDED_SIDE;
}


// Function to calculate the angle between two points and a common vertex
template <typename P>
double angle_between(const P& p1, const P& p2, const P& p3) {
    double a = std::sqrt(CGAL::to_double(CGAL::squared_distance(p2, p3)));
    double b = std::sqrt(CGAL::to_double(CGAL::squared_distance(p1, p3)));
    double c = std::sqrt(CGAL::to_double(CGAL::squared_distance(p1, p2)));

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

template <typename FaceHandle>
std::pair<int, double> obtuse_vertex_index_and_angle(const FaceHandle& face) {
    double angle1 = angle_between(face->vertex(0)->point(), face->vertex(1)->point(), face->vertex(2)->point());
    double angle2 = angle_between(face->vertex(1)->point(), face->vertex(2)->point(), face->vertex(0)->point());
    double angle3 = angle_between(face->vertex(2)->point(), face->vertex(0)->point(), face->vertex(1)->point());
    
    if (angle1 > 90.0) return std::make_pair(0, angle1);
    if (angle2 > 90.0) return std::make_pair(1, angle2);
    if (angle3 > 90.0) return std::make_pair(2, angle3);
    
    return std::make_pair(-1, 0.0);  // No obtuse angle
}

// Function to calculate the circumcenter of a triangle
Point circumcenter(const Point& p1, const Point& p2, const Point& p3) {
    // Using the CGAL function to calculate the circumcenter
    return CGAL::circumcenter(p1, p2, p3);
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

template <typename DT>
std::vector<Point> add_steiner_in_circumcenter(DT& dt, std::vector<Point> steiner_points,  const std::vector<Point>& convex_hull) {
    bool added_steiner = false;

    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        int obtuse_vertex = obtuse_vertex_index(face);
        if (obtuse_vertex != -1) {
            Point p1 = face->vertex(0)->point();
            Point p2 = face->vertex(1)->point();
            Point p3 = face->vertex(2)->point();
            Point circumcenter_point = circumcenter(p1, p2, p3);

            // Έλεγχος αν το Steiner σημείο βρίσκεται εντός του κυρτού περιβλήματος
            if (is_within_convex_hull(circumcenter_point, convex_hull)) {
                steiner_points.push_back(circumcenter_point);
                std::cout << "Adding Steiner point at (" << circumcenter_point.x() << ", " 
                          << circumcenter_point.y() << ")\n";
                added_steiner = true;
            } else {
                std::cout << "Steiner point (" << circumcenter_point.x() << ", " 
                          << circumcenter_point.y() << ") is outside the convex hull, discarding.\n";
            }
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

int circumcenter_steiner_points(std::vector<Point> points, const std::vector<std::vector<int>>& constraints) {

    std::vector<Point> convex_hull;
    CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(convex_hull));

    // Initialize Delaunay triangulation
    DT dt;

    bool obtuse_exists = true;
    int obtuse_count = 0;
    int iterations = 0;
    
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

    obtuse_count = 0;  
    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        int obtuse_vertex = obtuse_vertex_index(face);
        if (obtuse_vertex != -1) {
            obtuse_count++;
        }
    }
    
    std::cout << "Obtuse triangles before adding Steiner points in iteration 0" << ": " << obtuse_count << "\n";

    while (obtuse_exists && iterations <= 15) {
        
        add_steiner_in_circumcenter(dt, steiner_points, convex_hull);
        
        obtuse_exists = false;
        obtuse_count = 0;  
        // for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        //     int obtuse_vertex = obtuse_vertex_index(face);
        //     if (obtuse_vertex != -1) {
                
        //         obtuse_exists = true;
        //         obtuse_count++;
        //     }
        // }
        
        // std::cout << "Obtuse triangles after adding Steiner points in iteration " << iterations + 1 << ": " << obtuse_count << "\n";
        // iterations++;


        for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
            auto obtuse = obtuse_vertex_index_and_angle(face);
            auto obtuse_vertex = std::get<0>(obtuse);
            auto obtuse_angle = std::get<1>(obtuse);
            if (obtuse_vertex != -1) {
                std::cout << obtuse_vertex << "\n";
                obtuse_exists = true;
                obtuse_count++;

                // Get coordinates of the obtuse vertex and print detailed information
                Point obtuse_point = face->vertex(obtuse_vertex)->point();
                std::cout << "Obtuse triangle found at vertex (" 
                        << obtuse_point.x() << ", " << obtuse_point.y() << ")\n";
                std::cout << "Obtuse angle: " << obtuse_angle << " degrees\n";

                // Print the coordinates of the other two vertices
                Point p1 = face->vertex((obtuse_vertex + 1) % 3)->point();
                Point p2 = face->vertex((obtuse_vertex + 2) % 3)->point();
                std::cout << "Other vertices: (" << p1.x() << ", " << p1.y() << ") and ("
                        << p2.x() << ", " << p2.y() << ")\n\n";
            }
        }

        std::cout << "Obtuse triangles after adding Steiner points in iteration " << iterations + 1 
                << ": " << obtuse_count << "\n";

        CGAL::draw(dt);

    }


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
