import cStringIO
import traceback

import sys

def format(type, value, tb):
    out = cStringIO.StringIO()
    traceback.print_exception(type, value, tb, None, out)
    return out.getvalue()
