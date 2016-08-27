#ifndef IREWARDSSERVICE
#define IREWARDSSERVICE

#include <QVector>
#include <QString>

namespace RewardsService
{
    enum class ResponseStatus
    {
        unknown = 0,
        requestProcessedSuccessfully,
        invalidAccountNumber,
        eligibilityServiceFault,
        unknownFault
    };

    class IRewardsService
    {
    public:
        virtual void getRewards(
                QString const &accountNumber,
                QVector<QString> const &channels,
                ResponseStatus &status,
                QVector<QString> &rewards) const = 0;
    };
}

#endif // IREWARDSSERVICE

