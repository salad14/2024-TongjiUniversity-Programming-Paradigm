// Photon_lib.h
#pragma once

#include "LoadBalancing-cpp/inc/Client.h"
#include "UIListener.h"
#include <functional>

class PhotonLib : private ExitGames::LoadBalancing::Listener
{
public:
    PhotonLib(UIListener*);
    void update(void);
    ExitGames::Common::JString getStateString(void);

    // 设置房间加入后的回调
    void setRoomJoinedCallback(const std::function<void()>& callback);

    void joinOrCreateRoom(const ExitGames::Common::JString& roomName);


private:
    // Override 虚函数
    virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override;
    virtual void connectionErrorReturn(int errorCode) override;
    virtual void clientErrorReturn(int errorCode) override;
    virtual void warningReturn(int warningCode) override;
    virtual void serverErrorReturn(int errorCode) override;

    // 事件回调
    virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override;
    virtual void leaveRoomEventAction(int playerNr, bool isInactive) override;
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

    // 房间操作回调
    virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;
    virtual void disconnectReturn(void) override;
    virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void joinLobbyReturn(void) override;
    virtual void leaveLobbyReturn(void) override;

    void sendData(void);

    // 状态类
    class State
    {
    public:
        enum States
        {
            INITIALIZED = 0,
            CONNECTING,
            CONNECTED,
            JOINING,
            JOINED,
            SENT_DATA,
            RECEIVED_DATA,
            LEAVING,
            LEFT,
            DISCONNECTING,
            DISCONNECTED
        };
    };
    State::States mState;

    UIListener* mpOutputListener;
    ExitGames::LoadBalancing::Client mLoadBalancingClient;
    ExitGames::Common::Logger mLogger;

    int64 mSendCount;
    int64 mReceiveCount;

    // 房间加入后的回调
    std::function<void()> roomJoinedCallback;
};
