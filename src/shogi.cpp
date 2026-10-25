#include <cctype>
#include <cstdio>
#include <iostream>
#include <print>
#include <stdexcept>
#include <utility>
#include <string>
#include <shogi.hpp>

#define VALUE_PAWN 1
#define VALUE_LANCE 3
#define VALUE_KNIGHT 4
#define VALUE_SILVER 5
#define VALUE_GOLD 6
#define VALUE_BISHOP 8
#define VALUE_ROOK 9

// Helper Functions

int fileToIndex(int file) {
    if (file > 9)
        throw std::invalid_argument {"File greater than 9 passed to fileToIndex()"};
    file--;

    return 8 - file;
}

int rankToIndex(int rank) {
    if (rank > 9)
        throw std::invalid_argument {"Rank greater than 9 passed to rankToIndex()"};
    rank--;

    return rank * 9;
}

squareInfo Game::pieceAtSquare(unsigned int file, unsigned int rank) {
    unsigned int square = ((file * 10) + rank);
    try {
        file = fileToIndex(file);
        rank = rankToIndex(rank);
    }

    catch (const std::invalid_argument &e) {
        std::cerr << "Invalid argument passed! Message: " << e.what() << std::endl;
        std::cerr << "Returning first element in board!" << std::endl;
        return {0, {board[0], board_color[0]}};
    }
    int idx = file + rank;

    return {square, {board[idx], board_color[idx]}};
}

double Game::eval() {
    double senteEval{}, goteEval{};
    for (size_t i = 0; i < board.size(); i++) {
        double value{};
        switch(board[i]) {
            case (piece::PAWN):
                value = VALUE_PAWN;
                break;
            case (piece::LANCE):
                value = VALUE_LANCE;
                break;
            case (piece::KNIGHT):
                value = VALUE_KNIGHT;
                break;
            case (piece::SILVER):
                value = VALUE_SILVER;
                break;
            case (piece::GOLD):
                value = VALUE_GOLD;
                break;
            case (piece::BISHOP):
                value = VALUE_BISHOP;
                break;
            case (piece::ROOK):
                value = VALUE_ROOK;
                break;
        }

        if (board_color[i] == color::SENTE)
            senteEval += value;
        else if (board_color[i] == color::GOTE)
            goteEval += value;
    }

    return goteEval - senteEval;
}

// Game Logic

void Game::startMatch() {
    while (!gameOver) {
        std::string input;
        Kif move{};

        displayBoard();

        do {
            std::print("{}'s Turn: ", turn ? "Gote" : "Sente");
            std::cin >> input;
            std::println("Input was: {}", input);

            move = parseMove(input);
        } while (!movePiece(move));

        turn = !turn;
    }
}

void Game::displayBoard() {
    system("clear");
    for (int i = 0; i < board.size(); i++) {
        if ((i % 9) == 0)
            std::println();

        char p{};

        switch(board[i]) {
            case (piece::PAWN):
                p = 'P';
                break;
            case (piece::LANCE):
                p = 'L';
                break;
            case (piece::KNIGHT):
                p = 'N';
                break;
            case (piece::SILVER):
                p = 'S';
                break;
            case (piece::GOLD):
                p = 'G';
                break;
            case (piece::BISHOP):
                p = 'B';
                break;
            case (piece::ROOK):
                p = 'R';
                break;
            case (piece::KING):
                p = 'K';
                break;
            case (piece::EMPTY):
                p = ' ';
                break;
        }

        if (board_color[i] == color::GOTE && p != ' ')
            p = tolower(p);
        std::print("[{}]", p);
    }

    std::println();
}

