#include <cmdstan/command.hpp>
#include <stan/services/error_codes.hpp>

int main(int argc, const char *argv[]) {
  std::cout << "############################################################\n"
	    << "## Welcome to the CmdStan v2.29.1 - the Wordle Version    ##\n"
	    << "############################################################\n"
	    << std::endl;
  try {
    int err_code = cmdstan::command(argc, argv);
    if (err_code == 0)
      return cmdstan::return_codes::OK;
    else
      return cmdstan::return_codes::NOT_OK;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return cmdstan::return_codes::NOT_OK;
  }
}
