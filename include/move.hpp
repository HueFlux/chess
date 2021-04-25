#ifndef MOVE_HPP
#define MOVE_HPP

struct Move {
    int start_file;
    int start_rank;
    int target_file;
    int target_rank;

    Move (int start_file, int start_rank, int target_file, int target_rank) :
        start_file(start_file),
        start_rank(start_rank),
        target_file(target_file),
        target_rank(target_rank)
    {}

    bool operator==(const Move& rhs) const {
        return (this->start_file == rhs.start_file
                && this->start_rank == rhs.start_rank
                && this->target_file == rhs.target_file
                && this->target_rank == rhs.target_rank);
    }
};

#endif
