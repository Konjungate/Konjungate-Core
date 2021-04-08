// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "bitcoinunits.h"
#include "main.h"

#include <QSettings>
#include <QStringList>

KonjungateUnits::KonjungateUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<KonjungateUnits::Unit> KonjungateUnits::availableUnits()
{
    QList<KonjungateUnits::Unit> unitlist;
    unitlist.append(KONJ);
    unitlist.append(mKONJ);
    unitlist.append(uKONJ);
    return unitlist;
}

bool KonjungateUnits::valid(int unit)
{
    switch(unit)
    {
    case KONJ:
    case mKONJ:
    case uKONJ:
        return true;
    default:
        return false;
    }
}

QString KonjungateUnits::name(int unit)
{
    switch(unit)
    {
    case KONJ: return QString("KONJ");
    case mKONJ: return QString("mKONJ");
    case uKONJ: return QString::fromUtf8("μKONJ");
    default: return QString("???");
    }
}

QString KonjungateUnits::description(int unit)
{
    switch(unit)
    {
    case KONJ: return QString("Konjungates");
    case mKONJ: return QString("Milli-Konjungates (1 / 1,000)");
    case uKONJ: return QString("Micro-Konjungates (1 / 1,000,000)");
    default: return QString("???");
    }
}

qint64 KonjungateUnits::factor(int unit)
{
    switch(unit)
    {
    case KONJ:  return 100000000;
    case mKONJ: return 100000;
    case uKONJ: return 100;
    default:   return 100000000;
    }
}

int KonjungateUnits::amountDigits(int unit)
{
    switch(unit)
    {
    case KONJ: return 11;  // 21,000,000,000         (# digits, without commas)
    case mKONJ: return 14; // 21,000,000,000,000
    case uKONJ: return 17; // 21,000,000,000,000,000
    default: return 0;
    }
}

int KonjungateUnits::decimals(int unit)
{
    switch(unit)
    {
    case KONJ: return 8;
    case mKONJ: return 5;
    case uKONJ: return 2;
    default: return 0;
    }
}

QString KonjungateUnits::format(int unit, const CAmount& nIn, bool fPlus, SeparatorStyle separators)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    qint64 n = (qint64)nIn;
    qint64 coin = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    qint64 remainder = n_abs % coin;
    QString quotient_str = QString::number(quotient);
    QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');

    // Right-trim excess zeros after the decimal point
    int nTrim = 0;
    for (int i = remainder_str.size()-1; i>=2 && (remainder_str.at(i) == '0'); --i)
        ++nTrim;
    remainder_str.chop(nTrim);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');
    return quotient_str + QString(".") + remainder_str;
}

// TODO: Review all remaining calls to KonjungateUnits::formatWithUnit to
// TODO: determine whether the output is used in a plain text context
// TODO: or an HTML context (and replace with
// TODO: BtcoinUnits::formatHtmlWithUnit in the latter case). Hopefully
// TODO: there aren't instances where the result could be used in
// TODO: either context.

// NOTE: Using formatWithUnit in an HTML context risks wrapping
// quantities at the thousands separator. More subtly, it also results
// in a standard space rather than a thin space, due to a bug in Qt's
// XML whitespace canonicalisation
//
// Please take care to use formatHtmlWithUnit instead, when
// appropriate.

QString KonjungateUnits::formatWithUnit(int unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    return format(unit, amount, plussign, separators) + QString(" ") + name(unit);
}

QString KonjungateUnits::formatHtmlWithUnit(int unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    QString str(formatWithUnit(unit, amount, plussign, separators));
    str.replace(QChar(THIN_SP_CP), QString(THIN_SP_HTML));
    return QString("<span style='white-space: nowrap;'>%1</span>").arg(str);
}

QString KonjungateUnits::floorWithUnit(int unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    QSettings settings;
    int digits = settings.value("digits").toInt();

    QString result = format(unit, amount, plussign, separators);
    if(decimals(unit) > digits) result.chop(decimals(unit) - digits);

    return result + QString(" ") + name(unit);
}

QString KonjungateUnits::floorHtmlWithUnit(int unit, const CAmount& amount, bool plussign, SeparatorStyle separators)
{
    QString str(floorWithUnit(unit, amount, plussign, separators));
    str.replace(QChar(THIN_SP_CP), QString(THIN_SP_HTML));
    return QString("<span style='white-space: nowrap;'>%1</span>").arg(str);
}

bool KonjungateUnits::parse(int unit, const QString &value, CAmount *val_out)
{
    if(!valid(unit) || value.isEmpty())
        return false; // Refuse to parse invalid unit or empty string
    int num_decimals = decimals(unit);

    // Ignore spaces and thin spaces when parsing
    QStringList parts = removeSpaces(value).split(".");

    if(parts.size() > 2)
    {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if(parts.size() > 1)
    {
        decimals = parts[1];
    }
    if(decimals.size() > num_decimals)
    {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if(str.size() > 18)
    {
        return false; // Longer numbers will exceed 63 bits
    }
    CAmount retvalue = str.toLongLong(&ok);
    if(val_out)
    {
        *val_out = retvalue;
    }
    return ok;
}

QString KonjungateUnits::getAmountColumnTitle(int unit)
{
    QString amountTitle = QObject::tr("Amount");
    if (KonjungateUnits::valid(unit))
    {
        amountTitle += " ("+KonjungateUnits::name(unit) + ")";
    }
    return amountTitle;
}

int KonjungateUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant KonjungateUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(name(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}

CAmount KonjungateUnits::maxMoney()
{
    return MAX_SINGLE_TX;
}
