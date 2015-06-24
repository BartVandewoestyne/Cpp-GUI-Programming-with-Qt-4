#ifndef TRANSACTIONTHREAD_H
#define TRANSACTIONTHREAD_H

#include <QImage>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>

/**
 * The Transaction class is an abstract base class for operations that the
 * user can perform on an image.
 */
class Transaction
{
public:

    // The virtual destructor is necessary because we need to delete instances
    // of Transaction subclasses through a Transaction pointer.
    virtual ~Transaction() { }

    virtual QImage apply(const QImage &image) = 0;
    virtual QString message() = 0;
};

class FlipTransaction : public Transaction
{
public:

    // The FlipTransaction constructor takes one parameter that specifies the
    // orientation of the flip (horizontal or vertical).
    FlipTransaction(Qt::Orientation orientation);

    QImage apply(const QImage &image);
    QString message();

private:
    Qt::Orientation orientation;
};

class ResizeTransaction : public Transaction
{
public:
    ResizeTransaction(const QSize &size);

    QImage apply(const QImage &image);
    QString message();

private:
    QSize size;
};

class ConvertDepthTransaction : public Transaction
{
public:
    ConvertDepthTransaction(int depth);

    QImage apply(const QImage &image);
    QString message();

private:
    int depth;
};

/**
 * The TransactionThread class maintains a queue of transactions to process and executes them one after the
 * other in the background.
 */
class TransactionThread : public QThread
{
    Q_OBJECT

public:
    TransactionThread();
    ~TransactionThread();

    void addTransaction(Transaction *transact);
    void setImage(const QImage &image);
    QImage image();

signals:
    void transactionStarted(const QString &message);
    void allTransactionsDone();

protected:
    void run();

private:

    // holds the image onto which the transactions are applied
    QImage currentImage;

    // the queue of pending transactions
    QQueue<Transaction *> transactions;

    // a wait condition that is used to wake up the thread when a new transaction
    // has been added to the queue
    QWaitCondition transactionAdded;

    // used to protect the currentImage and transactions member variables against concurrent access
    QMutex mutex;
};

#endif
