#include "chess_board.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
    selected_piece(sf::Vector2i(-1, -1)),
    selected_sprite(sf::Vector2i(-1, -1))
{
    // Load piece textures
    if (!piece_textures.loadFromFile("../res/pieces/maestro/maestro_pieces.png")) {
        std::cout << "Load failed" << std::endl;
        system("pause");
    }
    piece_textures.setSmooth(true);
    // Load sound files
    if (!sound_buffers[0].loadFromFile("../res/sounds/move.ogg")) {
        std::cout << "Load failed" << std::endl;
        system("pause");
    }
    move_sound.setBuffer(sound_buffers[0]);
    if (!sound_buffers[1].loadFromFile("../res/sounds/capture.ogg")) {
        std::cout << "Load failed" << std::endl;
        system("pause");
    }
    capture_sound.setBuffer(sound_buffers[1]);

    loadPositionFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    for (sf::RectangleShape& square : last_move) {
        square.setSize(square_size);
        square.setFillColor(highlight);
    }
    selected_square.setSize(square_size);
    selected_square.setFillColor(highlight);

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
                        pieces[0].push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 3, 0, sprite_size, sprite_size));
                        pieces[1].push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Knight) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 2, sprite_size, sprite_size, sprite_size));
                        pieces[2].push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 2, 0, sprite_size, sprite_size));
                        pieces[3].push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Bishop) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(0, sprite_size, sprite_size, sprite_size));
                        pieces[4].push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(0, 0, sprite_size, sprite_size));
                        pieces[5].push_back(piece);
                    }
                }
                else if (square[file][rank].type == Piece::Type::Rook) {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 5, sprite_size, sprite_size, sprite_size));
                        pieces[6].push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 5, 0, sprite_size, sprite_size));
                        pieces[7].push_back(piece);
                    }
                }
                else {
                    if (square[file][rank].color == Piece::Color::White) {
                        piece.setTextureRect(sf::IntRect(sprite_size * 4, sprite_size, sprite_size, sprite_size));
                        pieces[8].push_back(piece);
                    }
                    else {
                        piece.setTextureRect(sf::IntRect(sprite_size * 4, 0, sprite_size, sprite_size));
                        pieces[9].push_back(piece);
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
    // Parse piece positions
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
    // Parse active color
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
    // Parse castling availability
    i += 2;
    for ( ; i < fen.length() && fen[i] != ' '; i++) {
        if (fen[i] == 'K') {
            white_king_side_castle = true;
        }
        else if (fen[i] == 'Q') {
            white_queen_side_castle = true;
        }
        else if (fen[i] == 'k') {
            black_king_side_castle = true;
        }
        else if (fen[i] == 'q') {
            black_queen_side_castle = true;
        }
        else {
            std::cout << "Invalid symbol." << std::endl;
        }
    }

}

void ChessBoard::selectPiece(const sf::Vector2f& mouse_position) {
    float relative_x = mouse_position.x - board_origin.x;
    float relative_y = mouse_position.y - board_origin.y;
    selected_sprite.x = selected_sprite.y = -1;
    // Check if mouse is inside board
    if (relative_x > board_size || relative_y > board_size ||
        relative_x < 0 || relative_y < 0) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        return;
    }
    int file = static_cast<int> (relative_x / square_size.x);
    int rank = static_cast<int> (relative_y / square_size.y);
    // Selected empty square
    if (square[file][rank].type == Piece::Type::None) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        return;
    }
    // Selected opponent's piece
    if (square[file][rank].color != active_color) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        return;
    }

    selected_piece.x = file;
    selected_piece.y = rank;
    selected_piece_type = square[file][rank].type;
    // Update highlight square position
    selected_square.setPosition(board_origin.x + square_size.x * file,
                                board_origin.y + square_size.y * rank);
}

void ChessBoard::updateSelectedPiecePosition(const sf::Vector2f& new_position) {
    int file = selected_piece.x;
    int rank = selected_piece.y;

    if (file == -1 || rank == -1) {
        return;
    }
    if (square[file][rank].type == Piece::Type::King) {
        if (square[file][rank].color == Piece::Color::White) {
            white_king.setPosition(new_position.x - square_size.x/2,
                                   new_position.y - square_size.y/2);
        }
        else {
            black_king.setPosition(new_position.x - square_size.x/2,
                                   new_position.y - square_size.y/2);
        }
        return;
    }
    if (selected_sprite.x == -1 && selected_sprite.y == -1) {
        selected_sprite = findPieceSprite(file, rank);
    }
    pieces[selected_sprite.x][selected_sprite.y].setPosition(new_position.x - square_size.x/2,
                                                             new_position.y - square_size.y/2);
}

