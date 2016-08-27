#ifndef IELIGIBILITYSERVICE
#define IELIGIBILITYSERVICE

#include <QString>

namespace EligibilityService
{
    enum class CustomerEligibility
    {
        unknown = 0,
        CUSTOMER_ELIGIBLE,
        CUSTOMER_INELIGIBLE
    };

    class TechnicalFailureException
    {
    };

    class InvalidAccountNumberException
    {
    };

    class IEligibilityService
    {
    public:
        virtual void getRewardEligibility(QString const &accountNumber, CustomerEligibility &eligibility) const = 0;
    };
}

#endif // IELIGIBILITYSERVICE

