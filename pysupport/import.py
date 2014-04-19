# This glue code used by pysupport/import.cpp
#
# Input: name, path
# Output: module
# Throws: ImportError
#

import imp
import dbglog as log

log.info3("Importing module {} from path {}.", name, path)
spec = imp.find_module(name, [path])

try:
    module = imp.load_module(name, *spec)
finally:
    if spec[0] is not None:
        spec[0].close()
