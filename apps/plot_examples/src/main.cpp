#include <cmath>
#include <iostream>
#include "pyplot_cpp/pyplot_cpp.h"

using namespace std;
using namespace Pyplotcpp;

// TODO: This code is a work-in-progress. It is not yet functional.
int main()
{
    // Prepare data
    int ncols = 500, nrows = 300;
    Eigen::ArrayXXf z(nrows, ncols);
    for (int j=0; j<nrows; ++j) {
        for (int i=0; i<ncols; ++i) {
            z(j,i) = std::sin(std::hypot(i - ncols/2, j - nrows/2));
        }
    }

    title("My matrix");
    imagesc(z, {0,1});
    //colorbar(mat);

    // Show plots
    show();
    //close();
}