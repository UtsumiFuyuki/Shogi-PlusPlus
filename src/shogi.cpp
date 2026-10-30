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
            case (piece::KING):
            case (piece::EMPTY):
                // Empty squares aren't worth anything and the King is priceless
                break;
        }

        if (board_color[i] == color::SENTE)
            senteEval += value;
        else if (board_color[i] == color::GOTE)
            goteEval += value;
    }

    return goteEval - senteEval;
}

// ===== Game Logic =====

void Game::startMatch() {
    while (!gameOver) {
        std::string input;
        Kif move{};

        system("clear");

        displayHand(!turn);
        displayBoard();
        displayHand(turn);

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
    for (size_t i = 0; i < board.size(); i++) {
        if ((i % 9) == 0)
            std::println();

        char p{};

        switch(board[i]) {
            case (piece::PAWN):
            case (piece::P_PAWN):
                p = 'P';
                break;
            case (piece::P_LANCE):
            case (piece::LANCE):
                p = 'L';
                break;
            case (piece::P_KNIGHT):
            case (piece::KNIGHT):
                p = 'N';
                break;
            case (piece::P_SILVER):
            case (piece::SILVER):
                p = 'S';
                break;
            case (piece::GOLD):
                p = 'G';
                break;
            case (piece::P_BISHOP):
            case (piece::BISHOP):
                p = 'B';
                break;
            case (piece::P_ROOK):
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

        std::print("[");

        if (static_cast<int>(board[i]) >= 8)
            std::print("\033[31m");

        std::print("{}", p);
        std::print("\033[0m]");
    }

    std::println();
}

void Game::displayHand(bool player) {
    auto hand = (player ? goteHand : senteHand);
    std::println("{}'s Hand | P: {} N: {} L: {} S: {} G: {} R: {} B: {}",
    (player ? "Gote" : "Sente"),
    hand[HAND_PAWNS],
    hand[HAND_KNIGHTS],
    hand[HAND_LANCES],
    hand[HAND_SILVERS],
    hand[HAND_GOLDS],
    hand[HAND_ROOKS],
    hand[HAND_BISHOPS]);
}

bool Game::movePiece(Kif &kif) {
    int originSquare{};
    int result{};
    bool isStrike{};
    auto opPiece = kif.opPiece;
    
    auto &hand = (turn ? goteHand : senteHand);

    if (kif.square < 11 || kif.square > 99) {
        std::println("Illegal Move!");
        return false;
    }

    if (kif.op == operation::STRIKE) {
        if (pieceAtSquare(kif.square / 10, kif.square % 10).info.first != piece::EMPTY)
            return false;
        isStrike = true;
    }

    switch (opPiece) {
        case (piece::PAWN):
            if (isStrike) {
                if (hand[HAND_PAWNS] == 0)
                    return false;
                
                // Check for nifu violation
                for (size_t i = 1; i <= 9; i++) {
                    auto p = pieceAtSquare(kif.square / 10, i);
                    if (p.info.first == piece::PAWN && p.info.second == (turn ? color::GOTE : color::SENTE))
                        return false;
                }

                // Ensure that pawn is not placed at end of board
                if ((kif.square % 10) == (turn ? 9 : 1))
                    return false;
                hand[HAND_PAWNS]--;
                break;
            }

            result = calculatePawn(originSquare, kif.square);
            break;
        case (piece::KNIGHT):
            if (isStrike) {
                if (hand[HAND_KNIGHTS] == 0)
                    return false;

                // Ensure that knight is not placed less than 2 squares away from the end of board
                if (((kif.square % 10) > 7 && turn) || ((kif.square % 10) < 3 && !turn))
                    return false;
                hand[HAND_KNIGHTS]--;
                break;
            }

            result = calculateKnight(originSquare, kif.square);
            break;
        case (piece::LANCE):
            if (isStrike) {
                if (hand[HAND_LANCES] == 0)
                    return false;

                // Ensure that lance is not placed at end of board
                if ((kif.square % 10) == (turn ? 9 : 1))
                    return false;
                hand[HAND_LANCES]--;
                break;
            }

            result = calculateLance(originSquare, kif.square);
            break;
        case (piece::SILVER):
            if (isStrike) {
                if (hand[HAND_SILVERS] == 0)
                    return false;
                hand[HAND_SILVERS]--;
                break;
            }

            result = calculateSilver(originSquare, kif.square);
            break;
        case (piece::P_PAWN):
            result = calculateGold(originSquare, kif.square, true, piece::P_PAWN);
            break;
        case (piece::P_KNIGHT):
            result = calculateGold(originSquare, kif.square, true, piece::P_KNIGHT);
            break;
        case (piece::P_LANCE):
            result = calculateGold(originSquare, kif.square, true, piece::P_LANCE);
            break;
        case (piece::P_SILVER):
            result = calculateGold(originSquare, kif.square, true, piece::P_SILVER);
            break;
        case (piece::GOLD):
            if (isStrike && opPiece == piece::GOLD) {
                if (hand[HAND_GOLDS] == 0)
                    return false;
                hand[HAND_GOLDS]--;
                break;
            }

            result = calculateGold(originSquare, kif.square, false, piece::EMPTY);
            break;
        case (piece::BISHOP):
            if (isStrike) {
                if (hand[HAND_BISHOPS] == 0)
                    return false;
                hand[HAND_BISHOPS]--;
                break;
            }

            result = calculateBishop(originSquare, kif.square, false);
            break;
        case (piece::ROOK):
            if (isStrike) {
                if (hand[HAND_ROOKS] == 0)
                    return false;
                hand[HAND_ROOKS]--;
                break;
            }

            result = calculateRook(originSquare, kif.square, false);
            break;
        case (piece::KING):
            if (isStrike)
                return false;

            result = calculateKing(originSquare, kif.square);
            break;
        case (piece::EMPTY):
            // Unhandled for obvious reasons
            break;
    }

    if (isStrike) {
        board[(fileToIndex(kif.square / 10) + rankToIndex(kif.square % 10))] = opPiece;
        board_color[(fileToIndex(kif.square / 10) + rankToIndex(kif.square % 10))] = (turn ? color::GOTE : color::SENTE);
        return true;
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
            auto p = pieceAtSquare((kif.square / 10), (kif.square % 10));

            switch (p.info.first) {
                case (piece::P_PAWN):
                case (piece::PAWN):
                    hand[HAND_PAWNS]++;
                    break;
                case (piece::P_KNIGHT):
                case (piece::KNIGHT):
                    hand[HAND_KNIGHTS]++;
                    break;
                case (piece::P_LANCE):
                case (piece::LANCE):
                    hand[HAND_LANCES]++;
                    break;
                case (piece::P_SILVER):
                case (piece::SILVER):
                    hand[HAND_SILVERS]++;
                    break;
                case (piece::GOLD):
                    hand[HAND_GOLDS]++;
                    break;
                case (piece::P_ROOK):
                case (piece::ROOK):
                    hand[HAND_ROOKS]++;
                    break;
                case (piece::P_BISHOP):
                case (piece::BISHOP):
                    hand[HAND_BISHOPS]++;
                    break;
                case (piece::KING):
                case (piece::EMPTY):
                    break;
            }

            break;
    }

    // Promotion Checks
    auto rank = (kif.square % 10);
    if (((turn && rank >= 7) || (!turn && rank <= 3)) && static_cast<int>(opPiece) < 8) {
        // For transcription reasons, I intepret declining a promotion and non-promotion moves differently
        // (even if you could argue they're treated the same)
        if (kif.promote == promotion_status::NONE) {
            std::print("Please clarify promotion status (y/n)?");
            char input{};
            std::cin >> input;

            if (input == 'n')
                goto check_detection;
            else
                goto promotion;
        }

        if (kif.promote == promotion_status::ACCEPTED) {
    
            promotion:

            switch (opPiece) {
                case (piece::PAWN):
                    opPiece = piece::P_PAWN;
                    break;
                case (piece::KNIGHT):
                    opPiece = piece::P_KNIGHT;
                    break;
                case (piece::LANCE):
                    opPiece = piece::P_LANCE;
                    break;
                case (piece::SILVER):
                    opPiece = piece::P_SILVER;
                    break;
                case (piece::ROOK):
                    opPiece = piece::P_ROOK;
                    break;
                case (piece::BISHOP):
                    opPiece = piece::P_BISHOP;
                    break;
                default:
                    std::println("Piece can not promote! Promotion status will be disregarded!");
                    kif.promote = promotion_status::NONE;
            }
        }
    }

    check_detection:

    // Check Detection

    board[(fileToIndex(kif.square / 10) + rankToIndex(kif.square % 10))] = opPiece;
    board[(fileToIndex(originSquare / 10) + rankToIndex(originSquare % 10))] = piece::EMPTY;
    board_color[(fileToIndex(kif.square / 10) + rankToIndex(kif.square % 10))] = (turn ? color::GOTE : color::SENTE);
    board_color[(fileToIndex(originSquare / 10) + rankToIndex(originSquare % 10))] = color::NONE;

    std::println("Origin Square: {}", originSquare);

    return true;
}

// ===== Piece Calculations =====

int Game::calculatePawn(int &originSquare, const unsigned int &destSquare) {
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

int Game::calculateKnight(int &originSquare, const unsigned int &destSquare) {
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

int Game::calculateLance(int &originSquare, const unsigned int &destSquare) {
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

int Game::calculateSilver(int &originSquare, const unsigned int &destSquare) {
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

int Game::calculateGold(int &originSquare, const unsigned int &destSquare, bool promoted, piece promotedPiece) {
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
        if (p.info.first == (promoted ? promotedPiece : piece::GOLD) && p.info.second == (turn ? color::GOTE : color::SENTE)) {
            originSquare = p.square;

            if (pieceAtSquare(file, rank).info.first != piece::EMPTY)
                return 1;
            return 0;
        }
    }

    return -1;
}

int Game::calculateBishop(int &originSquare, const unsigned int &destSquare, bool promoted) {
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

int Game::calculateRook(int &originSquare, const unsigned int &destSquare, bool promoted) {
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

int Game::calculateKing(int &originSquare, const unsigned int &destSquare) {
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