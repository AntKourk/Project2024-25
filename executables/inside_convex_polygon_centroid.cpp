//shmeia steiner sto meso thw pleuras apenanti apo thn ambleia

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
    
    if (angle1 > 90.0 + 0.01) return 0;
    if (angle2 > 90.0 + 0.01) return 1;
    if (angle3 > 90.0 + 0.01) return 2;

    return -1; // No obtuse angle
}


// Function to compute centroid of a convex polygon
Point compute_centroid(const std::vector<Point>& points) {
    double sum_x = 0.0, sum_y = 0.0;
    for (const auto& point : points) {
        sum_x += CGAL::to_double(point.x());
        sum_y += CGAL::to_double(point.y());
    }
    return Point(sum_x / points.size(), sum_y / points.size());
}

// Function to find all connected obtuse triangles forming a convex polygon
std::vector<Point> find_convex_polygon(DT& dt, FaceHandle start_face) {
    std::vector<Point> polygon_points;
    std::set<FaceHandle> visited_faces; // To keep track of visited faces
    std::stack<FaceHandle> face_stack; // Stack for DFS
    face_stack.push(start_face); // Start from the initial face

    while (!face_stack.empty()) {
        FaceHandle current_face = face_stack.top();
        face_stack.pop();

        // If this face has already been visited, skip it
        if (visited_faces.find(current_face) != visited_faces.end()) {
            continue;
        }

        visited_faces.insert(current_face);

        // Check if the current face is obtuse
        if (obtuse_vertex_index(current_face) != -1) {
            // Add the vertices of the obtuse triangle to polygon points
            for (int i = 0; i < 3; ++i) {
                Point pt = current_face->vertex(i)->point();
                polygon_points.push_back(pt);
            }

            // Explore neighboring faces
            for (int i = 0; i < 3; ++i) {
                FaceHandle neighbor_face = current_face->neighbor(i);
                if (!dt.is_infinite(neighbor_face) && visited_faces.find(neighbor_face) == visited_faces.end() && obtuse_vertex_index(neighbor_face) != -1) {
                    face_stack.push(neighbor_face);
                }
            }
        }
    }

    // Remove duplicate points (in case the same vertex was added multiple times)
    std::set<Point> unique_points(polygon_points.begin(), polygon_points.end());
    polygon_points.assign(unique_points.begin(), unique_points.end());

    // if (!polygon_points.empty()) {
    // }

    //return polygon_points;

    //Check if polygon is convex
    // std::vector<Point> convex_hull;
    // CGAL::convex_hull_2(polygon_points.begin(), polygon_points.end(), std::back_inserter(convex_hull));
    // std::cout << convex_hull.size() << " " << polygon_points.size() << " heheeeeeeeeeeeeeeeeeeeeeeeeee\n" << endl;
    // if (convex_hull.size() == polygon_points.size()) {
        return polygon_points;  // Return polygon points if convex
    // }

    //  return {};  // Return empty vector if not convex
}

// Function to add Steiner points at the center of convex polygons of obtuse triangles
template <typename DT>
void add_steiner_in_convex_polygon_centroid(DT& dt) {
    bool added_steiner = false;
    std::vector<Point> steiner_points;
    int count = 0;

    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
        count++;
        int obtuse_vertex = obtuse_vertex_index(face);
        if (obtuse_vertex != -1) {
            auto polygon_points = find_convex_polygon(dt, face);
            if (!polygon_points.empty()) {  // Only proceed if convex polygon found
                Point centroid = compute_centroid(polygon_points);
                steiner_points.push_back(centroid);
                //dt.insert(centroid);
                added_steiner = true;
            }
        }
    }

    for (const Point& p : steiner_points) {
        dt.insert(p);
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

    CGAL::draw(dt);

    while (obtuse_exists && iterations <= 15) {
        
        add_steiner_in_convex_polygon_centroid(dt);
        
        obtuse_exists = false;
        obtuse_count = 0;  

        for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face) {
            int obtuse_vertex = obtuse_vertex_index(face);
            if (obtuse_vertex != -1) {
                obtuse_exists = true;
                obtuse_count++;
            }
        }

        iterations++;
    }

    CGAL::draw(dt);

    return 0;
    
}
