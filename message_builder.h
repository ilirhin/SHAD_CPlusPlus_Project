#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "protocol.h"

#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

// Made for testing
std::string BuildClientSubscribeRequestMessage(const ClientSubscribeRequestMessage& message)
{

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
    writer.EndObject();
    return buffer.GetString();
}


std::string BuildClientSubscribeResultMessage(const ClientSubscribeResultMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
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

// Made for testing
std::string BuildViewerSubscribeRequestMessage(const ViewerSubscribeRequestMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
    writer.EndObject();
    return buffer.GetString();
}

std::string BuildViewerSubscribeResultMessage(const ViewerSubscribeResultMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
    if (message.result) {
        writer.String("result");
        writer.String("ok");
        writer.String("id");
        writer.Uint(message.viewer_id);
    } else {
        writer.String("result");
        writer.String("fail");
    }
    writer.EndObject();
    return buffer.GetString();
}

// Made for testing
std::string BuildWorldStateMessage(const WorldStateMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
    message.world.Serialize(writer);
    writer.EndObject();
    return buffer.GetString();
}

std::string BuildTurnMessage(const TurnMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
    message.turn.Serialize(writer);
    writer.EndObject();
    return buffer.GetString();
}

std::string BuildFinishMessage(const FinishMessage& message)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.String("type");
    writer.String(message.type.c_str(), message.type.size());
    writer.EndObject();
    return buffer.GetString();
}

std::string MessageToJson(const Message * const message)
{
    if (message->type == mClientSubscribeRequestType) {
        const ClientSubscribeRequestMessage * const client_subscribe_request_message
                = dynamic_cast<const ClientSubscribeRequestMessage* const>(message);
        return BuildClientSubscribeRequestMessage(*client_subscribe_request_message);
    } else if (message->type == mClientSubscribeResultType) {
        const ClientSubscribeResultMessage * const client_subscribe_result_message
                = dynamic_cast<const ClientSubscribeResultMessage* const>(message);
        return BuildClientSubscribeResultMessage(*client_subscribe_result_message);
    } else if (message->type == mViewerSubscribeRequestType) {
        const ViewerSubscribeRequestMessage * const viewer_subscribe_request_message
                = dynamic_cast<const ViewerSubscribeRequestMessage* const>(message);
        return BuildViewerSubscribeRequestMessage(*viewer_subscribe_request_message);
    } else if (message->type == mViewerSubscribeResultType) {
        const ViewerSubscribeResultMessage * const viewer_subscribe_result_message
                = dynamic_cast<const ViewerSubscribeResultMessage* const>(message);
        return BuildViewerSubscribeResultMessage(*viewer_subscribe_result_message);
    } else if (message->type == mWorldStateType) {
        const WorldStateMessage * const world_state_message
                = dynamic_cast<const WorldStateMessage* const>(message);
        return BuildWorldStateMessage(*world_state_message);
    } else if (message->type == mTurnType) {
        const TurnMessage * const turn_message
                = dynamic_cast<const TurnMessage* const>(message);
        return BuildTurnMessage(*turn_message);
    } else if (message->type == mFinishType) {
        const FinishMessage * const finish_message
                = dynamic_cast<const FinishMessage* const>(message);
        return BuildFinishMessage(*finish_message);
    } else {
        throw std::runtime_error("Error: can not convert message to json");
    }
}

#endif

