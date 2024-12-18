// Photon_lib.h
#pragma once

#include "LoadBalancing-cpp/inc/Client.h"
#include "Photon-cpp/inc/Enums/ReceiverGroup.h"
#include "UIListener.h"
#include <functional>
#include "network/CustomEventCodes.h"

class PhotonLib : private ExitGames::LoadBalancing::Listener
{
public:
    // 获取单例实例
    static PhotonLib* getInstance();

    // 初始化单例实例
    static void initialize(UIListener* listener);

    // 更新Photon状态
    void update(void);

    // 获取当前状态的字符串表示
    ExitGames::Common::JString getStateString(void);

    // 设置房间加入后的回调
    void setRoomJoinedCallback(const std::function<void()>& callback);

    // 设置玩家数量变化的回调
    void setPlayerCountChangedCallback(const std::function<void(int)>& callback);

    // 设置连接到Photon的回调
    void setConnectionCallback(const std::function<void(bool, const std::wstring&)>& callback);

    // 加入或创建房间
    void joinOrCreateRoom(const ExitGames::Common::JString& roomName);

    // 获取当前房间的玩家数量
    int getPlayerCount();

    // 显式连接到Photon服务器
    void connectToPhoton();

    // 显式断开Photon连接
    void disconnectFromPhoton();

    void leaveRoom();

    // 设置房间离开后的回调
    void setLeaveRoomCallback(const std::function<void()>& callback);

    // 设置自定义事件回调
    void setCustomEventCallback(const std::function<void(int, const ExitGames::Common::Hashtable&)>& callback);

    // 发送自定义事件
    void raiseCustomEvent(const ExitGames::Common::Hashtable& eventContent, int eventCode, nByte receiverGroup);

    // 获取本地玩家编号
    int getLocalPlayerNumber();

private:
    // 私有构造函数
    PhotonLib(UIListener* listener);

    // 禁止拷贝
    PhotonLib(const PhotonLib&) = delete;
    PhotonLib& operator=(const PhotonLib&) = delete;

    // 单例实例指针
    static PhotonLib* instance;

    // Override Listener 虚函数
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

    // 玩家数量变化的回调
    std::function<void(int)> playerCountChangedCallback;

    // 连接回调
    std::function<void(bool, const std::wstring&)> connectionCallback;

    // 离开房间后的回调
    std::function<void()> leaveRoomCallback;

    // 自定义事件回调
    std::function<void(int, const ExitGames::Common::Hashtable&)> customEventCallback;
};