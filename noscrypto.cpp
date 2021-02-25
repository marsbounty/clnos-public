#include "noscrypto.h"
#include <algorithm>
#include <sstream>
#include <QCryptographicHash>
#include <random>
#include "settings.h"
#include <QByteArray>
#include <QTime>
#include <QDebug>
#include <QFile>
#include <QRandomGenerator>
#include <QTextCodec>

QString NosCrypto::dxhash = "";
QString NosCrypto::glhash = "";
QString NosCrypto::version = "";
QString NosCrypto::vhash = "";          //v for Vendetta
QString NosCrypto::vversion = "";

QByteArray NosCrypto::encryptTable;
QByteArray NosCrypto::decryptTable;


NosCrypto::NosCrypto(QObject *parent) : QObject(parent)
{
    bigPacket = "";
    glhash = Settings::getGlobalSettings()->getSetting("loginvars/dat").toString();
    dxhash = Settings::getGlobalSettings()->getSetting("loginvars/xdat").toString();
    version = Settings::getGlobalSettings()->getSetting("loginvars/version").toString();

    vhash = Settings::getGlobalSettings()->getSetting("loginvars/vendettahash").toString();
    vversion = Settings::getGlobalSettings()->getSetting("loginvars/vendettaversion").toString();

    old_version = false;
    decCounter = 0;
    encCounter = 0;
    encoder = QTextCodec::codecForName("Windows-1250")->makeEncoder();
    decoder = QTextCodec::codecForName("Windows-1250")->makeDecoder();
}

void NosCrypto::setDxhash(const QString &value)
{
    dxhash = value;
}

void NosCrypto::setGlhash(const QString &value)
{
    glhash = value;
}

void NosCrypto::setVersion(const QString &value)
{
    version = value;
}

void NosCrypto::setVversion(const QString &value)
{
    vversion = value;
}

void NosCrypto::setVhash(const QString &value)
{
    vhash = value;
}

void NosCrypto::readVendettaFix()
{
    QFile file(":/files/ressources/xorTable");
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    encryptTable = file.read(0x10000);
    decryptTable = file.read(0x10000);
}


QString NosCrypto::decryptLoginPacket(const QByteArray& ba, std::size_t len)
{
//    QByteArray ba2 = ba;
//    if(old_version){
//        for (int i = 0; i < ba2.size(); ++i) {
//            //qDebug()<< QString::number(ba2[i], 16);
//            ba2[i] = ba2[i] ^ decryptTable.at(decCounter%0x10000);
//            //qDebug()<<QString::number(decryptTable.at(decCounter%0x10000),16);

//            //qDebug()<< QString::number(ba2[i], 16);
//            decCounter++;
//        }
//    }

//    std::vector<unsigned char> buf(ba2.begin(), ba2.end());
//    std::string temp;
//    QString output;
//    std::transform(buf.begin(), buf.begin() + len, std::back_inserter(temp),
//        [](unsigned char c)
//    {
//        return c - 0xF;
//    }
//    );

//    output = QString::fromLatin1(QByteArray(temp.c_str(), temp.length()));
//    return output;

    QByteArray inp = ba;
    char *data = inp.data();
    while (*data) {
        *data = *data-0xF;
        ++data;
    }
    QString decoded = decoder->toUnicode(inp);
    return decoded;
}

std::vector<unsigned char> NosCrypto::encryptLoginPacket(const QString& buf)
{
    std::string sbuf = buf.toStdString();

    std::vector<unsigned char> output;
    std::transform(sbuf.begin(), sbuf.end(), std::back_inserter(output),
        [](char c)
    {
        return (c ^ 0xC3) + 0xF;
    }
    );
    output.push_back(216);

    //qDebug()<< hex<<output;

    if(old_version){
        for(int i=0; i < output.size(); i++){
            //qDebug()<< hex<<output[i];
           output[i]= output[i]^encryptTable.at(encCounter%0x10000);
           //qDebug()<< hex<<encryptTable.at(counter%0x10000);
           //qDebug()<< hex<<output[i];
           encCounter++;
        }
    }

    //qDebug()<< hex<<output;
    return output;
}

