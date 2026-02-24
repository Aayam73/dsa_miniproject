#include "ArrayVisualizer.h"
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iostream>

// ------------------ Button ------------------
void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}

bool Button::isClicked(sf::Vector2f mousePos) {
    return shape.getGlobalBounds().contains(mousePos);
}

// ------------------ ArrayVisualizer ------------------
ArrayVisualizer::ArrayVisualizer(std::vector<int>& arr, sf::Font& fontRef)
    : array(arr), font(fontRef)
{
    window.create(sf::VideoMode(1200, 600), "Box Sorting Visualizer");
    currentSort = "Select Sort";
    initSortDropDown(800, 50, {"Selection", "Bubble"});
    initRandomizeButton();
    initStartButton();
    initBoxes();
}

void ArrayVisualizer::run() {
    while (window.isOpen()) {
        pollEvents();
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    sortingInProgress = false;
}

// ------------------ Boxes ------------------
void ArrayVisualizer::initBoxes() {
    arrayBoxes.clear();
    
    // Centering calculations
    int totalCols = cols;
    int totalRows = (array.size() + cols - 1) / cols;
    float arrayWidth = totalCols * (boxSize + spacing) - spacing;
    float arrayHeight = totalRows * (boxSize + spacing) - spacing;

    float offsetX = (800 - arrayWidth) / 2;   // 800 is array section width (leaving 400px for buttons)
    float offsetY = 150;                      // start below buttons (leave top margin)

    for (size_t i = 0; i < array.size(); ++i) {
        Box b;
        int row = i / cols;
        int col = i % cols;
        b.rect = sf::RectangleShape(sf::Vector2f(boxSize, boxSize));
        b.rect.setFillColor(sf::Color::Red);
        b.rect.setPosition(offsetX + col * (boxSize + spacing), offsetY + row * (boxSize + spacing));

        b.label.setFont(font);
        b.label.setString(std::to_string(array[i]));
        b.label.setCharacterSize(16);
        b.label.setFillColor(sf::Color::White);
        b.label.setPosition(offsetX + col * (boxSize + spacing) + 8, offsetY + row * (boxSize + spacing) + 8);

        arrayBoxes.push_back(b);
    }
}

void ArrayVisualizer::updateBoxes() {
    int totalCols = cols;
    int totalRows = (array.size() + cols - 1) / cols;
    float arrayWidth = totalCols * (boxSize + spacing) - spacing;
    float arrayHeight = totalRows * (boxSize + spacing) - spacing;

    float offsetX = (800 - arrayWidth) / 2;
    float offsetY = 150;

    for (size_t i = 0; i < array.size(); ++i) {
        int row = i / cols;
        int col = i % cols;

        arrayBoxes[i].label.setString(std::to_string(array[i]));
        arrayBoxes[i].rect.setFillColor(sf::Color::Red);
        arrayBoxes[i].rect.setPosition(offsetX + col * (boxSize + spacing), offsetY + row * (boxSize + spacing));
        arrayBoxes[i].label.setPosition(offsetX + col * (boxSize + spacing) + 8, offsetY + row * (boxSize + spacing) + 8);
    }
}

// ------------------ UI ------------------
void ArrayVisualizer::initSortDropDown(float x, float y, const std::vector<std::string>& sortNames) {
    sortDropDown.mainButton.shape = sf::RectangleShape(sf::Vector2f(150, 40));
    sortDropDown.mainButton.shape.setFillColor(sf::Color(100,100,200));
    sortDropDown.mainButton.shape.setPosition(x, y);

    sortDropDown.mainButton.label.setFont(font);
    sortDropDown.mainButton.label.setString(currentSort);
    sortDropDown.mainButton.label.setCharacterSize(18);
    sortDropDown.mainButton.label.setFillColor(sf::Color::White);
    sortDropDown.mainButton.label.setPosition(x+10, y+5);

    sortDropDown.mainButton.onClick = [this]() { sortDropDown.open = !sortDropDown.open; };

    float optionY = y + 40;
    for (auto& name : sortNames) {
        Button btn;
        btn.shape = sf::RectangleShape(sf::Vector2f(150, 40));
        btn.shape.setFillColor(sf::Color(80, 80, 180));
        btn.shape.setPosition(x, optionY);

        btn.label.setFont(font);
        btn.label.setString(name);
        btn.label.setCharacterSize(16);
        btn.label.setFillColor(sf::Color::White);
        btn.label.setPosition(x+10, optionY+5);

        btn.onClick = [this, name]() {
            currentSort = name;
            sortDropDown.mainButton.label.setString(currentSort);
            sortDropDown.open = false;
        };
        sortDropDown.options.push_back(btn);
        optionY += 40;
    }
}

void ArrayVisualizer::initRandomizeButton() {
    Button randBtn;
    randBtn.shape = sf::RectangleShape(sf::Vector2f(150, 40));
    randBtn.shape.setFillColor(sf::Color(100,200,100));
    randBtn.shape.setPosition(800, 10);
    randBtn.label.setFont(font);
    randBtn.label.setString("Randomize Array");
    randBtn.label.setCharacterSize(16);
    randBtn.label.setFillColor(sf::Color::White);
    randBtn.label.setPosition(810, 15);

    randBtn.onClick = [this]() {
        sortingInProgress = false;
        if (sortThread.joinable()) sortThread.join();
        for (auto& x : array) x = rand() % 99 + 1;
        updateBoxes();
    };
    buttons.push_back(randBtn);
}

void ArrayVisualizer::initStartButton() {
    Button startBtn;
    startBtn.shape = sf::RectangleShape(sf::Vector2f(150, 40));
    startBtn.shape.setFillColor(sf::Color(200,100,100));
    startBtn.shape.setPosition(800, 100);
    startBtn.label.setFont(font);
    startBtn.label.setString("Start Sorting");
    startBtn.label.setCharacterSize(16);
    startBtn.label.setFillColor(sf::Color::White);
    startBtn.label.setPosition(810, 105);

    startBtn.onClick = [this]() {
        if (currentSort == "Select Sort") {
            sf::RenderWindow popup(sf::VideoMode(400,100), "Error", sf::Style::Titlebar | sf::Style::Close);
            sf::Text text("Please select a sort type!", font, 20);
            text.setFillColor(sf::Color::White);
            text.setPosition(50, 30);

            while (popup.isOpen()) {
                sf::Event event;
                while (popup.pollEvent(event))
                    if (event.type == sf::Event::Closed) popup.close();
                popup.clear(sf::Color(50,50,50));
                popup.draw(text);
                popup.display();
            }
            return;
        }

        if (!sortingInProgress) {
            sortingInProgress = true;
            sortThread = std::thread([this]() {
                if (currentSort=="Bubble") bubbleSort();
                else if (currentSort=="Selection") selectionSort();
            });
            sortThread.detach();
        }
    };
    buttons.push_back(startBtn);
}

// ------------------ Events & Draw ------------------
void ArrayVisualizer::pollEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            for (auto& btn : buttons) if (btn.isClicked(mousePos)) btn.onClick();
            if (sortDropDown.mainButton.isClicked(mousePos)) sortDropDown.mainButton.onClick();
            if (sortDropDown.open)
                for (auto& opt : sortDropDown.options) if (opt.isClicked(mousePos)) opt.onClick();
        }
    }
}

