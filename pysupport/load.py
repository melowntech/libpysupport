# This glue code used by pysupport/load.cpp
#
# Input: fullname, contents, binary
# Output: module
# Throws: ImportError
#

class Loader:
    def __init__(self, fullname, contents, compiled=False):
        self.fullname = fullname
        self.contents = contents
        self.compiled = compiled

    def load_module(self, fullname):
        # this must be here
        import sys, imp
        if fullname in sys.modules:
            return sys.modules[fullname]

        mod = sys.modules.setdefault(fullname, imp.new_module(fullname))
        mod.__file__ = "<%s>" % fullname
        mod.__loader__ = self

        if self.compiled:
            import marshal
            # TODO: use buffer slice
            code = marshal.loads(self.contents[8:])
        else:
            code = compile(self.contents, mod.__file__, "exec")

        exec code in mod.__dict__
        return mod

module = Loader(fullname, contents, compiled).load_module(fullname)
