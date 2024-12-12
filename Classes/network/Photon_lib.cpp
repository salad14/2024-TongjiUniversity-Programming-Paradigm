// Photon_lib.cpp

#include "Photon_lib.h"
#include "UIListener.h"
#include "CocosUIListener.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <chrono>


//#define GETTIMEMS() static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())

// 替换为您的Photon应用ID
static const ExitGames::Common::JString appID = L"558bddbb-2dda-4767-ad34-1ea68fdec86e";
static const ExitGames::Common::JString appVersion = L"1.0";

static ExitGames::Common::JString gameName = L"Basics";

static const ExitGames::Common::JString PLAYER_NAME = L"user";
static const int MAX_SENDCOUNT = 100;

// Helper function to convert int to JString
ExitGames::Common::JString intToJString(int number) {
    std::wstringstream ss;
    ss << number;
    return ExitGames::Common::JString(ss.str().c_str());
}

// 初始化静态成员
PhotonLib* PhotonLib::instance = nullptr;

// 获取单例实例
PhotonLib* PhotonLib::getInstance()
{
    return instance;
}

// 初始化单例实例
void PhotonLib::initialize(UIListener* listener)
{
    if (instance == nullptr)
    {
        instance = new PhotonLib(listener);
        CCLOG("PhotonLib: Initialized singleton instance.");
    }
    else
    {
        CCLOG("PhotonLib: Singleton instance already initialized.");
    }
}

// 私有构造函数
PhotonLib::PhotonLib(UIListener* listener)
#ifdef _EG_MS_COMPILER
#pragma warning(push)
#pragma warning(disable:4355)
#endif
    : mState(State::INITIALIZED)
    , mpOutputListener(listener)
    , mLoadBalancingClient(*this, appID, appVersion, ExitGames::LoadBalancing::ClientConstructOptions().setAutoLobbyStats(true))
    , mSendCount(0)
    , mReceiveCount(0)
#ifdef _EG_MS_COMPILER
#pragma warning(pop)
#endif
{
    
    mLoadBalancingClient.setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO);
    mLogger.setListener(*this);
    mLogger.setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO);
    ExitGames::Common::Base::setListener(this);
    ExitGames::Common::Base::setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO);
}

// 更新Photon状态，仅调用 service()
void PhotonLib::update(void)
{
    mLoadBalancingClient.service();
}

// 获取当前状态字符串
ExitGames::Common::JString PhotonLib::getStateString(void)
{
    switch (mState)
    {
        case State::INITIALIZED:
            return L"disconnected";
        case State::CONNECTING:
            return L"connecting";
        case State::CONNECTED:
            return L"connected";
        case State::JOINING:
            return L"joining";
        case State::JOINED:
            return ExitGames::Common::JString(L"ingame\nsent event Nr. ") + intToJString(mSendCount) + L"\nreceived event Nr. " + intToJString(mReceiveCount);
        case State::SENT_DATA:
            return ExitGames::Common::JString(L"sending completed") + L"\nreceived event Nr. " + intToJString(mReceiveCount);
        case State::RECEIVED_DATA:
            return L"receiving completed";
        case State::LEAVING:
            return L"leaving";
        case State::LEFT:
            return L"left";
        case State::DISCONNECTING:
            return L"disconnecting";
        case State::DISCONNECTED:
            return L"disconnected";
        default:
            return L"unknown state";
    }
}

// 发送数据事件
void PhotonLib::sendData(void)
{
    ExitGames::Common::Hashtable event;
    event.put(static_cast<nByte>(0), ++mSendCount);
    // 发送给自己
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
    if (mSendCount >= MAX_SENDCOUNT)
        mState = State::SENT_DATA;
}

// 获取玩家数量
int PhotonLib::getPlayerCount()
{
    if (mLoadBalancingClient.getIsInRoom())
    {
        int count = mLoadBalancingClient.getCurrentlyJoinedRoom().getPlayers().getSize();
        CCLOG("PhotonLib: getPlayerCount() returns %d", count);
        return count;
    }
    CCLOG("PhotonLib: getPlayerCount() - not in room.");
    return 0;
}


// Override Listener 虚函数实现

void PhotonLib::debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& string)
{
    if (mpOutputListener)
        mpOutputListener->writeString(string);
    CCLOG("Photon Debug: %ls", string.cstr());
}

void PhotonLib::connectionErrorReturn(int errorCode)
{
    EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
    if (mpOutputListener)
        mpOutputListener->writeString(ExitGames::Common::JString(L"received connection error ") + intToJString(errorCode));
    mState = State::DISCONNECTED;

    // 触发连接失败回调
    if (connectionCallback)
    {
        // 创建 std::wstring 错误消息
        std::wstring errorMsg = L"Connection error: " + std::to_wstring(errorCode);

        // 调用回调函数
        connectionCallback(false, errorMsg);
    }

    // 重试连接（例如，最多重试3次，每次间隔2秒）
    static int retryCount = 0;
    if (retryCount < 3)
    {
        retryCount++;
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Retrying to connect... Attempt %d", retryCount);
        if (mpOutputListener)
            mpOutputListener->writeString(L"Retrying to connect... Attempt " + intToJString(retryCount));
        mLoadBalancingClient.disconnect(); // 确保之前的连接被断开

        // 延迟重试连接
        cocos2d::Director::getInstance()->getScheduler()->schedule([this](float dt) {
            connectToPhoton();
            }, this, 0.0f, 0, 2.0f, false, "connectToPhotonRetry");

    }
    else
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Max retry attempts reached. Unable to connect to Photon.");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Max retry attempts reached. Unable to connect to Photon.");
    }
}

