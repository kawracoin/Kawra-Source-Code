// Copyright (c) 2011-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KAWRA_QT_KAWRAADDRESSVALIDATOR_H
#define KAWRA_QT_KAWRAADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class KawraAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit KawraAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Kawra address widget validator, checks for a valid kawra address.
 */
class KawraAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit KawraAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // KAWRA_QT_KAWRAADDRESSVALIDATOR_H
