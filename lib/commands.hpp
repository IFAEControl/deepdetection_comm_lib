#pragma once

#include <iostream>
#include <array>

#include <nlohmann/json.hpp>

#include "header.hpp"

using json = nlohmann::json;

template<std::size_t S>
class UnsignedArray {
public:
    UnsignedArray(const unsigned* in) {
        for(std::size_t i = 0; i < S; i++) a[i] = in[i];
    }

    template<typename T>
    UnsignedArray(T& j) {
        a = j;
    }

    auto val() { return a; }

    auto begin() { return a.begin(); }
    auto end() { return a.end(); }

    std::array<unsigned,S> a;
};

struct Message {
    BaseHeaderType header{};
    json body{};
};

class Command {
public:
    Command(const std::string& name);

    Message& getMessage();

    friend std::ostream& operator<< (std::ostream& stream, const Command& c);

protected:
    Message m;
};

namespace CMD {

class ReadTemperature : public Command {
public:
    ReadTemperature(unsigned chips_bitmap);
    unsigned getAnswer();
};

class FullArrayReadTemperature : public Command {
public:
    FullArrayReadTemperature(unsigned chips_bitmap);
    UnsignedArray<30> getAnswer();
};

class SetHV : public Command {
public:
    SetHV(unsigned counts);
};

class SetTPDAC : public Command {
public:
    SetTPDAC(unsigned counts);
};

class ChipRegisterWrite : public Command {
public:
    ChipRegisterWrite(UnsignedArray<5>& val, unsigned chips_bitmap);
};

class ChipRegisterRead : public Command {
public:
    ChipRegisterRead(unsigned chips_bitmap);
    UnsignedArray<5> getAnswer();
};

class FullArrayChipRegisterRead : public Command {
public:
    FullArrayChipRegisterRead(unsigned chips_bitmap);
    UnsignedArray<150> getAnswer();
};

class PixelRegisterWrite : public Command {
public:
    PixelRegisterWrite(UnsignedArray<480>& val, unsigned chips_bitmap);
};

class PixelRegisterRead : public Command {
public:
    PixelRegisterRead(unsigned chips_bitmap);
    UnsignedArray<480> getAnswer();
};

class FullArrayPixelRegisterRead : public Command {
public:
    FullArrayPixelRegisterRead(unsigned chips_bitmap);
    UnsignedArray<14400> getAnswer();
};

class ReadEricaID : public Command {
public:
    ReadEricaID(unsigned chips_bitmap);
    unsigned getAnswer();
};

class FullArrayReadEricaID : public Command {
public:
    FullArrayReadEricaID(unsigned chips_bitmap);
    UnsignedArray<30> getAnswer();
};

class ACQuisition : public Command {
public:
    ACQuisition(const AcqInfo& info, unsigned frames, unsigned chips_bitmap);
    UnsignedArray<480> getAnswer();
};

class ACQuisitionCont : public Command {
public:
    ACQuisitionCont(const AcqInfo& info, unsigned chips_bitmap);
};

class ACQuisitionStop : public Command {
public:
    ACQuisitionStop();
};

class CameraReset : public Command {
public:
    CameraReset();
};

class ControllerReset : public Command {
public:
    ControllerReset();
};

class LoadFloodNormFactors : public Command {
public:
    LoadFloodNormFactors(UnsignedArray<60>& val, unsigned chips_bitmap);
};

class GetAllRegs : public Command {
public:
    GetAllRegs();
    json getAnswer();
};

class GetDataIRQs : public Command {
public:
    GetDataIRQs();
    unsigned getAnswer();
};

class UpdateHB : public Command {
public:
	UpdateHB();
};

class ReadTouchdown : public Command {
public:
    ReadTouchdown();
    unsigned getAnswer();
};

} // end namespace CMD
