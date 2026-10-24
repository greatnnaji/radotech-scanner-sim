#include "scanplotdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>

ScanPlotDialog::ScanPlotDialog( const QString& userName,
                                const QString& scanName,
                                const QJsonObject& scanData,
                                const QMap<QString, int>& minThresholds,
                                const QMap<QString, int>& maxThresholds,
                                QWidget *parent)
    : QDialog(parent),
    customPlot(new QCustomPlot(this)),
    recommendationBox(new QTextEdit(this)),
    saveButton(new QPushButton("Save Recommendation", this)), // Initialize save button
    userName(userName),
    scanName(scanName),
    scanData(scanData),
    organMinThreshold(minThresholds),
    organMaxThreshold(maxThresholds)
{
    setWindowTitle(scanName); // Set dialog title to the scan name
    resize(800, 700);         // Adjust height to accommodate the save button

    // Add the custom plot, recommendation box, and save button to the layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Reduce the size of the plot area
    customPlot->setMinimumHeight(500);
    layout->addWidget(customPlot);

    // Set up the text box for recommendations
    recommendationBox->setPlaceholderText("Enter your recommendations here...");
    recommendationBox->setMinimumHeight(100);
    layout->addWidget(recommendationBox);

    // Add the save button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    layout->addLayout(buttonLayout);

    // Connect the save button to its slot
    connect(saveButton, &QPushButton::clicked, this, &ScanPlotDialog::saveRecommendation);

    setupGraph(scanData);
}

ScanPlotDialog::~ScanPlotDialog()
{
}

void ScanPlotDialog::setupGraph(const QJsonObject& scanData)
{
    QVector<double> x;               // x-axis indices for organs
    QVector<double> y;               // y-axis values (scan data)
    QVector<double> minThresholds;   // y-axis values for minimum thresholds
    QVector<double> maxThresholds;   // y-axis values for maximum thresholds
    QStringList organLabels = scanData.keys(); // Organ names for x-axis labels

    int index = 0;
    for (const QString& organ : organLabels) {
        x.append(index);
        y.append(scanData[organ].toDouble());
        minThresholds.append(organMinThreshold.value(organ, 0)); // Fetch minimum threshold
        maxThresholds.append(organMaxThreshold.value(organ, 160)); // Fetch maximum threshold
        index++;
    }

    // Main scan data graph
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    customPlot->graph(0)->setPen(QPen(Qt::blue, 2)); // Main graph in blue

    // Minimum threshold graph
    customPlot->addGraph();
    customPlot->graph(1)->setData(x, minThresholds);
    customPlot->graph(1)->setPen(QPen(Qt::red, 2, Qt::DashLine)); // Dashed red line

    // Maximum threshold graph
    customPlot->addGraph();
    customPlot->graph(2)->setData(x, maxThresholds);
    customPlot->graph(2)->setPen(QPen(Qt::green, 2, Qt::DashLine)); // Dashed green line

    // Set axis labels
    customPlot->xAxis->setLabel("Organs");
    customPlot->yAxis->setLabel("Values");
    customPlot->yAxis->setRange(0, 160); // Assuming max value is 160

    // Use QSharedPointer to set custom tick labels for the x-axis
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (int i = 0; i < organLabels.size(); ++i) {
        textTicker->addTick(i, organLabels[i]);
    }
    customPlot->xAxis->setTicker(textTicker);

    // Adjust x-axis range and appearance
    customPlot->xAxis->setRange(-0.5, organLabels.size() - 0.5); // Adjust range to fit all labels
    customPlot->xAxis->setTickLabelRotation(60);                // Rotate labels for visibility
    customPlot->xAxis->setSubTicks(false);                      // No subticks
    customPlot->xAxis->setTickLength(0, 4);                     // Adjust tick length
    customPlot->xAxis->setTickLabelFont(QFont("sans", 8));      // Set a smaller font size

    // Adjust plot margins
    customPlot->plotLayout()->setMargins(QMargins(10, 10, 10, 50)); // Larger bottom margin for labels

    // Replot the graph
    customPlot->replot();
}

void ScanPlotDialog::saveRecommendation()
{
    QString recommendation = recommendationBox->toPlainText();

    if (recommendation.isEmpty()) {
        QMessageBox::warning(this, "Empty Recommendation", "Please enter a recommendation before saving.");
        return;
    }

    // Add the recommendation to the scan data
    scanData["recommendation"] = recommendation;

    // Save the updated scan to the file
    saveScanToFile();

    QMessageBox::information(this, "Saved", "Recommendation saved successfully.");
}

void ScanPlotDialog::saveScanToFile()
{
    QString filePath = "users.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open users.json for reading.");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    file.close();

    if (!doc.isArray()) {
        QMessageBox::critical(this, "Error", "Invalid JSON format in users.json.");
        return;
    }

    QJsonArray usersArray = doc.array();
    bool userFound = false;

    for (int i = 0; i < usersArray.size(); ++i) {
        if (usersArray[i].isObject()) {
            QJsonObject userObj = usersArray[i].toObject();

            // Match the user by name
            if (userObj["name"].toString() == userName) {
                userFound = true;

                QJsonObject scans = userObj["scans"].toObject();
                QJsonObject scanObj = scans[scanName].toObject(); // Retrieve the scan object
                scanObj["recommendation"] = scanData["recommendation"]; // Modify the recommendation attribute
                scans[scanName] = scanObj; // Reassign the modified object back to the scans
                userObj["scans"] = scans;
                usersArray[i] = userObj;
                qInfo() << "recommendation actually got saved";
                break;
            }
        }
    }

    if (!userFound) {
        QMessageBox::critical(this, "Error", "User not found in users.json.");
        return;
    }

    // Write back the updated data
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Could not open users.json for writing.");
        return;
    }

    QJsonDocument updatedDoc(usersArray);
    file.write(updatedDoc.toJson());
    file.close();
}

void ScanPlotDialog::sendResult() {
    qInfo() << "displaying Results";
}
