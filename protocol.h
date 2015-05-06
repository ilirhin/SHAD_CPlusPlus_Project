#include <iostream>
#include <string>
#include <memory>

#include "game_objects.h"

#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

enum MessageType
{
    kClientSubscribeRequestMessage,
    kClientSubscribeResultMessage,
    kViewerSubscribeRequestMessage,
    kViewerSubscribeResultMessage,
    kWorldStateMessage,
    kTurnMessage,
    kFinishMessage
};

class Message
{
public:
    MessageType type;
    virtual std::string typeToString() const {}
    virtual ~Message() {}
};

class ClientSubscribeRequestMessage : public Message
{
public:
    ClientSubscribeRequestMessage() {
        type = kClientSubscribeRequestMessage;
    }

    virtual std::string typeToString() const {
        return "CLI_SUB_REQUEST";
    }

    virtual ~ClientSubscribeRequestMessage() {}
};

class ClientSubscribeResultMessage : public Message
{
public:
    bool result;
    size_t player_id;

    ClientSubscribeResultMessage() {
        type = kClientSubscribeResultMessage;
    }

    virtual std::string typeToString() const {
        return "CLI_SUB_RESULT";
    }

    virtual ~ClientSubscribeResultMessage() {}
};

class ViewerSubscribeRequestMessage : public Message
{
public:
    ViewerSubscribeRequestMessage() {
        type = kViewerSubscribeRequestMessage;
    }

    virtual std::string typeToString() const {
        return "VIEW_SUB_REQUEST";
    }

    virtual ~ViewerSubscribeRequestMessage() {}
};

class ViewerSubscribeResultMessage : public Message
{
public:
    bool result;
    size_t viewer_id;

    ViewerSubscribeResultMessage() {
        type = kViewerSubscribeResultMessage;
    }

    virtual std::string typeToString() const {
        return "VIEW_SUB_RESULT";
    }

    virtual ~ViewerSubscribeResultMessage() {}
};

class WorldStateMessage : public Message
{
public:
    World world;

    WorldStateMessage() {
        type = kWorldStateMessage;
    }

    virtual std::string typeToString() const {
        return "STATE";
    }

    virtual ~WorldStateMessage() {}
};

class TurnMessage : public Message
{
public:
    Turn turn;

    TurnMessage() {
        type = kTurnMessage;
    }

    virtual std::string typeToString() const {
        return "TURN";
    }

    virtual ~TurnMessage() {}
};

class FinishMessage : public Message
{
public:
    FinishMessage() {
        type = kFinishMessage;
    }

    virtual std::string typeToString() const {
        return "FINISH";
    }

    virtual ~FinishMessage() {}
};

std::string BuildJsonMessage(const Message* const message);
std::unique_ptr<Message> ParseJsonMessage(const std::string& json);


std::string BuildClientSubscribeRequestMessage(const ClientSubscribeRequestMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    std::string type_to_string = message.typeToString();
    writer.String(type_to_string.c_str(), type_to_string.size());
    writer.EndObject();
    return buffer.GetString();
}


std::string BuildClientSubscribeResultMessage(const ClientSubscribeResultMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    std::string type_to_string = message.typeToString();
    writer.String(type_to_string.c_str(), type_to_string.size());
    if (message.result) {
        writer.String("result");
        writer.String("ok");
        writer.String("id");
        writer.Uint(message.player_id);
    } else {
        writer.String("result");
        writer.String("fail");
    }
    writer.EndObject();
    return buffer.GetString();
}