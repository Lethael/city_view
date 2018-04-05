/*  Par Benjamin Desfontaines
 *  Le 22/03/2018
 *  Projet visant à réaliser un programme de gestion
 *  simplifiée des villes de France.
 *      - Insérer des villes dans la BDD
 *          - Via un formulaire codé à la main
 *      - Supprimer des villes de la BDD
 *      - Modifier le nom d'une ville
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QtSql>

#include "connection.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStringList getRegion(QString &nomRegion);

public slots:

private slots:
    void on_action_Quitter_triggered();

    /**
     * @brief MainWindow::on_modify_button_clicked
     *
     * Catch the user click's
     * and catch city's id
     * for to update city (only city's name)
     */
    void on_modify_button_clicked();

    /**
     * @brief MainWindow::on_lineEdit_textChanged
     * @param saisie
     *
     * catch input's user on line edit search
     * and print in on live in table view with
     * number of cities
     */
    void on_lineEdit_textChanged(const QString &saisie);

    /**
     * @brief MainWindow::on_insert_button_clicked
     * Create form and insert department in
     * the combo box
     */
    void on_insert_button_clicked();

    /**
     * @brief MainWindow::closeForm
     * Delete insertion form
     */
    void closeForm();

    /**
     * @brief MainWindow::insertInto
     * query for to insert new city
     */
    void insertInto();

    /**
     * @brief MainWindow::on_delete_button_clicked
     *
     * Catch city's id
     */
    void on_delete_button_clicked();

private:
    Ui::MainWindow *ui;
    Connection *cnx;
    QSqlQueryModel *model;
    /*  Form  */

    QFormLayout *form;
    QHBoxLayout *buttons_vc;
    QVBoxLayout *formulaire;

    QLineEdit *ville_line;
    QLineEdit *cp_line;
    QLineEdit *lat_line;
    QLineEdit *lon_line;
    QComboBox *departement_cbbox;

    QPushButton *valider;
    QPushButton *cancel;

    QGroupBox *formulaireBox;
    /*  END */

    QString driver;
    QString database;
    QString hostname;
    QString username;
    QString password;

    /**
     * @brief MainWindow::checkDB
     * @return
     */
    bool checkDB();

    /**
     * @brief MainWindow::afficherVilles
     * Print cities by names Order on
     * table view
     */
    void afficherVilles();

    /**
     * @brief MainWindow::updateCity
     * @param id_city
     * @param city
     *
     * query for to change city's name
     * by city's id
     */
    void updateCity(QString &id_city, QString &city);

    /**
     * @brief MainWindow::deleteCity
     * @param id_city
     *
     * query for to delete city
     */
    void deleteCity(QString &id_city);

    /**
     * @brief MainWindow::activeSelection
     * @return
     *
     * Return the user's click on table view
     */

    QVariant activeSelection();
    /**
     * @brief MainWindow::createForm
     * Create form for to insert new city
     */
    void createForm();
};

#endif // MAINWINDOW_H
