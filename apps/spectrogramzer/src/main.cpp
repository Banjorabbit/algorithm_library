#include <cxxopts.hpp>
#include <iostream>

// set constants
enum Languages {EN, DE, ES, FR};
constexpr int VERSION = 1;
int main(int argc, char** argv)
{
    const std::unordered_map<std::string, Languages> languages{
      {"en", EN},
      {"de", DE},
      {"es", ES},
      {"fr", FR},
  };

    cxxopts::Options options(*argv, "A program to welcome the world!");
    
    std::string language;
    std::string name;

    options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("n,name", "Name to greet", cxxopts::value(name)->default_value("World"))
    ("l,lang", "Language code to use", cxxopts::value(language)->default_value("en"));
  
    options.allow_unrecognised_options();
    
    auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result["version"].as<bool>()) {
    std::cout << "version " << VERSION << std::endl;
    return 0;
  }

  auto langIt = languages.find(language);
  if (langIt == languages.end()) {
    std::cerr << "unknown language code: " << language << std::endl;
    return 1;
  }

    std::cout << "DONE!\n" << std::endl;

  return 0;
}