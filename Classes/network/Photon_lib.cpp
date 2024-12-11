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

// �滻Ϊ����PhotonӦ��ID
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

// ��ʼ����̬��Ա
PhotonLib* PhotonLib::instance = nullptr;

// ��ȡ����ʵ��
PhotonLib* PhotonLib::getInstance()
{
    return instance;
}

// ��ʼ������ʵ��
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

// ˽�й��캯��
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

// ����Photon״̬�������� service()
void PhotonLib::update(void)
{
    mLoadBalancingClient.service();
}

// ��ȡ��ǰ״̬�ַ���
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

// ���������¼�
void PhotonLib::sendData(void)
{
    ExitGames::Common::Hashtable event;
    event.put(static_cast<nByte>(0), ++mSendCount);
    // ���͸��Լ�
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
    if (mSendCount >= MAX_SENDCOUNT)
        mState = State::SENT_DATA;
}

// ��ȡ�������
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


// Override Listener �麯��ʵ��

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

    // ��������ʧ�ܻص�
    if (connectionCallback)
    {
        // ���� std::wstring ������Ϣ
        std::wstring errorMsg = L"Connection error: " + std::to_wstring(errorCode);

        // ���ûص�����
        connectionCallback(false, errorMsg);
    }

    // �������ӣ����磬�������3�Σ�ÿ�μ��2�룩
    static int retryCount = 0;
    if (retryCount < 3)
    {
        retryCount++;
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Retrying to connect... Attempt %d", retryCount);
        if (mpOutputListener)
            mpOutputListener->writeString(L"Retrying to connect... Attempt " + intToJString(retryCount));
        mLoadBalancingClient.disconnect(); // ȷ��֮ǰ�����ӱ��Ͽ�

        // �ӳ���������
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


// ������������仯�ص�
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

    // ������������仯�ص�
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

    // ������������仯�ص�
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

// ���ӷ��ػص�
void PhotonLib::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"connection error: " + ExitGames::Common::JString(errorString));
        mState = State::DISCONNECTING;

        // ���� std::wstring ������Ϣ
        std::wstring errorMsg = L"Connection error: " + std::to_wstring(errorCode);

        // ��������ʧ�ܻص�
        if (connectionCallback)
            connectionCallback(false, errorMsg);

        // �������ӣ����磬�������3�Σ�ÿ�μ��2�룩
        static int retryCount = 0;
        if (retryCount < 3)
        {
            retryCount++;
            EGLOG(ExitGames::Common::DebugLevel::INFO, L"Retrying to connect... Attempt %d", retryCount);
            if (mpOutputListener)
                mpOutputListener->writeString(L"Retrying to connect... Attempt " + intToJString(retryCount));
            mLoadBalancingClient.disconnect(); // ȷ��֮ǰ�����ӱ��Ͽ�

            // �ӳ���������
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

    // �������Լ���
    static int retryCount = 0;
    retryCount = 0;

    // �������ӳɹ��ص�
    if (connectionCallback)
        connectionCallback(true, L"Connected to Photon successfully.");
}

// �Ͽ����ӷ��ػص�
void PhotonLib::disconnectReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"disconnected");
    if (mpOutputListener)
        mpOutputListener->writeString(L"disconnected");
    mState = State::DISCONNECTED;
}

// �������䷵�ػص�
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

        ss.str(L""); // ����ַ�����
        ss << L"regularly sending dummy events now";
        message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
    mState = State::JOINED;

    // ���ûص�
    if (roomJoinedCallback) {
        roomJoinedCallback();
    }
}

// ʵ�� leaveRoom ����
void PhotonLib::leaveRoom()
{
    if (mLoadBalancingClient.getIsInRoom())
    {
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Leaving the current room...");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Leaving the current room...");

        // ���� Photon ���뿪�������
        mLoadBalancingClient.opLeaveRoom();

        // ����״̬
        mState = State::LEAVING;
    }
    else
    {
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Not currently in a room.");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Not currently in a room.");
    }
}

