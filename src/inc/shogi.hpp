#pragma once

#include <array>
#include <utility>
#include <string>

#define HAND_PAWNS 0
#define HAND_KNIGHTS 1
#define HAND_LANCES 2
#define HAND_SILVERS 3
#define HAND_GOLDS 4
#define HAND_ROOKS 5
#define HAND_BISHOPS 6

enum class color {SENTE, GOTE, NONE};
enum class piece {PAWN, KNIGHT, LANCE, SILVER, GOLD, BISHOP, ROOK, KING, EMPTY};

enum class operation {MOVE, STRIKE, CAPTURE};

struct Kif
{
    piece opPiece;
    operation op;
    unsigned int square;
};

// Parses the given string and outputs a Kif from it
Kif parseMove(std::string move);

struct squareInfo {
    unsigned int square{};
    std::pair<piece, color> info;
};

class Game
{
    public:
        void startMatch();
        void displayBoard();

        // Displays the hand of the requested player
        void displayHand(bool player);

        // Rudimentary evaluation function based purely on material
        double eval();

        // Returns information about a piece at a specific square
        squareInfo pieceAtSquare(unsigned int file, unsigned int rank);

        // Preforms legality checks and moves piece. Returns false on fail
        bool movePiece(const Kif &kif);

        // Piece-specific calculations
        // The return values are intepreted as follows:
        // 0 - Legal Move
        // 1 - Legal Capture Move
        // -1 - Illegal Move
        int calculatePawn(int &originSquare, const unsigned int &destSquare);
        int calculateKnight(int &originSquare, const unsigned int &destSquare);
        int calculateLance(int &originSquare, const unsigned int &destSquare);
        int calculateSilver(int &originSquare, const unsigned int &destSquare);
        int calculateGold(int &originSquare, const unsigned int &destSquare);
        int calculateBishop(int &originSquare, const unsigned int &destSquare, bool promoted);
        int calculateRook(int &originSquare, const unsigned int &destSquare, bool promoted);
        int calculateKing(int &originSquare, const unsigned int &destSquare);

    private:
        std::array<color, 81> board_color = {
    color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE,
    color::NONE, color::GOTE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::GOTE, color::NONE,
    color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE, color::GOTE,
    color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE,
    color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE,
    color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE,
    color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE,
    color::NONE, color::SENTE, color::NONE, color::NONE, color::NONE, color::NONE, color::NONE, color::SENTE, color::NONE,
    color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE, color::SENTE
    };

        std::array<piece, 81> board = {
    piece::LANCE, piece::KNIGHT, piece::SILVER, piece::GOLD, piece::KING, piece::GOLD, piece::SILVER, piece::KNIGHT, piece::LANCE,
    piece::EMPTY, piece::ROOK, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::BISHOP, piece::EMPTY,
    piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN,
    piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY,
    piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY,
    piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY,
    piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN, piece::PAWN,
    piece::EMPTY, piece::BISHOP, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::EMPTY, piece::ROOK, piece::EMPTY,
    piece::LANCE, piece::KNIGHT, piece::SILVER, piece::GOLD, piece::KING, piece::GOLD, piece::SILVER, piece::KNIGHT, piece::LANCE
    };

    bool gameOver{};
    bool turn{}; // True = Gote, False = Sente

    std::array<unsigned int, 7> senteHand{};
    std::array<unsigned int, 7> goteHand{};
};