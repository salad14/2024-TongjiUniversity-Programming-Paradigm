// Photon_lib.cpp

#include "Photon_lib.h"
#include "UIListener.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <locale>

// �滻Ϊ����PhotonӦ��ID
static const ExitGames::Common::JString appID = L"558bddbb-2dda-4767-ad34-1ea68fdec86e"; // ��������Ӧ��ID
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
    }
    // ����Ѿ���ʼ��������ѡ�����listener�򱣳ֲ���
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
    mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
    mLogger.setListener(*this);
    mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
    ExitGames::Common::Base::setListener(this);
    ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
}

// ����Photon״̬��������service()
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
    // ���͸��Լ�
    int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
    mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
    if (mSendCount >= MAX_SENDCOUNT)
        mState = State::SENT_DATA;
}

int PhotonLib::getPlayerCount()
{
    // ʹ�� getIsInRoom() ����鵱ǰ�Ƿ��ڷ�����
    if (mLoadBalancingClient.getIsInRoom())
    {
        // ʹ�� getCountPlayersIngame() ��ȡ��ǰ�����е��������
        return mLoadBalancingClient.getCountPlayersIngame();
    }
    return 0;
}

// Override Listener �麯��ʵ��

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

    // ��������ʧ�ܻص�
    if (connectionCallback)
    {
        // ���� std::wstring ������Ϣ
        std::wstring errorMsg = L"Connection error: ";
        errorMsg += intToJString(errorCode).cstr();
        // ���ûص�����
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

    // ������������仯�ص�
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

    // ������������仯�ص�
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

        // ת�� ExitGames::Common::JString �� std::wstring
        std::wstring errorMsg = L"Connection error: " + std::wstring(errorString.cstr());

        // ��������ʧ�ܻص�
        if (connectionCallback)
            connectionCallback(false, errorMsg);
        return;
    }
    if (mpOutputListener)
        mpOutputListener->writeString(L"connected to cluster " + cluster);
    mState = State::CONNECTED;

    // �������ӳɹ��ص�
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

    // ���ûص�
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

// ���÷�������Ļص�
void PhotonLib::setRoomJoinedCallback(const std::function<void()>& callback) {
    roomJoinedCallback = callback;
}

// ������������仯�Ļص�
void PhotonLib::setPlayerCountChangedCallback(const std::function<void(int)>& callback)
{
    playerCountChangedCallback = callback;
}

// �������ӻص�
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
