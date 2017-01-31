#ifndef NORI_H
#define NORI_H

typedef unsigned char Direction;

Direction opposite(const Direction dir) {
    switch(dir) {
        case NORTH:
            return SOUTH;
            break;
        case SOUTH:
            return NORTH;
            break;
        case WEST:
            return EAST;
            break;
        case EAST:
            return WEST;
            break;    
    }
}

void printMove(std::ofstream& out, hlt::Move move) {
    out << "(x,y): (" << move.loc.x << "," << move.loc.y << ") ";
    switch(move.dir) {
        case STILL:
            out << "STILL" << std::endl;
            break;
        case NORTH:
            out << "NORTH" << std::endl;
            break;
        case SOUTH:
            out << "SOUTH" << std::endl;
            break;
        case EAST:
            out << "EAST" << std::endl;
            break;
        case WEST:
            out << "WEST" << std::endl;
            break;
    }
}

// Check if the block is a border block
bool isBorder(hlt::GameMap& map, const hlt::Location loc, const unsigned char ID) {
    for(Direction dir : CARDINALS) {
        if(map.getSite(loc, dir).owner != ID) {
            return true;
        }
    }

    return false;
}

// Check whether given strength is enough for the block to attack something
bool isStrongEnough(hlt::GameMap& map, const hlt::Location loc, const int strength, const unsigned char ID) {
    for(Direction dir : CARDINALS) {
        const hlt::Site neighbor = map.getSite(loc, dir);
        if(neighbor.owner != ID && neighbor.strength <= strength ) {
            return true;
        }
    }

    return false;
}

#endif