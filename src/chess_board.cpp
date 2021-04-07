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
    square_size(sf::Vector2f(board_size / 8, board_size / 8)),
    sprite_size(189),
    selected_piece_type(Piece::Type::None),
    selected_piece_v_index(-1),
    selected_piece(sf::Vector2i(-1, -1))
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
                    white_king.setTextureRect(sf::IntRect(sprite_size, sprite_size, sprite_size, sprite_size));
                    white_king.setPosition(board_origin.x + square_size.x * file,
                                           board_origin.y + square_size.y * rank);
                    white_king.setScale(board_size / (sprite_size * 8) , board_size / (sprite_size * 8));
                }
                else {
                    black_king.setTexture(piece_textures);
                    black_king.setTextureRect(sf::IntRect(sprite_size, 0, sprite_size, sprite_size));
                    black_king.setPosition(board_origin.x + square_size.x * file,
                                           board_origin.y + square_size.y * rank);
                    black_king.setScale(board_size / (sprite_size * 8), board_size / (sprite_size * 8));
                }
            }
            else {
                sf::Sprite piece;
                piece.setTexture(piece_textures);
                piece.setPosition(board_origin.x + square_size.x * file,
                                  board_origin.y + square_size.y * rank);
                piece.setScale(board_size / (sprite_size * 8) , board_size / (sprite_size * 8));

                if (square[file][rank].type == Piece::Type::Pawn) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 3, sprite_size, sprite_size, sprite_size));
                        white_pawns.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 3, 0, sprite_size, sprite_size));
                        black_pawns.push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Knight) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 2, sprite_size, sprite_size, sprite_size));
                        white_knights.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 2, 0, sprite_size, sprite_size));
                        black_knights.push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Bishop) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(0, sprite_size, sprite_size, sprite_size));
                        white_bishops.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(0, 0, sprite_size, sprite_size));
                        black_bishops.push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Rook) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 5, sprite_size, sprite_size, sprite_size));
                        white_rooks.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 5, 0, sprite_size, sprite_size));
                        black_rooks.push_back(piece);
                    }
                }
                else {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 4, sprite_size, sprite_size, sprite_size));
                        white_queens.push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 4, 0, sprite_size, sprite_size));
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
    int i = 0;
    for (int file = 0, rank = 0; i < fen.length() && fen[i] != ' '; i++) {
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
    i++;
    if (i >= fen.length()) {
        std::cout << "No active color provided." << std::endl;
        return;
    }
    if (fen[i] == 'w') {
        active_color = Piece::Color::White;
    }
    else if (fen[i] == 'b') {
        active_color = Piece::Color::Black;
    }
    else {
        std::cout << "Invalid active color." << std::endl;
    }

}

void ChessBoard::selectPiece(const sf::Vector2i& mouse_position) {
    float relative_x = mouse_position.x - board_origin.x;
    float relative_y = mouse_position.y - board_origin.y;
    selected_piece_v_index = -1;
    // Check if mouse is inside board
    if (relative_x > board_size || relative_y > board_size ||
        relative_x < 0 || relative_y < 0) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        return;
    }
    int file = static_cast<int> (relative_x / square_size.x);
    int rank = static_cast<int> (relative_y / square_size.y);

    if (square[file][rank].type == Piece::Type::None) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        return;
    }
    // TODO: Add logic for capture
    if (square[file][rank].color != active_color) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        return;
    }

    selected_piece.x = file;
    selected_piece.y = rank;
    selected_piece_type = square[file][rank].type;
}