// ���÷����뿪��Ļص�
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

        ss.str(L""); // ����ַ�����
        ss << L"regularly sending dummy events now";
        message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
    mState = State::JOINED;

    // �����������ص�
    if (roomJoinedCallback) {
        CCLOG("PhotonLib: triggering roomJoinedCallback.");
        roomJoinedCallback();
    }

    // ������������仯�ص�����ʼ�����
    if (playerCountChangedCallback) {
        int currentPlayerCount = getPlayerCount();
        CCLOG("PhotonLib: triggering playerCountChangedCallback with count %d", currentPlayerCount);
        playerCountChangedCallback(currentPlayerCount);
    }
}

// ����������䷵�ػص�
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

        ss.str(L""); // ����ַ�����
        ss << L"regularly sending dummy events now";
        message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }
    mState = State::JOINED;

    // ���ûص�
    if (roomJoinedCallback) {
        roomJoinedCallback();
    }
}

// �뿪���䷵�ػص�
void PhotonLib::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
    if (errorCode)
    {
        EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"Failed to leave room: %ls", errorString.cstr());
        if (mpOutputListener)
            mpOutputListener->writeString(L"Failed to leave room: " + ExitGames::Common::JString(errorString));
        // �뿪����ʧ�ܣ���Ȼ����CONNECTED״̬�������ӵ����ڷ��䣩
        mState = State::CONNECTED;
        return;
    }

    // �뿪����ɹ���״̬��LEAVINGתΪCONNECTED�������ӵ��޷��䣩
    mState = State::CONNECTED;

    if (mpOutputListener)
    {
        std::wstringstream ss;
        ss << L"Successfully left the room.";
        ExitGames::Common::JString message = ExitGames::Common::JString(ss.str().c_str());
        mpOutputListener->writeString(message);
    }

    // �����뿪����Ļص������У�
    if (leaveRoomCallback)
    {
        leaveRoomCallback();
    }
}


// ����������ػص�
void PhotonLib::joinLobbyReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"joined lobby");
    if (mpOutputListener)
        mpOutputListener->writeString(L"joined lobby");
}

// �뿪�������ػص�
void PhotonLib::leaveLobbyReturn(void)
{
    EGLOG(ExitGames::Common::DebugLevel::INFO, L"left lobby");
    if (mpOutputListener)
        mpOutputListener->writeString(L"left lobby");
}

// ����򴴽�����
void PhotonLib::joinOrCreateRoom(const ExitGames::Common::JString& roomName)
{
    mLoadBalancingClient.opJoinOrCreateRoom(roomName);
}

// ���÷�������Ļص�
void PhotonLib::setRoomJoinedCallback(const std::function<void()>& callback)
{
    roomJoinedCallback = callback;
    CCLOG("PhotonLib: roomJoinedCallback has been set.");
}

// ������������仯�Ļص�
void PhotonLib::setPlayerCountChangedCallback(const std::function<void(int)>& callback)
{
    playerCountChangedCallback = callback;
    CCLOG("PhotonLib: playerCountChangedCallback has been set.");
}

// �������ӻص�
void PhotonLib::setConnectionCallback(const std::function<void(bool, const std::wstring&)>& callback)
{
    connectionCallback = callback;
}

// ���ӵ�Photon������
void PhotonLib::connectToPhoton()
{
    if (mState == State::INITIALIZED || mState == State::DISCONNECTED)
    {
        mLoadBalancingClient.connect(
            ExitGames::LoadBalancing::ConnectOptions()
            .setServerAddress(ExitGames::Common::JString(L"ns.photonengine.cn")) // �����й�����������ַ
            .setAuthenticationValues(
                ExitGames::LoadBalancing::AuthenticationValues()
                .setUserID(ExitGames::Common::JString() + intToJString(GETTIMEMS())) // ����Ψһ��UserID
            )
            .setUsername(PLAYER_NAME + intToJString(GETTIMEMS())) // ����Ψһ���û���
            .setTryUseDatagramEncryption(true)
        );
        mState = State::CONNECTING;
        EGLOG(ExitGames::Common::DebugLevel::INFO, L"Attempting to connect to Photon...");
        if (mpOutputListener)
            mpOutputListener->writeString(L"Attempting to connect to Photon...");
    }
}
