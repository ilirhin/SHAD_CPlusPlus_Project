#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <iostream>
#include <string>
#include <memory>

#include "game_objects.h"


static const std::string mClientSubscribeRequestType = "CLI_SUB_REQUEST";
static const std::string mClientSubscribeResultType = "CLI_SUB_RESULT";
static const std::string mViewerSubscribeRequestType = "VIEW_SUB_REQUEST";
static const std::string mViewerSubscribeResultType = "VIEW_SUB_RESULT";
static const std::string mWorldStateType = "STATE";
static const std::string mTurnType = "TURN";
static const std::string mFinishType = "FINISH";


class Message
{
public:
    std::string type;
    virtual ~Message() {}
};

class ClientSubscribeRequestMessage : public Message
{
public:
    ClientSubscribeRequestMessage() {
        type = mClientSubscribeRequestType;
    }

    virtual ~ClientSubscribeRequestMessage() {}
};

class ClientSubscribeResultMessage : public Message
{
public:
    bool result;
    size_t player_id;

    ClientSubscribeResultMessage() {
        type = mClientSubscribeResultType;
    }

    virtual ~ClientSubscribeResultMessage() {}
};

class ViewerSubscribeRequestMessage : public Message
{
public:
    ViewerSubscribeRequestMessage() {
        type = mViewerSubscribeRequestType;
    }

    virtual ~ViewerSubscribeRequestMessage() {}
};

class ViewerSubscribeResultMessage : public Message
{
public:
    bool result;
    size_t viewer_id;

    ViewerSubscribeResultMessage() {
        type = mViewerSubscribeResultType;
    }

    virtual ~ViewerSubscribeResultMessage() {}
};

class WorldStateMessage : public Message
{
public:
    World world;

    WorldStateMessage() {
        type = mWorldStateType;
    }

    virtual ~WorldStateMessage() {}
};

class TurnMessage : public Message
{
public:
    Turn turn;

    TurnMessage() {
        type = mTurnType;
    }

    virtual ~TurnMessage() {}
};

class FinishMessage : public Message
{
public:
    FinishMessage() {
        type = mFinishType;
    }

    virtual ~FinishMessage() {}
};

#endif