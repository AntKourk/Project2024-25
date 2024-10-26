// //shmeia steiner sto meso thw pleuras apenanti apo thn ambleia

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/convex_hull_2.h>
#include <cmath> // For angle calculations
#include "inside_convex_polygon_centroid.h"

// Define CGAL types
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> DT;  // Constrained Delaunay triangulation
typedef DT::Point Point;
typedef DT::Edge Edge;
typedef DT::Face_handle FaceHandle;

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

    std::cout << "Angles: " << angle1 << ", " << angle2 << ", " << angle3 << std::endl;
    std::cout << "Vertices: " << face->vertex(0)->point() << ", " 
              << face->vertex(1)->point() << ", " 
              << face->vertex(2)->point() << std::endl;
    
    if (angle1 > 90.0 + 0.01) return 0;
    if (angle2 > 90.0 + 0.01) return 1;
    if (angle3 > 90.0 + 0.01) return 2;

    return -1; // No obtuse angle
}

// Function to check if a triangle is obtuse and return the index and angle of the obtuse vertex
template <typename FaceHandle>
std::pair<int, double> obtuse_vertex_index_and_angle(const FaceHandle& face) {
    double angle1 = angle_between(face->vertex(0)->point(), face->vertex(1)->point(), face->vertex(2)->point());
    double angle2 = angle_between(face->vertex(1)->point(), face->vertex(2)->point(), face->vertex(0)->point());
    double angle3 = angle_between(face->vertex(2)->point(), face->vertex(0)->point(), face->vertex(1)->point());
    
    if (angle1 > 90.0 + 0.01) return std::make_pair(0, angle1);
    if (angle2 > 90.0 + 0.01) return std::make_pair(1, angle2);
    if (angle3 > 90.0 + 0.01) return std::make_pair(2, angle3);
    
    return std::make_pair(-1, 0.0);  // No obtuse angle
}


// Function to compute centroid of a convex polygon
Point compute_centroid(const std::vector<Point>& points) {
    int sum_x = 0, sum_y = 0;
    for (const auto& point : points) {
        sum_x += CGAL::to_double(point.x());
        sum_y += CGAL::to_double(point.y());
    }
    return Point(sum_x / points.size(), sum_y / points.size());
}

// Function to find connected obtuse triangles and check convexity
std::vector<Point> find_convex_polygon(DT& dt, FaceHandle start_face) {
    std::vector<Point> polygon_points;
    std::set<FaceHandle> visited_faces;
    std::stack<FaceHandle> face_stack;

    face_stack.push(start_face);
    visited_faces.insert(start_face);

    while (!face_stack.empty()) {
        auto current_face = face_stack.top();
        face_stack.pop();

        // Collect vertices for the current obtuse triangle
        for (int i = 0; i < 3; ++i) {
            polygon_points.push_back(current_face->vertex(i)->point());
        }

        // Visit neighboring obtuse triangles sharing an edge
        for (int i = 0; i < 3; ++i) {
            auto neighbor_face = current_face->neighbor(i);
            int obtuse_vertex_neighbor = obtuse_vertex_index(neighbor_face);
            if (visited_faces.find(neighbor_face) == visited_faces.end() && (obtuse_vertex_neighbor != -1)) {
                face_stack.push(neighbor_face);
                visited_faces.insert(neighbor_face);
            }
        }
    }

    // Check if polygon is convex
    std::vector<Point> convex_hull;
    CGAL::convex_hull_2(polygon_points.begin(), polygon_points.end(), std::back_inserter(convex_hull));
    if (convex_hull.size() == polygon_points.size()) {
        return polygon_points;  // Return polygon points if convex
    }

    return {};  // Return empty vector if not convex
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

// Function to add Steiner points at the center of convex polygons of obtuse triangles
template <typename DT>
void add_steiner_in_convex_polygon_centroid(DT& dt) {
    bool added_steiner = false;
    std::vector<Point> steiner_points;

    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        int obtuse_vertex = obtuse_vertex_index(face);
        if (obtuse_vertex != -1) {
            auto polygon_points = find_convex_polygon(dt, face);
            if (!polygon_points.empty()) {  // Only proceed if convex polygon found
                Point centroid = compute_centroid(polygon_points);
                steiner_points.push_back(centroid);
                //dt.insert(centroid);
                std::cout << "Adding Steiner point at (" << centroid.x() << ", " << centroid.y() << ")\n";
                added_steiner = true;
            }
        }
    }

    for (const Point& p : steiner_points) {
        dt.insert(p);
    }
    
    if (added_steiner) {
        CGAL::draw(dt);
        std::cout << "After adding Steiner points:\n";
        print_points(dt);
        print_edges(dt);
    } else {
        std::cout << "No obtuse triangles found.\n";
    }
}

int inside_convex_polygon_centroid_steiner_points(std::vector<Point> points, DT dt) {
    // Initialize Delaunay triangulation
    // DT dt;

    bool obtuse_exists = true;
    int obtuse_count = 0;
    int iterations = 0;

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
        
        add_steiner_in_convex_polygon_centroid(dt);
        
        obtuse_exists = false;
        obtuse_count = 0;  

        for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
            auto obtuse = obtuse_vertex_index_and_angle(face);
            auto obtuse_vertex = std::get<0>(obtuse);
            auto obtuse_angle = std::get<1>(obtuse);
            //int obtuse_vertex = obtuse_vertex_index(face);
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
        iterations++;
    }


    // Print edges after adding Steiner points
    std::cout << "After adding Steiner points:\n";
    print_edges(dt);

    CGAL::draw(dt);

    return 0;
    
}