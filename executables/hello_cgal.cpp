#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <iostream>

typedef CGAL::Simple_cartesian<double> Kernel;
// typedef Kernel::Point_2 ,Point_2;

int main() {
    Kernel::Point_2 p(1.0, 1.0); // Δημιουργούμε ένα σημείο (1.0, 1.0)
    std::cout << "Hello CGAL! Το σημείο p είναι: " << p << std::endl;
    return 0;
}
