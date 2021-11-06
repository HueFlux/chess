#include "chess_board.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "piece.hpp"
#include "move.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <exception>
#include <algorithm>

ChessBoard::ChessBoard(float board_size, float x, float y) :
    board_size(board_size),
    board_origin(sf::Vector2f(x, y)),
    square_size(sf::Vector2f(board_size / 8, board_size / 8)),
    selected_piece(sf::Vector2i(-1, -1)),
    selected_sprite(sf::Vector2i(-1, -1)),
    en_passant(sf::Vector2i(-1, -1))
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
    generateMoves(active_color);

    for (sf::RectangleShape& square : last_move) {
        square.setSize(square_size);
        square.setFillColor(highlight);
    }
    selected_square.setSize(square_size);
    selected_square.setFillColor(highlight);

    check_square.setSize(square_size);
    check_square.setFillColor(sf::Color(255, 0, 0, 178));

    // Create board squares and piece sprites
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

            sf::Sprite piece;
            piece.setTexture(piece_textures);
            piece.setPosition(board_origin.x + square_size.x * file,
                              board_origin.y + square_size.y * rank);
            piece.setScale(board_size / (sprite_size * 8) , board_size / (sprite_size * 8));

            if (square[file][rank].type == Piece::Type::King) {
                if (square[file][rank].color == Piece::Color::White) {
                    piece.setTextureRect(sf::IntRect(sprite_size, sprite_size, sprite_size, sprite_size));
                    pieces[10].push_back(piece);

                }
                else {
                    piece.setTextureRect(sf::IntRect(sprite_size, 0, sprite_size, sprite_size));
                    pieces[11].push_back(piece);
                }
            }
            else if (square[file][rank].type == Piece::Type::Pawn) {
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
    // Set up pawn promotion menu box and sprites
    pawn_promotion_menu_box.setSize(sf::Vector2f(square_size.x, square_size.y * 4));
    pawn_promotion_menu_box.setFillColor(sf::Color(255, 255, 255));

    for (sf::Sprite& piece : pawn_promotion_menu_sprites) {
        piece.setTexture(piece_textures);
        piece.setScale(board_size / (sprite_size * 8) , board_size / (sprite_size * 8));
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
    // Parse en passant target square
    i++;
    if (i + 1 >= fen.length()) {
        return;
    }
    if (fen[i] != '-') {
        int file = fen[i] - 'a';
        int rank = 7 - (fen[++i] - '1');
        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            std::cout << "Invalid en passant target square." << std::endl;
        } else {
            en_passant.x = file;
            en_passant.y = rank;
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

    // Pawn Promotion
    if (pawn_promotion) {
        if (active_color == Piece::Color::White) {
            if (file != pawn_promotion_file || rank < 0 || rank > 3) {
                return;
            }
            // Delete pawn sprite
            sf::Vector2i pawn_sprite = findPieceSprite(file, 0);
            pieces[pawn_sprite.x].erase(pieces[pawn_sprite.x].begin() + pawn_sprite.y);

            if (rank == 0) { // Promote to queen
                square[file][0].type = Piece::Type::Queen;
                pieces[8].push_back(pawn_promotion_menu_sprites[0]);
                pieces[8].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 0);
            }
            else if (rank == 1) { // Promote to knight
                square[file][0].type = Piece::Type::Knight;
                pieces[2].push_back(pawn_promotion_menu_sprites[1]);
                pieces[2].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 0);
            }
            else if (rank == 2) { // Promote to rook
                square[file][0].type = Piece::Type::Rook;
                pieces[6].push_back(pawn_promotion_menu_sprites[2]);
                pieces[6].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 0);
            }
            else if (rank == 3) { // Promote to bishop
                square[file][0].type = Piece::Type::Bishop;
                pieces[4].push_back(pawn_promotion_menu_sprites[3]);
                pieces[4].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 0);
            }

        }
        else {
            if (file != pawn_promotion_file || rank < 4 || rank > 7) {
                return;
            }
            // Delete pawn sprite
            sf::Vector2i pawn_sprite = findPieceSprite(file, 7);
            pieces[pawn_sprite.x].erase(pieces[pawn_sprite.x].begin() + pawn_sprite.y);

            if (rank == 7) { // Promote to queen
                square[file][7].type = Piece::Type::Queen;
                pieces[9].push_back(pawn_promotion_menu_sprites[3]);
                pieces[9].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 7);
            }
            else if (rank == 6) { // Promote to knight
                square[file][7].type = Piece::Type::Knight;
                pieces[3].push_back(pawn_promotion_menu_sprites[2]);
                pieces[3].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 7);
            }
            else if (rank == 5) { // Promote to rook
                square[file][7].type = Piece::Type::Rook;
                pieces[7].push_back(pawn_promotion_menu_sprites[1]);
                pieces[7].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 7);
            }
            else if (rank == 4) { // Promote to bishop
                square[file][7].type = Piece::Type::Bishop;
                pieces[5].push_back(pawn_promotion_menu_sprites[0]);
                pieces[5].back().setPosition(board_origin.x + square_size.x * file,
                                             board_origin.y + square_size.x * 7);
            }
        }
        pawn_promotion = false;
        nextMove();
        return;
    }

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
    // Mouse is outside of board
    if (relative_x > board_size || relative_y > board_size
        || relative_x < 0 || relative_y < 0) {
        selected_piece.x = selected_piece.y = -1;
        selected_piece_type = Piece::Type::None;
        selected_sprite.x = selected_sprite.y = -1;
        return;
    }
    if (!isLegalMove(Move(selected_piece.x, selected_piece.y, file, rank))) {
        return;
    }
    // Perform move
    movePiece(selected_piece.x, selected_piece.y, file, rank);
    // Reset selected piece variables
    selected_piece.x = selected_piece.y = -1;
    selected_piece_type = Piece::Type::None;
    selected_sprite.x = selected_sprite.y = -1;
    // Update last move highlight squares
    last_move[0].setPosition(selected_square.getPosition());
    last_move[1].setPosition(board_origin.x + square_size.x * file,
                             board_origin.y + square_size.y * rank);
    // Pawn promotion
    if (pawn_promotion) {
        togglePawnPromotionMenu(active_color, file);
        return;
    }
    nextMove();
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
        if (new_rank == rank && new_file >= 6) {
            // Move kingside rook to the other side of the king
            movePiece(7, rank, 5, rank);
            // Handle king position when placed on top of rook
            new_file = 6;
        }
        else if (new_rank == rank && (new_file == 2 || new_file == 0)) {
            // Move queenside rook to the other side of the king
            movePiece(0, rank, 3, rank);
            // Handle king position when placed on top of rook
            new_file = 2;
        }
        if (square[file][rank].color == Piece::Color::White) {
            white_king_side_castle = false;
            white_queen_side_castle = false;
        }
        else {
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
    // En passant
    if (square[file][rank].type == Piece::Type::Pawn) {
        // En passant capture
        if (new_file == en_passant.x && new_rank == en_passant.y) {
            updateSpritePosition(file, rank, new_file, new_rank);
            // Erase captured pawn's sprite and position on board
            sf::Vector2i captured_sprite;
            if (square[file][rank].color == Piece::Color::White) {
                captured_sprite = findPieceSprite(new_file, new_rank + 1);
                square[new_file][new_rank + 1].type = Piece::Type::None;
            }
            else {
                captured_sprite = findPieceSprite(new_file, new_rank - 1);
                square[new_file][new_rank - 1].type = Piece::Type::None;
            }
            pieces[captured_sprite.x].erase(pieces[captured_sprite.x].begin() + captured_sprite.y);

            square[new_file][new_rank] = square[file][rank];
            square[file][rank].type = Piece::Type::None;
            capture_sound.play();
            // Reset en passant target square
            en_passant.x = -1;
            en_passant.y = -1;
            return;
        }
        // New en passant target square
        else {
            if (square[file][rank].color == Piece::Color::White && rank == 6 && new_rank == 4) {
                en_passant.x = file;
                en_passant.y = 5;
            }
            else if (square[file][rank].color == Piece::Color::Black && rank == 1 && new_rank == 3) {
                en_passant.x = file;
                en_passant.y = 2;
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
    // Pawn promotion
    if (square[new_file][new_rank].type == Piece::Type::Pawn) {
        if ((square[new_file][new_rank].color == Piece::Color::White && new_rank == 0)
            || (square[new_file][new_rank].color == Piece::Color::Black && new_rank == 7)) {
            pawn_promotion = true;
            pawn_promotion_file = new_file;
        }
    }
    // Reset en passant target square
    if (en_passant.x != -1 && en_passant.y != -1) {
        if ((active_color == Piece::Color::White && en_passant.y == 2)
            || (active_color == Piece::Color::Black && en_passant.y == 5)) {
            en_passant.x = -1;
            en_passant.y = -1;
        }
    }
}

void ChessBoard::nextMove() {
    active_color = (active_color == Piece::Color::White) ? Piece::Color::Black : Piece::Color::White;
    move_count++;
    // Checking move
    if (inCheck(active_color)) {
        std::cout << ((active_color == Piece::Color::White) ? "White" : "Black") << " is in check!\n";
        // Set check_square position
        if (active_color == Piece::Color::White) {
            check_square.setPosition(pieces[10][0].getPosition());
        } else {
            check_square.setPosition(pieces[11][0].getPosition());
        }
        check = true;
    } else {
        check = false;
    }
    generateMoves(active_color);
    // Checkmate and stalemate
    if (legalMoves.size() == 0) {
        if (check) {
            std::cout << "Checkmate! " << ((active_color == Piece::Color::White) ? "Black" : "White") << " wins!\n";
        } else {
            std::cout << "Stalemate! It's a draw!\n";
        }
    }
}

void ChessBoard::updateSpritePosition(int file, int rank, const sf::Vector2f& new_position) {
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
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

sf::Vector2i ChessBoard::findPieceSprite(int file, int rank) const {
    sf::Vector2i indices(-1, -1);
    if (file < 0 || file > 7 || rank < 0 || rank > 7) {
        return indices;
    }
    if (square[file][rank].type == Piece::Type::None) {
        return indices;
    }
    if (square[file][rank].type == Piece::Type::King) {
        if (square[file][rank].color == Piece::Color::White) {
            indices.x = 10;
            indices.y = 0;
        }
        else {
            indices.x = 11;
            indices.y = 0;
        }
    }
    else if (square[file][rank].type == Piece::Type::Pawn) {
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

void ChessBoard::togglePawnPromotionMenu(Piece::Color color, int file) {
    if (color == Piece::Color::White) {
        pawn_promotion_menu_box.setPosition(board_origin.x + square_size.x * file, 0);
        // Queen, Knight, Rook, Bishop
        pawn_promotion_menu_sprites[0].setTextureRect(sf::IntRect(sprite_size * 4, sprite_size,
                                                                  sprite_size, sprite_size));
        pawn_promotion_menu_sprites[1].setTextureRect(sf::IntRect(sprite_size * 2, sprite_size,
                                                                  sprite_size, sprite_size));
        pawn_promotion_menu_sprites[2].setTextureRect(sf::IntRect(sprite_size * 5, sprite_size,
                                                                  sprite_size, sprite_size));
        pawn_promotion_menu_sprites[3].setTextureRect(sf::IntRect(0, sprite_size,
                                                                  sprite_size, sprite_size));

        for (int i = 0; i < pawn_promotion_menu_sprites.size(); i++) {
            pawn_promotion_menu_sprites[i].setPosition(board_origin.x + square_size.x * file,
                                                       board_origin.y + square_size.x * i);
        }
    } else {
        pawn_promotion_menu_box.setPosition(board_origin.x + square_size.x * file,
                                            board_origin.y + square_size.y * 4);
        // Bishop, Rook, Queen, Knight
        pawn_promotion_menu_sprites[3].setTextureRect(sf::IntRect(sprite_size * 4, 0,
                                                                  sprite_size, sprite_size));
        pawn_promotion_menu_sprites[2].setTextureRect(sf::IntRect(sprite_size * 2, 0,
                                                                  sprite_size, sprite_size));
        pawn_promotion_menu_sprites[1].setTextureRect(sf::IntRect(sprite_size * 5, 0,
                                                                  sprite_size, sprite_size));
        pawn_promotion_menu_sprites[0].setTextureRect(sf::IntRect(0, 0,
                                                                  sprite_size, sprite_size));
        for (int i = 0; i < pawn_promotion_menu_sprites.size(); i++) {
            pawn_promotion_menu_sprites[i].setPosition(board_origin.x + square_size.x * file,
                                                       board_origin.y + square_size.x * (4 + i));
        }
    }
}

void ChessBoard::generateMoves(Piece::Color color) {
    legalMoves.clear();
    for (int file = 0; file < std::size(square); file++) {
        for (int rank = 0; rank < std::size(square[file]); rank++) {
            if (square[file][rank].color != color) {
                continue;
            }
            if (square[file][rank].type == Piece::Type::Pawn) {
                generatePawnMoves(file, rank);
            }
            else if (square[file][rank].type == Piece::Type::Bishop) {
                generateBishopMoves(file, rank);
            }
            else if (square[file][rank].type == Piece::Type::Rook) {
                generateRookMoves(file, rank);
            }
            else if (square[file][rank].type == Piece::Type::Queen) {
                generateBishopMoves(file, rank);
                generateRookMoves(file, rank);
            }
            else if (square[file][rank].type == Piece::Type::Knight) {
                generateKnightMoves(file, rank);
            }
            else if (square[file][rank].type == Piece::Type::King) {
                generateKingMoves(file, rank);
            }
        }
    }
}

void ChessBoard::generatePawnMoves(int file, int rank) {
    if (square[file][rank].color == Piece::Color::White) {
        if (square[file][rank - 1].type == Piece::Type::None) {
            addMove(file, rank, file, rank - 1);
        }
        if (rank == 6) {
            addMove(file, rank, file, rank - 2);
        }
        if (square[file - 1][rank - 1].type != Piece::Type::None
            || file - 1 == en_passant.x && rank - 1 == en_passant.y) {
            addMove(file, rank, file - 1, rank - 1);
        }
        if (square[file + 1][rank - 1].type != Piece::Type::None
            || file + 1 == en_passant.x && rank - 1 == en_passant.y) {
            addMove(file, rank, file + 1, rank - 1);
        }
    }
    else {
        if (square[file][rank + 1].type == Piece::Type::None) {
            addMove(file, rank, file, rank + 1);
        }
        if (rank == 1) {
            addMove(file, rank, file, rank + 2);
        }
        if (square[file - 1][rank + 1].type != Piece::Type::None
            || file - 1 == en_passant.x && rank + 1 == en_passant.y) {
            addMove(file, rank, file - 1, rank + 1);
        }
        if (square[file + 1][rank + 1].type != Piece::Type::None
            || file + 1 == en_passant.x && rank + 1 == en_passant.y) {
            addMove(file, rank, file + 1, rank + 1);
        }
    }
}

void ChessBoard::generateRookMoves(int start_file, int start_rank) {
    // Right
    for (int file = start_file + 1; file < std::size(square); file++) {
        if (square[file][start_rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, file, start_rank);
        }
        else if (square[file][start_rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, file, start_rank);
            break;
        }
        else {
            break;
        }
    }
    // Left
    for (int file = start_file - 1; file >= 0; file--) {
        if (square[file][start_rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, file, start_rank);
        }
        else if (square[file][start_rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, file, start_rank);
            break;
        }
        else {
            break;
        }
    }
    // Down
    for (int rank = start_rank + 1; rank < std::size(square); rank++) {
        if (square[start_file][rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, start_file, rank);
        }
        else if (square[start_file][rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, start_file, rank);
            break;
        }
        else {
            break;
        }
    }
    // Up
    for (int rank = start_rank - 1; rank >= 0; rank--) {
        if (square[start_file][rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, start_file, rank);
        }
        else if (square[start_file][rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, start_file, rank);
            break;
        }
        else {
            break;
        }
    }
}

void ChessBoard::generateBishopMoves(int start_file, int start_rank) {
    // Right & Down
    for (int file = start_file + 1, rank = start_rank + 1; file < std::size(square) && rank < std::size(square[file]); file++, rank++) {
        if (square[file][rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, file, rank);
        }
        else if (square[file][rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, file, rank);
            break;
        }
        else {
            break;
        }
    }
    // Left & Up
    for (int file = start_file - 1, rank = start_rank - 1; file >= 0 && rank >= 0; file--, rank--) {
        if (square[file][rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, file, rank);
        }
        else if (square[file][rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, file, rank);
            break;
        }
        else {
            break;
        }
    }
    // Right & Up
    for (int file = start_file + 1, rank = start_rank - 1; file < std::size(square) && rank >= 0; file++, rank--) {
        if (square[file][rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, file, rank);
        }
        else if (square[file][rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, file, rank);
            break;
        }
        else {
            break;
        }
    }
    // Left & Down
    for (int file = start_file - 1, rank = start_rank + 1; file >= 0 && rank < std::size(square[file]); file--, rank++) {
        if (square[file][rank].type == Piece::Type::None) {
            addMove(start_file, start_rank, file, rank);
        }
        else if (square[file][rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, file, rank);
            break;
        }
        else {
            break;
        }
    }
}

void ChessBoard::generateKnightMoves(int start_file, int start_rank) {
    std::array<std::array<int, 2>, 8> targets = {{
        {{start_file + 2, start_rank + 1}},
        {{start_file + 2, start_rank - 1}},
        {{start_file - 2, start_rank + 1}},
        {{start_file - 2, start_rank - 1}},
        {{start_file + 1, start_rank + 2}},
        {{start_file + 1, start_rank - 2}},
        {{start_file - 1, start_rank + 2}},
        {{start_file - 1, start_rank - 2}}
    }};

    for (const auto& target : targets) {
        if (target[0] < 0 || target[0] > 7 || target[1] < 0 || target[1] > 7) {
            continue;
        }
        if (square[target[0]][target[1]].type == Piece::Type::None
            || square[target[0]][target[1]].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, target[0], target[1]);
        }
    }
}

void ChessBoard::generateKingMoves(int start_file, int start_rank) {
    // Moves to rank above and below the king
    for (int file = start_file - 1; file < std::size(square) && file <= start_file + 1; file++) {
        if (file < 0) {
            continue;
        }
        if (start_rank - 1 >= 0) {
            if (square[file][start_rank - 1].type == Piece::Type::None
                || square[file][start_rank - 1].color != square[start_file][start_rank].color) {
                addMove(start_file, start_rank, file, start_rank - 1);
            }
        }
        if (start_rank + 1 < 8) {
            if (square[file][start_rank + 1].type == Piece::Type::None
                || square[file][start_rank + 1].color != square[start_file][start_rank].color) {
                addMove(start_file, start_rank, file, start_rank + 1);
            }
        }
    }
    // Moves to squares on either side of the king on the same rank
    if (start_file - 1 > 0) {
        if (square[start_file - 1][start_rank].type == Piece::Type::None
            || square[start_file - 1][start_rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, start_file - 1, start_rank);
        }
    }

    if (start_file + 1 < 8) {
        if (square[start_file + 1][start_rank].type == Piece::Type::None
            || square[start_file + 1][start_rank].color != square[start_file][start_rank].color) {
            addMove(start_file, start_rank, start_file + 1, start_rank);
        }
    }
    // Exclude castling moves if in check
    if (inCheck(square[start_file][start_rank].color)) {
        return;
    }
    // Castling moves
    Piece::Color color = square[start_file][start_rank].color;
    // Kingside castling
    if (color == Piece::Color::White && white_king_side_castle
        || color == Piece::Color::Black && black_king_side_castle) {
        if (square[5][start_rank].type == Piece::Type::None
            && square[6][start_rank].type == Piece::Type::None
            && square[7][start_rank].type == Piece::Type::Rook
            && square[7][start_rank].color == color) {
            addMove(start_file, start_rank, start_file + 2, start_rank);
            addMove(start_file, start_rank, start_file + 3, start_rank);
        }
    }
    // Queenside castling
    if (color == Piece::Color::White && white_queen_side_castle
        || color == Piece::Color::Black && black_queen_side_castle) {
        if (square[1][start_rank].type == Piece::Type::None
            && square[2][start_rank].type == Piece::Type::None
            && square[3][start_rank].type == Piece::Type::None
            && square[0][start_rank].type == Piece::Type::Rook
            && square[0][start_rank].color == color) {
            addMove(start_file, start_rank, start_file - 2, start_rank);
            addMove(start_file, start_rank, 0, start_rank);
        }
    }
}

bool ChessBoard::isLegalMove(const Move& move) const {
    return std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end();
}

void ChessBoard::addMove(int start_file, int start_rank, int target_file, int target_rank) {
    if (isValidLegalMove(start_file, start_rank, target_file, target_rank)) {
        legalMoves.emplace_back(start_file, start_rank, target_file, target_rank);
    }
}

bool ChessBoard::isValidLegalMove(int start_file, int start_rank, int target_file, int target_rank) {
    bool is_valid = true;
    Piece target_square = square[target_file][target_rank];
    Piece::Color active_color = square[start_file][start_rank].color;
    // Make move
    square[target_file][target_rank] = square[start_file][start_rank];
    square[start_file][start_rank].type = Piece::Type::None;
    // Validate
    is_valid = !(inCheck(active_color));
    // Undo move
    square[start_file][start_rank] = square[target_file][target_rank];
    square[target_file][target_rank] = target_square;

    return is_valid;
}

bool ChessBoard::inCheck(Piece::Color color) const {
    // file and rank of king
    int file = 0;
    int rank = 0;
    for ( ; file < std::size(square); file++) {
        for (rank = 0 ; rank < std::size(square[file]); rank++) {
            if (square[file][rank].color == color
                && square[file][rank].type == Piece::Type::King) {
                break;
            }
        }
        if (rank < std::size(square[file])) {
            break;
        }
    }

    // Attacking pawns
    if (color == Piece::Color::White) {
        if ((square[file - 1][rank - 1].color != color
                && square[file - 1][rank - 1].type == Piece::Type::Pawn)
            || (square[file + 1][rank - 1].color != color
                && square[file + 1][rank - 1].type == Piece::Type::Pawn)) {
            return true;
        }
    }
    else {
        if ((square[file - 1][rank + 1].color != color
                && square[file - 1][rank + 1].type == Piece::Type::Pawn)
            || (square[file + 1][rank + 1].color != color
                && square[file + 1][rank + 1].type == Piece::Type::Pawn)) {
            return true;
        }
    }
    // Attacking Rooks & Queens
    // Right
    for (int f = file + 1; f < std::size(square); f++) {
        if (square[f][rank].type != Piece::Type::None) {
            if (square[f][rank].color != color
                && (square[f][rank].type == Piece::Type::Rook
                    || square[f][rank].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Left
    for (int f = file - 1; f >= 0; f--) {
        if (square[f][rank].type != Piece::Type::None) {
            if (square[f][rank].color != color
                && (square[f][rank].type == Piece::Type::Rook
                    || square[f][rank].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Down
    for (int r = rank + 1; r < std::size(square); r++) {
        if (square[file][r].type != Piece::Type::None) {
            if (square[file][r].color != color
                && (square[file][r].type == Piece::Type::Rook
                    || square[file][r].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Up
    for (int r = rank - 1; r >= 0; r--) {
        if (square[file][r].type != Piece::Type::None) {
            if (square[file][r].color != color
                && (square[file][r].type == Piece::Type::Rook
                    || square[file][r].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Attacking Bishops and Queens
    // Right & Down
    for (int f = file + 1, r = rank + 1; f < std::size(square) && r < std::size(square[f]); f++, r++) {
        if (square[f][r].type != Piece::Type::None) {
            if (square[f][r].color != color
                && (square[f][r].type == Piece::Type::Bishop
                    || square[f][r].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Left & Up
    for (int f = file - 1, r = rank - 1; f >= 0 && r >= 0; f--, r--) {
        if (square[f][r].type != Piece::Type::None) {
            if (square[f][r].color != color
                && (square[f][r].type == Piece::Type::Bishop
                    || square[f][r].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Right & Up
    for (int f = file + 1, r = rank - 1; f < std::size(square) && r >= 0; f++, r--) {
        if (square[f][r].type != Piece::Type::None) {
            if (square[f][r].color != color
                && (square[f][r].type == Piece::Type::Bishop
                    || square[f][r].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Left & Down
    for (int f = file - 1, r = rank + 1; f >= 0 && r < std::size(square[f]); f--, r++) {
        if (square[f][r].type != Piece::Type::None) {
            if (square[f][r].color != color
                && (square[f][r].type == Piece::Type::Bishop
                    || square[f][r].type == Piece::Type::Queen)) {
                return true;
            }
            break;
        }
    }
    // Attacking Knights
    std::array<std::array<int, 2>, 8> targets = {{
        {{file + 2, rank + 1}},
        {{file + 2, rank - 1}},
        {{file - 2, rank + 1}},
        {{file - 2, rank - 1}},
        {{file + 1, rank + 2}},
        {{file + 1, rank - 2}},
        {{file - 1, rank + 2}},
        {{file - 1, rank - 2}}
    }};
    for (const auto& target : targets) {
        if (target[0] < 0 || target[0] > 7 || target[1] < 0 || target[1] > 7) {
            continue;
        }
        if (square[target[0]][target[1]].color != color
            && square[target[0]][target[1]].type == Piece::Type::Knight) {
            return true;
        }
    }
    return false;
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
    // Draw check square
    if (check) {
        renderTarget.draw(check_square);
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

    if (selected_sprite.x != -1 && selected_sprite.y != -1) {
        renderTarget.draw(pieces[selected_sprite.x][selected_sprite.y]);
    }

    if (pawn_promotion) {
        renderTarget.draw(pawn_promotion_menu_box);
        for (const sf::Sprite& piece: pawn_promotion_menu_sprites) {
            renderTarget.draw(piece);
        }
    }
}
