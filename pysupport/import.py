# Copyright (c) 2017 Melown Technologies SE
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# *  Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# *  Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# This glue code is used by pysupport/import.cpp
#
# Input: name, path, module_type
# Output: module
# Throws: ImportError
#

import sys
import dbglog as log

log.info1("Importing module {}{} from path \"{}\"."
          , name, "[{}]".format(module_type) if module_type is not None else ""
          , path)

if module_type is not None:
    sys.path.insert(0, path)

if module_type == "zip":
    # ZIP archive
    import zipimport

    z = zipimport.zipimporter(path)
    module = z.load_module(name)

else:
    # everything else
    import imp
    spec = imp.find_module(name, [path])

    try:
        module = imp.load_module(name, *spec)
    finally:
        if spec[0] is not None:
            spec[0].close()

