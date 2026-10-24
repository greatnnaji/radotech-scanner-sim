#ifndef SCANPLOTDIALOG_H
#define SCANPLOTDIALOG_H

#include <QDialog>
#include <QMap>
#include <QJsonObject>
#include <QTextEdit>
#include <QPushButton> // Include QPushButton
#include "qcustomplot.h"

class ScanPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScanPlotDialog(const QString& userName,
                            const QString& scanName,
                            const QJsonObject& scanData,
                            const QMap<QString, int>& minThreshold,
                            const QMap<QString, int>& maxThreshold,
                            QWidget *parent = nullptr);

    ~ScanPlotDialog();
    //Helper
    void sendResult();

private slots:
    void saveRecommendation(); // Slot to handle the save button

private:
    QCustomPlot *customPlot;                   // Custom plot for displaying graph
    QTextEdit *recommendationBox;             // Text box for user recommendations
    QPushButton *saveButton;                  // Button to save recommendations
    QString userName;                          // User's name
    QString scanName;                          // Current scan name
    QJsonObject scanData;                      // Current scan data
    QMap<QString, int> organMinThreshold;     // Minimum thresholds for organs
    QMap<QString, int> organMaxThreshold;     // Maximum thresholds for organs

    void setupGraph(const QJsonObject& scanData); // Function to set up the graph
    void saveScanToFile();                       // Helper to save scan to file
};

#endif // SCANPLOTDIALOG_H
