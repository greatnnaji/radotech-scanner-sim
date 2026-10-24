#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

class User {
public:
    // Constructor
    User(const QString& name, int age, double height, double weight, int numberOfScans);

    // Getters
    QString getName() const;
    int getAge() const;
    double getHeight() const;
    double getWeight() const;
    int getNumberOfScans() const;
    QMap<QString, QJsonObject> getScans() const;
    QJsonObject getScan(const QString& scanName) const;

    // Setters
    void setName(const QString& name);
    void setAge(int age);
    void setHeight(double height);
    void setWeight(double weight);
    void setNumberOfScans(int numScans);
    void setScans(const QMap<QString, QJsonObject>& newScans);

    //Helpers
    void powerOn();
    void startPair();
    void powerOff();

private:
    QString name;
    int age;
    double height; // in cm
    double weight; // in kg
    int numberOfScans;
    QMap<QString, QJsonObject> scans; // Stores scan name -> scan data

};

#endif // USER_H

