#pragma once
#ifndef _DECODER_
    #define _DECODER_
    #include <Arduino.h>
    #include <map>
    #include <Wire.h>
    #include <WireSlave.h>
    #include <WireSlaveRequest.h>

    byte CharToByte(char symbol);
    char ByteToChar(byte value);

    class Node
    {
        public:
            Node(String key);
            Node(String key, size_t precision);
            void setNode(bool value);
            void setNode(byte value);
            void setNode(char value);
            void setNode(short value);
            void setNode(int value);
            void setNode(float value);
            void setNode(String value);
            const String& getKey(){return _key;}
            String getNode();
        private:
            String _key, _node;
            size_t _precision;
    };
    class Transceiver
    {
        public:
            Transceiver();
            Transceiver(size_t size);
            ~Transceiver();
            void setPackageSize(size_t size);
            void addNode(Node &node);
            void clearPackage();
            char * getPackage();
        private:
            size_t _packageSize = 30, _dataSize = 0;
            std::vector<String> _nodes;
            char *_package;
    };
    class Receiver
    {
        public:
            void addData(TwoWireSlave &twoWireSlave);
            void addData(WireSlaveRequest &wireSlaveRequest);
            bool notEmpty();
            std::pair<String, String> getNextNode();
        private:
            size_t _length;
            char *_data;
    };
    class Unpacker
    {
        public:
            ~Unpacker();
            void addReference(const String &key, bool &value);
            void addReference(const String &key, byte &value);
            void addReference(const String &key, char &value);
            void addReference(const String &key, short &value);
            void addReference(const String &key, int &value);
            void addReference(const String &key, float &value);
            void addReference(const String &key, String &value);
            bool linker(std::pair<String, String> &node, bool typeBool);
            bool linker(std::pair<String, String> &node, byte typeByte);
            bool linker(std::pair<String, String> &node, char typeChar);
            bool linker(std::pair<String, String> &node, short typeShort);
            bool linker(std::pair<String, String> &node, int typeInt);
            bool linker(std::pair<String, String> &node, float typeFloat);
            bool linker(std::pair<String, String> &node, String typeString);
            void clearUnpacker();
        private:
            std::map<String, bool &> _receiverBool;
            std::map<String, byte &> _receiverByte;
            std::map<String, char &> _receiverChar;
            std::map<String, short &> _receiverShort;
            std::map<String, int &> _receiverInt;
            std::map<String, float &> _receiverFloat;
            std::map<String, String &> _receiverString;
    };
#endif