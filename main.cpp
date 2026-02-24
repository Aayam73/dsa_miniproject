#include "ArrayVisualizer.h"
#include <vector>
#include <cstdlib>

int main() {
    std::vector<int> arr(10);
    for (auto& x : arr) x = rand() % 99 + 1;

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) return -1;

    ArrayVisualizer viz(arr, font);
    viz.run();

    return 0;
}