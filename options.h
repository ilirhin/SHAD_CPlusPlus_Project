#include "strategy.h"
#include "client.h"

class Options {
public:
    explicit Options(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << GetUsageMessage(std::string(argv[0]));
            exit(0);
        }

        std::string port = "-1";
        std::string str = "0";
        std::string mov_str;
        std::string count;

        int cur_param = 1;

        while (cur_param < argc) {
            std::string cur_param_name = std::string(argv[cur_param]);

            if (cur_param_name == PORT_PARAM_NAME) {
                port = argv[cur_param + 1];
                cur_param += 2;
            } else if (cur_param_name == GLOBAL_STR_PARAM_NAME) {
                str = argv[cur_param + 1];
                cur_param += 2;
            } else if (cur_param_name == MOVEMENT_STR_PARAM_NAME) {
                mov_str = argv[cur_param + 1];
                cur_param += 2;
            } else if (cur_param_name == COINS_COUNT_PARAM_NAME) {
                count = argv[cur_param + 1];
                cur_param += 2;
            } else if (cur_param_name == HELP_MESSAGE_NAME) {
                std::cerr << GetHelpMessage(argv[0]) << "\n";
                exit(0);
            } else {
                std::cerr << GetUnknownParameterMessage(argv[0], argv[cur_param]) << "\n";
                std::cerr << GetHelpMessage(argv[0]) << "\n";
                exit(0);
            }
        }

        port_ = std::atoi(port.c_str());

        if (str == NEAREST_COIN_STR) {
            globalStrategy_ = std::make_shared<GlobalStrategy>(new NearestCoinStrategy(1));
        } else if (str == K_NEAREST_COIN_STR) {
            globalStrategy_ = std::make_shared<GlobalStrategy>(new KNearestCoinsStrategy(1, std::atoi(count.c_str())));
        } else {
            std::cerr << GetWrongParameterMessage(argv[0], GLOBAL_STR_PARAM_NAME);
            exit(0);
        }

        if (mov_str == MOVE_STR_FIRST) {
            movementStrategy_ = std::make_shared<MovementStrategy>(new FirstMovementStrategyImpl());
        } else if (mov_str == MOVE_STR_SECOND) {
            movementStrategy_ = std::make_shared<MovementStrategy>(new SecondMovementStrategyImpl());
        } else if (mov_str == MOVE_STR_RANDOM) {
            movementStrategy_ = std::make_shared<MovementStrategy>(new RandomMovementStrategyImpl());
        } else {
            std::cerr << GetWrongParameterMessage(argv[0], MOVEMENT_STR_PARAM_NAME);
            exit(0);   
        }
    }

    int GetPort() const {
        return port_;
    }

    std::shared_ptr<GlobalStrategy> GetGlobalStrategy() {
        return globalStrategy_;
    }

    std::shared_ptr<MovementStrategy> GetMovementStrategy() {
        return movementStrategy_;
    }

private:
    const std::string PORT_PARAM_NAME         = "--port";
    const std::string GLOBAL_STR_PARAM_NAME   = "--global-strategy";
    const std::string MOVEMENT_STR_PARAM_NAME = "--movement-strategy";
    const std::string COINS_COUNT_PARAM_NAME  = "--coins-count";
    const std::string HELP_MESSAGE_NAME       = "--help";

    const std::string NEAREST_COIN_STR        = "nearest-coins-strategy";
    const std::string K_NEAREST_COIN_STR      = "k-nearest-coins-strategy";

    const std::string MOVE_STR_FIRST          = "first";
    const std::string MOVE_STR_SECOND         = "second";
    const std::string MOVE_STR_RANDOM         = "random";

    std::string GetUnknownParameterMessage(const std::string& app_name, const std::string& par_name) {
        std::string message = app_name + ": unknown option " + par_name;
        return message;
    }

    std::string GetWrongParameterMessage(const std::string& app_name, const std::string& par_name) {
        std::string message = app_name + ": unknown argument for " + par_name;
    }

    std::string GetUsageMessage(const std::string& app_name) {
        std::string message = "Try \'" + app_name + " " + HELP_MESSAGE_NAME + "\' for more information";
        return message;
    }

    std::string GetHelpMessage(const std::string& app_name) {
        std::string help_message = "Usage: " + app_name + " " +
                                        PORT_PARAM_NAME + " PORT " +
                                        GLOBAL_STR_PARAM_NAME + " STRATEGY " +
                                        MOVEMENT_STR_PARAM_NAME + " MOVEMENT-STRATEGY " +
                                        COINS_COUNT_PARAM_NAME + " COUNT" + "\n" +
                                        "  " + GLOBAL_STR_PARAM_NAME + "   nearest-coins-strategy, k-nearest-coin-strategy" + "\n" +
                                        "  " + COINS_COUNT_PARAM_NAME + "       parameter for k-nearest coin strategy" + "\n" +
                                        "  " + MOVEMENT_STR_PARAM_NAME + " first, second or random";
        return help_message;
    }

	int port_;
	std::shared_ptr<GlobalStrategy> globalStrategy_;
	std::shared_ptr<MovementStrategy> movementStrategy_;
};


