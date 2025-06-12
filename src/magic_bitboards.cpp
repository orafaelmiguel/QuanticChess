#include "magic_bitboards.h"
#include <random>
#include <vector>
#include <algorithm>

std::array<MagicBitboards::MagicEntry, 64> MagicBitboards::rook_magics;
std::array<MagicBitboards::MagicEntry, 64> MagicBitboards::bishop_magics;

void MagicBitboards::init() {
    init_magic_entries();
}

uint64_t MagicBitboards::get_rook_attacks(int square, uint64_t blockers) {
    const MagicEntry& entry = rook_magics[square];
    blockers &= entry.mask;
    blockers *= entry.magic;
    blockers >>= entry.shift;
    return entry.attacks[blockers];
}

uint64_t MagicBitboards::get_bishop_attacks(int square, uint64_t blockers) {
    const MagicEntry& entry = bishop_magics[square];
    blockers &= entry.mask;
    blockers *= entry.magic;
    blockers >>= entry.shift;
    return entry.attacks[blockers];
}

uint64_t MagicBitboards::generate_blocker_mask(int square, bool is_rook) {
    uint64_t mask = 0;
    int rank = square / 8;
    int file = square % 8;
    
    if (is_rook) {
        // Horizontal
        for (int f = file + 1; f < 7; f++) mask |= 1ULL << (rank * 8 + f);
        for (int f = file - 1; f > 0; f--) mask |= 1ULL << (rank * 8 + f);
        
        // Vertical
        for (int r = rank + 1; r < 7; r++) mask |= 1ULL << (r * 8 + file);
        for (int r = rank - 1; r > 0; r--) mask |= 1ULL << (r * 8 + file);
    } else {
        // Diagonais
        for (int i = 1; i < 7; i++) {
            if (rank + i < 7 && file + i < 7) mask |= 1ULL << ((rank + i) * 8 + (file + i));
            if (rank + i < 7 && file - i > 0) mask |= 1ULL << ((rank + i) * 8 + (file - i));
            if (rank - i > 0 && file + i < 7) mask |= 1ULL << ((rank - i) * 8 + (file + i));
            if (rank - i > 0 && file - i > 0) mask |= 1ULL << ((rank - i) * 8 + (file - i));
        }
    }
    
    return mask;
}

uint64_t MagicBitboards::generate_attack_mask(int square, uint64_t blockers, bool is_rook) {
    uint64_t attacks = 0;
    int rank = square / 8;
    int file = square % 8;
    
    if (is_rook) {
        // Horizontal
        for (int f = file + 1; f < 8; f++) {
            attacks |= 1ULL << (rank * 8 + f);
            if (blockers & (1ULL << (rank * 8 + f))) break;
        }
        for (int f = file - 1; f >= 0; f--) {
            attacks |= 1ULL << (rank * 8 + f);
            if (blockers & (1ULL << (rank * 8 + f))) break;
        }
        
        // Vertical
        for (int r = rank + 1; r < 8; r++) {
            attacks |= 1ULL << (r * 8 + file);
            if (blockers & (1ULL << (r * 8 + file))) break;
        }
        for (int r = rank - 1; r >= 0; r--) {
            attacks |= 1ULL << (r * 8 + file);
            if (blockers & (1ULL << (r * 8 + file))) break;
        }
    } else {
        // Diagonais
        for (int i = 1; i < 8; i++) {
            if (rank + i < 8 && file + i < 8) {
                attacks |= 1ULL << ((rank + i) * 8 + (file + i));
                if (blockers & (1ULL << ((rank + i) * 8 + (file + i)))) break;
            }
        }
        for (int i = 1; i < 8; i++) {
            if (rank + i < 8 && file - i >= 0) {
                attacks |= 1ULL << ((rank + i) * 8 + (file - i));
                if (blockers & (1ULL << ((rank + i) * 8 + (file - i)))) break;
            }
        }
        for (int i = 1; i < 8; i++) {
            if (rank - i >= 0 && file + i < 8) {
                attacks |= 1ULL << ((rank - i) * 8 + (file + i));
                if (blockers & (1ULL << ((rank - i) * 8 + (file + i)))) break;
            }
        }
        for (int i = 1; i < 8; i++) {
            if (rank - i >= 0 && file - i >= 0) {
                attacks |= 1ULL << ((rank - i) * 8 + (file - i));
                if (blockers & (1ULL << ((rank - i) * 8 + (file - i)))) break;
            }
        }
    }
    
    return attacks;
}

