#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <assert.h>
#include <stdexcept>

#include "action_manager.h"
#include "message_builder.h"
#include "message_parser.h"


enum ClientType { GAMER, VIEWER, UNDEFINED_TYPE };

class Client
{
protected:
    ActionManager actionManager_;
    ClientType type_;
    size_t id_;
    int sock_;

public:
    Client(ClientType type, const ActionManager& actionManager) :
            type_(type), actionManager_(actionManager)
    {
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("Error: failed to create socket");
        }
    }

    ~Client()
    {
        if (sock_ >= 0) {
            close(sock_);
        }
    }

    virtual void run(size_t port);

protected:
    virtual bool connectToServer(size_t port);

    bool isFinishConnectionMessage(const std::string& finish_message_str) {
        std::unique_ptr<Message> message = MessageFromJson(finish_message_str);
        if (message->type != mFinishType) {
            return false;
        }
        std::cout << "Finish connection" << std::endl;
        return true;
    }

    bool isWorldStateMessage(const std::string& world_state_message_str, World& world) {
        std::unique_ptr<Message> message = MessageFromJson(world_state_message_str);
        if (message->type != mWorldStateType) {
            return false;
        }
        std::unique_ptr<WorldStateMessage> world_state_message
                (dynamic_cast<WorldStateMessage*>(message.release()));
        world = world_state_message->world;
        return true;
    }

    int sendString(const std::string& str) {
        int total_send = 0;
        while (true) {
            int send = send(sock_, str.c_str() + total_send, str.size() - total_send, 0);
            if (send <= 0) {
                break;
            }
            total_send += send;
        }
        return total_send;
    }

    int recvString(std::string& str) {
        int total_recv = 0;
        char buf[1024];
        while (true) {
            int reads = recv(sock_, buf, 1024, 0);
            if (reads <= 0) {
                break;
            }
            total_recv += reads;
            str += std::string(buf, reads);
        }
        return total_recv;
    }
};

class Gamer : public Client {
public:
    Gamer(ClientType type, const ActionManager& actionManager) :
            Client(type, actionManager) {}

    void run(size_t port) {
        if (!connectToServer(port)) {
            return;
        }
        std::string message_str;
        while (recvString(message_str) >= 0) {
            World world_state;
            if (isFinishConnectionMessage(message_str)) {
                return;
            } else if (isWorldStateMessage(message_str, world_state)) {
                std::string turn_answer;
                performTurn(world_state, turn_answer);
                int send = sendString(turn_answer);
                if (send < 0) {
                    std::cout << "Error: can not send turn message to server" << std::endl;
                    continue;
                }
            } else {
                std::cout << "Error: incorrect message received" << std::endl;
            }
        }
    }

private:
    virtual bool connectToServer(size_t port) {
        // Connecting
        int connect;
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        do {
            connect = connect(sock_, (struct sockaddr *)&addr, sizeof(addr));
            std::cout << "Connection..." << std::endl;
        } while (connect < 0);

        // Sending request
        GamerSubscribeRequestMessage request_message;
        std::string message_to = MessageToJson(&request_message);
        int send = sendString(message_to);
        if (send != message_to.size()) {
            std::cout << "Error: can not send request message to server" << std::endl;
            return false;
        }

        // Obtaining answer
        std::string message_from;
        int recv = recvString(message_from);
        if (recv <= 0) {
            std::cout << "Error: can not recv request message from server" << std::endl;
            return false;
        }
        std::unique_ptr<Message> message = MessageFromJson(message_from);
        if (message->type != mGamerSubscribeResultType) {
            std::cout << "Error : bad response type" << std::endl;
            return false;
        }
        std::unique_ptr<GamerSubscribeResultMessage> subscribe_result_message
                (dynamic_cast<GamerSubscribeResultMessage*>(message.release()));
        if (!subscribe_result_message->result) {
            std::cout << "Error: server refused to accept gamer" << std::endl;
            return false;
        }
        id_ = subscribe_result_message->player_id;
        std::cout << "Gamer connected to server with id = " << id_ << std::endl;
        return true;
    }

    void performTurn(const World& world, std::string& turn_answer) const {
        TurnMessage turn_message;
        turn_message.turn.ball_id_ = id_;
        turn_message.turn.world_id_ = world.world_id;
        for (const Ball& ball : world.balls) {
            if (ball.id_ == id_) {
                turn_message.turn.acceleration_ = actionManager_.performGamerAction(world, ball);
                break;
            }
        }
        turn_answer = MessageToJson(&turn_message);
    }
};

class Viewer : public Client {
public:
    Viewer(ClientType type, const ActionManager& actionManager) :
            Client(type, actionManager) {}

    void run(size_t port) {
        if (!connectToServer(port)) {
            return;
        }
        std::string message_str;
        while (recvString(message_str) >= 0) {
            World world_state;
            if (isFinishConnectionMessage(message_str)) {
                return;
            } else if (isWorldStateMessage(message_str, world_state)) {
                performView(world_state);
            } else {
                std::cout << "Error: incorrect message received" << std::endl;
            }
        }
    }

private:
    virtual bool connectToServer(size_t port) {
        // Connecting
        int connect;
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        do {
            connect = connect(sock_, (struct sockaddr *)&addr, sizeof(addr));
            std::cout << "Connection..." << std::endl;
        } while (connect < 0);

        // Sending request
        ViewerSubscribeRequestMessage request_message;
        std::string message_to = MessageToJson(&request_message);
        int send = sendString(message_to);
        if (send != message_to.size()) {
            std::cout << "Error: can not send request message to server" << std::endl;
            return false;
        }

        // Obtaining answer
        std::string message_from;
        int recv = recvString(message_from);
        if (recv <= 0) {
            std::cout << "Error: can not recv request message from server" << std::endl;
            return false;
        }
        std::unique_ptr<Message> message = MessageFromJson(message_from);
        if (message->type != mViewerSubscribeResultType) {
            std::cout << "Error : bad response type" << std::endl;
            return false;
        }
        std::unique_ptr<ViewerSubscribeResultMessage> subscribe_result_message
                (dynamic_cast<ViewerSubscribeResultMessage*>(message.release()));
        if (!subscribe_result_message->result) {
            std::cout << "Error: server refused to accept viewer" << std::endl;
            return false;
        }
        id_ = subscribe_result_message->viewer_id;
        std::cout << "Viewer connected to server with id = " << id_ << std::endl;
        return true;
    }

    void performView(const World& world) {
        actionManager_.performViewerAction(world);
    }
};