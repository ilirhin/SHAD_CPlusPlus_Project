#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include "protocol.h"

#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

std::unique_ptr<Message> ParseGamerSubscribeRequestMessage(const rapidjson::Document &document) {
    return std::unique_ptr<Message>(new GamerSubscribeRequestMessage());
}

std::unique_ptr<Message> ParseGamerSubscribeResultMessage(const rapidjson::Document &document) {
    GamerSubscribeResultMessage *message = new GamerSubscribeResultMessage();
    std::string result = document["result"].GetString();
    message->result = result == "ok";
    if (message->result) {
        message->player_id = document["id"].GetUint();
    }
    return std::unique_ptr<Message>(message);
}

std::unique_ptr<Message> ParseViewerSubscribeRequestMessage(const rapidjson::Document &document) {
    return std::unique_ptr<Message>(new ViewerSubscribeRequestMessage());
}

std::unique_ptr<Message> ParseViewerSubscribeResultMessage(const rapidjson::Document &document) {
    ViewerSubscribeResultMessage *message = new ViewerSubscribeResultMessage();
    std::string result = document["result"].GetString();
    message->result = result == "ok";
    if (message->result) {
        message->viewer_id = document["id"].GetUint();
    }
    return std::unique_ptr<Message>(message);
}

std::unique_ptr<Message> ParseWorldStateMessage(const rapidjson::Document &document) {
    WorldStateMessage *message = new WorldStateMessage();
    World world;
    world.world_id = document["state_id"].GetUint64();
    world.field_radius = document["field_radius"].GetDouble();
    world.ball_radius = document["player_radius"].GetDouble();
    world.coin_radius = document["coin_radius"].GetDouble();
    world.delta_time = document["time_delta"].GetDouble();
    world.max_velocity = document["velocity_max"].GetDouble();
    for (rapidjson::SizeType ball_index = 0;
         ball_index < document["players"].Size(); ++ball_index) {
        const rapidjson::Value &ball_json = document["players"][ball_index];
        Point position(ball_json["x"].GetDouble(), ball_json["y"].GetDouble());
        Velocity velocity(ball_json["v_x"].GetDouble(), ball_json["v_y"].GetDouble());
        Ball ball(ball_json["id"].GetUint(), position, velocity, ball_json["score"].GetDouble());
        world.balls.push_back(ball);
    }
    for (rapidjson::SizeType coin_index = 0;
         coin_index < document["coins"].Size(); ++coin_index) {
        const rapidjson::Value &coin_json = document["coins"][coin_index];
        Point position(coin_json["x"].GetDouble(), coin_json["y"].GetDouble());
        Coin coin(position, coin_json["value"].GetDouble());
        world.coins.push_back(coin);
    }
    message->world = world;
    return std::unique_ptr<Message>(message);
}

std::unique_ptr<Message> ParseTurnMessage(const rapidjson::Document &document) {
    TurnMessage *message = new TurnMessage();
    Acceleration acceleration(document["a_x"].GetDouble(), document["a_y"].GetDouble());
    message->turn = Turn(document["state_id"].GetUint64(), document["id"].GetUint(), acceleration);
    return std::unique_ptr<Message>(message);
}

std::unique_ptr<Message> ParseFinishMessage(const rapidjson::Document &document) {
    return std::unique_ptr<Message>(new FinishMessage());
}

std::unique_ptr<Message> MessageFromJson(const std::string &json) {
    rapidjson::Document document;
    document.Parse(json.c_str());
    std::string message_type = document["type"].GetString();
    if (message_type == mGamerSubscribeRequestType) {
        return ParseGamerSubscribeRequestMessage(document);
    } else if (message_type == mGamerSubscribeResultType) {
        return ParseGamerSubscribeResultMessage(document);
    } else if (message_type == mViewerSubscribeRequestType) {
        return ParseViewerSubscribeRequestMessage(document);
    } else if (message_type == mViewerSubscribeResultType) {
        return ParseViewerSubscribeResultMessage(document);
    } else if (message_type == mWorldStateType) {
        return ParseWorldStateMessage(document);
    } else if (message_type == mTurnType) {
        return ParseTurnMessage(document);
    } else if (message_type == mFinishType) {
        return ParseFinishMessage(document);
    } else {
        throw std::runtime_error("Error: can not parse from json");
    }

}

#endif