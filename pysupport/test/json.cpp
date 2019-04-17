#include <cstdlib>

#include "dbglog/dbglog.hpp"

#include "jsoncpp/io.hpp"

#include "pydbglog/dbglogmodule.hpp"
#include "pysupport/import.hpp"
#include "pysupport/formatexception.hpp"
#include "pysupport/json.hpp"

namespace bp = boost::python;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Invalid number of arguments." << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize python interpreter
    ::Py_Initialize();
    try {
        pysupport::formatCurrentException();
        dbglog::py::import();

        LOG(info4) << "Importing <" << argv[1] << ">";

        Json::write(std::cout
                    , pysupport::asJson
                    (pysupport::import(argv[1]).attr("run")()));
    } catch (bp::error_already_set) {
        std::cerr << pysupport::formatCurrentException();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
