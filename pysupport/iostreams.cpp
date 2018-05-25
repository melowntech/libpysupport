#include <boost/iostreams/filtering_stream.hpp>

#include "./iostreams.hpp"
#include "./hasattr.hpp"

namespace bp = boost::python;
namespace bi = boost::iostreams;

namespace pysupport {

namespace detail {

template <typename Category>
class Device {
public:
    typedef char char_type;
    typedef Category category;

    Device(const bp::object &read, const bp::object &write
           , const bp::object &seek)
        : read_(read), write_(write), seek_(seek)
    {}

    std::streamsize write(const char *s, std::streamsize n) {
        return bp::extract<int>(write_(bp::str(s, n)));
    }

    std::streamsize read(char *data, std::streamsize size
                         , bi::stream_offset pos)
    {
        auto str(read_(size));
        const char *raw(bp::extract<const char*>(str));
        int len(bp::extract<int>(str.attr("__len__")()));
        std::memcpy(data, raw, len);
        return len;
    }

    std::streampos seek(bi::stream_offset off
                        , std::ios_base::seekdir way)
    {
        int whence(0);
        switch (way) {
        case std::ios_base::beg: whence = 0; break;
        case std::ios_base::cur: whence = 1; break;
        case std::ios_base::end: whence = 2; break;
        default: break; // ???
        }

        return std::streampos(bp::extract<int>(seek_(off, whence)));
    }

private:
    bp::object read_;
    bp::object write_;
    bp::object seek_;
};

struct Output : bi::device_tag, bi::output {};
struct SeekableOutput : bi::device_tag, bi::output_seekable {};

class OStreamImpl : public OStream {
public:
    OStreamImpl(const bp::object &obj) {
        bp::object write(obj.attr("write"));
        if (pysupport::hasattr(obj, "seek")) {
            bp::object seek(obj.attr("seek"));
            fis_.push(std::move(Device<SeekableOutput>({}, write, seek)));
        } else {
            fis_.push(std::move(Device<Output>({}, write, {})));
        }
    }

    virtual ~OStreamImpl() {}

    virtual std::ostream& ostream() { return fis_; }

private:
    bi::filtering_ostream fis_;
};

struct from_python_ostream
{
    typedef bp::converter::rvalue_from_python_stage1_data Data;
    typedef bp::converter::rvalue_from_python_storage<
        OStream::pointer> Storage;

    from_python_ostream() {
        bp::converter::registry::push_back
            (&convertible, &construct, bp::type_id<OStream::pointer>());
    }

    // Determine if ptr can be converted to OStream::pointer
    static void* convertible(::PyObject *ptr) {
        if (::PyObject_HasAttrString(ptr, "write")) { return ptr; }
        return nullptr;
    }

    // Convert ptr into a fs::path
    static void construct(::PyObject *ptr, Data *data) {
        // get memory
        void *storage(((Storage*) data)->storage.bytes);

        new (storage) OStream::pointer
            (std::make_shared<OStreamImpl>
             (bp::object(bp::handle<>(bp::borrowed<>(ptr)))));

        // Stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

} // namespace detail

void registerIOStreams()
{
    detail::from_python_ostream();
}

} // namespace pysupport
