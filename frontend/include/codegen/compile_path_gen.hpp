#ifndef IR_PATH_GEN_HPP
#define IR_PATH_GEN_HPP

#include <filesystem>

namespace language {

struct Compile_paths {
    std::filesystem::path ll;
    std::filesystem::path exe;
};

inline std::filesystem::path default_compl_out_dir() {
    if (const char *p = std::getenv("COMPL_OUT_DIR"); p && *p)
        return std::filesystem::path(p);

    std::error_code ec;
    auto cwd = std::filesystem::current_path(ec);
    if (!ec)
        return cwd / "build/compile_out";

    return std::filesystem::temp_directory_path() / "compile_out";
}

inline Compile_paths
make_compile_paths(std::string_view basename = "out",
                   std::filesystem::path base = default_compl_out_dir()) {
    std::filesystem::create_directories(base);
    std::string stem = std::filesystem::path(basename).stem().string();

    if (stem.empty()) {
        stem = "out";
    }

    return {base / (stem + ".ll"), base / (stem)};
}

} // namespace language

#endif // IR_PATH_GEN_HPP
