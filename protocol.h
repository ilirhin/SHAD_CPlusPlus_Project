#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <iostream>
#include <string>
#include <memory>

#include "game_objects.h"


static const std::string mGamerSubscribeRequestType = "CLI_SUB_REQUEST";
static const std::string mGamerSubscribeResultType = "CLI_SUB_RESULT";
static const std::string mViewerSubscribeRequestType = "VIEW_SUB_REQUEST";
static const std::string mViewerSubscribeResultType = "VIEW_SUB_RESULT";
static const std::string mWorldStateType = "STATE";
static const std::string mTurnType = "TURN";
static const std::string mFinishType = "FINISH";


class Message {
public:
    std::string type;

    virtual ~Message() { }
};

class GamerSubscribeResultMessage : public Message {
public:
    bool result;
    size_t player_id;

    GamerSubscribeResultMessage() {
        type = mGamerSubscribeResultType;
    }

    virtual ~GamerSubscribeResultMessage() { }
};

class GamerSubscribeRequestMessage : public Message {
public:
    typedef GamerSubscribeResultMessage ResultMessage;

    GamerSubscribeRequestMessage() {
        type = mGamerSubscribeRequestType;
    }

    virtual ~GamerSubscribeRequestMessage() { }
};

class ViewerSubscribeResultMessage : public Message {
public:
    bool result;
    size_t viewer_id;

    ViewerSubscribeResultMessage() {
        type = mViewerSubscribeResultType;
    }

    virtual ~ViewerSubscribeResultMessage() { }
};

class ViewerSubscribeRequestMessage : public Message {
public:
    typedef ViewerSubscribeResultMessage ResultMessage;

    ViewerSubscribeRequestMessage() {
        type = mViewerSubscribeRequestType;
    }

    virtual ~ViewerSubscribeRequestMessage() { }
};

class WorldStateMessage : public Message {
public:
    World world;

    WorldStateMessage() {
        type = mWorldStateType;
    }

    virtual ~WorldStateMessage() { }
};

class TurnMessage : public Message {
public:
    Turn turn;

    TurnMessage() {
        type = mTurnType;
    }

    virtual ~TurnMessage() { }
};

class FinishMessage : public Message {
public:
    FinishMessage() {
        type = mFinishType;
    }

    virtual ~FinishMessage() { }
};

#endif