bool Game::movePiece(const Kif &kif) {
    int originSquare{};
    int result{};

    if (kif.square < 11 || kif.square > 99) {
        std::println("Illegal Move!");
        return false;
    }

    switch (kif.opPiece) {
        case (piece::PAWN):
            result = calculatePawn(originSquare, kif.square);
            break;
        case (piece::KNIGHT):
            result = calculateKnight(originSquare, kif.square);
            break;
        case (piece::LANCE):
            result = calculateLance(originSquare, kif.square);
            break;
        case (piece::SILVER):
            result = calculateSilver(originSquare, kif.square);
            break;
        case (piece::GOLD):
            result = calculateGold(originSquare, kif.square);
            break;
        case (piece::BISHOP):
            result = calculateBishop(originSquare, kif.square, false);
            break;
        case (piece::ROOK):
            result = calculateRook(originSquare, kif.square, false);
            break;
        case (piece::KING):
            result = calculateKing(originSquare, kif.square);
            break;
    }

    if (result < 0) {
        std::println("Illegal Move!");
        return false;
    }

    switch (kif.op) {
        case (operation::MOVE):
            if (result != 0) {
                std::println("Illegal Move!");
                return false;
            }
            break;
        case (operation::STRIKE):
            // Piece calculations are disregarded for strikes
            break;

        case (operation::CAPTURE):
            if (result != 1) {
                std::println("Illegal Move!");
                return false;
            }
            break;
    }

    board[(fileToIndex(kif.square / 10) + rankToIndex(kif.square % 10))] = kif.opPiece;
    board[(fileToIndex(originSquare / 10) + rankToIndex(originSquare % 10))] = piece::EMPTY;
    board_color[(fileToIndex(kif.square / 10) + rankToIndex(kif.square % 10))] = (turn ? color::GOTE : color::SENTE);
    board_color[(fileToIndex(originSquare / 10) + rankToIndex(originSquare % 10))] = color::NONE;

    std::println("Origin Square: {}", originSquare);

    return true;
}

// ===== Piece Calculations =====

int Game::calculatePawn(int &originSquare, const int &destSquare) {
    int file = destSquare / 10;
    int rank = destSquare % 10;

    squareInfo p;

    p = pieceAtSquare(file, rank + (turn ? -1 : 1));
    if (p.info.first != piece::PAWN || p.info.second != (turn ? color::GOTE : color::SENTE))
        return -1;

    originSquare = ((file * 10) + (rank + (turn ? -1 : 1)));

    if (pieceAtSquare(file, rank).info.first != piece::EMPTY)
        return 1;

    return 0;
}

int Game::calculateKnight(int &originSquare, const int &destSquare) {
    int file = destSquare / 10;
    int rank = destSquare % 10;

    squareInfo p1, p2;

    // TODO: fix this, will overflow from moves to the edge.
    p1 = pieceAtSquare(file - 1, rank + (turn ? -2 : 2));
    p2 = pieceAtSquare(file + 1, rank + (turn ? -2 : 2));

    if (p1.info.first == piece::KNIGHT && p1.info.second == (turn ? color::GOTE : color::SENTE))
        originSquare = (((file - 1) * 10) + (rank + (turn ? -2 : 2)));
    else if (p2.info.first == piece::KNIGHT && p2.info.second == (turn ? color::GOTE : color::SENTE))
        originSquare = (((file + 1) * 10) + (rank + (turn ? -2 : 2)));
    else
        return -1;

    if (pieceAtSquare(file, rank).info.first != piece::EMPTY) {
        if (pieceAtSquare(file, rank).info.second == (turn ? color::GOTE : color::SENTE))
            return -1;
        return 1;
    }

    return 0;
}

int Game::calculateLance(int &originSquare, const int &destSquare) {
    int file = destSquare / 10;
    int rank = destSquare % 10;

    for (int i = rank + (turn ? -1 : 1); i <= 9; i += (turn ? -1 : 1)) {
        squareInfo p = pieceAtSquare(file, i);
        if (p.info.first != piece::EMPTY) {
            if (p.info.first == piece::LANCE && p.info.second == (turn ? color::GOTE : color::SENTE)) {
                originSquare = ((file * 10) + i);
                break;
            }

            return -1;
        }
    }

    if (pieceAtSquare(file, rank).info.first != piece::EMPTY)
        return 1;
    return 0;
}

int Game::calculateSilver(int &originSquare, const int &destSquare) {
    unsigned int file = destSquare / 10;
    unsigned int rank = destSquare % 10;

    // Silver's move pattern
    std::array<squareInfo, 5> spaces {
        pieceAtSquare(file - 1, rank + (turn ? -1 : 1)),
        pieceAtSquare(file - 1, rank + (turn ? 1 : -1)),
        pieceAtSquare(file + 1, rank + (turn ? -1 : 1)),
        pieceAtSquare(file + 1, rank + (turn ? 1 : -1)),
        pieceAtSquare(file, rank + (turn ? -1 : 1))};
    
    for (auto p : spaces) {
        if (p.info.first == piece::SILVER && p.info.second == (turn ? color::GOTE : color::SENTE)) {
            originSquare = p.square;

            if (pieceAtSquare(file, rank).info.first != piece::EMPTY)
                return 1;
            return 0;
        }
    }

    return -1;
}

