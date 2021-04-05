#include "chess_board.hpp"

ChessBoard::ChessBoard(float board_size) :
    board_size(board_size),
    square_size(sf::Vector2f(board_size / 8, board_size / 8))
{
    sf::Vector2f board_origin(0,0);
    for (int file = 0; file < squares.size(); file++) {
        for (int rank = 0; rank < squares.size(); rank++) {
            squares[file][rank].setSize(square_size);
            squares[file][rank].setPosition(board_origin.x + square_size.x * file,
                                            board_origin.y + square_size.y * rank);

            bool is_light_square = (file + rank) % 2 == 0;

            if (is_light_square) {
                squares[file][rank].setFillColor(light);
            }
            else {
                squares[file][rank].setFillColor(dark);
            }

        }
    }
}

ChessBoard::~ChessBoard() {
}

void ChessBoard::draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const {
    for (int file = 0; file < squares.size(); file++) {
        for (int rank = 0; rank < squares.size(); rank++) {
            renderTarget.draw(squares[file][rank]);
        }
    }
}
