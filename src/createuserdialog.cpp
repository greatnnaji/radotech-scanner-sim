#include "createuserdialog.h"
#include <QFormLayout>
#include <QIntValidator>
#include <QDoubleValidator>

CreateUserDialog::CreateUserDialog(QWidget* parent) : QDialog(parent) {
    QFormLayout* layout = new QFormLayout(this);

    nameInput = new QLineEdit(this);
    ageInput = new QLineEdit(this);
    heightInput = new QLineEdit(this);
    weightInput = new QLineEdit(this);

    // Set validators to ensure valid input
    ageInput->setValidator(new QIntValidator(0, 100, this)); // Valid age range
    heightInput->setValidator(new QDoubleValidator(0, 300, 2, this)); // Valid height range
    weightInput->setValidator(new QDoubleValidator(0, 500, 2, this)); // Valid weight range

    saveButton = new QPushButton("Save", this);

    layout->addRow("Name:", nameInput);
    layout->addRow("Age:", ageInput);
    layout->addRow("Height (cm):", heightInput);
    layout->addRow("Weight (kg):", weightInput);
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept); // Close the dialog on save
}

QString CreateUserDialog::getName() const {
    return nameInput->text();
}

int CreateUserDialog::getAge() const {
    return ageInput->text().toInt();
}

double CreateUserDialog::getHeight() const {
    return heightInput->text().toDouble();
}

double CreateUserDialog::getWeight() const {
    return weightInput->text().toDouble();
}

// Setters
void CreateUserDialog::setName(const QString& name) {
    nameInput->setText(name);
}

void CreateUserDialog::setAge(int age) {
    ageInput->setText(QString::number(age));
}

void CreateUserDialog::setHeight(double height) {
    heightInput->setText(QString::number(height));
}

void CreateUserDialog::setWeight(double weight) {
    weightInput->setText(QString::number(weight));
}
