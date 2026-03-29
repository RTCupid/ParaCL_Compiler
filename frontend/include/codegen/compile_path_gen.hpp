#ifndef IR_PATH_GEN_HPP
#define IR_PATH_GEN_HPP

#include <filesystem>
#include <string>
#include <cstring>
#include <optional>
#include <utility>

namespace language {

struct Compile_paths {
    std::filesystem::path ll;
    std::filesystem::path exe;
};

inline std::pair<std::optional<std::string>, const char *>
    parse_commandline_arguments(int argc, const char **argv) {
    const char* input_file = nullptr;
    std::optional<std::string> opt_output_name;

    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (strcmp(arg, "-o") == 0) {
            if (i + 1 >= argc)
                throw std::runtime_error("Option -o requires an argument");
            opt_output_name = argv[++i];
        } else if (arg[0] != '-') {
            if (input_file)
                throw std::runtime_error("Multiple input files not supported");
            input_file = arg;
        } else {
            throw std::runtime_error("Unknown option: " + std::string{arg});
        }
    }

    if (!input_file)
        throw std::runtime_error("No input file provided");

    return std::make_pair(opt_output_name, input_file);
}

inline std::filesystem::path default_compl_out_dir() {
    if (const char *p = std::getenv("COMPL_OUT_DIR"); p && *p)
        return std::filesystem::path(p);

    std::error_code ec;
    auto cwd = std::filesystem::current_path(ec);
    if (!ec)
        return cwd;

    return std::filesystem::temp_directory_path();
}

inline Compile_paths
make_compile_paths(std::optional<std::string> opt_output_name) {
    language::Compile_paths compile_paths;
    if (opt_output_name.has_value()) {
        std::filesystem::path out_exe = std::filesystem::path(opt_output_name.value());
        std::filesystem::path out_ll = out_exe;
        out_ll.replace_extension(".ll");

        std::filesystem::path base_dir = out_exe.parent_path();
        if (base_dir.empty())
            base_dir = std::filesystem::current_path();
        std::filesystem::create_directories(base_dir);

        compile_paths.ll = out_ll;
        compile_paths.exe = out_exe;
    } else {
        std::filesystem::path cwd = std::filesystem::current_path();
        compile_paths.ll = cwd / "a.ll";
        compile_paths.exe = cwd / "a.out";
    }

    return compile_paths;
}

} // namespace language

#endif // IR_PATH_GEN_HPP