void ArrayVisualizer::drawLegend() {
    float x = 800, y = 200;
    sf::RectangleShape compareBox(sf::Vector2f(20,20)); compareBox.setFillColor(sf::Color::Yellow); compareBox.setPosition(x, y);
    sf::Text compareLabel("Compare", font, 16); compareLabel.setFillColor(sf::Color::White); compareLabel.setPosition(x+30, y);
    window.draw(compareBox); window.draw(compareLabel);

    sf::RectangleShape swapBox(sf::Vector2f(20,20)); swapBox.setFillColor(sf::Color(186,160,255)); swapBox.setPosition(x, y+30);
    sf::Text swapLabel("Swap", font, 16); swapLabel.setFillColor(sf::Color::White); swapLabel.setPosition(x+30, y+30);
    window.draw(swapBox); window.draw(swapLabel);

    sf::RectangleShape sortedBox(sf::Vector2f(20,20)); sortedBox.setFillColor(sf::Color::Green); sortedBox.setPosition(x, y+60);
    sf::Text sortedLabel("Sorted", font, 16); sortedLabel.setFillColor(sf::Color::White); sortedLabel.setPosition(x+30, y+60);
    window.draw(sortedBox); window.draw(sortedLabel);

    sf::RectangleShape unsortedBox(sf::Vector2f(20,20)); unsortedBox.setFillColor(sf::Color::Red); unsortedBox.setPosition(x, y+90);
    sf::Text unsortedLabel("Unsorted", font, 16); unsortedLabel.setFillColor(sf::Color::White); unsortedLabel.setPosition(x+30, y+90);
    window.draw(unsortedBox); window.draw(unsortedLabel);
}

void ArrayVisualizer::draw() {
    window.clear(sf::Color::Black);
    for (auto& b : arrayBoxes) window.draw(b.rect), window.draw(b.label);
    drawLegend();
    for (auto& btn : buttons) btn.draw(window);
    for (auto& opt : sortDropDown.options) if (sortDropDown.open) opt.draw(window);
    sortDropDown.mainButton.draw(window);
    window.display();
}

// ------------------ Sorting ------------------
void ArrayVisualizer::bubbleSort() {
    int n = array.size();
    for (int i=0;i<n-1 && sortingInProgress;i++)
        for (int j=0;j<n-i-1 && sortingInProgress;j++){
            arrayBoxes[j].rect.setFillColor(sf::Color::Yellow);
            arrayBoxes[j+1].rect.setFillColor(sf::Color::Yellow);
            draw(); std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (!sortingInProgress) return;
            if (array[j]>array[j+1]) std::swap(array[j], array[j+1]);
            updateBoxes();
        }
    if (sortingInProgress) {
        for (auto& b : arrayBoxes) b.rect.setFillColor(sf::Color::Green);
        draw();
    }
    sortingInProgress=false;
}

void ArrayVisualizer::selectionSort() {
    int n = array.size();
    for (int i=0;i<n-1 && sortingInProgress;i++){
        int minIdx=i;
        for (int j=i+1;j<n && sortingInProgress;j++){
            arrayBoxes[j].rect.setFillColor(sf::Color::Yellow);
            arrayBoxes[minIdx].rect.setFillColor(sf::Color::Yellow);
            draw(); std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (!sortingInProgress) return;
            if (array[j]<array[minIdx]) minIdx=j;
        }
        if (!sortingInProgress) return;
        std::swap(array[i], array[minIdx]);
        updateBoxes();
    }
    if (sortingInProgress) {
        for (auto& b : arrayBoxes) b.rect.setFillColor(sf::Color::Green);
        draw();
    }
    sortingInProgress=false;
}