QByteArray NosCrypto::createLoginPacket(const QString& username, const QString& password)
{
    QByteArray output;
    std::vector<unsigned char> temp;
    QString loginhash = NosCrypto::createLoginHash(username);
    QString passwordhash;
    if(old_version){
        passwordhash = NosCrypto::pwEncrypt(password);
    }else{
        passwordhash = NosCrypto::Sha512(password);
    }
    QString hwid = Settings::getGlobalSettings()->getSetting("hwid").toString();
    QString diff = " "+hwid+" ";
    if(old_version){
        diff = " ";
    }

    QString version = NosCrypto::createLoginVersion();
    QString beginn = "NoS0575 2789576";
    QString complete = beginn + " " + username + " " + passwordhash + diff + version+ " 0" + " " +loginhash;
    //qDebug()<< complete;
    //qDebug()<< old_version;
    temp = NosCrypto::encryptLoginPacket(complete);
    for (auto &var : temp)
    {
        output.append(var);
    }
    return output;
}


QByteArray NosCrypto::createGFLoginPacket(const QString &uid,QString username, int langid)
{
    QByteArray output;
    QString hwid = Settings::getGlobalSettings()->getSetting("hwid").toString();
    QString loginhash = NosCrypto::createLoginHash("");
    QString version = NosCrypto::createLoginVersion();
    QString beginn = "NoS0577";
    //QString complete = beginn + " " + uid +" "+ username+" " +hwid+ " " + version+ " 0" + " " +loginhash;
    QString complete = beginn + " " + uid +"  "+hwid+ " " + version+ " 0 " +loginhash;
    if(Settings::getGlobalSettings()->getSetting("logLoginStuff").toBool()){
        qDebug()<<complete;
    }
    std::vector<unsigned char> temp;
    temp = NosCrypto::encryptLoginPacket(complete);
    for (auto &var : temp)
    {
        output.append(var);
    }
    return output;
}

//encrypt Password
QString NosCrypto::pwEncrypt(QString pw) const
{
        std::string password = pw.toStdString();
        const unsigned char secondtable[] = { 0x2E, 0x2A, 0x17, 0x4F, 0x20, 0x24, 0x47, 0x11, 0x5B, 0x37, 0x53,
                                          0x43, 0x15, 0x34, 0x45, 0x25, 0x4B, 0x1D, 0x2F, 0x58, 0x2B, 0x32, 0x63 };

        std::string hex, temp;
        std::stringstream ss;

        short pos = randomNumber(0, 23);
        char low, high;

        for(unsigned int i = 0; i < password.size(); i++)
                ss << std::uppercase << std::hex << (int)password[i];

        temp += ss.str();
        ss.str("");

        ss << std::uppercase << std::hex << (int)secondtable[randomNumber(0, 23)];

        for(unsigned int i = 0; i < temp.size(); i += 2)
        {
                high = secondtable[pos] & 0xF;
                low = (secondtable[pos] & 0xF0) >> 4;

                ss << std::uppercase << std::hex << (int)low;
                ss << std::uppercase << temp[i];
                ss << std::uppercase << std::hex << (int)high;
                ss << std::uppercase << temp[i + 1];

                pos == 22 ? pos = 0 : pos++;
        }

        return QString::fromStdString(ss.str());
}

bool NosCrypto::getOld_version() const
{
    return old_version;
}

void NosCrypto::setOld_version(bool value)
{
    old_version = value;
}

void NosCrypto::resetVCounter()
{
    encCounter = 0;
    decCounter = 0;
}

QString NosCrypto::createLoginHash(const QString& user) const
{
    QString output;
    QString login_string_to_hash;
    if(old_version){
        login_string_to_hash += vhash.toUpper();
        login_string_to_hash += vhash.toUpper();
    }else{
        login_string_to_hash += dxhash.toUpper();
        login_string_to_hash += glhash.toUpper();
    }
    login_string_to_hash += user;
    QByteArray textTemp = login_string_to_hash.toUtf8() ;
    output = QString(QCryptographicHash::hash((textTemp),QCryptographicHash::Md5).toHex());
    return output.toUpper();
}

