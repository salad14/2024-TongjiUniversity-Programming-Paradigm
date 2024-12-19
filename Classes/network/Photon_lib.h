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
    // ��ȡ����ʵ��
    static PhotonLib* getInstance();

    // ��ʼ������ʵ��
    static void initialize(UIListener* listener);

    // ����Photon״̬
    void update(void);

    // ��ȡ��ǰ״̬���ַ�����ʾ
    ExitGames::Common::JString getStateString(void);

    // ���÷�������Ļص�
    void setRoomJoinedCallback(const std::function<void()>& callback);

    // ������������仯�Ļص�
    void setPlayerCountChangedCallback(const std::function<void(int)>& callback);

    // �������ӵ�Photon�Ļص�
    void setConnectionCallback(const std::function<void(bool, const std::wstring&)>& callback);

    // ����򴴽�����
    void joinOrCreateRoom(const ExitGames::Common::JString& roomName);

    // ��ȡ��ǰ������������
    int getPlayerCount();

    // ��ʽ���ӵ�Photon������
    void connectToPhoton();

    // ��ʽ�Ͽ�Photon����
    void disconnectFromPhoton();

    void leaveRoom();

    // ���÷����뿪��Ļص�
    void setLeaveRoomCallback(const std::function<void()>& callback);

    // �����Զ����¼��ص�
    void setCustomEventCallback(const std::function<void(int, const ExitGames::Common::Hashtable&)>& callback);

    // �����Զ����¼�
    void raiseCustomEvent(const ExitGames::Common::Hashtable& eventContent, int eventCode, nByte receiverGroup);

    // ��ȡ������ұ��
    int getLocalPlayerNumber();

private:
    // ˽�й��캯��
    PhotonLib(UIListener* listener);

    // ��ֹ����
    PhotonLib(const PhotonLib&) = delete;
    PhotonLib& operator=(const PhotonLib&) = delete;

    // ����ʵ��ָ��
    static PhotonLib* instance;

    // Override Listener �麯��
    virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override;
    virtual void connectionErrorReturn(int errorCode) override;
    virtual void clientErrorReturn(int errorCode) override;
    virtual void warningReturn(int warningCode) override;
    virtual void serverErrorReturn(int errorCode) override;

    // �¼��ص�
    virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override;
    virtual void leaveRoomEventAction(int playerNr, bool isInactive) override;
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

    // ��������ص�
    virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;
    virtual void disconnectReturn(void) override;
    virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override;
    virtual void joinLobbyReturn(void) override;
    virtual void leaveLobbyReturn(void) override;

    void sendData(void);

    // ״̬��
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

    // ��������Ļص�
    std::function<void()> roomJoinedCallback;

    // ��������仯�Ļص�
    std::function<void(int)> playerCountChangedCallback;

    // ���ӻص�
    std::function<void(bool, const std::wstring&)> connectionCallback;

    // �뿪�����Ļص�
    std::function<void()> leaveRoomCallback;

    // �Զ����¼��ص�
    std::function<void(int, const ExitGames::Common::Hashtable&)> customEventCallback;
};