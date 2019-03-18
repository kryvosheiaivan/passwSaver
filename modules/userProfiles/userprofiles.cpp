#include "userprofiles.h"
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>
#include <QCoreApplication>


userProfiles::userProfiles(QObject *parent) : QObject(parent)
{

}

/*
 * Adding user profile to file
 * returns true if success
*/
bool userProfiles::addUserProfile(QString username, QString pswd, QWidget *parent)
{
    if(username.length() < MIN_STRING_LEN) {
        QMessageBox::information(parent, "Incorrect value", "Too short username");
        return false;
    }
    else if(pswd.length() < MIN_STRING_LEN) {
        QMessageBox::information(parent, "Incorrect value", "Too short password");
        return false;
    }

    /* Open file for writing */
    QString appDir = QCoreApplication::applicationDirPath();
    QFile file(appDir + "/users.json");

    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "addUserProfile: failed to open file" << endl;
        return false;
    }
    QString strFile = file.readAll();

    //first need to read from file (in order to know what was written previously)
    QJsonDocument jsReadDoc;
    jsReadDoc = QJsonDocument::fromJson(strFile.toUtf8());
    file.close();


    if (jsReadDoc.isEmpty()) {
        qDebug() << "doc is empty" << endl;
    }
    else {
        qDebug() << "doc is not an empty" << endl;
    }

    QByteArray byteArray = jsReadDoc.toJson();
    qDebug() << byteArray << endl;

    QJsonObject jsObject;
    jsObject["username"] = username;
    jsObject["password"] = pswd;

    QJsonValue jsValue(jsObject);

    QJsonArray jsArray;

    if ( jsReadDoc.isArray()) {
        qDebug() << "doc is array" << endl;
        jsArray = jsReadDoc.array();        // copying old array
    }

    //need to know whether that username was not already registered
    if ( isUsernameExist(username) )
    {
        QMessageBox::information(parent, "Registration", "That user was already registered!\n"
                                                         "Please choose another username..."
                                                         );
        return false;
    }

    jsArray.append(jsValue);

    //qDebug() << "freeing jsReadDoc" << endl;
    //~jsReadDoc();
    jsReadDoc.setArray(jsArray);


    if ( !file.open(QIODevice::WriteOnly)) {
        qDebug() << "addUserProfile: failed to open file 2" << endl;
        return false;
    }
    file.write(jsReadDoc.toJson());
    file.close();

    return true;
}

bool userProfiles::isUsernameExist(QString username)
{
    /* Open file for reading */
    QString appDir = QCoreApplication::applicationDirPath();
    QFile file(appDir + "/users.json");

    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "isUsernameExist: failed to open file" << endl;
        return false;
    }

    QString strFile = file.readAll();
    QJsonDocument jsReadDoc;
    jsReadDoc = QJsonDocument::fromJson(strFile.toUtf8());
    file.close();

    QJsonArray jsArray = jsReadDoc.array();
    QJsonValue jsVal;
    QJsonObject jsObj;
    QJsonArray::iterator it;
    int i = 0;
    for( it = jsArray.begin(); it != jsArray.end(); it++)
    {
       jsVal = jsArray.at(i);
       jsObj = jsVal.toObject();
       if ( jsObj.contains("username"))
       {
        //qDebug() << "[" << i << "]" << "Ok!" << endl;
        jsVal = jsObj.value("username");
        if ( jsVal.toString() == username)
        {
            qDebug() << "current user was previously registered!" << endl;
            /* If we here then current user was previously registered! */
            return true;
        }
       }
       i++;
    }

    /* current username is unique */
    return false;
}

















