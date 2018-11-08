#include "core/event-dispatcher.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace troll {

class EventDispatcherTest : public testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override { EventDispatcher::Instance().UnregisterAll(); }
};

namespace {
constexpr char kSampleEvent[] = "sample.event";
constexpr char kNoTriggerEvent[] = "sample.event.notrigger";
}  // namespace

TEST_F(EventDispatcherTest,
       EmitDoesNotTriggerHandlersUntilProcessingTriggeredEvents) {
  int call_count = 0;
  EventDispatcher::Instance().Register(kSampleEvent,
                                       [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EXPECT_EQ(0, call_count);

  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, call_count);
}

TEST_F(EventDispatcherTest, RegisterOneOffHandlerTriggerEventManyTimes) {
  int call_count = 0;
  EventDispatcher::Instance().Register(kSampleEvent,
                                       [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, call_count);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, call_count);
}

TEST_F(EventDispatcherTest, RegisterPermanentHandlerTriggerEventManyTimes) {
  int call_count = 0;
  EventDispatcher::Instance().RegisterPermanent(
      kSampleEvent, [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, call_count);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(2, call_count);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(3, call_count);
}

TEST_F(EventDispatcherTest, RegisterMultipleHandlersForEvent) {
  int count_a = 0;
  EventDispatcher::Instance().RegisterPermanent(kSampleEvent,
                                                [&count_a]() { ++count_a; });
  int count_b = 0;
  EventDispatcher::Instance().Register(kSampleEvent,
                                       [&count_b]() { ++count_b; });
  int count_c = 0;
  EventDispatcher::Instance().RegisterPermanent(kNoTriggerEvent,
                                                [&count_c]() { ++count_c; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, count_a);
  EXPECT_EQ(1, count_b);
  EXPECT_EQ(0, count_c);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(2, count_a);
  EXPECT_EQ(1, count_b);
  EXPECT_EQ(0, count_c);
}

TEST_F(EventDispatcherTest, CancelOneOffHandlerBeforeBeingTriggered) {
  int call_count = 0;
  const auto handler_id = EventDispatcher::Instance().Register(
      kSampleEvent, [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);

  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(0, call_count);
}

TEST_F(EventDispatcherTest, CancelOneOffHandlerAfterBeingTriggered) {
  int call_count = 0;
  const auto handler_id = EventDispatcher::Instance().Register(
      kSampleEvent, [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, call_count);

  // Expect no death, but it is a noop.
  EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);
}

TEST_F(EventDispatcherTest, CancelPermanentHandlerBeforeBeingTriggered) {
  int call_count = 0;
  const auto handler_id = EventDispatcher::Instance().RegisterPermanent(
      kSampleEvent, [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);

  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(0, call_count);
}

TEST_F(EventDispatcherTest, CancelPermanentHandlerAfterBeingTriggeredFewTimes) {
  int call_count = 0;
  const auto handler_id = EventDispatcher::Instance().RegisterPermanent(
      kSampleEvent, [&call_count]() { ++call_count; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, call_count);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(2, call_count);

  EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(2, call_count);
}

TEST_F(EventDispatcherTest, CancelHandlerOnEventWithMultipleHandlers) {
  int count_a = 0;
  EventDispatcher::Instance().RegisterPermanent(kSampleEvent,
                                                [&count_a]() { ++count_a; });
  int count_b = 0;
  const auto handler_id = EventDispatcher::Instance().RegisterPermanent(
      kSampleEvent, [&count_b]() { ++count_b; });
  int count_c = 0;
  EventDispatcher::Instance().RegisterPermanent(kSampleEvent,
                                                [&count_c]() { ++count_c; });

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(1, count_a);
  EXPECT_EQ(1, count_b);
  EXPECT_EQ(1, count_c);

  EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);

  EventDispatcher::Instance().Emit(kSampleEvent);
  EventDispatcher::Instance().ProcessTriggeredEvents();
  EXPECT_EQ(2, count_a);
  EXPECT_EQ(1, count_b);
  EXPECT_EQ(2, count_c);
}

}  // namespace troll
