#include "chess_board.hpp"
#include "piece.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <exception>

ChessBoard::ChessBoard(float board_size, float x, float y) :
    board_size(board_size),
    board_origin(sf::Vector2f(x, y)),
    square_size(sf::Vector2f(board_size / 8, board_size / 8))
{
    if (!piece_textures.loadFromFile("../res/pieces/maestro/maestro_pieces.png")) {
        std::cout << "Load failed" << std::endl;
        system("pause");
    }
    piece_textures.setSmooth(true);

    loadPositionFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    for (int file = 0; file < square_rectangles.size(); file++) {
        for (int rank = 0; rank < square_rectangles.size(); rank++) {
            square_rectangles[file][rank].setSize(square_size);
            square_rectangles[file][rank].setPosition(board_origin.x + square_size.x * file,
                                            board_origin.y + square_size.y * rank);

            bool is_light_square = (file + rank) % 2 == 0;

            if (is_light_square) {
                square_rectangles[file][rank].setFillColor(light);
            }
            else {
                square_rectangles[file][rank].setFillColor(dark);
            }

            if (square[file][rank].type == Piece::Type::None) {
                continue;
            }
            if (square[file][rank].type == Piece::Type::King) {
                if (square[file][rank].color == Piece::Color::White) {
                    white_king.setTexture(piece_textures);
                    white_king.setTextureRect(sf::IntRect(189, 189, 189, 189));
                    white_king.setPosition(board_origin.x + square_size.x * file,
                                           board_origin.y + square_size.y * rank);
                    white_king.setScale(board_size / (189 * 8) , board_size / (189 * 8));
                }
                else {
                    black_king.setTexture(piece_textures);
                    black_king.setTextureRect(sf::IntRect(189, 0, 189, 189));
                    black_king.setPosition(board_origin.x + square_size.x * file,
                                           board_origin.y + square_size.y * rank);
                    black_king.setScale(board_size / (189 * 8), board_size / (189 * 8));
                }
            }
            else {
                sf::Sprite piece;
                piece.setTexture(piece_textures);
                piece.setPosition(board_origin.x + square_size.x * file,
                                  board_origin.y + square_size.y * rank);
                piece.setScale(board_size / (189 * 8) , board_size / (189 * 8));

                if (square[file][rank].type == Piece::Type::Pawn) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(189 * 3, 189, 189, 189));
                        white_pawns.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(189 * 3, 0, 189, 189));
                        black_pawns.push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Knight) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(189 * 2, 189, 189, 189));
                        white_knights.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(189 * 2, 0, 189, 189));
                        black_knights.push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Bishop) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(0, 189, 189, 189));
                        white_bishops.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(0, 0, 189, 189));
                        black_bishops.push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Rook) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(189 * 5, 189, 189, 189));
                        white_rooks.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(189 * 5, 0, 189, 189));
                        black_rooks.push_back(piece);
                    }
                }
                else {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(189 * 4, 189, 189, 189));
                        white_queens.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(189 * 4, 0, 189, 189));
                        black_queens.push_back(piece);
                    }
                }
            }
        }
    }
}

ChessBoard::~ChessBoard() {
}

void ChessBoard::loadPositionFromFEN(const std::string& fen) {
    std::unordered_map<char, Piece::Type> charToPieceType = {
        {'k', Piece::Type::King},
        {'p', Piece::Type::Pawn},
        {'n', Piece::Type::Knight},
        {'b', Piece::Type::Bishop},
        {'r', Piece::Type::Rook},
        {'q', Piece::Type::Queen}
    };
    int file = 0, rank = 0;
    for (int i = 0; i < fen.length() && fen[i] != ' '; i++) {
        if (std::isdigit(fen[i])) {
            file += fen[i] - '0';
            if (file > 8) {
                std::cout << "Invalid FEN string." << std::endl;
            }
        }
        else if (fen[i] == '/') {
            rank++;
            file = 0;
        }
        else {
            try {
                square[file][rank].type = charToPieceType.at(std::tolower(fen[i]));
                if (std::islower(fen[i])) {
                    square[file][rank].color = Piece::Color::Black;
                }
                else {
                    square[file][rank].color = Piece::Color::White;
                }
                file++;
            } catch (std::out_of_range& e) {
                std::cout << "Invalid symbol." << std::endl;
            }
        }
    }

}

void ChessBoard::draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const {
    for (int file = 0; file < square_rectangles.size(); file++) {
        for (int rank = 0; rank < square_rectangles.size(); rank++) {
            renderTarget.draw(square_rectangles[file][rank]);
        }
    }
    // Draw pieces
    renderTarget.draw(white_king);
    renderTarget.draw(black_king);

    for (const sf::Sprite& s : white_pawns) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : black_pawns) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : white_knights) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : black_knights) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : white_bishops) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : black_bishops) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : white_rooks) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : black_rooks) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : white_queens) {
        renderTarget.draw(s);
    }
    for (const sf::Sprite& s : black_queens) {
        renderTarget.draw(s);
    }
}
