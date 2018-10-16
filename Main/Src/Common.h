#pragma once
#include <QString>
#include <QHash>

namespace std {
template<>
struct hash<QString>
{
    std::size_t operator ()(const QString &str) const noexcept
    {
        return qHash(str);
    }
};
}
