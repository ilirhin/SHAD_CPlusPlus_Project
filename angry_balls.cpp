#include <iostream>
#include <string>

#include "protocol.h"

int main() {
    std::cout << BuildClientSubscribeRequestMessage(ClientSubscribeRequestMessage()) << std::endl;
    ClientSubscribeResultMessage message1;
    message1.result = false;
    std::cout << BuildClientSubscribeResultMessage(message1) << std::endl;
    message1.result = true;
    message1.player_id = 1;
    std::cout << BuildClientSubscribeResultMessage(message1) << std::endl;

    std::cout << BuildViewerSubscribeRequestMessage(ViewerSubscribeRequestMessage()) << std::endl;
    ViewerSubscribeResultMessage message2;
    message2.result = false;
    std::cout << BuildViewerSubscribeResultMessage(message2) << std::endl;
    message2.result = true;
    message2.viewer_id = 1;
    std::cout << BuildViewerSubscribeResultMessage(message2) << std::endl;

    World world;
    world.world_id = 1;
    world.field_radius = 1.2;
    world.ball_radius = 0.2;
    world.coin_radius = 0.3;
    world.delta_time = 0.4;
    world.max_velocity = 0.5;
    world.balls.push_back(Ball(1, Point(0.1, 0.1), Velocity(0.2, 0.2), 0.0));
    world.balls.push_back(Ball(2, Point(0.2, 0.2), Velocity(0.1, 0.1), 0.0));
    world.coins.push_back(Coin(Point(0.4, 0.1), 0.1));
    world.coins.push_back(Coin(Point(0.1, 0.4), 0.2));
    WorldStateMessage message3;
    message3.world = world;
    std::cout << BuildWorldStateMessage(message3) << std::endl;

    Turn turn;
    turn.world_id_ = 1;
    turn.ball_id_ = 2;
    turn.acceleration_ = Acceleration(0.1, 0.1);

    TurnMessage message4;
    message4.turn = turn;
    std::cout << BuildTurnMessage(message4) << std::endl;

    std::cout << BuildFinishMessage(FinishMessage()) << std::endl;

    return 0;
}