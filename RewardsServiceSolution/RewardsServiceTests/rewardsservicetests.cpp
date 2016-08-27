#include "gmock/gmock.h"
#include "crz_gmock_support.h"
using namespace ::testing;

#include "irewardsservice.h"
#include "ieligibilityservice.h"
#include "rewardsserviceimpl.h"

#include <QtTest>
#include <QString>
#include <QHash>

// crz: this macro retrieves a rewardable channel name to avoid hard-coding in the tests
#define REWARDABLE_CHANNEL(INDEX) (channelRewardsData.begin() + (INDEX)).key()

class EligibilityServiceMock : public EligibilityService::IEligibilityService
{
public:
    MOCK_CONST_METHOD2(getRewardEligibility, void(QString const &, EligibilityService::CustomerEligibility &));
};

class RewardsServiceTests : public QObject
{
    Q_OBJECT

    // crz: I use this acceptance test data thoughout the tests
    QHash<QString, QString> channelRewardsData
    {
        { "SPORTS", "CHAMPIONS_LEAGUE_FINAL_TICKET" },
        { "MUSIC", "KARAOKE_PRO_MICROPHONE" },
        { "MOVIES", "PIRATES_OF_THE_CARIBBEAN_COLLECTION" }
    };

    // crz: this mock is used when a test is not concerned with the customer eligibility
    EligibilityServiceMock alwaysEligibleEligibilityServiceMock;

public:
    RewardsServiceTests()
    {
        // crz: this mock will always consider the customer eligible for rewards
        EXPECT_CALL(alwaysEligibleEligibilityServiceMock, getRewardEligibility(_, _))
                .WillRepeatedly(SetArgReferee<1>(EligibilityService::CustomerEligibility::CUSTOMER_ELIGIBLE));
    }

private Q_SLOTS:

    // crz: these tests were written in the order presented, before the corresponding code was written