void PhotonLib::clientErrorReturn(int errorCode)
{
    EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
    if (mpOutputListener)
        mpOutputListener->writeString(ExitGames::Common::JString(L"received error ") + intToJString(errorCode) + L" from client");
}

void PhotonLib::warningReturn(int warningCode)
{
    EGLOG(ExitGames::Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
    if (mpOutputListener)
        mpOutputListener->writeString(ExitGames::Common::JString(L"received warning ") + intToJString(warningCode) + L" from client");
}

void PhotonLib::serverErrorReturn(int errorCode)
{
    EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
    if (mpOutputListener)
        mpOutputListener->writeString(ExitGames::Common::JString(L"received error ") + intToJString(errorCode) + L" from server");
}


// 触发玩家数量变化回调
void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"player " << playerNr << L" " << player.getName();
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }

    // 触发玩家数量变化回调
    if (playerCountChangedCallback)
    {
        int currentPlayerCount = getPlayerCount();
        CCLOG("PhotonLib: triggering playerCountChangedCallback with count %d", currentPlayerCount);
        playerCountChangedCallback(currentPlayerCount);
    }
    else
    {
        CCLOG("PhotonLib: playerCountChangedCallback is not set.");
    }
}


void PhotonLib::leaveRoomEventAction(int playerNr, bool isInactive)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"player " << playerNr << L" has " << (isInactive ? L"inactive " : L"") << L"left the game";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }

    // 触发玩家数量变化回调
    if (playerCountChangedCallback)
    {
        int currentPlayerCount = getPlayerCount();
        CCLOG("PhotonLib: triggering playerCountChangedCallback with count %d", currentPlayerCount);
        playerCountChangedCallback(currentPlayerCount);
    }
}


void PhotonLib::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
{
    ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
    switch (eventCode)
    {
        case 0:
            if (eventContent.getValue((nByte)0))
                mReceiveCount = ((ExitGames::Common::ValueObject<int64>*)(eventContent.getValue((nByte)0)))->getDataCopy();
            if (mState == State::SENT_DATA && mReceiveCount >= mSendCount)
            {
                mState = State::RECEIVED_DATA;
                mSendCount = 0;
                mReceiveCount = 0;
            }
            break;
        default:
            break;
    }
}

// 连接返回回调
void PhotonLib::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"connection error: " + ExitGames::Common::JString(errorString));
        mState = State::DISCONNECTING;

        // 创建 std::wstring 错误消息
        std::wstring errorMsg = L"Connection error: " + std::to_wstring(errorCode);

        // 触发连接失败回调
        if (connectionCallback)
            connectionCallback(false, errorMsg);

        // 重试连接（例如，最多重试3次，每次间隔2秒）
        static int retryCount = 0;
        if (retryCount < 3)
        {
            retryCount++;
            EGLOG(ExitGames::Common::DebugLevel::INFO, L"Retrying to connect... Attempt %d", retryCount);
            if (mpOutputListener)
                mpOutputListener->writeString(L"Retrying to connect... Attempt " + intToJString(retryCount));
            mLoadBalancingClient.disconnect(); // 确保之前的连接被断开

            // 延迟重试连接
            cocos2d::Director::getInstance()->getScheduler()->schedule([this](float dt) {
                connectToPhoton();
                }, this, 0.0f, 0, 2.0f, false, "connectToPhotonRetry");

        }
        else
        {
            EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Max retry attempts reached. Unable to connect to Photon.");
            if (mpOutputListener)
                mpOutputListener->writeString(L"Max retry attempts reached. Unable to connect to Photon.");
        }
        return;
    }
    if (mpOutputListener)
        mpOutputListener->writeString(L"connected to cluster " + cluster);
    mState = State::CONNECTED;

    // 重置重试计数
    static int retryCount = 0;
    retryCount = 0;

    // 触发连接成功回调
    if (connectionCallback)
        connectionCallback(true, L"Connected to Photon successfully.");
}

// 断开连接返回回调
void PhotonLib::disconnectReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"disconnected");
    if (mpOutputListener)
        mpOutputListener->writeString(L"disconnected");
    mState = State::DISCONNECTED;
}

// 创建房间返回回调
void PhotonLib::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"opCreateRoom() failed: " + ExitGames::Common::JString(errorString));
        mState = State::CONNECTED;
        return;
    }

    EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"... room " << mLoadBalancingClient.getCurrentlyJoinedRoom().getName() << L" has been created";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);

        ss.str(L""); // 清空字符串流
        ss << L"regularly sending dummy events now";
        message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
    mState = State::JOINED;

    // 调用回调
    if (roomJoinedCallback) {
        roomJoinedCallback();
    }
}

