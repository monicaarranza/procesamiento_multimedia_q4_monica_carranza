#include <vector>
#include <iostream>
#include <algorithm>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

const int matrixSize = 4;
const int matrix[matrixSize][matrixSize] = {
    {16, 144, 48, 176},
    {208, 80, 240, 112},
    {64, 192, 32, 160},
    {240, 128, 224, 96}
};

std::vector<unsigned char> apply_ordered_dithering(const std::vector<unsigned char>& image_greyscale, int width, int height){

    std::vector<unsigned char> newImage(image_greyscale.size());

    for(int y=0; y<height; y++ ){
        for(int x=0; x<width;x++){
            int index =y*width+x;
            int valor_pixel= image_greyscale[index];

            int mat_x = x% matrixSize;
            int mat_y = y%matrixSize;

            int valor_umbral=matrix[mat_y][mat_x];

           

            if(valor_pixel< valor_umbral){
                newImage[index]=0;

            } else{
                newImage[index]=255;
            }
        }

    }
    return newImage;


}

PYBIND11_MODULE(dither_cpp, m) {
    m.def("dither_4x4", &apply_ordered_dithering);
}

