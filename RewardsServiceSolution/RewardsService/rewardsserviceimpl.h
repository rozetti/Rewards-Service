#ifndef REWARDSSERVICE_H
#define REWARDSSERVICE_H

#include "rewardsserviceimpl_global.h"
#include "ieligibilityservice.h"
#include "irewardsservice.h"

#include <QHash>
#include <QString>

namespace RewardsService
{
    class REWARDSSERVICESHARED_EXPORT RewardsServiceImpl : public IRewardsService
    {
    public:
        RewardsServiceImpl(QHash<QString, QString> const &channelRewards,
                           EligibilityService::IEligibilityService const &es);

        virtual void getRewards(
                QString const &accountNumber,
                QVector<QString> const &channels,
                RewardsService::ResponseStatus &status,
                QVector<QString> &rewards) const override;

    private:
        void getRewardsInner(
                QString const &accountNumber,
                QVector<QString> const &channels,
                QVector<QString> &rewards) const;

        QVector<QString> buildRewards(QVector<QString> const &channels) const;

        QHash<QString, QString> const &m_channelRewards;
        EligibilityService::IEligibilityService const &m_eligibilityService;
    };
}
#endif // REWARDSSERVICE_H