QString NosCrypto::createLoginVersion(int langid) const
{
    std::stringstream output;
    std::string vers = version.toStdString();
    if(old_version){
        vers = vversion.toStdString();
    }
    output << "00" << std::uppercase << std::hex << int(randomNumber(0,126))
        << std::uppercase << std::hex << int(randomNumber(0,126))
        << std::uppercase << std::hex << int(randomNumber(0,126))
        <<" "<<langid<< '\v' << vers ;

    return QString::fromStdString(output.str());
}

int NosCrypto::randomNumber(int min, int max) const
{
//    if (min > max)
//    {
//        int Temp = min;
//        min = max;
//        max = Temp;
//    }
//    return ((rand()%(max-min+1))+min);
    return QRandomGenerator::global()->bounded(min,max);
}


QString NosCrypto::Sha512(const QString plainText) const
{
    QString output;
    QByteArray textTemp = plainText.toUtf8() ;
    output = QString(QCryptographicHash::hash((textTemp),QCryptographicHash::Sha512).toHex());
    return output.toUpper();
}

QStringList NosCrypto::decryptGamePacket(const QByteArray& inp, std::size_t len)
{
    QByteArray ba2 = inp;
    if(old_version){
        for (int i = 0; i < ba2.size(); ++i) {
            //qDebug()<< QString::number(ba2[i], 16);
            ba2[i] = ba2[i] ^ decryptTable.at(decCounter%0x10000);
            //qDebug()<<QString::number(decryptTable.at(decCounter%0x10000),16);

            //qDebug()<< QString::number(ba2[i], 16);
            decCounter++;
        }
    }


    std::vector<unsigned char> buf(ba2.begin(), ba2.end());
    buf.insert(buf.begin(), last_buf.begin(), last_buf.end());
    last_buf.clear();
    len = buf.size();
    std::vector<std::string> output;
    QStringList normOutput;
    std::string current_packet;
    //std::string current_packet = last_current_packet;
    static const char keys[] = { ' ','-','.','0','1','2','3','4','5','6','7','8','9','n' };
    std::size_t index = 0;
    unsigned char currentByte = 0, length = 0, first = 0, second = 0;

    while (index < len)
    {
        currentByte = buf[index];
        ++index;
        if (currentByte == 0xFF)
        {
            output.push_back(current_packet);
            current_packet = "";
            continue;
        }

        length = currentByte & 0x7F;
        if (currentByte & 0x80)
        {
            while (length)
            {
                if (index <= len)
                {
                    currentByte = buf[index];
                    ++index;

                    first = keys[((currentByte & 0xF0u) >> 4) - 1];
                    if (first != 0x6E)
                        current_packet += first;
                    if (length <= 1)
                        break;

                    second = keys[(currentByte & 0xF) - 1];
                    if (second != 0x6E)
                        current_packet += second;
                    length -= 2;
                }
                else
                {
                    --length;
                }
            }
        }
        else
        {
            while (length)
            {
                if (index <= len)
                {
                    current_packet += buf[index] ^ 0xFF;
                    ++index;
                }

                --length;
            }
        }
    }

    if(current_packet.length()>0){
        last_buf = buf;
        //qDebug()<<"CURRENT"<<QString::fromStdString(current_packet);
    }else{
        for(unsigned int i = 0; i < output.size();i++){
            normOutput.append(decoder->toUnicode(QByteArray(output[i].c_str(), output[i].length())));
        }

        return normOutput;
    }
    return {};
    //last_current_packet = current_packet;



}

