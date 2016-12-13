#include <QApplication>
#include "fermiplot.h"
#include "mainwindow.h"
#include <boost/program_options.hpp>
#include <stdio.h>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    po::variables_map vm;

    try {
        desc.add_options()
            ("help,h", "Produce help message.")
            ("gui", "Run graphical user interface.")
            ("logft", "Calculate logft value. Have to specify options --Z, --Q, --t, --I, --mode.")
            ("integral", "Calculate fermi integral. Have to specify --Z and --mode. Options --from, --to, --tick are optional.")
            ("Z", po::value<int>(), "specify Z of daughter nucleus")
            ("Q", po::value<double>(), "Q energy in MeV")
            ("t", po::value<std::string>(), "half-life time")
            ("I", po::value<double>(), "intensity of the decay in %")
            ("mode", po::value<std::string>(), "beta decay mode (minus, plus or EC)")
            ("from", po::value<double>(), "Low end of integral range.")
            ("to", po::value<double>(), "High end of integral range.")
            ("tick", po::value<double>(), "Tick between integral values.")
        ;

        po::store(po::command_line_parser(argc, argv).
                  options(desc).run(), vm);
        po::notify(vm);
    } catch (po::error& ex) {
        std::cerr << ex.what() << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }


    if (vm.count("logft")) {
        if (!vm.count("Z")) {
            std::cerr << "You need to specify value of Z!" << std::endl;
        } else if (!vm.count("Q")) {
            std::cerr << "You need to specify value of Q!" << std::endl;
        } else if (!vm.count("t")) {
            std::cerr << "You need to specify half-time!" << std::endl;
        } else if (!vm.count("I")) {
            std::cerr << "You need to specify intensity of decay!" << std::endl;
        } else if (!vm.count("mode")) {
            std::cerr << "You need to specify decay mode!" << std::endl;
        } else {
            int z = vm["Z"].as<int>();
            double q = vm["Q"].as<double>();
            std::string halftime = vm["t"].as<std::string>();
            double intensity = vm["I"].as<double>() / 100;

            double time;
            char unit[10];
            sscanf(halftime.c_str(), "%lf%s", &time, unit);

            time = convert_to_seconds(time, std::string(unit));

            std::string mode = vm["mode"].as<std::string>();

            if (validate_data(z, q, time, intensity)) {
                bool positron = mode == "minus" ? false : true;

                //TODO: Verify that arguments are in acceptable range.
                double logft_value = logft(q, z, positron, time, intensity);

                std::cout << logft_value << std::endl;
            }
        }
    } else if (vm.count("integral")) {
        if (!vm.count("Z")) {
            std::cerr << "You need to specify value of Z!" << std::endl;
        } else if (!vm.count("mode")) {
            std::cerr << "You need to specify decay mode!" << std::endl;
        } else {
            int z = vm["Z"].as<int>();
            double from = 0.01;
            double to = 25.044;
            double tick = 0.1;

            if (vm.count("from")) {
                from = vm["from"].as<double>();
            }
            if (vm.count("to")) {
                from = vm["to"].as<double>();
            }
            if (vm.count("till")) {
                from = vm["till"].as<double>();
            }

            //TODO: Diffrentiate plus and EC
            std::string mode = vm["mode"].as<std::string>();
            bool positron = mode == "minus" ? false : true;

            for (double i = from; i < to; i = i + tick) {
                double fval = f_value(i, z, positron);
                std::cout << fval << std::endl;
            }
        }
    }

    if (vm.count("gui")) {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec();
    } else {
        return 0;
    }
}
