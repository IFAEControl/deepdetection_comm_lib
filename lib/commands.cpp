#include <array>

#ifdef _WIN32
#pragma once
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "commands.hpp"

Command::Command(const std::string& name) {
    m.body["command"] = "ctrl." + name;

    // Defaults
    m.body["answer"] = json({});
    m.body["arguments"] = json({});
}

Message& Command::getMessage() {
    auto buf = m.body.dump();

    m.header.packtype = HEADER_PACKTYPE::COMMAND;
    m.header.packetsize = htonl(buf.size());
    return m;
}

std::ostream& operator<<(std::ostream& stream, const Command& c) {
    stream << c.m.body;
    return stream;
}

namespace CMD {

ReadTemperature::ReadTemperature(unsigned chips_bitmap)
    : Command("read_temperature") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

unsigned ReadTemperature::getAnswer() {
    return m.body["answer"]["temperature"];
}

FullArrayReadTemperature::FullArrayReadTemperature(unsigned chips_bitmap)
    : Command("full_read_temperature") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

UnsignedArray<30> FullArrayReadTemperature::getAnswer() {
    return m.body["answer"]["temperature"];
}

SetHV::SetHV(unsigned counts) : Command("set_hv") {
    json args;
    args["counts"] = counts;
    m.body["arguments"] = args;
}

SetTPDAC::SetTPDAC(unsigned counts) : Command("set_tpdac") {
    json args;
    args["counts"] = counts;
    m.body["arguments"] = args;
}

ChipRegisterWrite::ChipRegisterWrite(UnsignedArray<5>& val, unsigned chips_bitmap)
    : Command("chip_reg_write") {
    json args;
    args["chip_reg"] = val.val();
    m.body["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

ChipRegisterRead::ChipRegisterRead(unsigned chips_bitmap)
    : Command("chip_reg_read") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

UnsignedArray<5> ChipRegisterRead::getAnswer() {
    return m.body["answer"]["chip_reg"];
}

FullArrayChipRegisterRead::FullArrayChipRegisterRead(unsigned chips_bitmap)
    : Command("full_chip_reg_read") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

UnsignedArray<150> FullArrayChipRegisterRead::getAnswer() {
    return m.body["answer"]["chip_reg"];
}

PixelRegisterWrite::PixelRegisterWrite(UnsignedArray<480>& val, unsigned chips_bitmap)
    : Command("pixel_reg_write") {
    json args;
    args["pixel_reg"] = val.val();
    m.body["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

PixelRegisterRead::PixelRegisterRead(unsigned chips_bitmap)
    : Command("pixel_reg_read") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

UnsignedArray<480> PixelRegisterRead::getAnswer() {
    return m.body["answer"]["pixel_reg"];
}

FullArrayPixelRegisterRead::FullArrayPixelRegisterRead(unsigned chips_bitmap)
    : Command("full_pixel_reg_read") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

UnsignedArray<14400> FullArrayPixelRegisterRead::getAnswer() {
    return m.body["answer"]["pixel_reg"];
}

ReadEricaID::ReadEricaID(unsigned chips_bitmap) : Command("read_chip_id") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

unsigned ReadEricaID::getAnswer() {
    return m.body["answer"]["chip_id"];
}

FullArrayReadEricaID::FullArrayReadEricaID(unsigned chips_bitmap)
    : Command("read_full_chip_id") {
    json args;
    args["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

UnsignedArray<30> FullArrayReadEricaID::getAnswer() {
    return m.body["answer"]["chip_id"];
}

ACQuisition::ACQuisition(const AcqInfo& info, unsigned frames, unsigned chips_bitmap)
    : Command("acq") {
    json args;
    args["belt_dir"] = info.belt_dir;
    args["chips_bitmap"] = chips_bitmap;
    args["frames"] = frames;
    args["pulses"] = info.pulses;
    args["pulses_width"] = info.pulses_width;
    args["tdi"] = info.tdi;
    args["test_pulses"] = info.test_pulses;
    args["timer_reg"] = info.timer_reg;
    args["continuous"] = false;
    m.body["arguments"] = args;
}

UnsignedArray<480> ACQuisition::getAnswer() {
    return m.body["answer"]["frame"];
}

ACQuisitionCont::ACQuisitionCont(const AcqInfo& info, unsigned chips_bitmap)
    : Command("acq") {
    json args;
    args["belt_dir"] = info.belt_dir;
    args["chips_bitmap"] = chips_bitmap;
    args["pulses"] = info.pulses;
    args["pulses_width"] = info.pulses_width;
    args["tdi"] = info.tdi;
    args["test_pulses"] = info.test_pulses;
    args["timer_reg"] = info.timer_reg;
    args["continuous"] = true;
    m.body["arguments"] = args;
}

ACQuisitionStop::ACQuisitionStop() : Command("acq_stop") {}

CameraReset::CameraReset() : Command("camera_reset") {}

ControllerReset::ControllerReset() : Command("controller_reset") {}

LoadFloodNormFactors::LoadFloodNormFactors(UnsignedArray<60>& val, unsigned chips_bitmap)
    : Command("load_flood_norm_factors") {
    json args;
    args["factors"] = val.val();
    m.body["chips_bitmap"] = chips_bitmap;
    m.body["arguments"] = args;
}

GetAllRegs::GetAllRegs() : Command("get_all_regs") {}

json GetAllRegs::getAnswer() {
    return m.body["answer"];
}

GetDataIRQs::GetDataIRQs() : Command("get_data_irqs") {}

unsigned GetDataIRQs::getAnswer() {
    return m.body["answer"]["val"];
}

UpdateHB::UpdateHB() : Command("update_hb") {}

} // end namespace CMD
