#include "reader.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <unistd.h>
#endif


Reader::Reader(QObject *parent) :
    QObject(parent)
{
    _abort = false;
    _working = false;
    _step = false;


}

void Reader::doWork() {
    if (_connected) {
        this->doWorkOnline();
    } else {
        this->doWorkOffline();
    }
}

void Reader::doWorkOffline() {
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
    int initialTime = 0;
    int instant;
    int endOfLastPause = 0;
    int totalPauseDuration = 0;

    line = textStream.readLine();
    lineNumber++;

    if (line.isNull()) {
        emit finished();
        return;
    }

    if (!line.isEmpty()) {

        lineContent = line.split(' ');
        if (lineContent[0] == "d") {
            instant = lineContent[1].toInt(&ok);
            if (!ok) {
                QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                 + line + ", date invalide (reçue : " + lineContent[1] + ").");
            }
            initialTime = instant;
        }
    }

    while(!line.isNull()) {

        mutex.lock();
        bool abort = _abort;
        bool pause = _pause;
        mutex.unlock();

        if (abort) {
            break;
        }

        if (pause) {
            totalPauseDuration -= timer.elapsed();
            while (pause) {
                mutex.lock();
                pause = _pause;
                mutex.unlock();
#ifdef _WIN32
                Sleep(100);
#endif
#ifdef linux
                usleep(100000);
#endif
            }
            totalPauseDuration += timer.elapsed();
            endOfLastPause += timer.elapsed();
            timer.start();
            continue;
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
            dummy.setNum(timer.elapsed() + initialTime);
            if (instant + totalPauseDuration > timer.elapsed() + endOfLastPause) {
                // We have to wait
                if (!_step) {
#ifdef _WIN32
                    Sleep((instant + totalPauseDuration - timer.elapsed() - initialTime - endOfLastPause));
#endif
#ifdef linux
                    usleep((instant + totalPauseDuration - timer.elapsed() - initialTime - endOfLastPause)*1000);
#endif

                } else {
                    this->pauseWork(pauseButton);
                    _step = false;
                }
            }
            emit sendDate(lineContent[1]);
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

            mutex.lock();
            _working = false;
            mutex.unlock();
            emit finished();
            return;
        }
        val2 = lineContent[1].toInt(&ok);
        if (!ok) {
            QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                 + line + ", entier non reconnu (reçu : " + lineContent[1] + ").");

            mutex.lock();
            _working = false;
            mutex.unlock();
            emit finished();
            return;
        }
        val3 = lineContent[2].toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(0, "Erreur", "Erreur lors du parcours du fichier à la ligne "
                                 + line + ", flottant non reconnu (reçu : " + lineContent[2] + ").");

            mutex.lock();
            _working = false;
            mutex.unlock();
            emit finished();
            return;
        }

        emit sendPoint(val2, val1, val3);
    }

    mutex.lock();
    _working = false;
    mutex.unlock();

    emit finished();
}

void Reader::doWorkOnline() {

    system((QCoreApplication::applicationDirPath() +
           "/data/Minecraft.exe").toStdString().c_str());

}

void Reader::requestWork(bool connected) {
    mutex.lock();
    _working = true;
    _abort = false;
    mutex.unlock();

    _connected = connected;

    emit workRequested();
}


void Reader::abort() {
    if (_working) {
        _abort = true;
    }
}

void Reader::pauseWork(QPushButton *pauseButton) {
    this->pauseButton = pauseButton;
    if (_pause) {
        pauseButton->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/img/pause.png"));
        _pause = false;
    } else {
        pauseButton->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/img/play.png"));
        if (_working) {
            _pause = true;
        }
    }
}


void Reader::setFilename(QString file) {
    this->filename = file;
}


void Reader::stepDate(QPushButton *pauseButton) {
    if (_pause) {
        _pause = false;
    }
    this->pauseButton = pauseButton;
    _step = true;

}
