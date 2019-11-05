#include "connectionprofile.h"

#include <QCryptographicHash>

#define DEFAULT_PORT 6660

ConnectionProfile::ConnectionProfile()
    : m_character(nullptr), m_server(false), m_port(DEFAULT_PORT), m_player(nullptr), m_isGM(false)
{
}
ConnectionProfile::~ConnectionProfile() {}
void ConnectionProfile::setTitle(QString str)
{
    m_title= str;
}
void ConnectionProfile::setName(QString str)
{
    m_name= str;
}
void ConnectionProfile::setAddress(QString str)
{
    m_address= str;
}
void ConnectionProfile::setPort(quint16 i)
{
    m_port= i;
}
void ConnectionProfile::setServerMode(bool b)
{
    m_server= b;
}
void ConnectionProfile::setPlayer(Player* p)
{
    m_player= p;
}
void ConnectionProfile::setGm(bool b)
{
    m_isGM= b;
}
QString ConnectionProfile::getTitle() const
{
    return m_title;
}
QString ConnectionProfile::getName() const
{
    return m_name;
}
QString ConnectionProfile::getAddress() const
{
    return m_address;
}
quint16 ConnectionProfile::getPort() const
{
    return m_port;
}
bool ConnectionProfile::isServer() const
{
    return m_server;
}
Player* ConnectionProfile::getPlayer() const
{
    return m_player;
}
bool ConnectionProfile::isGM() const
{
    return m_isGM;
}
Character* ConnectionProfile::getCharacter() const
{
    return m_character;
}
void ConnectionProfile::setCharacter(Character* character)
{
    m_character= character;
}

QByteArray ConnectionProfile::getPassword() const
{
    return m_password;
}

void ConnectionProfile::setPassword(const QString& password)
{
    if(password.isEmpty())
    {
        m_password.clear();
    }
    else
    {
        m_password= QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);
    }
}
void ConnectionProfile::setHash(const QByteArray& password)
{
    m_password= password;
}
void ConnectionProfile::cloneProfile(const ConnectionProfile* src)
{
    m_password= src->getPassword();
    setGm(src->isGM());
    setPort(src->getPort());
    setTitle(src->getTitle());
    setName(src->getName());
    setAddress(src->getAddress());
    setServerMode(src->isServer());
    m_player= new Player();
    m_player->copyPlayer(src->getPlayer());
}
