#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>


struct Color {
    int r, g, b;
};


struct ColorBucket {
    std::vector<Color> pixels; 
    int min_r, max_r;        
    int min_g, max_g;          
    int min_b, max_b;        
};


int get_max_range_channel(const ColorBucket& bucket) {
    int range_r = bucket.max_r - bucket.min_r;
    int range_g = bucket.max_g - bucket.min_g;
    int range_b = bucket.max_b - bucket.min_b;
    
    // 1 es red 2 es green 3 blue
    if (range_r >= range_g && range_r >= range_b) return 1;
    if (range_g >= range_r && range_g >= range_b) return 2;
    return 3;
}


void median_cut(ColorBucket& bucket, std::vector<Color>& palette, int depth, int max_depth) {
    // si no hay pixels muere
    if (bucket.pixels.empty()) {
        return;
    }

    // prof max osea todos los colores para sacar prom
    if (depth == max_depth) {
        
        long sum_r = 0, sum_g = 0, sum_b = 0;
        for (const auto& p : bucket.pixels) {
            sum_r += p.r;
            sum_g += p.g;
            sum_b += p.b;
        }
        
        
        palette.push_back({
            (int)std::round((double)sum_r / bucket.pixels.size()),
            (int)std::round((double)sum_g / bucket.pixels.size()),
            (int)std::round((double)sum_b / bucket.pixels.size())
        });
        return;
    }

    //el qur mas rango tenga
    int channel = get_max_range_channel(bucket);

    //se ordenan los pixeles y enontrar mediana
    if (channel == 1) { // red
        std::sort(bucket.pixels.begin(), bucket.pixels.end(), [](const Color& a, const Color& b) { return a.r < b.r; });
    } else if (channel == 2) { // verde
        std::sort(bucket.pixels.begin(), bucket.pixels.end(), [](const Color& a, const Color& b) { return a.g < b.g; });
    } else { // azuk 
        std::sort(bucket.pixels.begin(), bucket.pixels.end(), [](const Color& a, const Color& b) { return a.b < b.b; });
    }

    //pixel entre dos cubos nuevos
    size_t median_index = bucket.pixels.size() / 2;
    
    ColorBucket bucket1, bucket2;
    
    bucket1.pixels.assign(bucket.pixels.begin(), bucket.pixels.begin() + median_index);
    bucket2.pixels.assign(bucket.pixels.begin() + median_index, bucket.pixels.end());

    //se copian los rangos
    bucket1.min_r = bucket2.min_r = bucket.min_r;
    bucket1.max_r = bucket2.max_r = bucket.max_r;
    bucket1.min_g = bucket2.min_g = bucket.min_g;
    bucket1.max_g = bucket2.max_g = bucket.max_g;
    bucket1.min_b = bucket2.min_b = bucket.min_b;
    bucket1.max_b = bucket2.max_b = bucket.max_b;
    
    
    median_cut(bucket1, palette, depth + 1, max_depth);
    median_cut(bucket2, palette, depth + 1, max_depth);
}


int main() {
   
    ColorBucket initial_bucket = {
        // pixels (std::vector<Color>)
        {
            {255, 10, 20}, {250, 15, 25},                  // reds
            {100, 200, 50}, {105, 195, 55}, {95, 205, 45}, // greens
            {10, 10, 240}, {15, 15, 235}, {20, 10, 250}    // azzules
        },
        // min_r 
        10, 
        // max_r 
        255, 
        // min_g
        10, 
        // max_g
        205,
        // min_b
        20, 
        // max_b
        250
    }; 

    
    const int target_colors = 8; 
    
    //  prof max es log2(num colores)
    const int max_depth = (target_colors > 0) ? (int)std::round(std::log2(target_colors)) : 0; 

    
    std::vector<Color> final_palette;

    std::cout << "\n pix iniciales " << initial_bucket.pixels.size() << "\n";
    std::cout << " colores objetivo " << target_colors << " (prof: " << max_depth << ")\n";

   
    median_cut(initial_bucket, final_palette, 0, max_depth);

 
    std::cout << "\npaleta final  (" << final_palette.size() << " colores)\n";
    for (size_t i = 0; i < final_palette.size(); ++i) {
        const auto& c = final_palette[i];
        std::cout << std::setw(2) << i + 1 << ": r=" << std::setw(3) << c.r 
                  << ", g=" << std::setw(3) << c.g 
                  << ", b=" << std::setw(3) << c.b << "\n";
    }

    return 0;
}