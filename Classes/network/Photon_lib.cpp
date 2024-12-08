// Photon_lib.cpp

#include "Photon_lib.h"
#include "UIListener.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>

// 替换为您的Photon应用ID
static const ExitGames::Common::JString appID = L"558bddbb-2dda-4767-ad34-1ea68fdec86e"; // 设置您的应用ID
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
    }
    // 如果已经初始化，可以选择更新listener或保持不变
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
    mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
    mLogger.setListener(*this);
    mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
    ExitGames::Common::Base::setListener(this);
    ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
}

// 更新Photon状态，仅调用service()
void PhotonLib::update(void)
{
    mLoadBalancingClient.service();
}

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

int PhotonLib::getPlayerCount()
{
    // 使用 getIsInRoom() 来检查当前是否在房间中
    if (mLoadBalancingClient.getIsInRoom())
    {
        // 使用 getCountPlayersIngame() 获取当前房间中的玩家数量
        return mLoadBalancingClient.getCountPlayersIngame();
    }
    return 0;
}

// Override Listener 虚函数实现

void PhotonLib::debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& string)
{
    if (mpOutputListener)
        mpOutputListener->writeString(string);
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
        std::wstring errorMsg = L"Connection error: ";
        errorMsg += intToJString(errorCode).cstr();
        // 调用回调函数
        connectionCallback(false, errorMsg);
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
        playerCountChangedCallback(getPlayerCount());
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
        playerCountChangedCallback(getPlayerCount());
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

void PhotonLib::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"connection error: " + ExitGames::Common::JString(errorString));
        mState = State::DISCONNECTING;

        // 转换 ExitGames::Common::JString 到 std::wstring
        std::wstring errorMsg = L"Connection error: " + std::wstring(errorString.cstr());

        // 触发连接失败回调
        if (connectionCallback)
            connectionCallback(false, errorMsg);
        return;
    }
    if (mpOutputListener)
        mpOutputListener->writeString(L"connected to cluster " + cluster);
    mState = State::CONNECTED;

    // 触发连接成功回调
    if (connectionCallback)
        connectionCallback(true, L"Connected to Photon successfully.");
}


void PhotonLib::disconnectReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"disconnected");
    if (mpOutputListener)
        mpOutputListener->writeString(L"disconnected");
    mState = State::DISCONNECTED;
}

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

    // 调用回调
    if (roomJoinedCallback) {
        roomJoinedCallback();
    }
}

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

void PhotonLib::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"opLeaveRoom() failed: " + ExitGames::Common::JString(errorString));
        mState = State::DISCONNECTING;
        return;
    }
    mState = State::LEFT;
    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"room has been successfully left";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
}

void PhotonLib::joinLobbyReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (mpOutputListener)
        mpOutputListener->writeString(L"joined lobby");
}

void PhotonLib::leaveLobbyReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
    if (mpOutputListener)
        mpOutputListener->writeString(L"left lobby");
}

void PhotonLib::joinOrCreateRoom(const ExitGames::Common::JString& roomName)
{
    mLoadBalancingClient.opJoinOrCreateRoom(roomName);
}

// 设置房间加入后的回调
void PhotonLib::setRoomJoinedCallback(const std::function<void()>& callback) {
    roomJoinedCallback = callback;
}

// 设置玩家数量变化的回调
void PhotonLib::setPlayerCountChangedCallback(const std::function<void(int)>& callback)
{
    playerCountChangedCallback = callback;
}

// 设置连接回调
void PhotonLib::setConnectionCallback(const std::function<void(bool, const std::wstring&)>& callback)
{
    connectionCallback = callback;
}

void PhotonLib::connectToPhoton()
{
    if (mState == State::INITIALIZED || mState == State::DISCONNECTED)
    {
        mLoadBalancingClient.connect(ExitGames::LoadBalancing::ConnectOptions()
            .setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS()))
            .setUsername(PLAYER_NAME + GETTIMEMS())
            .setTryUseDatagramEncryption(true));
        mState = State::CONNECTING;
    }
}

void PhotonLib::disconnectFromPhoton()
{
    if (mState == State::CONNECTED || mState == State::JOINING || mState == State::JOINED)
    {
        mLoadBalancingClient.disconnect();
        mState = State::DISCONNECTING;
    }
}
