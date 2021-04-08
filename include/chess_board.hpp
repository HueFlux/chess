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

        // Color objects for light, dark and hightlight square colors
        static inline const sf::Color light{240, 217, 181};
        static inline const sf::Color dark{148, 111, 81};
        static inline const sf::Color highlight{155, 199, 0, 104};
        // Method load a board position using FEN
        void loadPositionFromFEN(const std::string& fen);
        // Method used to find and select piece under the mouse
        void selectPiece(const sf::Vector2f& mouse_position);
        // Method used to update position of selected piece to mouse position
        void updateSelectedPiecePosition(const sf::Vector2f& new_position);
        // Method used to update board after dropping a piece
        void dropPiece(const sf::Vector2f& mouse_position);
        // Method used to find the indices of a piece's corresponding
        // sprite in the pieces array
        sf::Vector2i findPieceSprite(int file, int rank);

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
        // Current color to move
        Piece::Color active_color;
        int move_count;
        // Contains the file and rank of currently selected piece
        // (-1, -1) if no piece has been selected
        sf::Vector2i selected_piece;
        Piece::Type selected_piece_type;
        // Contains the indices of the sprite in pieces array
        // corresponding to the selected piece
        sf::Vector2i selected_sprite;
        // RectangleShapes used to hightlight squares from the last move
        std::array<sf::RectangleShape, 2> last_move;
        // RectangleShape used to hightlight the selected square
        sf::RectangleShape selected_square;
        // Texture to hold piece set sprite sheet
        sf::Texture piece_textures;
        // Size of a single piece sprite in pixels
        int sprite_size;
        // King sprites
        sf::Sprite white_king;
        sf::Sprite black_king;
        // Array of vectors containing the Sprites for every other type of piece
        // Structure is:
        //      pieces[0] : white pawns
        //      pieces[1] : black pawns
        //      pieces[2] : white knights
        //      pieces[3] : black knights
        //      pieces[4] : white bishops
        //      pieces[5] : black bishops
        //      pieces[6] : white rooks
        //      pieces[7] : black rooks
        //      pieces[8] : white queens
        //      pieces[9] : black queens
        std::array<std::vector<sf::Sprite>, 10> pieces;


        // Overridden draw method to draw CubeFace to the RenderTarget
        virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const;
};

#endif
