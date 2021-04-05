#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#include <SFML/Graphics.hpp>
#include <array>

class ChessBoard : public sf::Drawable {
    public:
        ChessBoard(float board_size);

        // Color objects for light and dark square colors
        static inline const sf::Color light{240, 217, 181};
        static inline const sf::Color dark{148, 111, 81};

        virtual ~ChessBoard();

    private:
        std::array<std::array<sf::RectangleShape, 8>, 8> squares;
        float board_size;
        sf::Vector2f square_size;

        // Overridden draw method to draw CubeFace to the RenderTarget
        virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const;
};

#endif
