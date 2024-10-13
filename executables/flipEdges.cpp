#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <cmath> // Για τις γωνίες
#include "flipEdges.h"

// Ορισμός των τύπων της CGAL
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;
typedef CDT::Edge Edge;
typedef CDT::Face_handle FaceHandle;

// Συνάρτηση για τον υπολογισμό της γωνίας μεταξύ δύο σημείων και του κοινό σημείο
template <typename P>
double angle_between(const P& p1, const P& p2, const P& p3) {
    double a = std::sqrt(CGAL::squared_distance(p2, p3));
    double b = std::sqrt(CGAL::squared_distance(p1, p3));
    double c = std::sqrt(CGAL::squared_distance(p1, p2));

    // Χρήση του νόμου του συνημιτόνου για τον υπολογισμό της γωνίας
    double cos_angle = (b * b + c * c - a * a) / (2 * b * c);
    return std::acos(cos_angle) * 180.0 / M_PI; // Επιστροφή της γωνίας σε μοίρες
}

// Συνάρτηση που ελέγχει αν ένα τρίγωνο είναι αμβλυγώνιο (έχει γωνία > 90)
template <typename FaceHandle>
bool is_obtuse_triangle(const FaceHandle& face) {
    double angle1 = angle_between(face->vertex(0)->point(), face->vertex(1)->point(), face->vertex(2)->point());
    double angle2 = angle_between(face->vertex(1)->point(), face->vertex(2)->point(), face->vertex(0)->point());
    double angle3 = angle_between(face->vertex(2)->point(), face->vertex(0)->point(), face->vertex(1)->point());
    
    // Έλεγχος αν κάποια από τις γωνίες είναι μεγαλύτερη από 90 μοίρες
    return (angle1 > 90.0 || angle2 > 90.0 || angle3 > 90.0);
}

// Συνάρτηση για την περιστροφή της διαγωνίου
template <typename CDT>
void flip_if_obtuse(CDT& cdt) {
    for (auto edge = cdt.finite_edges_begin(); edge != cdt.finite_edges_end(); ++edge) {
        FaceHandle face1 = edge->first; // Ένα τρίγωνο
        FaceHandle face2 = face1->neighbor(edge->second); // Το γειτονικό τρίγωνο
        
        // Ελέγχουμε αν και τα δύο τρίγωνα έχουν αμβλείες γωνίες
        if (is_obtuse_triangle(face1) || is_obtuse_triangle(face2)) {
            // Δοκιμάζουμε την περιστροφή της διαγωνίου
            if (cdt.is_flipable(edge->first, edge->second)) {  // Corrected line
                cdt.flip(edge->first, edge->second); // Αν είναι επιτρεπτό, περιστρέφουμε τη διαγώνιο
            }
        }
    }
}

int flip_edges() {
    // Αρχικοποίηση της Τριγωνοποίησης Delaunay με περιορισμούς (CDT)
    CDT cdt;

    // Παράδειγμα σημείων (μπορείς να τα αντικαταστήσεις με αυτά που παίρνεις από το executable)
    std::vector<Point> points = {
        Point(7817, 13), Point(2871, 1116), Point(523, 8263), Point(734, 9043),
        Point(4077, 8616), Point(7583, 9635), Point(7855, 8978), Point(6153, 6995), Point(5927, 6541), Point(5267, 5779)
    };

    // Εισαγωγή σημείων στην τριγωνοποίηση
    for (const Point& p : points) {
        cdt.insert(p);
    }

    // Ορισμός και προσθήκη περιορισμένων ακμών
    // cdt.insert_constraint(points[0], points[3]);
    // cdt.insert_constraint(points[1], points[2]);
    // cdt.insert_constraint(points[4], points[7]);

    // Δοκιμή περιστροφής ακμών για να βελτιώσουμε τα τρίγωνα
    flip_if_obtuse(cdt);

    // Σχεδιασμός της τριγωνοποίησης
    CGAL::draw(cdt);

    return 0;
}
