#pragma once

namespace othello {
namespace bitwise {

void Init();

int CountBits(uint64_t data);
uint8_t MaskShiftRow8(const uint64_t data, const int index);
uint64_t MaskShiftRow64(const uint64_t data, const int index);
uint64_t GetBitMask(const Position position);
uint64_t GetPutBits(const uint8_t row, const uint8_t column,
    const uint8_t cross_white, const uint8_t cross_black,
    const std::pair<int, int> xy_pair);
uint64_t LinePuttableBits(const uint64_t black_state,
    const uint64_t white_state, const int index, const int line_length,
    const State state);
uint8_t Column2Uint8(const uint64_t data);
uint8_t CrossWhite2Uint8(const uint64_t data);
uint8_t CrossBlack2Uint8(const uint64_t data);
uint64_t Uint82Column(const uint8_t data);
uint64_t Uint82CrossWhite(const uint8_t data);
uint64_t Uint82CrossBlack(const uint8_t data);
uint64_t TransposeBits(uint64_t data);
uint64_t FlipHorizonalBits(uint64_t data);
uint64_t DistortBitsWhite1(uint64_t data);
uint64_t DistortBitsWhite2(uint64_t data);
uint64_t DistortBitsBlack1(uint64_t data);
uint64_t DistortBitsBlack2(uint64_t data);
uint64_t UndistortBitsWhite1(uint64_t data);
uint64_t UndistortBitsWhite2(uint64_t data);
uint64_t UndistortBitsBlack1(uint64_t data);
uint64_t UndistortBitsBlack2(uint64_t data);

} // namespace bitwise
} // namespace othello
