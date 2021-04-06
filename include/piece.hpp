#ifndef PIECE_HPP
#define PIECE_HPP

struct Piece {
    enum Type : unsigned char {
        None = 0, King, Pawn, Knight, Bishop, Rook, Queen
    };
    enum Color : bool {
        White = true, Black = false
    };

    Piece() : type(None) {};

    Type type;
    Color color;
};

#endif
