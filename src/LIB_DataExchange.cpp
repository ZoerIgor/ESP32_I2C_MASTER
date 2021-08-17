#include "LIB_DataExchange.h"

byte CharToByte(char symbol)
{
    byte value = symbol;
    return value;
}

char ByteToChar(byte value)
{
    char symbol = value;
    return symbol;
}

Node::Node(String key)
{
    _key = key;
}
Node::Node(String key, size_t precision)
{
    _key = key;
    _precision = precision;
}
void Node::setNode(bool value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    _node += (value) ? String(1, DEC) : String(0, DEC);
    _node += ';';
}
void Node::setNode(byte value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    _node += String(value, DEC);
    _node += ';';
}
void Node::setNode(char value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    _node += value;
    _node += ';';
}
void Node::setNode(short value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    _node += String(value, DEC);
    _node += ';';
}
void Node::setNode(int value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    _node += String(value, DEC);
    _node += ';';
}
void Node::setNode(float value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    String valueStr = String(round(value * 10000000) / 10000000, DEC);
    size_t sizeStr = valueStr.length();
    bool point = false;
    size_t counter = 0;
    for (size_t i = 0; i < sizeStr; i++)
    {
        if (point)
        {
            if (counter == _precision)
            {
                break;
            }
            counter++;
        }
        if (valueStr[i] == '.')
        {
            if (_precision == 0)
            {
                break;
            }
            else
            {
                point = true;
            }
        }
        _node += valueStr[i];

    }
    _node += ';';
}
void Node::setNode(String value)
{
    _node.clear();
    _node = _key;
    _node += '=';
    _node += value;
    _node += ';';
}
String Node::getNode()
{
    return _node;
}

Transceiver::Transceiver()
{
    _nodes.resize(_packageSize);
}
Transceiver::Transceiver(size_t size)
{
    _packageSize = size;
    _nodes.resize(_packageSize);
}
Transceiver::~Transceiver()
{
    clearPackage();
}
void Transceiver::setPackageSize(size_t size)
{
    _packageSize = size;
    _nodes.resize(_packageSize);
}
void Transceiver::addNode(Node &node)
{
    String data = node.getNode();
    _dataSize += data.length();
    _nodes.emplace_back(data);
}
void Transceiver::clearPackage()
{
    delete[] _package;
    _nodes.clear();
    _dataSize = 0;
}
char * Transceiver::getPackage()
{
    if (_dataSize > _packageSize)
    {
        _package = new char[16]{'P','a','c','k','g','e',' ','o','v','e','r','f','l','o','w','\0'};
        return _package;;
    }
    else
    {
        _package = new char[_dataSize + 1];
        size_t indx = 0;
        for (auto i = _nodes.begin(); i != _nodes.end(); i++)
        {
            String part = *i;
            size_t partSize = part.length();
            for (size_t j = 0; j < partSize; j++)
            {
                _package[indx] = part[j];
                indx++;
            }
            _package[indx] = '\0';
        }
        return _package;
    }
}

void Receiver::addData(TwoWireSlave &twoWireSlave)
{
    _length = twoWireSlave.available();
    size_t indx = 0;
    delete[] _data;
    _data = new char[_length + 1];
    while (twoWireSlave.available() > 0)
    {
        _data[indx++] = twoWireSlave.read();
    }
    _data[indx] = '\0';
}
void Receiver::addData(WireSlaveRequest &wireSlaveRequest)
{
    _length = wireSlaveRequest.available();
    size_t indx = 0;
    delete[] _data;
    _data = new char[_length + 1];
    while (wireSlaveRequest.available() > 0)
    {
        _data[indx++] = wireSlaveRequest.read();
    }
    _data[indx] = '\0';
}
bool Receiver::notEmpty()
{
    if (_length > 0)
    {
        return true;
    }
    return false;
}
std::pair<String, String> Receiver::getNextNode()
{
    size_t indx = 0;
    int segment = 0;
    bool writeKey = false;
    String key = "";
    String value = "";
    if (_length > 0)
    {
        for (; indx < _length; indx++)
        {
            if (indx != 0 && _data[map(indx + 2, 0, _length, _length, 0)] == ';')
            {
                break;
            }
        }
        segment = _length - indx;
        for (size_t i = segment; i < _length; i++)
        {
            if (!writeKey)
            {
                if (_data[i] != '=')
                {
                    key += _data[i];
                }
                else
                {
                    writeKey = true;
                }
            }
            else
            {
                if (_data[i] != ';')
                {
                    value += _data[i];
                }
            }
        }
        _length = segment;
        _data[_length] = '\0';
    }
    std::pair<String, String> node(key, value);
    return node;
}

Unpacker::~Unpacker()
{
    clearUnpacker();
}
void Unpacker::addReference(const String &key, bool &value)
{
    _receiverBool.emplace(key, value);
}
void Unpacker::addReference(const String &key, byte &value)
{
    _receiverByte.emplace(key, value);
}
void Unpacker::addReference(const String &key, char &value)
{
    _receiverChar.emplace(key, value);
}
void Unpacker::addReference(const String &key, short &value)
{
    _receiverShort.emplace(key, value);
}
void Unpacker::addReference(const String &key, int &value)
{
    _receiverInt.emplace(key, value);
}
void Unpacker::addReference(const String &key, float &value)
{
    _receiverFloat.emplace(key, value);
}
void Unpacker::addReference(const String &key, String &value)
{
    _receiverString.emplace(key, value);
}
bool Unpacker::linker(std::pair<String, String> &node, bool typeBool)
{
    for (auto it = _receiverBool.begin(); it != _receiverBool.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = (node.second.toInt() == 0) ? false : true;
            return true;
        }
    }
    return false;
}
bool Unpacker::linker(std::pair<String, String> &node, byte typeByte)
{
    for (auto it = _receiverByte.begin(); it != _receiverByte.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = node.second.toInt();
            return true;
        }
    }
    return false;
}
bool Unpacker::linker(std::pair<String, String> &node, char typeChar)
{
    for (auto it = _receiverChar.begin(); it != _receiverChar.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = node.second[0];
            return true;
        }
    }
    return false;
}
bool Unpacker::linker(std::pair<String, String> &node, short typeShort)
{
    for (auto it = _receiverShort.begin(); it != _receiverShort.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = node.second.toInt();
            return true;
        }
    }
    return false;
}
bool Unpacker::linker(std::pair<String, String> &node, int typeInt)
{
    for (auto it = _receiverInt.begin(); it != _receiverInt.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = node.second.toInt();
            return true;
        }
    }
    return false;
}
bool Unpacker::linker(std::pair<String, String> &node, float typeFloat)
{
    for (auto it = _receiverFloat.begin(); it != _receiverFloat.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = node.second.toFloat();
            return true;
        }
    }
    return false;
}
bool Unpacker::linker(std::pair<String, String> &node, String typeString)
{
    for (auto it = _receiverString.begin(); it != _receiverString.end(); it++)
    {
        if (it->first == node.first)
        {
            it->second = node.second;
            return true;
        }
    }
    return false;
}
void Unpacker::clearUnpacker()
{
    if (!_receiverBool.empty()){_receiverBool.clear();}
    if (!_receiverByte.empty()){_receiverByte.clear();}
    if (!_receiverChar.empty()){_receiverChar.clear();}
    if (!_receiverShort.empty()){_receiverShort.clear();}
    if (!_receiverInt.empty()){_receiverInt.clear();}
    if (!_receiverFloat.empty()){_receiverFloat.clear();}
    if (!_receiverString.empty()){_receiverString.clear();}
}