#include "reader.h"


Reader::Reader(QObject *parent) :
    QObject(parent)
{
    _abort = false;
    _working = false;
}

void Reader::doWork() {
    QFile file(filename);
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        qFatal( "Could not open the file" );

    QTextStream textStream(&file);
    QString line = "";
    QStringList lineContent;

    QString dummy;

    bool ok = true;
    int val1, val2;
    double val3;
    int lineNumber = 0;

    QTime timer;
    timer.start();
    int instant;

    while(!line.isNull()) {

        mutex.lock();
        bool abort = _abort;
        mutex.unlock();

        if (abort) {
            std::cout << "Aborting..." << std::endl;
            break;
        }

        line = textStream.readLine();
        lineNumber++;

        if (line.isNull()) {
            emit finished();
            return;
        }

        if (line.isEmpty()) {
            continue;
        }

        lineContent = line.split(' ');

        if (lineContent[0] == "d") {
            instant = lineContent[1].toInt(&ok);
            if (!ok) {
                QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                     + line + ", date invalide (reçue : " + lineContent[1] + ").");
            }
            dummy.setNum(timer.elapsed());
            if (instant > timer.elapsed()) {
                // We have to wait
                Sleep(instant - timer.elapsed());
            }
            continue;
        }

        if (lineContent[0] == "reset") {
            emit sendReset();
            continue;
        }

        val1 = lineContent[0].toInt(&ok);
        if (!ok) {
            QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                 + line + ", entier non reconnu (reçu : " + lineContent[0] + ").");
            emit finished();
            return;
        }
        val2 = lineContent[1].toInt(&ok);
        if (!ok) {
            QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                 + line + ", entier non reconnu (reçu : " + lineContent[1] + ").");
            emit finished();
            return;
        }
        val3 = lineContent[2].toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                 + line + ", flottant non reconnu (reçu : " + lineContent[2] + ").");
            emit finished();
            return;
        }

        emit sendPoint(val1 - 1, val2 - 1, val3);
    }

    mutex.lock();
    _working = false;
    mutex.unlock();

    emit finished();
}

void Reader::requestWork() {
    mutex.lock();
    _working = true;
    _abort = false;
    mutex.unlock();

    emit workRequested();
}

void Reader::abort() {
    if (_working) {
        _abort = true;
    }
}

void Reader::setFilename(QString file) {
    this->filename = file;
}

