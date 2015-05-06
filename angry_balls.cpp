#include <iostream>
#include <string>

#include "protocol.h"

int main() {
    std::cout << BuildClientSubscribeRequestMessage(ClientSubscribeRequestMessage()) << std::endl;
    ClientSubscribeResultMessage message;
    std::cout << BuildClientSubscribeResultMessage(message) << std::endl;
    message.result = true;
    message.player_id = 1;
    std::cout << BuildClientSubscribeResultMessage(message) << std::endl;

    return 0;
}