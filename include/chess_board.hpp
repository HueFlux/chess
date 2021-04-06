#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#include <SFML/Graphics.hpp>
#include "piece.hpp"
#include <array>
#include <vector>
#include <string>

class ChessBoard : public sf::Drawable {
    public:
        // Constructor which takes the desired size and position of the board
        ChessBoard(float board_size, float x = 0, float y = 0);

        virtual ~ChessBoard();

        // Color objects for light and dark square colors
        static inline const sf::Color light{240, 217, 181};
        static inline const sf::Color dark{148, 111, 81};
        // Method load a board position using FEN
        void loadPositionFromFEN(const std::string& fen);

    private:
        // 2D array containing the RectangleShapes for each board square
        std::array<std::array<sf::RectangleShape, 8>, 8> square_rectangles;
        float board_size;
        // Coordinates of the top left corner of the board
        sf::Vector2f board_origin;
        sf::Vector2f square_size;
        // Logical chess board structured as [file][rank]
        // with the rank going in decending order i.e. 8 to 1
        Piece square[8][8];
        // Texture to hold piece set sprite sheet
        sf::Texture piece_textures;

        sf::Sprite white_king;
        sf::Sprite black_king;
        // Vectors containing the Sprites for each type of piece
        std::vector <sf::Sprite> white_pawns;
        std::vector <sf::Sprite> black_pawns;
        std::vector <sf::Sprite> white_knights;
        std::vector <sf::Sprite> black_knights;
        std::vector <sf::Sprite> white_bishops;
        std::vector <sf::Sprite> black_bishops;
        std::vector <sf::Sprite> white_rooks;
        std::vector <sf::Sprite> black_rooks;
        std::vector <sf::Sprite> white_queens;
        std::vector <sf::Sprite> black_queens;

        // Overridden draw method to draw CubeFace to the RenderTarget
        virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const;
};

#endif
