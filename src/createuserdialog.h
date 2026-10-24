#ifndef CREATEUSERDIALOG_H
#define CREATEUSERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>

class CreateUserDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateUserDialog(QWidget* parent = nullptr);

    QString getName() const;
    int getAge() const;
    double getHeight() const;
    double getWeight() const;

    // Setters for pre-filling user data
    void setName(const QString& name);
    void setAge(int age);
    void setHeight(double height);
    void setWeight(double weight);

private:
    QLineEdit* nameInput;
    QLineEdit* ageInput;
    QLineEdit* heightInput;
    QLineEdit* weightInput;
    QPushButton* saveButton;
};

#endif // CREATEUSERDIALOG_H