// 实现 leaveRoom 方法
void PhotonLib::leaveRoom()
{
    if (mLoadBalancingClient.getIsInRoom())
    {
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Leaving the current room...");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Leaving the current room...");

        // 调用 Photon 的离开房间操作
        mLoadBalancingClient.opLeaveRoom();

        // 更新状态
        mState = State::LEAVING;
    }
    else
    {
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Not currently in a room.");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Not currently in a room.");
    }
}

// 设置房间离开后的回调
void PhotonLib::setLeaveRoomCallback(const std::function<void()>& callback)
{
    leaveRoomCallback = callback;
    CCLOG("PhotonLib: leaveRoomCallback has been set.");
}

void PhotonLib::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"opJoinOrCreateRoom() failed: " + ExitGames::Common::JString(errorString));
        mState = State::CONNECTED;
        return;
    }

    EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"... room " << mLoadBalancingClient.getCurrentlyJoinedRoom().getName() << L" has been entered";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);

        ss.str(L""); // 清空字符串流
        ss << L"regularly sending dummy events now";
        message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
    mState = State::JOINED;

    // 触发房间加入回调
    if (roomJoinedCallback) {
        CCLOG("PhotonLib: triggering roomJoinedCallback.");
        roomJoinedCallback();
    }

    // 触发玩家数量变化回调，初始玩家数
    if (playerCountChangedCallback) {
        int currentPlayerCount = getPlayerCount();
        CCLOG("PhotonLib: triggering playerCountChangedCallback with count %d", currentPlayerCount);
        playerCountChangedCallback(currentPlayerCount);
    }
}

// 加入随机房间返回回调
void PhotonLib::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"opJoinRandomRoom() failed: " + ExitGames::Common::JString(errorString));
        mState = State::CONNECTED;
        return;
    }

    EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"... room " << mLoadBalancingClient.getCurrentlyJoinedRoom().getName() << L" has been successfully joined";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);

        ss.str(L""); // 清空字符串流
        ss << L"regularly sending dummy events now";
        message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
    mState = State::JOINED;

    // 调用回调
    if (roomJoinedCallback) {
        roomJoinedCallback();
    }
}

// 离开房间返回回调
void PhotonLib::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Failed to leave room: %ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"Failed to leave room: " + ExitGames::Common::JString(errorString));
        // 离开房间失败，仍然处于CONNECTED状态（已连接但不在房间）
        mState = State::CONNECTED;
        return;
    }

    // 离开房间成功后，状态从LEAVING转为CONNECTED（已连接但无房间）
    mState = State::CONNECTED;

    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"Successfully left the room.";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }

    // 触发离开房间的回调（如有）
    if (leaveRoomCallback)
    {
        leaveRoomCallback();
    }
}


// 加入大厅返回回调
void PhotonLib::joinLobbyReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"joined lobby");
    if (mpOutputListener)
        mpOutputListener->writeString(L"joined lobby");
}

// 离开大厅返回回调
void PhotonLib::leaveLobbyReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"left lobby");
    if (mpOutputListener)
        mpOutputListener->writeString(L"left lobby");
}

// 加入或创建房间
void PhotonLib::joinOrCreateRoom(const ExitGames::Common::JString& roomName)
{
    mLoadBalancingClient.opJoinOrCreateRoom(roomName);
}

// 设置房间加入后的回调
void PhotonLib::setRoomJoinedCallback(const std::function<void()>& callback)
{
    roomJoinedCallback = callback;
    CCLOG("PhotonLib: roomJoinedCallback has been set.");
}

// 设置玩家数量变化的回调
void PhotonLib::setPlayerCountChangedCallback(const std::function<void(int)>& callback)
{
    playerCountChangedCallback = callback;
    CCLOG("PhotonLib: playerCountChangedCallback has been set.");
}

// 设置连接回调
void PhotonLib::setConnectionCallback(const std::function<void(bool, const std::wstring&)>& callback)
{
    connectionCallback = callback;
}

// 连接到Photon服务器
void PhotonLib::connectToPhoton()
{
    if (mState == State::INITIALIZED || mState == State::DISCONNECTED)
    {
        mLoadBalancingClient.connect(
            ExitGames::LoadBalancing::ConnectOptions()
            .setServerAddress(ExitGames::Common::JString(L"ns.photonengine.cn")) // 设置中国区服务器地址
            .setAuthenticationValues(
                ExitGames::LoadBalancing::AuthenticationValues()
                .setUserID(ExitGames::Common::JString() + intToJString(GETTIMEMS())) // 设置唯一的UserID
            )
            .setUsername(PLAYER_NAME + intToJString(GETTIMEMS())) // 设置唯一的用户名
            .setTryUseDatagramEncryption(true)
        );
        mState = State::CONNECTING;
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Attempting to connect to Photon...");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Attempting to connect to Photon...");
    }
}