void ChessBoard::dropPiece(const sf::Vector2f& mouse_position) {
    // No piece has been selected
    if (selected_piece.x == -1 || selected_piece.y == -1) {
        return;
    }
    float relative_x = mouse_position.x - board_origin.x;
    float relative_y = mouse_position.y - board_origin.y;
    int file = static_cast<int> (relative_x / square_size.x);
    int rank = static_cast<int> (relative_y / square_size.y);
    // Reset selected piece's sprite position
    sf::Vector2f original_position(square_size.x * selected_piece.x + square_size.x/2,
                                   square_size.y * selected_piece.y + square_size.y/2);
    updateSelectedPiecePosition(original_position);
    // Mouse is outside of board or on another piece of the same color
    // or the opponent's king
    if (relative_x > board_size || relative_y > board_size
        || relative_x < 0 || relative_y < 0
        || (square[file][rank].type != Piece::Type::None && square[file][rank].color == active_color)
        || square[file][rank].type == Piece::Type::King) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        selected_sprite.x = selected_sprite.y = -1;
        return;
    }
    // Perform move
    movePiece(selected_piece.x, selected_piece.y, file, rank);
    // Reset selected piece variables
    selected_piece.x = selected_piece.y = -1;
    selected_piece_type = Piece::Type::None;
    selected_sprite.x = selected_sprite.y = -1;

    active_color = (active_color == Piece::Color::White) ? Piece::Color::Black : Piece::Color::White;
    move_count++;
    // Update last move highlight squares
    last_move[0].setPosition(selected_square.getPosition());
    last_move[1].setPosition(board_origin.x + square_size.x * file,
                             board_origin.y + square_size.y * rank);
}

void ChessBoard::movePiece(int file, int rank, int new_file, int new_rank) {
    if (file < 0 || file > 7
        || rank < 0 || rank > 7
        || new_file < 0 || new_file > 7
        || new_rank < 0 || new_rank > 7) {
        return;
    }
    // Handle castling if king move
    if (square[file][rank].type == Piece::Type::King) {
        if (square[file][rank].color == Piece::Color::White) {
            if (new_rank == 7 && new_file == 6) {
                if (white_king_side_castle && square[5][7].type == Piece::Type::None
                                           && square[6][7].type == Piece::Type::None
                                           && square[7][7].type == Piece::Type::Rook
                                           && square[7][7].color == Piece::Color::White) {
                    // Move kingside rook to the other side of the king
                    movePiece(7, 7, 5, 7);
                }
            }
            else if (new_rank == 7 && new_file == 2) {
                if (white_queen_side_castle && square[1][7].type == Piece::Type::None
                                            && square[2][7].type == Piece::Type::None
                                            && square[3][7].type == Piece::Type::None
                                            && square[0][7].type == Piece::Type::Rook
                                            && square[0][7].color == Piece::Color::White) {
                    // Move queenside rook to the other side of the king
                    movePiece(0, 7, 3, 7);
                }
            }
            white_king_side_castle = false;
            white_queen_side_castle = false;
        }
        else {
            if (new_rank == 0 && new_file == 6) {
                if (black_king_side_castle && square[5][0].type == Piece::Type::None
                                           && square[6][0].type == Piece::Type::None
                                           && square[7][0].type == Piece::Type::Rook
                                           && square[7][0].color == Piece::Color::Black) {
                    // Move kingside rook to the other side of the king
                    movePiece(7, 0, 5, 0);
                }
            }
            else if (new_rank == 0 && new_file == 2) {
                if (black_queen_side_castle && square[1][0].type == Piece::Type::None
                                            && square[2][0].type == Piece::Type::None
                                            && square[3][0].type == Piece::Type::None
                                            && square[0][0].type == Piece::Type::Rook
                                            && square[0][0].color == Piece::Color::Black) {
                    // Move queenside rook to the other side of the king
                    movePiece(0, 0, 3, 0);
                }
            }
            black_king_side_castle = false;
            black_queen_side_castle = false;
        }
    }
    // Handle castling rights if rook move
    if (square[file][rank].type == Piece::Type::Rook) {
        if (square[file][rank].color == Piece::Color::White) {
            if (rank == 7 && file == 7) {
                white_king_side_castle = false;
            }
            else if (rank == 7 && file == 0) {
                white_queen_side_castle = false;
            }
        }
        else {
            if (rank == 0 && file == 7) {
                black_king_side_castle = false;
            }
            else if (rank == 0 && file == 0) {
                black_queen_side_castle = false;
            }
        }
    }
    // Regular move
    if (square[new_file][new_rank].type == Piece::Type::None) {
        updateSpritePosition(file, rank, new_file, new_rank);
        square[new_file][new_rank] = square[file][rank];
        square[file][rank].type = Piece::Type::None;
        move_sound.play();
    }
    // Capture
    else if (square[new_file][new_rank].color != active_color) {
        updateSpritePosition(file, rank, new_file, new_rank);
        // Erase captured piece's sprite
        sf::Vector2i captured_sprite = findPieceSprite(new_file, new_rank);
        pieces[captured_sprite.x].erase(pieces[captured_sprite.x].begin() + captured_sprite.y);

        square[new_file][new_rank] = square[file][rank];
        square[file][rank].type = Piece::Type::None;
        capture_sound.play();
    }
}

