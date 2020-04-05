#include "CallbackList.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/gmock-function-mocker.h"
#include "Includes.h"
#include "./Callback_Tests.h"

using ::testing::MockFunction;
using testing::Mock;

//test multiple callbacks have the same argument
TEST(CallbackList, lambdaSameArgumentMultiple) {

    MockFunction<void(int &)> callback;
    MockFunction<void(int &)> callback1;
    MockFunction<void(int &)> callback2;

     int CALL_VAL = 3;

    EXPECT_CALL(callback, Call(CALL_VAL));
    EXPECT_CALL(callback1, Call(CALL_VAL));
    EXPECT_CALL(callback2, Call(CALL_VAL));

    CallbackList<int, void, int &> list;
    typedef CallbackList<int, void, int &>::Callback Callback;

    list.addCallback(1 , callback::getCallback<Callback, int &> (callback));
    list.addCallback(2, callback::getCallback<Callback, int &> (callback1));
    list.addCallback(3, callback::getCallback<Callback, int &> (callback2));
    
    list.executeAll(CALL_VAL);
}

//multie arguments passed to multiple call backs
TEST(CallbackList, lambdaMultipleArgumentMultiple) {

    MockFunction<void(const int&, const std::string &)> callback;
    MockFunction<void(const int &, const std::string &)> callback1;
    MockFunction<void(const int &, const std::string &)> callback2;

    const int CALL_VAL1 = 3;
    const std::string CALL_VAL2 = "HELLO";

    EXPECT_CALL(callback, Call(CALL_VAL1, CALL_VAL2));
    EXPECT_CALL(callback1, Call(CALL_VAL1, CALL_VAL2));
    EXPECT_CALL(callback2, Call(CALL_VAL1, CALL_VAL2));

    CallbackList<int, void, const int &, const std::string &> list;
    typedef CallbackList<int, void, const int &, const std::string &>::Callback Callback;

    list.addCallback(1, callback::getCallback<Callback, const int &, const std::string &> (callback));
    list.addCallback(2, callback::getCallback<Callback, const int &, const std::string &> (callback1));
    list.addCallback(3, callback::getCallback<Callback, const int &, const std::string &> (callback2));
    list.executeAll(CALL_VAL1, CALL_VAL2);
}

//test callback deletion and callback replacement
TEST(CallbackList, callbackDeletionReplacement) {
    MockFunction<void(const int&)> callback;
    MockFunction<void(const int &)> callback1;
    MockFunction<void(const int &)> callback2;

    const int CALL_VAL = 3;

    //only first clalback should be called
    EXPECT_CALL(callback, Call(CALL_VAL));
    EXPECT_CALL(callback1, Call).Times(0);
    EXPECT_CALL(callback2, Call).Times(0);

    CallbackList<std::string, void, const int &> list;
    typedef CallbackList<std::string, void, const int &>::Callback Callback;

    //callback2 replaced by callback, so it shouldn't be called.
    //callback1 is removed so it shouldn't be called
    list.addCallback("1", callback::getCallback<Callback, const int &> (callback2));
    list.addCallback("2", callback::getCallback<Callback, const int &> (callback1));
    list.addCallback("1", callback::getCallback<Callback, const int &> (callback));
    list.removeCallback("2");

    list.executeAll(CALL_VAL);

    //manually verify to ru nntext test
    Mock::VerifyAndClearExpectations(&callback);
    Mock::VerifyAndClearExpectations(&callback1);
    Mock::VerifyAndClearExpectations(&callback2);

    //test that all callback's get removed
    EXPECT_CALL(callback, Call).Times(0);
    EXPECT_CALL(callback1, Call).Times(0);
    EXPECT_CALL(callback2, Call).Times(0);

    list.addCallback("1", callback::getCallback<Callback, const int &> (callback));
    list.addCallback("2", callback::getCallback<Callback, const int &> (callback1));
    list.addCallback("3", callback::getCallback<Callback, const int &> (callback2));
    list.removeCallback("1");
    list.removeCallback("2");
    list.removeCallback("3");

    list.executeAll(CALL_VAL);
}

//checks if a class memeber function used as a callback sucessfully modifies it's internal state
TEST(CallbackList, classMethodMemberVariableMutation) {
    callback::MockClass mock;

    const int CALL_VAL = 3;

    //run function manually to change internal value
    EXPECT_CALL(mock, Call)
        .WillOnce([&](int x){ mock.internalValue = x;} );

    
    CallbackList<int, void, const int &> list;

    list.addCallback(1, mock.getCallback());

    list.executeAll(CALL_VAL);

    EXPECT_EQ(mock.internalValue, CALL_VAL) << "internal value not changed with member callback";
}