void ChessBoard::updateSelectedPiecePosition(const sf::Vector2i& new_position) {
    int file = selected_piece.x;
    int rank = selected_piece.y;

    if (file == -1 || rank == -1) {
        return;
    }
    if (square[file][rank].type == Piece::Type::King) {
        if (square[file][rank].color == Piece::Color::White) {
            white_king.setOrigin(sprite_size/2, sprite_size/2);
            white_king.setPosition(new_position.x, new_position.y);
        }
        else {
            black_king.setOrigin(sprite_size/2, sprite_size/2);
            black_king.setPosition(new_position.x, new_position.y);
        }
    }
    else if (square[file][rank].type == Piece::Type::Pawn) {
        if (square[file][rank].color == Piece::Color::White) {
            if (selected_piece_v_index != -1) {
                white_pawns[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                white_pawns[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < white_pawns.size(); i++) {
                    if (static_cast<int> (white_pawns[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (white_pawns[i].getPosition().y) / square_size.y == rank) {
                        white_pawns[i].setOrigin(sprite_size/2, sprite_size/2);
                        white_pawns[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
        else {
            if (selected_piece_v_index != -1) {
                black_pawns[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                black_pawns[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < black_pawns.size(); i++) {
                    if (static_cast<int> (black_pawns[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (black_pawns[i].getPosition().y) / square_size.y == rank) {
                        black_pawns[i].setOrigin(sprite_size/2, sprite_size/2);
                        black_pawns[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
    }
    else if (square[file][rank].type == Piece::Type::Knight) {
        if (square[file][rank].color == Piece::Color::White) {
            if (selected_piece_v_index != -1) {
                white_knights[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                white_knights[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < white_knights.size(); i++) {
                    if (static_cast<int> (white_knights[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (white_knights[i].getPosition().y) / square_size.y == rank) {
                        white_knights[i].setOrigin(sprite_size/2, sprite_size/2);
                        white_knights[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
        else {
            if (selected_piece_v_index != -1) {
                black_knights[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                black_knights[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < black_knights.size(); i++) {
                    if (static_cast<int> (black_knights[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (black_knights[i].getPosition().y) / square_size.y == rank) {
                        black_knights[i].setOrigin(sprite_size/2, sprite_size/2);
                        black_knights[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
    }
    else if (square[file][rank].type == Piece::Type::Bishop) {
        if (square[file][rank].color == Piece::Color::White) {
            if (selected_piece_v_index != -1) {
                white_bishops[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                white_bishops[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < white_bishops.size(); i++) {
                    if (static_cast<int> (white_bishops[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (white_bishops[i].getPosition().y) / square_size.y == rank) {
                        white_bishops[i].setOrigin(sprite_size/2, sprite_size/2);
                        white_bishops[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
        else {
            if (selected_piece_v_index != -1) {
                black_bishops[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                black_bishops[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < black_bishops.size(); i++) {
                    if (static_cast<int> (black_bishops[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (black_bishops[i].getPosition().y) / square_size.y == rank) {
                        black_bishops[i].setOrigin(sprite_size/2, sprite_size/2);
                        black_bishops[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
    }
    else if (square[file][rank].type == Piece::Type::Rook) {
        if (square[file][rank].color == Piece::Color::White) {
            if (selected_piece_v_index != -1) {
                white_rooks[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                white_rooks[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < white_rooks.size(); i++) {
                    if (static_cast<int> (white_rooks[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (white_rooks[i].getPosition().y) / square_size.y == rank) {
                        white_rooks[i].setOrigin(sprite_size/2, sprite_size/2);
                        white_rooks[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
        else {
            if (selected_piece_v_index != -1) {
                black_rooks[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                black_rooks[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < black_rooks.size(); i++) {
                    if (static_cast<int> (black_rooks[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (black_rooks[i].getPosition().y) / square_size.y == rank) {
                        black_rooks[i].setOrigin(sprite_size/2, sprite_size/2);
                        black_rooks[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
    }
    else {
        if (square[file][rank].color == Piece::Color::White) {
            if (selected_piece_v_index != -1) {
                white_queens[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                white_queens[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < white_queens.size(); i++) {
                    if (static_cast<int> (white_queens[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (white_queens[i].getPosition().y) / square_size.y == rank) {
                        white_queens[i].setOrigin(sprite_size/2, sprite_size/2);
                        white_queens[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
            }
        }
        else {
            if (selected_piece_v_index != -1) {
                black_queens[selected_piece_v_index].setOrigin(sprite_size/2, sprite_size/2);
                black_queens[selected_piece_v_index].setPosition(new_position.x, new_position.y);
            }
            else {
                for (int i = 0; i < black_queens.size(); i++) {
                    if (static_cast<int> (black_queens[i].getPosition().x) / square_size.x == file &&
                            static_cast<int> (black_queens[i].getPosition().y) / square_size.y == rank) {
                        black_queens[i].setOrigin(sprite_size/2, sprite_size/2);
                        black_queens[i].setPosition(new_position.x, new_position.y);
                        selected_piece_v_index = i;
                        break;
                    }
                }
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
