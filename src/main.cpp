#include "main.h"

//#define DEBUG_SERIAL
#define DEBUG_BLINK
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04
#define I2C_BUFFER_SIZE 100
#define I2C_FREQUENCY 400000
#define I2C_RETRY_DELAY 0
#define I2C_COMMUNICATION_INTERVAL_MSEC 10

bool myBool = true;
bool myBool2 = true;
byte myByte = 117;
char myChar = 'g';
short myShort = 3455;
int myInt = 65234;
float myFloat = 12.39;
size_t myFloatprecision = 2;
String myString = "Sunny";

Transceiver transmitData(I2C_BUFFER_SIZE);
Receiver receiveData;

Node node_myBool("bo");
Node node_myBool2("bo2");
Node node_myByte("by");
Node node_myChar("ch");
Node node_myShort("sh");
Node node_myInt("in");
Node node_myFloat("fl", myFloatprecision);
Node node_myString("st");

Unpacker unpacker_bool;
Unpacker unpacker_byte;
Unpacker unpacker_char;
Unpacker unpacker_short;
Unpacker unpacker_int;
Unpacker unpacker_float;
Unpacker unpacker_String;

void UpdateNodes()
{
    node_myBool.setNode(myBool);
    node_myBool2.setNode(myBool2);
    node_myByte.setNode(myByte);
    node_myChar.setNode(myChar);
    node_myShort.setNode(myShort);
    node_myInt.setNode(myInt);
    node_myFloat.setNode(myFloat);
    node_myString.setNode(myString);
}

void CollectData()
{
    transmitData.addNode(node_myBool);
    transmitData.addNode(node_myBool2);
    transmitData.addNode(node_myByte);
    transmitData.addNode(node_myChar);
    transmitData.addNode(node_myShort);
    transmitData.addNode(node_myInt);
    transmitData.addNode(node_myFloat);
    transmitData.addNode(node_myString);
}

void I2C_Request()
{
    static unsigned long lastWireTransmit = 0;
    if (millis() - lastWireTransmit > I2C_COMMUNICATION_INTERVAL_MSEC)
    {
        #ifdef DEBUG_BLINK
            myBool = !myBool;
        #endif
        UpdateNodes();
        CollectData();
        WirePacker packer;
        packer.write(transmitData.getPackage());
        transmitData.clearPackage();
        packer.end();
        Wire.beginTransmission(I2C_SLAVE_ADDR);
        while (packer.available()) 
        {
            Wire.write(packer.read());
        }
        Wire.endTransmission();
        lastWireTransmit = millis();
    }
}

void ListOfUpdateVaribles()
{
    unpacker_bool.addReference(node_myBool.getKey(), myBool);
    unpacker_bool.addReference(node_myBool2.getKey(), myBool2);
    unpacker_byte.addReference(node_myByte.getKey(), myByte);
    unpacker_char.addReference(node_myChar.getKey(), myChar);
    unpacker_short.addReference(node_myShort.getKey(), myShort);
    unpacker_int.addReference(node_myInt.getKey(), myInt);
    unpacker_float.addReference(node_myFloat.getKey(), myFloat);
    unpacker_String.addReference(node_myString.getKey(), myString);
}

void UnpackData()
{
    while (receiveData.notEmpty())
    {
        bool BOOL = false;
        byte BYTE = 0;
        char CHAR = 'A';
        short SHORT = 0;
        int INT = 0;
        float FLOAT = 0;
        String STRING = "";
        std::pair<String, String> node = receiveData.getNextNode();
        if (unpacker_bool.linker(node, BOOL)){continue;}
        if (unpacker_byte.linker(node, BYTE)){continue;}
        if (unpacker_char.linker(node, CHAR)){continue;}
        if (unpacker_short.linker(node, SHORT)){continue;}
        if (unpacker_int.linker(node, INT)){continue;}
        if (unpacker_float.linker(node, FLOAT)){continue;}
        if (unpacker_String.linker(node, STRING)){continue;}
    }
}

void I2C_Receive()
{
    static unsigned long lastReadMillis = 0;
    if (millis() - lastReadMillis > I2C_COMMUNICATION_INTERVAL_MSEC)
    {
        WireSlaveRequest slaveRequest(Wire, I2C_SLAVE_ADDR, I2C_BUFFER_SIZE);
        slaveRequest.setRetryDelay(I2C_RETRY_DELAY);
        if (slaveRequest.request())
        {
            receiveData.addData(slaveRequest);
        }
        lastReadMillis = millis();
    }
}

void setup()
{
    #ifdef DEBUG_BLINK
        pinMode(LED_BUILTIN, OUTPUT);
    #endif
    #ifdef DEBUG_SERIAL
        Serial.begin(921600);
        Serial.println();
    #endif
    #ifdef DEBUG_SERIAL
        Serial.println(node_myBool.getKey() + ":\t" + String(myBool, DEC));
        Serial.println(node_myBool2.getKey() + ":\t" + String(myBool2, DEC));
        Serial.println(node_myByte.getKey() + ":\t" + String(myByte, DEC));
        Serial.println(node_myChar.getKey() + ":\t" + String(myChar));
        Serial.println(node_myShort.getKey() + ":\t" + String(myShort, DEC));
        Serial.println(node_myInt.getKey() + ":\t" + String(myInt, DEC));
        Serial.println(node_myFloat.getKey() + ":\t" + String(myFloat, DEC));
        Serial.println(node_myString.getKey() + ":\t" + myString);
        Serial.println();
    #endif
    ListOfUpdateVaribles();
    if (!Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY))
    {
        while(true) delay(I2C_COMMUNICATION_INTERVAL_MSEC);
    }
}

#ifdef DEBUG_BLINK
    void Blink()
    {
        if (myBool2)
        {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        else
        {
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
#endif

void loop()
{
    I2C_Receive();
    I2C_Request();
    if (receiveData.notEmpty())
    {
        UnpackData();
        #ifdef DEBUG_SERIAL
            Serial.println(node_myBool.getKey() + ":\t" + String(myBool, DEC));
            Serial.println(node_myBool2.getKey() + ":\t" + String(myBool2, DEC));
            Serial.println(node_myByte.getKey() + ":\t" + String(myByte, DEC));
            Serial.println(node_myChar.getKey() + ":\t" + String(myChar));
            Serial.println(node_myShort.getKey() + ":\t" + String(myShort, DEC));
            Serial.println(node_myInt.getKey() + ":\t" + String(myInt, DEC));
            Serial.println(node_myFloat.getKey() + ":\t" + String(myFloat, DEC));
            Serial.println(node_myString.getKey() + ":\t" + myString);
            Serial.println();
        #endif
        #ifdef DEBUG_BLINK
            Blink();
        #endif
    }
}