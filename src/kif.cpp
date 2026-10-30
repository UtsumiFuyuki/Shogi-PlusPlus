#include <print>
#include <shogi.hpp>

// NOTE: This is not KIF notation, this is Western Notation (numeric only)
// I dunno why I refer to this as KIF, I'll change the name later.

Kif parseMove(std::string move)
{
    Kif kif{};
    size_t p_char{};

    std::println("Parsing KIF...");
    if (move[0] == '+')
        p_char++;

    if (move.size() < 4 || (p_char > 0 && move.size() < 5)) {
        std::println("Invalid KIF Notation!");
        return {};
    }

    std::print("Operating Piece: ");

    switch (move[0 + p_char])
    {
        case ('P'):
            kif.opPiece = (p_char != 0 ? piece::P_PAWN : piece::PAWN);
            std::println("Pawn");
            break;
        case ('L'):
            kif.opPiece = (p_char != 0 ? piece::P_LANCE : piece::LANCE);
            std::println("Lance");
            break;
        case ('N'):
            kif.opPiece = (p_char != 0 ? piece::P_KNIGHT : piece::KNIGHT);
            std::println("Knight");
            break;
        case ('S'):
            kif.opPiece = (p_char != 0 ? piece::P_SILVER : piece::SILVER);
            std::println("Silver");
            break;
        case ('G'):
            kif.opPiece = piece::GOLD;
            std::println("Gold");
            break;
        case ('B'):
            kif.opPiece = (p_char != 0 ? piece::P_BISHOP : piece::BISHOP);
            std::println("Bishop");
            break;
        case ('R'):
            kif.opPiece = (p_char != 0 ? piece::P_ROOK : piece::ROOK);
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

    switch (move[1 + p_char])
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

    if (move.back() == '=')
        kif.promote = promotion_status::DECLINED;
    else if (move.back() == '+')
        kif.promote = promotion_status::ACCEPTED;
    else
        kif.promote = promotion_status::NONE;

    kif.square += (move[2 + p_char] - '0') * 10;
    kif.square += (move[3 + p_char] - '0');

    std::println("Square: {}", kif.square);

    return kif;
}