#pragma once

#include <condition_variable>
#include <thread>

#include <nlohmann/json.hpp>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>

#include "commands.hpp"
#include "header.hpp"
#include "frame_buffer.hpp"

extern FrameBuffer fb;

class CmdSender {
public:
	CmdSender() =default;
	CmdSender(const std::string& ip, unsigned short p);
	template<typename T> T sendCommand(T&& c);
private:
	Poco::Net::SocketAddress _sa;
	Poco::Net::StreamSocket _socket;
};

class DataReceiver {
public:
	DataReceiver() =default;
	DataReceiver(const std::string& ip, unsigned short port);
	int initThread();
	void joinThread();
	bool threadRunning() const;

	unsigned getTimeouts() const;
	void resetTimeouts();

	std::string& getLastError();

private:
	void readerThread();
	int connect();

	unsigned _timeouts{0};
	bool _thread_running{false};
	std::string _last_error_msg{};
	std::thread _reader{};

	Poco::Net::SocketAddress _sa;
    Poco::Net::DatagramSocket _dgs;

};

class Networking {
public:
	int initialize(std::string ip, unsigned short port, unsigned short aport);
	template<typename T> T sendCommand(T&& c);
	int initReceiverThread();
	void joinThread();
	unsigned getTimeoutsCounter() const;
	void resetTimeoutsCounter();
	std::string& getLastError();
private:
	std::string _ip{"8.8.8.8"};
	CmdSender _cmd_sender;
	DataReceiver _data_receiver;
};
