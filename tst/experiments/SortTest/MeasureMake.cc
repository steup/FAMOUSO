#include "logging/logging.h"

using namespace ::logging;

#include "sstream"
#include "iostream"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "time.h"
#include "sys/time.h"

const char* generateListString(const int length, const bool reverse) {
    std::ostringstream buf;

    buf << "boost::mpl::vector_c\\<int";

    for (int i = 0; i < length; ++i) {
        buf << ",";

        if (reverse) {
            buf << (length - i);
        } else {
            buf << (i + 1);
        }

        // TODO: Why does this sometimes not work if I omit this?
        buf.str();
    }

    buf << "\\>::type";

    std::string res(buf.str());

    return (res.c_str());
}

int main(int argc, char **argv) {
    if (argc < 2) {
        log::emit() << "No make target given" << log::endl;
        log::emit() << log::endl;
        log::emit() << "usage: MeasureMake MakeTarget" << log::endl;
        log::emit() << log::endl;
    } else {
        for (int b = 0; b < 2; ++b) {
            for (int j = 0; j < 20; ++j) {
                std::string cmd("make ");

                cmd.append(argv[1]);

                if (argc > 2) {
                    for (int i = 2; i < argc; ++i) {
                        cmd.append(" ");
                        cmd.append(argv[i]);
                    }
                }

                cmd.append(" -B ");

                cmd.append("\"LIST=").append(generateListString((j + 1), (b != 0))).append("\"");

                log::emit() << "Executing \"" << cmd.c_str() << "\"" << log::endl;

                timeval t1;
                timeval t2;

                gettimeofday(&t1, NULL);

                const int exitCode = system(cmd.c_str());

                if (exitCode == 2) {
                    log::emit() << "Execution interrupted, exiting..." << log::endl;

                    exit(2);
                }

                gettimeofday(&t2, NULL);

                const uint64_t result = ((t2.tv_sec * 1000) + (t2.tv_usec / 1000)) - ((t1.tv_sec * 1000) + (t1.tv_usec / 1000));

                log::emit() << "Finished, took " << result << " ms, exit code: " << exitCode << log::endl;

                log::emit() << "Executing program:" << log::endl;

                const int execExitCode = system(std::string().append("./").append(argv[1]).c_str());

                log::emit() << "Progam finished with exit code " << execExitCode << "." << log::endl << log::endl;
            }
        }
    }
}