    void getRewards_noData_setsCorrectStatus()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut({}, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", {}, status, rewards);

        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);
    }

    void getRewards_invalidAccountNumber_setsCorrectStatus()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _))
                .WillOnce(Throw(EligibilityService::InvalidAccountNumberException()));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", {}, status, rewards);

        QCOMPARE(status, RewardsService::ResponseStatus::invalidAccountNumber);
    }

    // crz: this is not strictly TDD as this test will not fail, but I generally
    // write these as I go incase they don't arise later and leave edge cases untested
    // crz: ..which actually happened in a different context, see the bug fix below
    void getRewards_invalidAccountNumber_returnsZeroRewards()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _))
                .WillOnce(Throw(EligibilityService::InvalidAccountNumberException()));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { REWARDABLE_CHANNEL(0) }, status, rewards);

        QCOMPARE(rewards.size(), 0);
    }

    void getRewards_faultedEligibilityService_setsCorrectStatus()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _))
                .WillOnce(Throw(EligibilityService::TechnicalFailureException()));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", {}, status, rewards);

        QCOMPARE(status, RewardsService::ResponseStatus::eligibilityServiceFault);
    }

    void getRewards_faultedEligibilityService_returnsZeroRewards()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _))
                .WillOnce(Throw(EligibilityService::TechnicalFailureException()));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { REWARDABLE_CHANNEL(0) }, status, rewards);

        QCOMPARE(rewards.size(), 0);
    }

    // crz: I have opted to prevent exceptions from getting out of the service
    void getRewards_unexpectedException_doesNotPropagate()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _)).WillOnce(Throw(1729));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards { "some state" };

        try
        {
            rewards_service_sut.getRewards("", {}, status, rewards);
        }
        catch(int)
        {
            QFAIL("Exception propagated from getRewards()");
        }

        QCOMPARE(status, RewardsService::ResponseStatus::unknownFault);
        QCOMPARE(QString("some state"), rewards[0]); // crz: check no harm was done
    }

    void getRewards_ineligibleCustomer_setsCorrectStatus()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _))
                .WillOnce(SetArgReferee<1>(EligibilityService::CustomerEligibility::CUSTOMER_INELIGIBLE));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", {}, status, rewards);

        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);
    }

    void getRewards_ineligibleCustomerWithRewardableSubscription_returnsZeroRewards()
    {
        EligibilityServiceMock eligibility_service_mock;
        EXPECT_CALL(eligibility_service_mock, getRewardEligibility(_, _))
                .WillOnce(SetArgReferee<1>(EligibilityService::CustomerEligibility::CUSTOMER_INELIGIBLE));

        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, eligibility_service_mock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { REWARDABLE_CHANNEL(0) }, status, rewards);

        QCOMPARE(rewards.size(), 0);
    }

    void getRewards_eligibleCustomer_setsCorrectStatus()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", {}, status, rewards);

        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);
    }

    void getRewards_eligibleCustomerWithZeroSubscriptions_returnsZeroRewards()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", {}, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QCOMPARE(rewards.size(), 0);
    }

    void getRewards_eligibleCustomerWithOneNonRewardableSubscription_returnsZeroRewards()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { "unknown channel" }, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QCOMPARE(rewards.size(), 0);
    }

    void getRewards_eligibleCustomerWithTwoNonRewardableSubscription_returnsZeroRewards()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { "unknown channel", "other unknown channel" }, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QCOMPARE(rewards.size(), 0);
    }

    void getRewards_eligibleCustomerWithOneRewardableSubscription_returnsTheReward()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { REWARDABLE_CHANNEL(0) }, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QCOMPARE(rewards.size(), 1);
        QCOMPARE(channelRewardsData[REWARDABLE_CHANNEL(0)], rewards[0]);
    }

    void getRewards_eligibleCustomerWithTwoRewardableSubscriptions_returnsTheRewards()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { REWARDABLE_CHANNEL(0), REWARDABLE_CHANNEL(1) }, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QCOMPARE(rewards.size(), 2);
        QVERIFY(rewards.contains(channelRewardsData[REWARDABLE_CHANNEL(0)]));
        QVERIFY(rewards.contains(channelRewardsData[REWARDABLE_CHANNEL(1)]));
    }

    void getRewards_eligibleCustomerWithTwoSubscriptionsOneRewardable_returnsTheReward()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        rewards_service_sut.getRewards("", { REWARDABLE_CHANNEL(1), "unknown channel" }, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QCOMPARE(rewards.size(), 1);
        QVERIFY(rewards.contains(channelRewardsData[REWARDABLE_CHANNEL(1)]));
    }

    // crz: I don't have an EligibilityService implementation so I can't do an integration test. In lieu of that, this test
    // demonstrates that RewardsServiceImpl satisfies the acceptance criteria given in the spec.
    void getRewards_acceptanceTest()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards;

        // crz: individully...
        rewards_service_sut.getRewards("", { "SPORTS" }, status, rewards);
        QCOMPARE(rewards.size(), 1);
        QVERIFY(rewards.contains("CHAMPIONS_LEAGUE_FINAL_TICKET"));

        rewards_service_sut.getRewards("", { "KIDS" }, status, rewards);
        QCOMPARE(rewards.size(), 0);

        rewards_service_sut.getRewards("", { "MUSIC" }, status, rewards);
        QCOMPARE(rewards.size(), 1);
        QVERIFY(rewards.contains("KARAOKE_PRO_MICROPHONE"));

        rewards_service_sut.getRewards("", { "NEWS" }, status, rewards);
        QCOMPARE(rewards.size(), 0);

        rewards_service_sut.getRewards("", { "MOVIES" }, status, rewards);
        QCOMPARE(rewards.size(), 1);
        QVERIFY(rewards.contains("PIRATES_OF_THE_CARIBBEAN_COLLECTION"));

        // crz: ...and all together
        rewards_service_sut.getRewards("", { "SPORTS", "KIDS", "MUSIC", "NEWS", "MOVIES" }, status, rewards);
        QCOMPARE(rewards.size(), 3);
        QVERIFY(rewards.contains("CHAMPIONS_LEAGUE_FINAL_TICKET"));
        QVERIFY(rewards.contains("KARAOKE_PRO_MICROPHONE"));
        QVERIFY(rewards.contains("PIRATES_OF_THE_CARIBBEAN_COLLECTION"));
    }

    // crz: protects fix for bug discovered during the acceptance test
    void getRewards_PreExistingNonEmptyRewards_areClearedIfRequestProcessedSuccessfully()
    {
        RewardsService::RewardsServiceImpl rewards_service_sut(channelRewardsData, alwaysEligibleEligibilityServiceMock);

        auto status = RewardsService::ResponseStatus::unknown;
        QVector<QString> rewards { "pre-existing reward" };

        QVERIFY(!rewards.empty());

        rewards_service_sut.getRewards("", {}, status, rewards);
        QCOMPARE(status, RewardsService::ResponseStatus::requestProcessedSuccessfully);

        QVERIFY(rewards.empty());
    }
};

// crz: this macro extends the QTEST default main() with Google Mocks init code
crz_QTEST_QMOCK_APPLESS_MAIN(RewardsServiceTests)

#include "rewardsservicetests.moc"
