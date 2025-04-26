#include "udpsender.hpp"
#include <boost/program_options.hpp>
#include <cstdlib>
#include <filesystem>
#include <future>
#include <json_data_reader.h>
#include <threadpool.hpp>

constexpr auto SENDER_AMOUNT = 1U;

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()("help,h","produce help message")
                      ("file,f", po::value<std::string>(),"path to file with data")
                      ("port,p", po::value<unsigned>(), "server prot")
                      ("address,a", po::value<std::string>(),"server ip adress");
    // clang-format on
    std::string fileName;
    std::string ipAdress{bclasses::DefaultAdress};
    unsigned ipPort{bclasses::UDPPortFirst};

    try {
        po::variables_map varMap;
        po::store(po::parse_command_line(argc, argv, desc), varMap);
        po::notify(varMap);

        if (varMap.count("help")) {
            std::cout << desc << "\n";
            return EXIT_FAILURE;
        }

        if (varMap.count("file")) {
            fileName = varMap["file"].as<std::string>();
            if (!std::filesystem::exists(fileName)) {
                LOG_ERROR_MESSAGE("No file found {}", fileName);
                return EXIT_FAILURE;
            }
            LOG_INFO_MESSAGE("Data file is: {}", fileName);
        } else {
            LOG_ERROR_MESSAGE("Mandatory data file wasn't provided");
            return EXIT_FAILURE;
        }

        if (varMap.count("port")) {
            ipPort = varMap["port"].as<unsigned>();
        }

        if (varMap.count("address")) {
            ipAdress = varMap["address"].as<std::string>();
        }

    } catch (po::error const& error) {
        LOG_ERROR_MESSAGE("options: {} ", error.what());
        return EXIT_FAILURE;
    }

    auto sendingData = bclasses::load_data_extend_debug(fileName);
    bclasses::TimeCalc timer;

    try {
        bclasses::Shared_ptr<UDPSender> m_sender;
        constexpr unsigned ThreadsCount = 2;

        auto pool = bclasses::ThreadPool::createInstance(ThreadsCount);
        LOG_INFO_MESSAGE(std::format("Duration after Thread Pool initialization: {}", timer.getDuration()));

        auto sender = UDPSender::instance(pool->service(), std::move(sendingData), ipAdress, ipPort);
        LOG_INFO_MESSAGE(std::format("Duration after starts: {}", timer.getDuration()));

        sender->getIsFinishedMarker().get();
        LOG_INFO_MESSAGE(std::format("Duration: {}", timer.getDuration()));

    } catch (std::exception& e) {
        LOG_ERROR_MESSAGE("Catched exception: ", e.what());
    }
    return EXIT_SUCCESS;
}