bool MagicBitboards::verify_magic_number(uint64_t magic, int square, bool is_rook) {
    uint64_t mask = generate_blocker_mask(square, is_rook);
    int bits = __builtin_popcountll(mask);
    std::vector<uint64_t> blockers;
    std::vector<uint64_t> attacks;
    
    // Gerar todas as combinações possíveis de bloqueadores
    for (uint64_t i = 0; i < (1ULL << bits); i++) {
        uint64_t blocker = 0;
        uint64_t temp_mask = mask;
        for (int j = 0; j < bits; j++) {
            int lsb = __builtin_ctzll(temp_mask);
            if (i & (1ULL << j)) blocker |= 1ULL << lsb;
            temp_mask &= temp_mask - 1;
        }
        blockers.push_back(blocker);
        attacks.push_back(generate_attack_mask(square, blocker, is_rook));
    }
    
    // Verificar se o número mágico gera índices únicos
    std::vector<uint64_t> indices;
    for (uint64_t blocker : blockers) {
        uint64_t index = (blocker * magic) >> (64 - bits);
        if (std::find(indices.begin(), indices.end(), index) != indices.end()) {
            return false;
        }
        indices.push_back(index);
    }
    
    return true;
}

uint64_t MagicBitboards::find_magic_number(int square, bool is_rook) {
    uint64_t mask = generate_blocker_mask(square, is_rook);
    int bits = __builtin_popcountll(mask);
    
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    
    for (int i = 0; i < 100000000; i++) {
        uint64_t magic = dis(gen) & dis(gen) & dis(gen);
        if (verify_magic_number(magic, square, is_rook)) {
            return magic;
        }
    }
    
    return 0; // Falha ao encontrar número mágico
}

void MagicBitboards::init_magic_entries() {
    for (int square = 0; square < 64; square++) {
        // Inicializar torres
        rook_magics[square].magic = ROOK_MAGICS[square];
        rook_magics[square].mask = generate_blocker_mask(square, true);
        rook_magics[square].shift = 64 - __builtin_popcountll(rook_magics[square].mask);
        rook_magics[square].attacks = new uint64_t[1ULL << __builtin_popcountll(rook_magics[square].mask)];
        
        // Inicializar bispos
        bishop_magics[square].magic = BISHOP_MAGICS[square];
        bishop_magics[square].mask = generate_blocker_mask(square, false);
        bishop_magics[square].shift = 64 - __builtin_popcountll(bishop_magics[square].mask);
        bishop_magics[square].attacks = new uint64_t[1ULL << __builtin_popcountll(bishop_magics[square].mask)];
        
        // Preencher tabelas de ataque
        uint64_t mask = rook_magics[square].mask;
        int bits = __builtin_popcountll(mask);
        for (uint64_t i = 0; i < (1ULL << bits); i++) {
            uint64_t blocker = 0;
            uint64_t temp_mask = mask;
            for (int j = 0; j < bits; j++) {
                int lsb = __builtin_ctzll(temp_mask);
                if (i & (1ULL << j)) blocker |= 1ULL << lsb;
                temp_mask &= temp_mask - 1;
            }
            uint64_t index = (blocker * rook_magics[square].magic) >> rook_magics[square].shift;
            rook_magics[square].attacks[index] = generate_attack_mask(square, blocker, true);
        }
        
        mask = bishop_magics[square].mask;
        bits = __builtin_popcountll(mask);
        for (uint64_t i = 0; i < (1ULL << bits); i++) {
            uint64_t blocker = 0;
            uint64_t temp_mask = mask;
            for (int j = 0; j < bits; j++) {
                int lsb = __builtin_ctzll(temp_mask);
                if (i & (1ULL << j)) blocker |= 1ULL << lsb;
                temp_mask &= temp_mask - 1;
            }
            uint64_t index = (blocker * bishop_magics[square].magic) >> bishop_magics[square].shift;
            bishop_magics[square].attacks[index] = generate_attack_mask(square, blocker, false);
        }
    }
} 