int Game::calculateGold(int &originSquare, const int &destSquare) {
    unsigned int file = destSquare / 10;
    unsigned int rank = destSquare % 10;

    // Gold's move pattern
    std::array<squareInfo, 6> spaces {
        pieceAtSquare(file - 1, rank + (turn ? -1 : 1)),
        pieceAtSquare(file + 1, rank + (turn ? -1 : 1)),
        pieceAtSquare(file, rank + (turn ? -1 : 1)),
        pieceAtSquare(file, rank + (turn ? 1 : -1)),
        pieceAtSquare(file - 1 , rank),
        pieceAtSquare(file + 1, rank)};
    
    for (auto p : spaces) {
        if (p.info.first == piece::GOLD && p.info.second == (turn ? color::GOTE : color::SENTE)) {
            originSquare = p.square;

            if (pieceAtSquare(file, rank).info.first != piece::EMPTY)
                return 1;
            return 0;
        }
    }

    return -1;
}

int Game::calculateBishop(int &originSquare, const int &destSquare, bool promoted) {
    unsigned int file = destSquare / 10;
    unsigned int rank = destSquare % 10;

    unsigned int i;
    unsigned int j;
    unsigned int direction{};

    while (direction < 4) {
        i = file;
        j = rank;

        for (;(i <= 9 && j <= 9) && (i >= 1 && j >= 1);) {
            auto p = pieceAtSquare(i, j);

            if (p.info.first != piece::EMPTY && ((i * 10) + j) != destSquare) {
                if (p.info.first == piece::BISHOP && p.info.second == (turn ? color::GOTE : color::SENTE)) {
                    originSquare = (i * 10) + j;
                    if (pieceAtSquare(file, rank).info.first != piece::EMPTY && pieceAtSquare(file, rank).info.second == (turn ? color::SENTE : color::GOTE))
                        return 1;
                    else if (pieceAtSquare(file, rank).info.first == piece::EMPTY)
                        return 0;
                }
                else
                    break;
            }

            switch (direction) {
                case (0):
                    i++;
                    j++;
                    break;
                case (1):
                    i--;
                    j++;
                    break;
                case (2):
                    i++;
                    j--;
                    break;
                case (3):
                    i--;
                    j--;
                    break;
            }

        }
        direction++;
    }

    return -1;
}

int Game::calculateRook(int &originSquare, const int &destSquare, bool promoted) {
    unsigned int file = destSquare / 10;
    unsigned int rank = destSquare % 10;

    unsigned int i;
    unsigned int j;
    unsigned int direction{};

    while (direction < 4) {
        i = file;
        j = rank;

        for (;(i <= 9 && j <= 9) && (i >= 1 && j >= 1);) {
            auto p = pieceAtSquare(i, j);

            if (p.info.first != piece::EMPTY && ((i * 10) + j) != destSquare) {
                if (p.info.first == piece::ROOK && p.info.second == (turn ? color::GOTE : color::SENTE)) {
                    originSquare = (i * 10) + j;
                    if (pieceAtSquare(file, rank).info.first != piece::EMPTY && pieceAtSquare(file, rank).info.second == (turn ? color::SENTE : color::GOTE))
                        return 1;
                    else if (pieceAtSquare(file, rank).info.first == piece::EMPTY)
                        return 0;
                }
                else
                    break;
            }

            switch (direction) {
                case (0):
                    i++;
                    break;
                case (1):
                    i--;
                    break;
                case (2):
                    j++;
                    break;
                case (3):
                    j--;
                    break;
            }

        }
        direction++;
    }

    return -1;
}

int Game::calculateKing(int &originSquare, const int &destSquare) {
    unsigned int file = destSquare / 10;
    unsigned int rank = destSquare % 10;

    // King's move pattern
    std::array<squareInfo, 8> spaces {
        pieceAtSquare(file - 1, rank + (turn ? -1 : 1)),
        pieceAtSquare(file - 1, rank + (turn ? 1 : -1)),
        pieceAtSquare(file + 1, rank + (turn ? -1 : 1)),
        pieceAtSquare(file + 1, rank + (turn ? 1 : -1)),
        pieceAtSquare(file, rank + (turn ? -1 : 1)),
        pieceAtSquare(file, rank + (turn ? 1 : -1)),
        pieceAtSquare(file - 1 , rank),
        pieceAtSquare(file + 1, rank)};
    
    for (auto p : spaces) {
        if (p.info.first == piece::KING && p.info.second == (turn ? color::GOTE : color::SENTE)) {
            originSquare = p.square;

            if (pieceAtSquare(file, rank).info.first != piece::EMPTY)
                return 1;
            return 0;
        }
    }

    return -1;
}