A few notes about my RewardsService solution
============================================

Here I call out a few points and justify some design decisions that would normally have been sorted out with the PO or other stakeholders before work started.

1. It is not clear what is meant by "notify the client", so I have returned a status code enumeration along with the rewards payload. This can be used to notify a consumer of a bad account number.  By the time it gets to a human it wants to be localisable text, so returning a string message here would not be appropriate.

2. There is a (presumably deliberate..) flaw in the spec in that a "technical failure" in the Eligibility Service would be silently swallowed, and would appear to the outside world as if the request had succeeded, with the customer not due any rewards (regardless eligibility or subscription status). Similarly to the point above, I return a status code which indicates that there should probably be a further attempt when the fault is cleared.

3. I have chosen not to allow exceptions to propagate outside of the service. This appears to be what the spec is driving at (perhaps because exceptions are a bit of a grey area in Qt). I return an `unknownFault` status code that covers any `std::bad_alloc` or whatever else that might be thrown in the service. The enumeration is below:
```
	enum class ResponseStatus
	{
		unknown = 0,
		requestProcessedSuccessfully,
		invalidAccountNumber,
		eligibilityServiceFault,
		unknownFault
	};
```
The returned rewards are valid if and only if the returned status is `requestProcessedSuccessfully`. In all other cases the Strong Exception Guarantee is maintained.

4. If I were doing this for real I would have abstracted the channel->reward mappings into a separate class or service. For simplicity it is a raw `QHash`, constructor-injected along with the Eligibility Service. It is set up with the given acceptance data in the test suite, one of the final tests runs through the whole lot to show that it works as required.

5. I am assuming that the "N/A" response for non-rewardable channels does not mean "return the literal string `"N/A"`" as the reward. Rather I simply return a vector of concrete rewards. I treat unknown channels equivalently. This implies that the returned rewards vector may have fewer elements than the channels vector.

6. In my telephone interview it was mentioned that you don't use STL so I have generally avoided it. I have used it in a few places where there appears to be no Qt alternative (particularly `std::move` and `std::swap`) or because the Qt5.5 docs recommend using STL `<algorithms>` over the deprecated `<QtAlgorithms>`.

7. There are a lot noisy comments in the code, which I would normally avoid. Most of them are there only to help you evaluate my reasoning or methods (although some are important).

8. The mocked Eligibility Service is specifically required by the spec; it is at the top of the tests source file for simplicity. In practice I usually keep mocks with the classes that they mock so that they can be found and used by anyone that needs them.

9. I have a header-only project called `ServiceInterfaces` which has abstract interfaces `IRewardsService` and `IEligibilityService` for the required services, along with the public exceptions and enumerations that consumers need. This enables dependency injection and mocking the classes without a dependency on an implementation. My implementation is `RewardsServiceImpl`, in a separate project.

You should be able to load the top-level `.pro` into Qt-Creator and build and run the tests (the tests are the only executable product). The tests use QTest and Google Mocks (the latter is included with the source, you should not have to do anything special). I wrote the code test-first in a 32bit Ubuntu 15.04 VM, using Qt-Creator 3.4.2, Qt 5.5 and gcc 4.9.2. The whole process including typing this document took about five hours.

I hope you like it!

Conrad

