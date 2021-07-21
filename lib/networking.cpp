#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include <Poco/Net/SocketStream.h>
#include <Poco/StreamCopier.h>

#include "log.hpp"
#include "networking.hpp"

#define TEMPLATE_COMMAND(V) template V Networking::sendCommand<V>(V && c)

using Poco::Net::IPAddress;
using namespace CMD;

FrameBuffer fb;

CmdSender::CmdSender(const std::string& ip, unsigned short p) : _sa{ip, p} {}

template <typename T> T CmdSender::sendCommand(T&& c) {
    auto& m = c.getMessage();
    _socket.connect(_sa);
    _socket.sendBytes(&m.header, HEADER_BYTE_SIZE);
    Poco::Net::SocketStream str(_socket);
    str << m.body;
    str.flush();
    std::stringstream ss;
    Poco::StreamCopier::copyStream(str, ss);
    std::memcpy(&m.header, ss.str().c_str(), HEADER_BYTE_SIZE);
    if(m.header.packtype == HEADER_PACKTYPE::ERROR) {
        _socket.close();
        throw std::runtime_error("Command error");
    }
    m.body = json::parse(ss.seekg(HEADER_BYTE_SIZE));
    _socket.close();
    return std::move(c);
}

DataReceiver::DataReceiver(const std::string& ip, unsigned short p)
    : _sa{ip, p} {}

int DataReceiver::initThread() {
    if(connect() < 0) return -1;

    if(!_thread_running) {
        _thread_running = true;
        _reader = std::thread(&DataReceiver::readerThread, this);
    }

    return 0;
}

void DataReceiver::readerThread() {
    constexpr unsigned max_dgram_size = 57608; // 1920*30(chips) + 8(header size)
    std::optional<uint16_t> old_pnum{};
    while(_thread_running) {
        try {
            char buf[max_dgram_size];
            _dgs.receiveBytes(&buf, max_dgram_size);

            BaseHeaderType header;
            memcpy(&header, buf, sizeof(header));
            if(header.packtype == HEADER_PACKTYPE::ERROR) {
                _timeouts++;
                logger->error("Error on asyncs");

                // Remove buffered data
                _dgs.close();
                connect();
                fb.cancel();
                continue;
            }

            auto number = header.number;
            if(old_pnum.has_value() && uint16_t(number - 1) != old_pnum) {
                logger->warn(
                    "Packets lost. Last seen packet={}, current packet={}",
                    old_pnum.value(), uint16_t(number));
            }
            old_pnum = number;

            auto bytes = header.packetsize;
            Frame f(bytes);
            memcpy(f.get(), buf + sizeof(header), bytes);

            fb.addFrame(std::move(f));
        } catch(Poco::TimeoutException& e) {
            // ignore timeouts
        }
    }
}

bool DataReceiver::threadRunning() const {
    return _thread_running;
}

int DataReceiver::connect() {
    _dgs.connect(_sa);
    _dgs.setBlocking(true);
    _dgs.setReceiveTimeout(Poco::Timespan(1, 0));
    unsigned tries = 10;
    for(unsigned i = 0; i < tries; i++) {
        // Tell the server we are listening
        unsigned char c = 0xff;
        _dgs.sendBytes(&c, 1);

        try {
            _dgs.receiveBytes(&c, 1);
            // If we receive a packet then client has been registered
            return 0;
        } catch(Poco::TimeoutException& e) {
            logger->warn("Registration: try {} failed", i);
        }
    }
    logger->error("Given up. Registration failed");
    return -1;
}

void DataReceiver::joinThread() {
    if(_thread_running) {
        _thread_running = false;
        _reader.join();
    }
}

unsigned DataReceiver::getTimeouts() const {
    return _timeouts;
}

void DataReceiver::resetTimeouts() {
    _timeouts = 0;
}

int Networking::initialize(std::string ip, unsigned short port, unsigned short aport) {
    if(_data_receiver.threadRunning()) {
        logger->error("Already configured, doing nothing");
        return -1;
    }

    _ip = ip;
    _cmd_sender = CmdSender(_ip, port);
    _data_receiver = DataReceiver(_ip, aport);
    return 0;
}

int Networking::initReceiverThread() {
    return _data_receiver.initThread();
}

void Networking::joinThread() {
    _data_receiver.joinThread();
}

template <typename T> T Networking::sendCommand(T&& c) {
    return std::move(_cmd_sender.sendCommand(std::move(c)));
}

unsigned Networking::getTimeoutsCounter() const {
    return _data_receiver.getTimeouts();
}

void Networking::resetTimeoutsCounter() {
    _data_receiver.resetTimeouts();
}

TEMPLATE_COMMAND(ReadTemperature);
TEMPLATE_COMMAND(FullArrayReadTemperature);
TEMPLATE_COMMAND(SetHV);
TEMPLATE_COMMAND(SetTPDAC);
TEMPLATE_COMMAND(ChipRegisterWrite);
TEMPLATE_COMMAND(ChipRegisterRead);
TEMPLATE_COMMAND(PixelRegisterWrite);
TEMPLATE_COMMAND(PixelRegisterRead);
TEMPLATE_COMMAND(ReadEricaID);
TEMPLATE_COMMAND(FullArrayReadEricaID);
TEMPLATE_COMMAND(FullArrayChipRegisterRead);
TEMPLATE_COMMAND(FullArrayPixelRegisterRead);
TEMPLATE_COMMAND(ACQuisition);
TEMPLATE_COMMAND(ACQuisitionCont);
TEMPLATE_COMMAND(ACQuisitionStop);
TEMPLATE_COMMAND(CameraReset);
TEMPLATE_COMMAND(ControllerReset);
TEMPLATE_COMMAND(LoadFloodNormFactors);
TEMPLATE_COMMAND(GetAllRegs);
TEMPLATE_COMMAND(GetDataIRQs);
TEMPLATE_COMMAND(UpdateHB);
