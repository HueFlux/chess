#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "piece.hpp"
#include "move.hpp"
#include <array>
#include <vector>
#include <string>

class ChessBoard : public sf::Drawable {
    public:
        // Constructor which takes the desired size and position of the board
        ChessBoard(float board_size, float x = 0, float y = 0);

        virtual ~ChessBoard();

        // Color objects for light, dark and highlight square colors
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
        sf::Vector2i findPieceSprite(int file, int rank) const;
        // Method used to move a piece and update their corresponding sprite's position
        // on the board
        void movePiece(int file, int rank, int new_file, int new_rank);
        // Method used to update the board for the next move
        void nextMove();
        // Overloaded method used to update a piece's sprite position on the board
        void updateSpritePosition(int file, int rank, int new_file, int new_rank);
        void updateSpritePosition(int file, int rank, const sf::Vector2f& new_position);
        // Method used to toggle the pawn promotion menu for the given color and file
        void togglePawnPromotionMenu(Piece::Color color, int file);
        // Method used to generate all legal moves for the given color
        void generateMoves(Piece::Color color);
        // Helper methods to generate legal moves for each piece type
        void generatePawnMoves(int file, int rank);
        void generateRookMoves(int start_file, int start_rank);
        void generateBishopMoves(int start_file, int start_rank);
        void generateKnightMoves(int start_file, int start_rank);
        void generateKingMoves(int start_file, int start_rank);
        // Method used to check if a move is legal
        bool isLegalMove(const Move& move) const;
        // Method used to validate and add a move to the list of legal moves
        void addMove(int start_file, int start_rank, int target_file, int target_rank);
        // Helper method used validate psuedo-legal moves
        bool isValidLegalMove(int start_file, int start_rank, int target_file, int target_rank);
        // Method used to determine if a color is currently in check
        bool inCheck(Piece::Color color) const;

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
        Piece::Color active_color = Piece::Color::White;
        // Keeps track of number of half-moves made since starting position
        int move_count = 0;
        // Boolean to keep track of checks
        bool check = false;
        // Booleans to keep track of castling availability;
        bool white_king_side_castle = false;
        bool white_queen_side_castle = false;
        bool black_king_side_castle = false;
        bool black_queen_side_castle = false;
        // Contains the file and rank of an en passant target square
        // (-1, -1) if there is none
        sf::Vector2i en_passant;
        // Boolean to activate pawn promotion menu
        bool pawn_promotion = false;
        // Keeps track of the file of the pawn to be promoted
        int pawn_promotion_file;
        // Contains the file and rank of currently selected piece
        // (-1, -1) if no piece has been selected
        sf::Vector2i selected_piece;
        Piece::Type selected_piece_type = Piece::Type::None;
        // Contains the indices of the sprite in pieces array
        // corresponding to the selected piece
        sf::Vector2i selected_sprite;
        // RectangleShapes used to hightlight squares from the last move
        std::array<sf::RectangleShape, 2> last_move;
        // RectangleShape used to hightlight the selected square
        sf::RectangleShape selected_square;
        // RectangleShape used to highlight the king's square during check
        sf::RectangleShape check_square;
        // Texture to hold piece set sprite sheet
        sf::Texture piece_textures;
        // Size of a single piece sprite in pixels
        int sprite_size = 189;
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
        //      pieces[10] : white king
        //      pieces[11] : black king
        std::array<std::vector<sf::Sprite>, 12> pieces;
        // RectangleShape and sprites for the pawn promotion menu
        sf::RectangleShape pawn_promotion_menu_box;
        std::array<sf::Sprite, 4> pawn_promotion_menu_sprites;
        // Array to store SoundBuffers for all in game sounds
        std::array<sf::SoundBuffer, 2> sound_buffers;
        sf::Sound move_sound;
        sf::Sound capture_sound;
        // Vector to store all legal moves from current position
        std::vector<Move> legalMoves;
        // Overridden draw method to draw ChessBoard to the RenderTarget
        virtual void draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const;
};

#endif
