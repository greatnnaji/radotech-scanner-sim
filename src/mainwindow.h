#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "user.h"
#include "createuserdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "qcustomplot.h"
#include "scanplotdialog.h"
#include <QTimer>
#include <QPrinter>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createUser();
    void deleteUser();
    void updateUser();
    User* findUser(QString userName);
    void onUserSelectionChanged(const QString& userName);
    void updateMetricsForCurrentUser();
    void createJsonFileIfNotExists(const QString& filepath);
    void loadUsersFromFile(const QString& filePath);
    void saveUsersToFile(const QString& filePath);
    void takeMeasurement();
    QVector<QString> getValues();
    void initializeScans();
    void displayScan();
    void initializeBatteryLevel();
    void onBatteryLevelChanged(int newValue);
    void updateBatteryBar(int batteryLevel);
    void depleteBattery();
    void indicateLowPower();
    void performGracefulShutdown();
    void setGuiEnabled(bool enabled);
    void initializeOrganThresholds(); // Method to populate the organ maps
    void initializeAllScansFile(const QString& userName, const QVector<QString>& userScans);
    void resetScansToOriginal(const QString& userName, const QVector<QString>& originalScans);
    //Helpers
    void powerButton();
    void pairSuccess();
    //void retryPairing();


private:
    Ui::MainWindow *ui;
    QVector<User*> users;
    User* currentUser;
    bool power;
    bool pairUp;
    QMap<QString, QVector<QString>> userMetrics;
    QTimer* batteryDepletionTimer; //Timer for charge depletion
    bool lowPowerWarningShown;
    QMap<QString, int> organMaxThreshold;
    QMap<QString, int> organMinThreshold;
    int numberOfDataPoints;

};
#endif // MAINWINDOW_H
