#include <QtGui>

#include "transactionthread.h"

Transaction * const EndTransaction = 0;

FlipTransaction::FlipTransaction(Qt::Orientation orientation)
{
    this->orientation = orientation;
}

QImage FlipTransaction::apply(const QImage &image)
{
    return image.mirrored(orientation == Qt::Horizontal,
                          orientation == Qt::Vertical);
}

/**
 * The message() function returns the message to display in the status bar
 * while the operation is in progress.  This function is called in
 * TransactionThread::run() when emitting the transactionStarted() signal.
 */
QString FlipTransaction::message()
{
    if (orientation == Qt::Horizontal) {
        return QObject::tr("Flipping image horizontally...");
    } else {
        return QObject::tr("Flipping image vertically...");
    }
}

ResizeTransaction::ResizeTransaction(const QSize &size)
{
    this->size = size;
}

QString ResizeTransaction::message()
{
    return QObject::tr("Resizing image...");
}

QImage ResizeTransaction::apply(const QImage &image)
{
    return image.scaled(size, Qt::IgnoreAspectRatio,
                        Qt::SmoothTransformation);
}

ConvertDepthTransaction::ConvertDepthTransaction(int depth)
{
    this->depth = depth;
}

QImage ConvertDepthTransaction::apply(const QImage &image)
{
    QImage::Format format;

    switch (depth) {
    case 1:
        format = QImage::Format_Mono;
        break;
    case 8:
        format = QImage::Format_Indexed8;
        break;
    case 24:
    default:
        format = QImage::Format_RGB32;
    }

    return image.convertToFormat(format);
}

QString ConvertDepthTransaction::message()
{
    return QObject::tr("Converting image depth...");
}

TransactionThread::TransactionThread()
{
    start();
}

TransactionThread::~TransactionThread()
{
    {
	// Note that it is important to unlock the mutex before calling wait();
	// otherwise the program could end up in a deadlock situation, where
	// the secondary thread waits forever for the mutex to be unlocked, and
	// the main thread holds the mutex and waits for the secondary thread
	// to finish before proceeding.
        QMutexLocker locker(&mutex);

	// empty the transaction que
        while (!transactions.isEmpty())
            delete transactions.dequeue();

	// add a special EndTransaction marker
        transactions.enqueue(EndTransaction);

        // wake up the thread
        transactionAdded.wakeOne();
    }

    // wait for the thread to finish
    wait();
}

/**
 * The addTransaction() function adds a transaction to the transaction queue
 * and wakes up the transaction thread if it isn't already running.  All
 * accesses to the transactions member variable are protected by a mutex,
 * because the main thread might modify them through addTransaction() at the
 * same time as the secondary thread is iterating over transactions.
 */
void TransactionThread::addTransaction(Transaction *transact)
{
    QMutexLocker locker(&mutex);
    transactions.enqueue(transact);
    transactionAdded.wakeOne();
}


/**
 * The run() function goes through the transaction queue and executes each
 * transaction in turn by calling apply() on them, until it reaches the
 * EndTransaction marker. If the transaction queue is empty, the thread waits
 * on the "transaction added" condition.
 */
void TransactionThread::run()
{
    Transaction *transact = 0;
    QImage oldImage;

    forever {
        {
            QMutexLocker locker(&mutex);

            if (transactions.isEmpty())
                transactionAdded.wait(&mutex);
            transact = transactions.dequeue();
            if (transact == EndTransaction)
                break;

            oldImage = currentImage;
        }

	// Just before we execute a transaction, we emit the
	// transactionStarted() signal with a message to display in the
	// application's status bar.
        emit transactionStarted(transact->message());

        QImage newImage = transact->apply(oldImage);
        delete transact;

        {
            QMutexLocker locker(&mutex);
            currentImage = newImage;

	    // When all the transactions have finished processing, we emit the
	    // allTransactionsDone() signal.
            if (transactions.isEmpty())
                emit allTransactionsDone();
        }
    }
}

/**
 * The setImage() function allows the main thread to set the image on which
 * the transactions should be performed.
 */
void TransactionThread::setImage(const QImage &image)
{
    QMutexLocker locker(&mutex);
    currentImage = image;
}

/**
 * The image() function allows the main thread to retrieve the resulting image
 * once all the transactions are done.
 */
QImage TransactionThread::image()
{
    QMutexLocker locker(&mutex);
    return currentImage;
}
