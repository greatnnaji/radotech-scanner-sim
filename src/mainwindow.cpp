#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    power =false;

    ui->setupUi(this);
    connect(ui->createUserButton, &QPushButton::released, this, &MainWindow::createUser);
    connect(ui->updateUserButton, &QPushButton::released, this, &MainWindow::updateUser);
    connect(ui->deleteUserButton, &QPushButton::released, this, &MainWindow::deleteUser);
    connect(ui->powerButton, &QPushButton::released, this, &MainWindow::powerButton);
    connect(ui->pairUpButton, &QPushButton::released, this, &MainWindow::pairSuccess);
    connect(ui->takeMeasurementButton, &QPushButton::released, this, &MainWindow::takeMeasurement);
    connect(ui->userHistoryComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onUserSelectionChanged);
    connect(ui->selectScanButton, &QPushButton::released, this, &MainWindow::displayScan);

    setGuiEnabled(false);
    // QFile file("users.json");
    // file.remove();

    initializeBatteryLevel();
    lowPowerWarningShown = false;
    // Initialize battery depletion timer
    batteryDepletionTimer = new QTimer(this);
    connect(batteryDepletionTimer, &QTimer::timeout, this, &MainWindow::depleteBattery);
    batteryDepletionTimer->start(10000); // Deplete every 10 seconds
}

MainWindow::~MainWindow()
{
    delete batteryDepletionTimer;
    delete ui;
}

void MainWindow::pairSuccess() {
    qInfo() << "Pairing sucessful";
    initializeOrganThresholds();
    createJsonFileIfNotExists("users.json");
    // Initialize the organ threshold maps
    loadUsersFromFile("users.json");

    numberOfDataPoints = 24;

    //ui->metricsComboBox->addItem("View Metrics");

    for(int i=0; i<users.size();i++){
        ui->userComboBox->addItem(users.at(i)->getName());
        ui->userHistoryComboBox->addItem(users.at(i)->getName());
    }
}

void MainWindow::powerButton(){
    if(power){
        qInfo() << "Powering Off";
        power = false;
        setGuiEnabled(power);
    }
    else{
        qInfo() << "Powering On";
        power = true;
        setGuiEnabled(power);
    }
}

void MainWindow::createJsonFileIfNotExists(const QString& filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        // Create a new JSON file with default data
        QJsonArray defaultUsers;

        // Create scan data for user 1
        QJsonObject user1Scans;
        QJsonObject user1Scan1;
        QJsonObject user1Scan2;

        // Generate random values for organs for Scan 1
        for (auto it = organMinThreshold.cbegin(); it != organMinThreshold.cend(); ++it) {
            const QString& organ = it.key();
            int randomValue = QRandomGenerator::global()->bounded(161); // Random between 0-160
            user1Scan1.insert(organ, randomValue);
        }
        user1Scan1.insert("recommendation", ""); // Add empty recommendation field

        // Generate random values for organs for Scan 2
        for (auto it = organMinThreshold.cbegin(); it != organMinThreshold.cend(); ++it) {
            const QString& organ = it.key();
            int randomValue = QRandomGenerator::global()->bounded(161); // Random between 0-160
            user1Scan2.insert(organ, randomValue);
        }

        user1Scan2.insert("recommendation", ""); // Add empty recommendation field

        // Add scans to the user 1 scans object
        user1Scans.insert("Scan 1", user1Scan1);
        user1Scans.insert("Scan 2", user1Scan2);

        // Create user 1 object with scans
        QJsonObject user1 {
            {"name", "John Doe"},
            {"age", 30},
            {"height", 5.9},
            {"weight", 70.5},
            {"scans", user1Scans}
        };

        // Create scan data for user 2
        QJsonObject user2Scans;
        QJsonObject user2Scan1;
        QJsonObject user2Scan2;
        QJsonObject user2Scan3;

        // Generate random values for organs for Scan 1
        for (auto it = organMinThreshold.cbegin(); it != organMinThreshold.cend(); ++it) {
            const QString& organ = it.key();
            int randomValue = QRandomGenerator::global()->bounded(161); // Random between 0-160
            user2Scan1.insert(organ, randomValue);
        }
        user2Scan1.insert("recommendation", ""); // Add empty recommendation field

        // Generate random values for organs for Scan 2
        for (auto it = organMinThreshold.cbegin(); it != organMinThreshold.cend(); ++it) {
            const QString& organ = it.key();
            int randomValue = QRandomGenerator::global()->bounded(161); // Random between 0-160
            user2Scan2.insert(organ, randomValue);
        }
        user2Scan2.insert("recommendation", ""); // Add empty recommendation field

        // Generate random values for organs for Scan 3
        for (auto it = organMinThreshold.cbegin(); it != organMinThreshold.cend(); ++it) {
            const QString& organ = it.key();
            int randomValue = QRandomGenerator::global()->bounded(161); // Random between 0-160
            user2Scan3.insert(organ, randomValue);
        }
        user2Scan3.insert("recommendation", ""); // Add empty recommendation field

        // Add scans to the user 2 scans object
        user2Scans.insert("Scan 1", user2Scan1);
        user2Scans.insert("Scan 2", user2Scan2);
        user2Scans.insert("Scan 3", user2Scan3);

        // Create user 2 object with scans
        QJsonObject user2 {
            {"name", "Jane Smith"},
            {"age", 25},
            {"height", 5.5},
            {"weight", 60.0},
            {"scans", user2Scans}
        };

        // Append users to the default users array
        defaultUsers.append(user1);
        defaultUsers.append(user2);

        // Write JSON to file
        QJsonDocument doc(defaultUsers);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
            qDebug() << "Created default users.json file.";
        } else {
            qWarning() << "Could not create the JSON file.";
        }
    }
}

