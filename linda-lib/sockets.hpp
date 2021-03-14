#pragma once

#include <nlohmann/json.hpp>

#include "commands.hpp"
#include "header.hpp"

std::unique_ptr<char> read_bytes(unsigned bytes);


Message send_command(Message& c);

template<typename T>
T send_command(T& c);

void set_dest_ip(const std::string& ip) noexcept;
void set_ports(unsigned p, unsigned ap) noexcept;