QByteArray NosCrypto::encryptGamePacket(const QString& inp, int session, bool is_session_packet)
{
    std::string buf = encoder->fromUnicode(inp).toStdString();
    std::size_t packet_length = buf.size();
    std::string packet_mask;
    std::transform(buf.begin(), buf.end(), std::back_inserter(packet_mask),
        [](char c)
    {
        if (c == '#' || c == '/' || c == '$' || c == '(' || c == ')'|| c == '!'|| c == '%'|| c == '='|| c == '?')
            return '0';

        if (!(c -= 0x20) || (c += 0xF1) < 0 || (c -= 0xB) < 0 || !(c -= 0xC5))
            return '1';
        return '0';
    }
    );

    std::vector<unsigned char> output;
    std::size_t sequences = 0, sequence_counter = 0;
    std::size_t last_position = 0, current_position = 0, length = 0;
    unsigned char current_byte = 0;
    while (current_position <= packet_length)
    {
        last_position = current_position;
        while (current_position < packet_length && packet_mask[current_position] == '0')
            ++current_position;

        if (current_position)
        {
            length = (current_position - last_position);
            sequences = (length / 0x7E);
            for (std::size_t i = 0; i < length; ++i, ++last_position)
            {
                if (i == (sequence_counter * 0x7E))
                {
                    if (!sequences)
                    {
                        output.push_back(length - i);
                    }
                    else
                    {
                        output.push_back(0x7E);
                        --sequences;
                        ++sequence_counter;
                    }
                }

                output.push_back(buf[last_position] ^ 0xFF);
            }
        }

        if (current_position >= packet_length)
            break;

        last_position = current_position;
        while (current_position < packet_length && packet_mask[current_position] == '1')
            ++current_position;

        if (current_position)
        {
            length = (current_position - last_position);
            sequences = (length / 0x7E);
            for (std::size_t i = 0; i < length; ++i, ++last_position)
            {
                if (i == (sequence_counter * 0x7E))
                {
                    if (!sequences)
                    {
                        output.push_back((length - i) | 0x80);
                    }
                    else
                    {
                        output.push_back(0x7E | 0x80);
                        --sequences;
                        ++sequence_counter;
                    }
                }

                current_byte = buf[last_position];
                switch (current_byte)
                {
                case 0x20:
                    current_byte = 1;
                    break;
                case 0x2D:
                    current_byte = 2;
                    break;
                case 0x2E:
                    current_byte = 3;
                    break;
                case 0xFF:
                    current_byte = 0xE;
                    break;
                default:
                    current_byte -= 0x2C;
                    break;
                }

                if (current_byte != 0x00)
                {
                    if (i % 2 == 0)
                    {
                        output.push_back(current_byte << 4);
                    }
                    else
                    {
                        output.back() |= current_byte;
                    }
                }
            }
        }
    }

    output.push_back(0xFF);
    completeGamePacketEncrypt(output, session, is_session_packet);
    QByteArray temp;
    for (auto &var : output)
    {
        temp.append(var);
    }

    if(old_version){
        for(int i=0; i < temp.size(); i++){
            //qDebug()<< hex<<output[i];
           temp[i]= temp[i]^encryptTable.at(encCounter%0x10000);
           //qDebug()<< hex<<encryptTable.at(counter%0x10000);
           //qDebug()<< hex<<output[i];
           encCounter++;
        }
    }

    return temp;
}

void NosCrypto::completeGamePacketEncrypt(std::vector<unsigned char>& buf, int session, bool is_session_packet) const
{
    unsigned char session_number = (((session >> 6) & 0xFF) & 0x80000003);

//    if (session_number < 0)
//        session_number = (((session_number - 1) | 0xFFFFFFFC) + 1);

    unsigned char session_key = (session & 0xFF);

    if (is_session_packet)
        session_number = -1;

    switch (session_number)
    {
    case 0:
        for (std::size_t i = 0; i < buf.size(); ++i)
            buf[i] = (buf[i] + (session_key + 0x40));
        break;

    case 1:
        for (std::size_t i = 0; i < buf.size(); ++i)
            buf[i] = (buf[i] - (session_key + 0x40));
        break;

    case 2:
        for (std::size_t i = 0; i < buf.size(); ++i)
            buf[i] = ((buf[i] ^ 0xC3) + (session_key + 0x40));
        break;

    case 3:
        for (std::size_t i = 0; i < buf.size(); ++i)
            buf[i] = ((buf[i] ^ 0xC3) - (session_key + 0x40));
        break;

    default:
        for (std::size_t i = 0; i < buf.size(); ++i)
            buf[i] = buf[i] + 0x0F;
        break;
    }
}