void MainWindow::loadUsersFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning() << "Invalid JSON format: expected an array.";
        return;
    }

    QJsonArray userArray = doc.array();
    users.clear();

    for (const QJsonValue& value : userArray) {
        QJsonObject obj = value.toObject();
        QString name = obj["name"].toString();
        int age = obj["age"].toInt();
        double height = obj["height"].toDouble();
        double weight = obj["weight"].toDouble();

        int numberOfScans = 0;
        QMap<QString, QJsonObject> scans;

        // Load scans
        if (obj.contains("scans") && obj["scans"].isObject()) {
            QJsonObject scansObj = obj["scans"].toObject();
            for (auto it = scansObj.begin(); it != scansObj.end(); ++it) {
                scans.insert(it.key(), it.value().toObject());
            }
            numberOfScans = scans.size();
        }

        User* newUser = new User(name, age, height, weight, numberOfScans);
        newUser->setScans(scans);
        users.append(newUser);
    }

    qDebug() << "Loaded" << users.size() << "users from file.";
}


void MainWindow::saveUsersToFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return;
    }

    QJsonArray userArray;
    for (const User* user : users) {
        QJsonObject obj;
        obj["name"] = user->getName();
        obj["age"] = user->getAge();
        obj["height"] = user->getHeight();
        obj["weight"] = user->getWeight();
        obj["numberOfScans"] = user->getNumberOfScans();

        // Add scans
        QJsonObject scansObj;
        for (auto it = user->getScans().cbegin(); it != user->getScans().cend(); ++it) {
            scansObj.insert(it.key(), it.value());
        }
        obj["scans"] = scansObj;

        userArray.append(obj);
    }

    QJsonDocument doc(userArray);
    file.write(doc.toJson());
    file.close();

    qDebug() << "Saved" << users.size() << "users to file.";
}

