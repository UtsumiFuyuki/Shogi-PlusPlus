#include <print>
#include <shogi.hpp>

// NOTE: This is not KIF notation, this is Western Notation (numeric only)
// I dunno why I refer to this as KIF, I'll change the name later.

Kif parseMove(std::string move)
{
    Kif kif{};
    std::println("Parsing KIF...");
    if (move.size() < 4)
    {
        std::println("Invalid KIF Notation!");
        return {};
    }

    std::print("Operating Piece: ");

    switch (move[0])
    {
        case ('P'):
            kif.opPiece = piece::PAWN;
            std::println("Pawn");
            break;
        case ('L'):
            kif.opPiece = piece::LANCE;
            std::println("Lance");
            break;
        case ('N'):
            kif.opPiece = piece::KNIGHT;
            std::println("Knight");
            break;
        case ('S'):
            kif.opPiece = piece::SILVER;
            std::println("Silver");
            break;
        case ('G'):
            kif.opPiece = piece::GOLD;
            std::println("Gold");
            break;
        case ('B'):
            kif.opPiece = piece::BISHOP;
            std::println("Bishop");
            break;
        case ('R'):
            kif.opPiece = piece::ROOK;
            std::println("Rook");
            break;
        case ('K'):
            kif.opPiece = piece::KING;
            std::println("King");
            break;
        default:
            std::println("Invalid KIF Notation!");
            return {};
    }

    std::print("Operation: ");

    switch (move[1])
    {
        case ('-'):
            kif.op = operation::MOVE;
            std::println("Move");
            break;
        case ('x'):
            kif.op = operation::CAPTURE;
            std::println("Capture");
            break;
        case ('*'):
            kif.op = operation::STRIKE;
            std::println("Strike");
            break;
        default:
            std::println("Invalid KIF Notation!");
            return {};
    }

    kif.square += (move[2] - '0') * 10;
    kif.square += (move[3] - '0');

    std::println("Square: {}", kif.square);

    return kif;
}