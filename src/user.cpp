#include "user.h"

// Constructor
User::User(const QString& name, int age, double height, double weight, int numberOfScans)
    : name(name), age(age), height(height), weight(weight), numberOfScans(numberOfScans) {}

// Getters
QString User::getName() const {
    return name;
}

int User::getAge() const {
    return age;
}

double User::getHeight() const {
    return height;
}

double User::getWeight() const {
    return weight;
}

int User::getNumberOfScans() const {
    return numberOfScans;
}

QJsonObject User::getScan(const QString& scanName) const {
    return scans.value(scanName, QJsonObject());
}

QMap<QString, QJsonObject> User::getScans() const { return scans; }

// Setters
void User::setName(const QString& name) {
    this->name = name;
}

void User::setAge(int age) {
    this->age = age;
}

void User::setHeight(double height) {
    this->height = height;
}

void User::setWeight(double weight) {
    this->weight = weight;
}

void User::setNumberOfScans(int numScans) {
    this->numberOfScans = numScans;
}

void User::setScans(const QMap<QString, QJsonObject>& newScans) {
    this->scans = newScans;
    this->numberOfScans = newScans.size();
}

//Helpers
void User::powerOn() {
    qInfo() << "Power is On";
}

void User::startPair() {
    qInfo() << "Pairing initiated";
}

void User::powerOff() {
    qInfo() << "Power is Off";
}