void MainWindow::displayScan() {
    QString selectedUser = ui->userHistoryComboBox->currentText();
    QString selectedScan = ui->metricsComboBox->currentText();

    if (selectedUser.isEmpty() || selectedScan.isEmpty() || selectedScan == "View Metrics") {
        QMessageBox::warning(this, "No Selection", "Please select a user and scan.");
        return;
    }

    // Access the centralized users.json file
    QString filePath = "users.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open the users.json file.");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray()) {
        QMessageBox::critical(this, "Error", "Invalid JSON format in users.json.");
        file.close();
        return;
    }

    QJsonArray usersArray = doc.array();
    file.close();

    // Find the selected user's data
    QJsonObject userData;
    bool userFound = false;

    for (const QJsonValue& value : usersArray) {
        if (value.isObject()) {
            QJsonObject userObj = value.toObject();
            if (userObj["name"].toString() == selectedUser) {
                userData = userObj;
                userFound = true;
                break;
            }
        }
    }

    if (!userFound) {
        QMessageBox::critical(this, "Error", "The selected user does not exist in the users.json file.");
        return;
    }

    // Check if the scans object exists
    if (!userData.contains("scans") || !userData["scans"].isObject()) {
        QMessageBox::critical(this, "Error", "No scans found for the selected user.");
        return;
    }

    QJsonObject scans = userData["scans"].toObject();

    // Check if the selected scan exists
    if (!scans.contains(selectedScan)) {
        QMessageBox::critical(this, "Error", "The selected scan does not exist for this user.");
        return;
    }

    // Extract the scan data
    QJsonObject scanData = scans[selectedScan].toObject();

    if (!scanData.contains("recommendation")) {
        scanData.insert("recommendation", ""); // Add recommendation attribute if missing
    }

    // Open the ScanPlotDialog
    qInfo() << "Creating graph for" << selectedScan;
    ScanPlotDialog *dialog = new ScanPlotDialog(selectedUser, selectedScan, scanData, organMinThreshold, organMaxThreshold, this);
    dialog->exec();
    dialog->sendResult();
}


void MainWindow::createUser() {
    CreateUserDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Retrieve user details from dialog
        QString name = dialog.getName();
        int age = dialog.getAge();
        double height = dialog.getHeight();
        double weight = dialog.getWeight();

        // Validate the input parameters
        if (name.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Name cannot be empty. Please enter a valid name.");
            return; // Exit the function
        }
        if (age <= 0) {
            QMessageBox::warning(this, "Invalid Input", "Age must be a positive integer.");
            return; // Exit the function
        }
        if (height <= 0) {
            QMessageBox::warning(this, "Invalid Input", "Height must be a positive value.");
            return; // Exit the function
        }
        if (weight <= 0) {
            QMessageBox::warning(this, "Invalid Input", "Weight must be a positive value.");
            return; // Exit the function
        }

        int numberOfScans = 0;
        // Add the new user to the list if all validations pass
        users.append(new User(name, age, height, weight, numberOfScans));
        saveUsersToFile("users.json");
        ui->userComboBox->addItem(name);
        ui->userHistoryComboBox->addItem(name);
    }
}

void MainWindow::deleteUser(){
    if(users.isEmpty()){
        return;
    }
    int index = ui->userComboBox->currentIndex();
    User* user = findUser(ui->userComboBox->currentText());
    if(user == nullptr){
        qInfo() << "Did not find user";
        return;
    }
    qInfo() << index;
    delete user;
    users.remove(index);
    saveUsersToFile("users.json");
    ui->userComboBox->removeItem(index);
    ui->userHistoryComboBox->removeItem(index);
}

