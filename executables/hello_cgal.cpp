/*/#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <vector>

int main() {
    // Δημιουργία του property tree
    boost::property_tree::ptree pt;

    // Ανάγνωση από JSON αρχείο
    try {
        read_json("input.json", pt);
    } catch (const boost::property_tree::json_parser_error &e) {
        std::cerr << "Error reading JSON: " << e.what() << std::endl;
        return 1;
    }

    // Ανάκτηση δεδομένων από το property tree
    std::string instance_uid = pt.get<std::string>("instance_uid");
    int num_points = pt.get<int>("num_points");

    // Ανάκτηση σημείων
    std::vector<int> points_x;
    for (const auto& point : pt.get_child("points_x")) {
        points_x.push_back(point.second.get_value<int>());
    }

    std::vector<int> points_y;
    for (const auto& point : pt.get_child("points_y")) {
        points_y.push_back(point.second.get_value<int>());
    }


    // Ανάκτηση περιορισμών
    int num_constraints = pt.get<int>("num_constraints");
    std::vector<std::pair<int, int>> additional_constraints;

    for (const auto& constraint : pt.get_child("additional_constraints")) {
        // Οι κόμβοι για κάθε περιορισμό είναι μορφής [0, 1]
        auto it = constraint.second.begin();
        int first = it->second.get_value<int>(); // Πρώτο στοιχείο
        ++it; // Μετακίνηση στον επόμενο κόμβο
        int second = it->second.get_value<int>(); // Δεύτερο στοιχείο
        additional_constraints.emplace_back(first, second);
    }

    // Εκτύπωση δεδομένων
    std::cout << "Instance UID: " << instance_uid << std::endl;
    std::cout << "Number of Points: " << num_points << std::endl;
    std::cout << "Points X: ";
    for (const auto& x : points_x) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Points Y: ";
    for (const auto& y : points_y) {
        std::cout << y << " ";
    }
    std::cout << std::endl;

    std::cout << "Number of Constraints: " << num_constraints << std::endl;
    std::cout << "Additional Constraints: " << std::endl;
    for (const auto& constraint : additional_constraints) {
        std::cout << "[" << constraint.first << ", " << constraint.second << "]" << std::endl;
    }

    return 0;
}*/



// #include <QApplication>
// #include <QWidget>
// #include <QPainter>
// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/Delaunay_triangulation_2.h>
// #include <vector>

// // Define a simple Cartesian kernel and the triangulation type
// typedef CGAL::Simple_cartesian<double> K;
// typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
// typedef Delaunay::Point Point;

// class DelaunayWidget : public QWidget {
// public:
//     DelaunayWidget(QWidget *parent = nullptr) : QWidget(parent) {
//         // Sample points
//         points.push_back(Point(100, 100));
//         points.push_back(Point(200, 50));
//         points.push_back(Point(300, 150));
//         points.push_back(Point(250, 250));
//         points.push_back(Point(150, 200));

//         // Create the Delaunay triangulation
//         dt.insert(points.begin(), points.end());
//     }

// protected:
//     void paintEvent(QPaintEvent *event) override {
//         QPainter painter(this);
//         painter.setRenderHint(QPainter::Antialiasing);

//         // Draw the triangulation
//         for (auto e = dt.finite_edges_begin(); e != dt.finite_edges_end(); ++e) {
//             auto segment = dt.segment(e);
//             painter.drawLine(QPointF(segment.source().x(), segment.source().y()),
//                              QPointF(segment.target().x(), segment.target().y()));
//         }

//         // Draw the points
//         painter.setBrush(Qt::red);
//         for (const auto &p : points) {
//             painter.drawEllipse(QPointF(p.x(), p.y()), 3, 3);
//         }
//     }

// private:
//     std::vector<Point> points;
//     Delaunay dt;
// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     DelaunayWidget widget;
//     widget.resize(400, 400);
//     widget.show();

//     return app.exec();
// }



#include <QApplication>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef Delaunay::Point Point;

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Define a set of points
    std::vector<Point> points;
    points.push_back(Point(100, 100));
    points.push_back(Point(200, 50));
    points.push_back(Point(300, 150));
    points.push_back(Point(250, 250));
    points.push_back(Point(150, 200));

    // Create a Delaunay triangulation with the points
    Delaunay dt;
    dt.insert(points.begin(), points.end());

    // Use CGAL's built-in draw function to display the triangulation
    CGAL::draw(dt);

    return app.exec();
}


