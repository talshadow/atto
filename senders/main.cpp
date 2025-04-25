#include <future>
#include "udpsender.hpp"
#include <threadpool.hpp>
#include <json_data_reader.h>
#include <boost/program_options.hpp>
#include <filesystem>
#include <cstdlib>

constexpr auto SENDER_AMOUNT = 1U;

namespace po=boost::program_options;

int main(int argc, char**argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")("file,f", po::value<std::string>(), "path to file with data");
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
        LOG_ERROR_MESSAGE("Catched exception: ",e.what());
    }
    return EXIT_SUCCESS;
}