void MainWindow::updateUser(){
    QString userName = ui->userComboBox->currentText();
    if(users.size()==0){
        return;
    }
    User* user = findUser(userName);
    if(user == nullptr){
        qInfo() << "Did not find user";
        return;
    }
    CreateUserDialog dialog(this);
    // Pre-fill dialog fields with current user information
        dialog.setName(user->getName());
        dialog.setAge(user->getAge());
        dialog.setHeight(user->getHeight());
        dialog.setWeight(user->getWeight());

        if (dialog.exec() == QDialog::Accepted) {
            // Retrieve user details from dialog
            QString name = dialog.getName();
            int age = dialog.getAge();
            double height = dialog.getHeight();
            double weight = dialog.getWeight();

            // Validate the input and apply changes only if needed
            bool userUpdated = false;

            if (!name.trimmed().isEmpty() && name != user->getName()) {
                user->setName(name);
                int index = ui->userComboBox->currentIndex();
                ui->userComboBox->setItemText(index, name);
                ui->userHistoryComboBox->setItemText(index,name);
                userUpdated = true;
            }

            if (age > 0 && age != user->getAge()) {
                user->setAge(age);
                userUpdated = true;
            }

            if (height > 0 && height != user->getHeight()) {
                user->setHeight(height);
                userUpdated = true;
            }

            if (weight > 0 && weight != user->getWeight()) {
                user->setWeight(weight);
                userUpdated = true;
            }

            if (userUpdated) {
                saveUsersToFile("users.json");
                QMessageBox::information(this, "User Updated", "User details have been successfully updated.");
            } else {
                QMessageBox::information(this, "No Changes", "No changes were made to the user.");
            }
        } else {
            qInfo() << "Update dialog canceled.";
        }
}

User* MainWindow::findUser(QString userName){
    for(int i=0; i< users.size();i++){
        if(users.at(i)->getName() == userName)
        {
            return users.at(i);
        }
    }
    return nullptr;
}

void MainWindow::onUserSelectionChanged(const QString& userName) {
    currentUser = findUser(userName);
    // Update metrics if a valid user is selected
    if (currentUser) {
        updateMetricsForCurrentUser();
    } else {
        // Clear metrics if no valid user is found
        ui->metricsComboBox->clear();
        ui->metricsComboBox->addItem("No metrics available");
    }
}

void MainWindow::updateMetricsForCurrentUser() {
    if (!currentUser) {
        return;
    }

    ui->metricsComboBox->clear(); // Clear existing metrics

    // Access the centralized users.json file
    QString filePath = "users.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << filePath;
        return;
    }

    // Read and parse the JSON document
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray()) {
        qWarning() << "Invalid JSON format: expected an array.";
        file.close();
        return;
    }

    QJsonArray usersArray = doc.array();
    file.close();

    // Find the current user's data
    const QString& userName = currentUser->getName();
    QJsonObject userData;
    bool userFound = false;

    for (const QJsonValue& value : usersArray) {
        if (value.isObject()) {
            QJsonObject userObj = value.toObject();
            if (userObj["name"].toString() == userName) {
                userData = userObj;
                userFound = true;
                break;
            }
        }
    }

    if (!userFound) {
        qWarning() << "User not found in file:" << userName;
        return;
    }

    // Check if the scans object exists
    if (!userData.contains("scans") || !userData["scans"].isObject()) {
        qWarning() << "No scans found for user:" << userName;
        return;
    }

    QJsonObject scans = userData["scans"].toObject();

    // Update the number of scans
    currentUser->setNumberOfScans(scans.size());

    // Populate the metrics combo box with scan names
    for (const QString& scanName : scans.keys()) {
        ui->metricsComboBox->addItem(scanName);
    }

    //qInfo() << "Updated metrics for user:" << userName;
}

