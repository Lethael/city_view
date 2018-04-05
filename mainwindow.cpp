#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cnx = new Connection(this);
    setWindowTitle("Cities View");

    driver = "QMYSQL";
    database = "localisation";
    hostname = "localhost";
    username = "root";
    password = "1234512345";


    if(checkDB())
    {
        model = new QSqlQueryModel;
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        afficherVilles();
        ui->statusBar->showMessage("Connected");
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::createForm
 * Create form for to insert new city
 */
void MainWindow::createForm()
{
    /***************************  Formulaire  ***************************/

    /*  Layouts */
    buttons_vc = new QHBoxLayout;
    formulaire = new QVBoxLayout;
    /*  END Layout   */

    /*  Init pushbutton   */
    valider = new QPushButton("Valider");
    cancel = new QPushButton("Cancel");
    /*  END */

    /*  Init Fields Edit + GroupBox  */
    ville_line = new QLineEdit;
    cp_line = new QLineEdit;
    lat_line = new QLineEdit;
    lon_line = new QLineEdit;
    departement_cbbox= new QComboBox;
    formulaireBox = new QGroupBox;

    /*  END */

    /*  Form Layout   */
    form = new QFormLayout;
    form->addRow("Ville", ville_line);
    form->addRow("Code Postal", cp_line);
    form->addRow("Latitude", lat_line);
    form->addRow("Longitude", lon_line);
    form->addRow("Département", departement_cbbox);

    buttons_vc->addWidget(valider);
    buttons_vc->addWidget(cancel);
    /*  End Form Layout    */


    /*  Vertical Layout */
    formulaire->addLayout(form);
    formulaire->addLayout(buttons_vc);
    /*  End Vertical Layout */

    /*  GroupBox */
    formulaireBox->setLayout(formulaire);
    formulaireBox->setStyleSheet("&Saisie Ville");

    /***************************  END Formulaire  ***************************/
}


/**
 * @brief MainWindow::checkDB
 * @return
 */
bool MainWindow::checkDB()
{

    if(!cnx->getDrivers().contains("QMYSQL")) {
       QMessageBox::critical(this, "Chargement base impossible",
                                        "<font color=red>Cette application nécessite le driver QMySQL</font>");
       return false;
    }

    if (!cnx->setConnection(driver, database, hostname, username, password))
    {
       QMessageBox::critical(this, "Impossible de se connecter a la base de donnees.",
                                   "<font color=red>Le serveur n'est pas disponible, "
                                   "ou vous avez renseigné de mauvais parametres de connexion.</font>");
       return false;
    }

    if(!cnx->allowTransaction())
    {
        QMessageBox::critical(this, "La base de données ne supporte pas le mode transactionnel.",
                                    "<font color=yellow>Les fonctionnalités inhérentes à l'execution de cette application ne sont pas disponibles."
                                    "\nVeuillez contacter votre administrateur réseau.</font>");
        return false;
    }
    return true;
}

/**
 * @brief MainWindow::afficherVilles
 * Print cities by names Order on
 * table view
 */
void MainWindow::afficherVilles()
{
    cnx->setAutocommit();
    try {
        model->setQuery("SELECT V.nom , D.nom, cp, R.nom, N.nom, lon, lat "
                        "FROM nouvelle_region N INNER JOIN region R "
                        "ON R.id_nouvelle_region=N.id "
                        "INNER JOIN departement D "
                        "ON D.id_region = R.id "
                        "INNER JOIN ville V "
                        "ON V.id_departement=D.id "
                        "ORDER by V.nom;");

        if(!model->lastError().databaseText().isEmpty())
            throw model->lastError();

        model->setHeaderData(0, Qt::Horizontal, "Nom Ville");
        model->setHeaderData(1, Qt::Horizontal, "Departement");
        model->setHeaderData(2, Qt::Horizontal, "Code Postal");
        model->setHeaderData(3, Qt::Horizontal, "Région");
        model->setHeaderData(4, Qt::Horizontal, "Nouvelle Région");
        model->setHeaderData(5, Qt::Horizontal, "Longitude");
        model->setHeaderData(6, Qt::Horizontal, "Latitude");


        ui->tableView->setModel(model);

    } catch (QSqlError &e) {
        QMessageBox::warning(this, "Erreur execution Requête affichage",
                             "L'affichage des villes a échoué.< br />Message Erreur : <strong><font color =red>"+e.text()+"</font></strong>");
    }
}

/**
 * @brief MainWindow::activeSelection
 * @return
 *
 * Return the user's click on table view
 */
QVariant MainWindow::activeSelection()
{

    QString id;
    /* Selection possible des champs sur QTableView pour récupérer les infos du modèle */
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    QItemSelectionModel *selection = ui->tableView->selectionModel();
    QModelIndex indexElementSelectionne = selection->currentIndex();
    QVariant elementSelectionne = model->data(indexElementSelectionne, Qt::DisplayRole);

    QString userChoice = elementSelectionne.toString();
    /* Pour vérifier que c'est OK */
    return elementSelectionne;
}

/**
 * @brief MainWindow::on_modify_button_clicked
 *
 * Catch the user click's
 * and catch city's id
 * for to update city (only city's name)
 */
void MainWindow::on_modify_button_clicked()
{
    /*  QString for to catch unique id from city with Mysql query   */
    QString id;
    QVariant elementSelectionne = activeSelection();
    QString userChoice = elementSelectionne.toString();
    bool ok = false;

    QString champ = QInputDialog::getText(this, "Modification ville: "+userChoice,
                                          " Veillez saisir le nouveau nom de la ville :",
                                          QLineEdit::Normal, userChoice, &ok);
    if(!ok)
       return;
    if(champ.isEmpty())
    {
        QMessageBox::warning(this," Champ vide ","Le nom de la ville ne peut être vide");
        return;
    }

    QSqlQuery query;
    if(!query.prepare("SELECT V.id "
                      "FROM ville V "
                      "WHERE V.nom LIKE :choix; "))
       QMessageBox::critical(this," Query error ","erreur :"+query.lastError().text());
    else
    {
        query.bindValue(":choix", userChoice);
        query.exec();
        if(query.first())
            id = query.value(0).toString();
    }
    updateCity(id, champ);
}

/**
 * @brief MainWindow::updateCity
 * @param id_city
 * @param city
 *
 * query for to change city's name
 * by city's id
 */
void MainWindow::updateCity(QString &id_city, QString &city)
{
    cnx->allowTransaction();
    QSqlQuery query;

    if(!query.prepare("UPDATE ville SET nom = :city_name WHERE id = :c_id; "))
       QMessageBox::critical(this, "Wrong Query", "error : "+query.lastError().text());
    else
    {
        query.bindValue(":city_name", city);
        query.bindValue(":c_id", id_city);

        if(!query.exec())
        {
            QMessageBox::critical(this, "Erreur modification de la ville dans la base de donnée",
                              "Erreur lors de l'exécution de la requête.<br />"+query.lastError().text());
            cnx->rollback();
            return;
        }
        else
            cnx->commit();
    }
    model->setQuery(query);
    ui->tableView->setModel(model);
    afficherVilles();


}

/**
 * @brief MainWindow::on_lineEdit_textChanged
 * @param saisie
 *
 * catch input's user on line edit search
 * and print in on live in table view with
 * number of cities
 */
void MainWindow::on_lineEdit_textChanged(const QString &saisie)
{
    QSqlQuery query;
    if(!query.prepare("SELECT V.nom, D.nom, cp, R.nom, N.nom, lon, lat "
                      "FROM ville V INNER JOIN "
                      "departement D ON V.id_departement = D.id "
                      "INNER JOIN region R ON D.id_region = R.id "
                      "INNER JOIN nouvelle_region N ON R.id_nouvelle_region = N.id "
                      "WHERE V.nom LIKE :param "
                      "ORDER BY V.nom;" ))
       QMessageBox::critical(this," Titre ","erreur :"+query.lastError().text());
   else
   {
        query.bindValue(":param", saisie+"%");
        query.exec();
        model->setQuery(query);
        ui->tableView->setModel(model);
        /*  number lines in the table view  */
        int compteur = model->rowCount();
        ui->statusBar->showMessage("Nombre ville : " +QString::number(compteur));
   }

}


/*  INSERT INTO MANAGMENT   */
/**
 * @brief MainWindow::on_insert_button_clicked
 * Create form and insert department in
 * the combo box
 */
void MainWindow::on_insert_button_clicked()
{
    cnx->setAutocommit();
    QSqlQuery query_dep;
    createForm();
    formulaireBox->show();

    /*  Insert department in combobox department    */
    if(!query_dep.prepare("SELECT nom FROM departement "
                             "ORDER BY nom; "))
        QMessageBox::critical(this, "Query Error", "Error : "+query_dep.lastError().text());
    else
    {
        /*  for to fill combobox    */
        query_dep.exec();
        while(query_dep.next())
            departement_cbbox->addItem(query_dep.value(0).toString());

    }

    connect(valider, SIGNAL(clicked()), this, SLOT(insertInto()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(closeForm()));

}

/**
 * @brief MainWindow::insertInto
 * query for to insert new city
 */
void MainWindow::insertInto()
{
    cnx->allowTransaction();
    QSqlQuery query;

    QString ville = ville_line->text();
    QString cp = cp_line->text();
    QString lat = lat_line->text();
    QString lon = lon_line->text();
    QString depart = departement_cbbox->currentText();

    if(ville.isEmpty())
    {
       QMessageBox::warning(this, "Attention", "Le nom de la ville ne peut avoir une valleur nulle");
    }
    else
    {
        if(!query.prepare("INSERT INTO ville (id_departement, nom, cp, lat, lon) "
                          "VALUES ((SELECT id FROM departement WHERE nom = :departement), :city, :code, :lati, :longi); "))
                QMessageBox::critical(this, "Query error", "Error :"+query.lastError().text());

        else
            {
                query.bindValue(":city", ville);
                query.bindValue(":code", cp);
                query.bindValue(":lati", lat);
                query.bindValue(":longi", lon);
                query.bindValue(":departement", depart);

                if(!query.exec())
                {
                    QMessageBox::critical(this, "Erreur modification de la ville dans la base de donnée",
                                      "Erreur lors de l'exécution de la requête.<br />"+query.lastError().text());
                    cnx->rollback();
                    return;
                }
                else
                {
                    cnx->commit();
                }
            }
    }
    closeForm();
    afficherVilles();
}

/**
 * @brief MainWindow::closeForm
 * Delete insertion form
 */
void MainWindow::closeForm()
{
    formulaireBox->deleteLater();

}
/*  END INISERT INTO    */

/*  DELETE MANAGEMENT   */
/**
 * @brief MainWindow::on_delete_button_clicked
 *
 * Catch city's id
 */
void MainWindow::on_delete_button_clicked()
{
    /*  QString for to catch unique id from city with Mysql query   */
    QString id;
    QVariant elementSelectionne = activeSelection();
    QString userChoice = elementSelectionne.toString();

    QSqlQuery query;
    if(!query.prepare("SELECT V.id "
                      "FROM ville V  "
                      "WHERE V.nom LIKE :choix ;"))
       QMessageBox::critical(this," Titre ","erreur :"+query.lastError().text());
    else
    {
        query.bindValue(":choix", userChoice);
        query.exec();
        if(query.next()==true)
            id = query.value(0).toString();
    }
    deleteCity(id);
}

/**
 * @brief MainWindow::deleteCity
 * @param id_city
 *
 * query for to delete city
 */
void MainWindow::deleteCity(QString &id_city)
{
   QSqlQuery query;

   if(!query.prepare("DELETE FROM ville WHERE id = :id_c;"))
        QMessageBox::critical(this," Titre ","erreur :"+query.lastError().text());
   else
   {
       query.bindValue(":id_c", id_city);
       query.exec();
   }
   afficherVilles();
}
/*  END DELETE MANAGMENT    */

/**
 * @brief MainWindow::on_action_Quitter_triggered
 */
void MainWindow::on_action_Quitter_triggered()
{
    close();
}
