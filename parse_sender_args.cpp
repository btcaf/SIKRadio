#include <boost/program_options.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <regex>

#include "parse_sender_args.hpp"

namespace po = boost::program_options;
namespace mp = boost::multiprecision;

const size_t MAX_UDP_PACKET = 65507 - 16;
const size_t MAX_NAME_LEN = 64;

Temp_Sender parse_sender_args(int argc, char *argv[]) {
    int32_t DATA_PORT;
    int32_t PSIZE;
    std::string name;
    std::string multicast_address;
    int32_t CTRL_PORT;
    mp::int128_t FSIZE;
    mp::int128_t RTIME;

    po::options_description desc("Program usage");
    desc.add_options()
            (
                    "data-port,P",
                    po::value<int32_t>(&DATA_PORT)->default_value(29922)
            )
            (
                    "packet-size,p",
                    po::value<int32_t>(&PSIZE)->default_value(512)
            )
            (
                    "name,n",
                    po::value<std::string>(&name)->
                            default_value("Nienazwany nadajnik")
            )
            (
                    "address,a",
                    po::value<std::string>(&multicast_address)->required()
            )
            (
                    "control-port,C",
                    po::value<int32_t>(&CTRL_PORT)->default_value(39922)
            )
            (
                    "fifo-size,f",
                    po::value<mp::int128_t>(&FSIZE)->default_value(131072)
            )
            (
                    "retransmission-time,R",
                    po::value<mp::int128_t>(&RTIME)->default_value(250)
            );

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    po::notify(vm);

    if (name.length() > MAX_NAME_LEN ||
        !std::regex_match(name,
            std::regex(R"([\x21-\x7F][\x20-\x7F]*[\x21-\x7F]|[\x21-\x7F])"))) {
        throw std::runtime_error("Invalid name");
    }

    if (DATA_PORT < 0 || DATA_PORT > UINT16_MAX) {
        throw std::runtime_error("Invalid data port number");
    }

    if (PSIZE <= 0 || static_cast<size_t>(PSIZE) > MAX_UDP_PACKET) {
        throw std::runtime_error("Invalid packet size");
    }

    if (CTRL_PORT < 0 || CTRL_PORT > UINT16_MAX) {
        throw std::runtime_error("Invalid control port number");
    }

    if (FSIZE < 0 || FSIZE > SIZE_MAX) {
        throw std::runtime_error("Invalid queue size");
    }

    if (RTIME <= 0 || RTIME > UINT64_MAX) {
        throw std::runtime_error("Invalid time");
    }

    return {static_cast<uint16_t>(DATA_PORT), static_cast<size_t>(PSIZE),
            name, multicast_address, static_cast<uint16_t>(CTRL_PORT),
            static_cast<size_t>(FSIZE), static_cast<uint64_t>(RTIME)};
}