void MainWindow::takeMeasurement() {
    if (!currentUser) {
        qWarning() << "No current user selected.";
        return;
    }

    // Static map to track each user's current measurement counter
    static QMap<QString, int> userMeasurementCounters;
    static QMap<QString, QJsonObject> currentMeasurements; // Temporary storage for ongoing scans

    QString userName = currentUser->getName();

    // Initialize the counter and current measurement object for the user if not already done
    if (!userMeasurementCounters.contains(userName)) {
        userMeasurementCounters[userName] = 0;
        currentMeasurements[userName] = QJsonObject();
    }

    // Get the current counter for this user
    int& counter = userMeasurementCounters[userName];
    QJsonObject& currentScan = currentMeasurements[userName];

    // Generate random data for the next organ
    auto organIt = organMinThreshold.cbegin();
    std::advance(organIt, counter % organMinThreshold.size()); // Cycle through organs
    const QString& organ = organIt.key();
    int randomValue = QRandomGenerator::global()->bounded(161); // Random between 0-160
    currentScan.insert(organ, randomValue);

    counter++;

    qInfo() << "User:" << userName << "Measurement #" << counter << "for organ:" << organ << "Value:" << randomValue;

    // Check if the counter reached the specified number of points (a full scan)
    if (counter == numberOfDataPoints) {
        // Determine the new scan number
        int newScanNum = currentUser->getNumberOfScans() + 1;
        QString newScan = "Scan " + QString::number(newScanNum);

        // Add a recommendation attribute
        currentScan.insert("recommendation", ""); // Initialize with an empty string

        // Access the centralized users.json file
        QString filePath = "users.json";
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open file:" << filePath;
            return;
        }

        // Read and parse the JSON document
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) {
            qWarning() << "Invalid JSON format in users.json.";
            file.close();
            return;
        }

        QJsonArray usersArray = doc.array();
        file.close();

        // Find and update the current user's data
        bool userFound = false;
        for (int i = 0; i < usersArray.size(); ++i) {
            if (usersArray[i].isObject()) {
                QJsonObject userObj = usersArray[i].toObject();
                if (userObj["name"].toString() == userName) {
                    userFound = true;

                    // Ensure the scans object exists
                    QJsonObject scans = userObj["scans"].toObject();

                    // Add the completed scan
                    scans.insert(newScan, currentScan);
                    userObj["scans"] = scans;

                    // Update the user data in the array
                    usersArray[i] = userObj;
                    break;
                }
            }
        }

        if (!userFound) {
            qWarning() << "User not found in file:" << userName;
            return;
        }

        // Save the updated data back to the JSON file
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Could not open file for writing:" << filePath;
            return;
        }

        QJsonDocument updatedDoc(usersArray);
        file.write(updatedDoc.toJson());
        file.close();

        qInfo() << "Saved new scan:" << newScan << "for user:" << userName;

        // Update the user's scan count and reset the counter
        currentUser->setNumberOfScans(newScanNum);
        counter = 0; // Reset counter
        currentScan = QJsonObject(); // Clear current scan
        updateMetricsForCurrentUser();
    }
}



void MainWindow::initializeBatteryLevel() {
    ui->batteryLevelSpinBox->setRange(0, 100);    // Set range from 0 to 100
    ui->batteryLevelSpinBox->setValue(100);        // Set initial battery level to 75%
    ui->batteryLevelSpinBox->setSingleStep(1);    // Step size of 1%
    ui->batteryLevelProgressBar->setValue(100);

    // Connect the valueChanged signal to update the battery level in your logic
    connect(ui->batteryLevelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onBatteryLevelChanged);
}

void MainWindow::onBatteryLevelChanged(int newValue) {
    // Handle the battery level change

    updateBatteryBar(newValue); // update the progress bar

    if(newValue > 0 && power){
        setGuiEnabled(true);// Re-enable the GUI if the battery is charged
    }

    if (newValue > 10) {
           lowPowerWarningShown = false; // Reset the warning flag if battery level is sufficient
       }

    if (newValue <= 10 && !lowPowerWarningShown) {
       lowPowerWarningShown = true;
       indicateLowPower();
    }

    if (newValue == 0) {
       performGracefulShutdown();
    }
}

void MainWindow::updateBatteryBar(int batteryLevel) {
    //Update QProgressBar
    if(power){
        ui->batteryLevelProgressBar->setValue(batteryLevel);
    }
}

void MainWindow::depleteBattery() {
    int currentBatteryLevel = ui->batteryLevelSpinBox->value();
    if(power){
        if (currentBatteryLevel > 0) {
            ui->batteryLevelSpinBox->setValue(currentBatteryLevel - 1); // Decrease by 1%
        }
        currentBatteryLevel = ui->batteryLevelSpinBox->value();
        if(currentBatteryLevel <= 10 && !lowPowerWarningShown) {
            lowPowerWarningShown = true;
            indicateLowPower();
        }
        if(currentBatteryLevel == 0){
            performGracefulShutdown();
        }
    }
}

