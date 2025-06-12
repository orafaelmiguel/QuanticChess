#pragma once

#include <cstdint>
#include <array>

class MagicBitboards {
public:
    static void init();
    static uint64_t get_rook_attacks(int square, uint64_t blockers);
    static uint64_t get_bishop_attacks(int square, uint64_t blockers);
    
private:
    struct MagicEntry {
        uint64_t magic;
        uint64_t mask;
        uint64_t* attacks;
        int shift;
    };
    
    static std::array<MagicEntry, 64> rook_magics;
    static std::array<MagicEntry, 64> bishop_magics;
    
    static uint64_t find_magic_number(int square, bool is_rook);
    static uint64_t generate_blocker_mask(int square, bool is_rook);
    static uint64_t generate_attack_mask(int square, uint64_t blockers, bool is_rook);
    static void init_magic_entries();
    static bool verify_magic_number(uint64_t magic, int square, bool is_rook);
    
    static constexpr uint64_t ROOK_MAGICS[64] = {
        0x8a80104000800020ULL, 0x140002000100040ULL, 0x2801880a0017001ULL, 0x1000100010002ULL,
        0x2000200010002ULL, 0x2000100010002ULL, 0x2000100010002ULL, 0x2000100010002ULL,
        0x2000100010002ULL, 0x2000100010002ULL, 0x2000100010002ULL, 0x2000100010002ULL,
        0x2000100010002ULL, 0x2000100010002ULL, 0x2000100010002ULL, 0x2000100010002ULL,
    };
    
    static constexpr uint64_t BISHOP_MAGICS[64] = {
        0x40040844404084ULL, 0x2004208a004208ULL, 0x10190041080202ULL, 0x108060845042010ULL,
        0x581104180800210ULL, 0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,
        0x4050404440404ULL, 0x21001420088ULL, 0x24d0080801082102ULL, 0x1020a0a020400ULL,
        0x40308200402ULL, 0x4011002100800ULL, 0x401484104104005ULL, 0x801010402020200ULL,
    };
}; 