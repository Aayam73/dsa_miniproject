#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <functional>

struct Box {
    sf::RectangleShape rect;
    sf::Text label;
};

struct Button {
    sf::RectangleShape shape;
    sf::Text label;
    std::function<void()> onClick;

    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2f mousePos);
};

struct DropDown {
    Button mainButton;
    std::vector<Button> options;
    bool open = false;
};

class ArrayVisualizer {
public:
    ArrayVisualizer(std::vector<int>& arr, sf::Font& fontRef);
    void run();

private:
    sf::RenderWindow window;
    sf::Font& font;
    std::vector<int>& array;

    std::vector<Box> arrayBoxes;
    DropDown sortDropDown;
    std::vector<Button> buttons;

    std::atomic<bool> sortingInProgress{false};
    std::thread sortThread;
    std::string currentSort;

    const int cols = 10;
    const int boxSize = 40;
    const int spacing = 5;

    void initBoxes();
    void updateBoxes();
    void initSortDropDown(float x, float y, const std::vector<std::string>& sortNames);
    void initRandomizeButton();
    void initStartButton();
    void pollEvents();
    void draw();
    void drawLegend();
    void bubbleSort();
    void selectionSort();
};