void MainWindow::indicateLowPower(){
    QMessageBox::warning(this, "Low Battery",
               "Battery level is critically low. Please recharge to avoid shutdown.");
}

void MainWindow::performGracefulShutdown() {
    QMessageBox::critical(this, "Shutdown", "Battery depleted. The application is now in shutdown mode. Please recharge to resume.");
    setGuiEnabled(false); // Grey out the GUI
}

void MainWindow::setGuiEnabled(bool enabled) {
    // Disable or enable specific widgets in the UI, excluding the batteryLevelSpinBox
    ui->createUserButton->setEnabled(enabled);
    ui->updateUserButton->setEnabled(enabled);
    ui->deleteUserButton->setEnabled(enabled);
    ui->takeMeasurementButton->setEnabled(enabled);
    ui->userComboBox->setEnabled(enabled);
    ui->userHistoryComboBox->setEnabled(enabled);
    ui->metricsComboBox->setEnabled(enabled);
    ui->selectScanButton->setEnabled(enabled);
    ui->pairUpButton->setEnabled(enabled);

    // Always enable the batteryLevelSpinBox
    ui->batteryLevelSpinBox->setEnabled(true);
    ui->powerButton->setEnabled(true);
}

void MainWindow::initializeOrganThresholds() {
    // Initialize maximum thresholds
    organMaxThreshold.insert("Heart", 160);
    organMaxThreshold.insert("Lungs", 150);
    organMaxThreshold.insert("Liver", 145);
    organMaxThreshold.insert("Kidneys", 140);
    organMaxThreshold.insert("Stomach", 135);
    organMaxThreshold.insert("Pancreas", 130);
    organMaxThreshold.insert("Intestines", 125);
    organMaxThreshold.insert("Brain", 160);
    organMaxThreshold.insert("Eyes", 120);
    organMaxThreshold.insert("Ears", 110);
    organMaxThreshold.insert("Skin", 100);
    organMaxThreshold.insert("Bones", 105);
    organMaxThreshold.insert("Muscles", 115);
    organMaxThreshold.insert("Thyroid", 125);
    organMaxThreshold.insert("Adrenal Glands", 130);
    organMaxThreshold.insert("Bladder", 140);
    organMaxThreshold.insert("Spleen", 135);
    organMaxThreshold.insert("Gallbladder", 130);
    organMaxThreshold.insert("Esophagus", 120);
    organMaxThreshold.insert("Lymph Nodes", 140);
    organMaxThreshold.insert("Prostate", 130);
    organMaxThreshold.insert("Uterus", 130);
    organMaxThreshold.insert("Nerves", 160);
    organMaxThreshold.insert("Arteries", 145);

    // Initialize minimum thresholds
    organMinThreshold.insert("Heart", 80);
    organMinThreshold.insert("Lungs", 75);
    organMinThreshold.insert("Liver", 70);
    organMinThreshold.insert("Kidneys", 65);
    organMinThreshold.insert("Stomach", 60);
    organMinThreshold.insert("Pancreas", 55);
    organMinThreshold.insert("Intestines", 50);
    organMinThreshold.insert("Brain", 80);
    organMinThreshold.insert("Eyes", 40);
    organMinThreshold.insert("Ears", 45);
    organMinThreshold.insert("Skin", 50);
    organMinThreshold.insert("Bones", 55);
    organMinThreshold.insert("Muscles", 60);
    organMinThreshold.insert("Thyroid", 65);
    organMinThreshold.insert("Adrenal Glands", 70);
    organMinThreshold.insert("Bladder", 75);
    organMinThreshold.insert("Spleen", 60);
    organMinThreshold.insert("Gallbladder", 55);
    organMinThreshold.insert("Esophagus", 50);
    organMinThreshold.insert("Lymph Nodes", 65);
    organMinThreshold.insert("Prostate", 55);
    organMinThreshold.insert("Uterus", 60);
    organMinThreshold.insert("Nerves", 80);
    organMinThreshold.insert("Arteries", 70);
}
