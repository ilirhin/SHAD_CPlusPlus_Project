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
// #include "viewer.h"

#pragma once

class Client {
protected:
    ActionManager actionManager_;
    size_t id_;
    int sock_;

public:
    explicit Client(const ActionManager &actionManager) :
            actionManager_(actionManager) {
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("Error: failed to create socket");
        }
    }

    virtual ~Client() {
        if (sock_ >= 0) {
            close(sock_);
        }
    }

    virtual void run(size_t port) = 0;

protected:

    template<class SubscribeMessageType>
    bool subscribeForServer(size_t port, const SubscribeMessageType &request_message) {
        int connected;
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        do {
            connected = connect(sock_, (struct sockaddr *) &addr, sizeof(addr));
            std::cout << "Connection..." << std::endl;
        } while (connected < 0);

        std::string message_to = MessageToJson(&request_message);
        int send = sendString(message_to);
        if (send != message_to.size() + sizeof(uint32_t)) {
            std::cout << "Error: can not send request message to server" << std::endl;
            return false;
        }

        // Obtaining answer
        std::string message_from;
        int recv = recvString(message_from);
        std::cout << message_from << std::endl;
        if (recv < 0) {
            std::cout << "Error: can not recv request message from server" << std::endl;
            return false;
        }

        std::unique_ptr<Message> message = MessageFromJson(message_from);

        typedef typename SubscribeMessageType::ResultMessage AnswerMessage;

        std::unique_ptr<AnswerMessage> subscribe_result_message
                (dynamic_cast<AnswerMessage *>(message.release()));

        if (!subscribe_result_message) {
            std::cout << "Error : bad response type" << std::endl;
            return false;
        }

        if (!subscribe_result_message->result) {
            std::cout << "Error: server refused to accept gamer" << std::endl;
            return false;
        }
        id_ = subscribe_result_message->id();
        std::cout << "Gamer connected to server with id = " << id_ << std::endl;
        return true;
    }

    virtual bool connectToServer(size_t port) = 0;

    bool isFinishConnectionMessage(const std::string &finish_message_str) {
        std::unique_ptr<Message> message = MessageFromJson(finish_message_str);
        if (message->type != mFinishType) {
            return false;
        }
        std::cout << "Finish connection" << std::endl;
        return true;
    }

    bool isWorldStateMessage(const std::string &world_state_message_str, World &world) {
        std::unique_ptr<Message> message = MessageFromJson(world_state_message_str);
        if (message->type != mWorldStateType) {
            return false;
        }
        std::unique_ptr<WorldStateMessage> world_state_message
                (dynamic_cast<WorldStateMessage *>(message.release()));
        world = world_state_message->world;
        return true;
    }

    int sendString(const std::string &str) {
        std::cout << "Client send " << str << std::endl;
        u_int32_t message_length = str.size();
        char *message_length_str = (char *)(&message_length);
        std::string full_str = std::string(message_length_str,
                                           message_length_str + sizeof(message_length)) + str;
        int total_sent = 0;
        while (true) {
            int sent = send(sock_, full_str.c_str() + total_sent, full_str.size() - total_sent, 0);
            if (sent <= 0) {
                break;
            }
            total_sent += sent;
        }
        return total_sent;
    }

    int recvString(std::string &str) {
        char buf_length[sizeof(u_int32_t)];
        int message_length_length = recv(sock_, buf_length, sizeof(u_int32_t), 0);
        if (message_length_length < 0) {
            return -1;
        }
        size_t message_length = *(u_int32_t *)buf_length;
        char buf[10240];
        int total_reads = 0;
        while (total_reads < message_length) {
            int reads = recv(sock_, buf, 10240, 0);
            if (reads < 0) {
                return -1;
            }
            str = std::string(buf, reads);
            total_reads += reads;
        }
        std::cout << "Client read " << str << std::endl;
        return total_reads;
    }
};

class Gamer : public Client {
public:
    explicit Gamer(const ActionManager &actionManager) :
            Client(actionManager) { }

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
            }
        }
    }

private:
    virtual bool connectToServer(size_t port) {
        return subscribeForServer(port, GamerSubscribeRequestMessage());
    }

    void performTurn(const World &world, std::string &turn_answer) {
        TurnMessage turn_message;
        turn_message.turn.ball_id_ = id_;
        turn_message.turn.world_id_ = world.world_id;
        for (const Ball &ball : world.balls) {
            if (ball.id_ == id_) {
                turn_message.turn.acceleration_ = actionManager_.performGamerAction(world, ball);
                std::cerr << turn_message.turn.acceleration_.a_x_ << " " << turn_message.turn.acceleration_.a_y_ << std::endl;
                //turn_message.turn.acceleration_ = Acceleration(0.0, 0.1);
                break;
            }
        }
        turn_answer = MessageToJson(&turn_message);
    }
};
/*
class Viewer : public Client {
private:
    Notifier* notifier_;
public:
    explicit Viewer(const ActionManager &actionManager, Notifier* notifier) :
            Client(actionManager), notifier_(notifier) { }

    void run(size_t port) {
        if (!connectToServer(port)) {
            return;
        }
        bool show_first_time = true;
        std::string message_str;
        while (recvString(message_str) >= 0) {
            World world_state;
            if (isFinishConnectionMessage(message_str)) {
                notifier_->finishShowing();
                return;
            } else if (isWorldStateMessage(message_str, world_state)) {
                performView(world_state, show_first_time);
            }
        }
    }

private:
    virtual bool connectToServer(size_t port) {
        return subscribeForServer(port, ViewerSubscribeRequestMessage());
    }

    void performView(const World &world, bool& show_first_time) {
        actionManager_.performViewerAction(world, notifier_, show_first_time);
    }
};
*/
