
#ifndef networkmessagetypes_h
#define networkmessagetypes_h

enum NetworkMessages {
    Message_EchoRequestMessage = 10,
    Message_EchoResponseMessage = 20,
    Message_EntitySnapshotMessage = 30,
    Message_CommandMessage = 40,
    Message_ResendPartialMessage = 50,
    Message_AckMessage = 60,
    Message_HelloMessage = 70
};

#endif