void ChessBoard::updateSpritePosition(int file, int rank, const sf::Vector2f& new_position) {
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return;
    }
    if (square[file][rank].type == Piece::Type::King) {
        if (square[file][rank].color == Piece::Color::White) {
            white_king.setPosition(new_position.x,
                                   new_position.y);
        }
        else {
            black_king.setPosition(new_position.x,
                                   new_position.y);
        }
        return;
    }
    sf::Vector2i piece_sprite = findPieceSprite(file, rank);
    pieces[piece_sprite.x][piece_sprite.y].setPosition(new_position.x,
                                                       new_position.y);
}

void ChessBoard::updateSpritePosition(int file, int rank, int new_file, int new_rank) {
    if (new_file < 0 || new_file > 7 || new_rank < 0 || new_rank > 7) {
        return;
    }
    updateSpritePosition(file, rank, sf::Vector2f(square_size.x * new_file,
                                              square_size.y * new_rank));
}

sf::Vector2i ChessBoard::findPieceSprite(int file, int rank) {
    sf::Vector2i indices(-1, -1);
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return indices;
    }

    if (square[file][rank].type == Piece::Type::Pawn) {
        if (square[file][rank].color == Piece::Color::White) {
            indices.x = 0;
            for (int i = 0; i < pieces[0].size(); i++) {
                if (static_cast<int> (pieces[0][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[0][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
        else {
            indices.x = 1;
            for (int i = 0; i < pieces[1].size(); i++) {
                if (static_cast<int> (pieces[1][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[1][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
    }
    else if (square[file][rank].type == Piece::Type::Knight) {
        if (square[file][rank].color == Piece::Color::White) {
            indices.x = 2;
            for (int i = 0; i < pieces[2].size(); i++) {
                if (static_cast<int> (pieces[2][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[2][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
        else {
            indices.x = 3;
            for (int i = 0; i < pieces[3].size(); i++) {
                if (static_cast<int> (pieces[3][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[3][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
    }
    else if (square[file][rank].type == Piece::Type::Bishop) {
        if (square[file][rank].color == Piece::Color::White) {
            indices.x = 4;
            for (int i = 0; i < pieces[4].size(); i++) {
                if (static_cast<int> (pieces[4][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[4][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
        else {
            indices.x = 5;
            for (int i = 0; i < pieces[5].size(); i++) {
                if (static_cast<int> (pieces[5][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[5][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
    }
    else if (square[file][rank].type == Piece::Type::Rook) {
        if (square[file][rank].color == Piece::Color::White) {
            indices.x = 6;
            for (int i = 0; i < pieces[6].size(); i++) {
                if (static_cast<int> (pieces[6][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[6][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
        else {
            indices.x = 7;
            for (int i = 0; i < pieces[7].size(); i++) {
                if (static_cast<int> (pieces[7][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[7][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
    }
    else {
        if (square[file][rank].color == Piece::Color::White) {
            indices.x = 8;
            for (int i = 0; i < pieces[8].size(); i++) {
                if (static_cast<int> (pieces[8][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[8][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
        else {
            indices.x = 9;
            for (int i = 0; i < pieces[9].size(); i++) {
                if (static_cast<int> (pieces[9][i].getPosition().x) / square_size.x == file &&
                        static_cast<int> (pieces[9][i].getPosition().y) / square_size.y == rank) {
                    indices.y = i;
                    break;
                }
            }
        }
    }
    return indices;
}

void ChessBoard::draw(sf::RenderTarget &renderTarget, sf::RenderStates renderStates) const {
    // Draw board
    for (int file = 0; file < square_rectangles.size(); file++) {
        for (int rank = 0; rank < square_rectangles.size(); rank++) {
            renderTarget.draw(square_rectangles[file][rank]);
        }
    }
    // Draw highlight squares
    if (selected_piece.x != -1 && selected_piece.y != -1) {
        renderTarget.draw(selected_square);
    }
    if (move_count > 0) {
        for (const auto& square : last_move) {
            renderTarget.draw(square);
        }
    }
    // Draw pieces
    for (int i = 0; i < pieces.size(); i++) {
        for (int j = 0; j < pieces[i].size(); j++) {
            if (i == selected_sprite.x && j == selected_sprite.y) {
                continue;
            }
            renderTarget.draw(pieces[i][j]);
        }
    }
    if (active_color == Piece::Color::White) {
        renderTarget.draw(black_king);
        renderTarget.draw(white_king);
    } else {
        renderTarget.draw(white_king);
        renderTarget.draw(black_king);
    }
    if (selected_sprite.x != -1 && selected_sprite.y != -1) {
        renderTarget.draw(pieces[selected_sprite.x][selected_sprite.y]);
    }
}
