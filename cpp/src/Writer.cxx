#include "hdtree/Writer.h"

#include "hdtree/Constants.h"

namespace hdtree {

Writer::Writer(const int& event_limit, const config::Parameters& ps)
    : create_props_{},
      space_(std::vector<std::size_t>({0}), 
          std::vector<std::size_t>({HighFive::DataSpace::UNLIMITED})) {
  auto filename{ps.get<std::string>("name")};
  if (filename.empty()) {
    throw std::runtime_error("NoOutputFile",
        "No output file was provided to fire\n"
        "         Use `p.output_file = 'my-file.h5'` in your python config.",
        false);
  }
  file_ = std::make_unique<HighFive::File>(filename,
        HighFive::File::Create | HighFive::File::Truncate);
  // down here with = to allow implicit cast from 'int' to 'std::size_t'
  entries_ = event_limit;
  rows_per_chunk_ = ps.get<int>("rows_per_chunk");
  // copy creation properties into HighFive structure
  create_props_.add(HighFive::Chunking({rows_per_chunk_}));
  if (ps.get<bool>("shuffle")) create_props_.add(HighFive::Shuffle());
  create_props_.add(HighFive::Deflate(ps.get<int>("compression_level")));
}

Writer::~Writer() { this->flush(); }

void Writer::flush() {
  for (auto& [path, buff] : buffers_) {
    buff->flush();
  }
  file_->flush();
}

const std::string& Writer::name() const { return file_->getName(); }

void Writer::structure(const std::string& full_path, const std::pair<std::string,int>& type) {
  if (file_->exist(full_path)) {
    // group already been written to, check that we are the same
    auto grp = file_->getGroup(full_path);
    int v;
    grp.getAttribute(constants::VERS_ATTR_NAME).read(v);
    std::string t;
    grp.getAttribute(constants::TYPE_ATTR_NAME).read(t);
    if (t != type.first or v != type.second) {
      std::stringstream ss;
      ss << "Attempting to write multiple types (or versions of a type) as the same output data.\n"
         << "     Data: " << full_path << "\n"
         << "     Type Already on Disk: " << t << " (version " << v << ")\n"
         << "     Type to Write: " << type.first << " (version " << type.second << ")";
      throw Exception("MisType",ss.str(),false);
    }
  } else {
    // group structure doesn't exist yet, put in this type
    auto grp = file_->createGroup(full_path);
    grp.createAttribute(constants::TYPE_ATTR_NAME, type.first);
    grp.createAttribute(constants::VERS_ATTR_NAME, type.second);
  }
}

}  // namespace fire::h5
