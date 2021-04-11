#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "chess_board.hpp"
#include "piece.hpp"
#include <iostream>

// Function used to maintain the view aspect ratio as the window size changes
sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight);

int main() {
    // Create window
    int res_x = 800;
    int res_y = 800;
    sf::RenderWindow window(sf::VideoMode(res_x, res_y), "Chess", (sf::Style::Resize + sf::Style::Close));
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Create a view
    sf::View view;
    view.setSize(res_x, res_y);
    view.setCenter(view.getSize().x / 2, view.getSize().y / 2);

    // Create chess board
    ChessBoard board(res_x);
    bool mouse_pressed = false;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    view = getLetterboxView(view, event.size.width, event.size.height);
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        mouse_pressed = true;
                        board.selectPiece(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        board.dropPiece(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                        mouse_pressed = false;
                    }
                    break;
                default:
                    break;
            }
         }

         if (mouse_pressed) {
             board.updateSelectedPiecePosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
         }

         window.clear();
         window.setView(view);
         window.draw(board);
         window.display();
    }
    return 0;
}

// https://github.com/SFML/SFML/wiki/Source%3A-Letterbox-effect-using-a-view
sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {

    // Compares the aspect ratio of the window to the aspect ratio of the view,
    // and sets the view's viewport accordingly in order to archieve a letterbox effect.
    // A new view (with a new viewport set) is returned.

    float windowRatio = windowWidth / (float) windowHeight;
    float viewRatio = view.getSize().x / (float) view.getSize().y;
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    // If horizontalSpacing is true, the black bars will appear on the left and right side.
    // Otherwise, the black bars will appear on the top and bottom.

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.f;
    }

    else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));

    return view;
}
