#include "rewardsserviceimpl.h"
using namespace RewardsService;

#include <QHash>
#include <QString>

RewardsServiceImpl::RewardsServiceImpl(QHash<QString, QString> const &channelRewards,
                                       EligibilityService::IEligibilityService const &es) :
    m_channelRewards(channelRewards),
    m_eligibilityService(es)
{
}

void RewardsServiceImpl::getRewards(
        QString const &accountNumber,
        QVector<QString> const &channels,
        ResponseStatus &status,
        QVector<QString> &rewards) const
{
    try
    {
        // crz: SRP doesn't like to mix exception handling with real work
        getRewardsInner(accountNumber, channels, rewards);

        status = ResponseStatus::requestProcessedSuccessfully;
    }
    catch(EligibilityService::InvalidAccountNumberException)
    {
        status = ResponseStatus::invalidAccountNumber;
    }
    catch(EligibilityService::TechnicalFailureException)
    {
        status = ResponseStatus::eligibilityServiceFault;
    }
    catch(...)
    {
        // crz: prevent exceptions from leaving the service
        status = ResponseStatus::unknownFault;
    }
}

void RewardsServiceImpl::getRewardsInner(
        QString const &accountNumber,
        QVector<QString> const &channels,
        QVector<QString> &rewards) const
{
    EligibilityService::CustomerEligibility eligibility = EligibilityService::CustomerEligibility::unknown;

    m_eligibilityService.getRewardEligibility(accountNumber, eligibility);

    // crz: assert the post-condition on the service
    Q_ASSERT(EligibilityService::CustomerEligibility::unknown != eligibility);

    QVector<QString> temp_rewards;

    if (EligibilityService::CustomerEligibility::CUSTOMER_ELIGIBLE == eligibility)
    {
        temp_rewards = buildRewards(channels);
    }

    std::swap(temp_rewards, rewards); // crz: maintain the strong exception guarantee
}

QVector<QString> RewardsServiceImpl::buildRewards(QVector<QString> const &channels) const
{
    QVector<QString> rewards;

    for (auto const &s : channels)
    {
        auto const it = m_channelRewards.find(s);
        if (m_channelRewards.end() != it)
        {
            rewards.append(it.value());
        }
    }

    return rewards; // crz: NRVO is required by c++11
}
