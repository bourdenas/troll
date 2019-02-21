#include "core/event-dispatcher.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

namespace troll {

namespace {
constexpr char kSampleEvent[] = "sample.event";
constexpr char kNoTriggerEvent[] = "sample.event.notrigger";
}  // namespace

SCENARIO("One-off event handler", "[EventDispatcher.OneOffHandler]") {
  GIVEN("an one-off event handler") {
    int call_count = 0;
    EventDispatcher::Instance().Register(kSampleEvent,
                                         [&call_count]() { ++call_count; });

    WHEN("the event is emitted and processed") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 1); }

      WHEN("batch processing runs again") {
        EventDispatcher::Instance().ProcessTriggeredEvents();
        THEN("the event handler is not executed a second time") {
          REQUIRE(call_count == 1);
        }
        WHEN("the event is emitted a second time") {
          EventDispatcher::Instance().Emit(kSampleEvent);
          THEN("has no effect on the event handler") {
            REQUIRE(call_count == 1);
          }
          WHEN("the second event is batch processed") {
            EventDispatcher::Instance().ProcessTriggeredEvents();
            THEN("the event handler still does not execute being an one-off") {
              REQUIRE(call_count == 1);
            }
          }
        }
      }
    }
  }
}

SCENARIO("One-off event handler cancelation before processing",
         "[EventDispatcher.OneOffHandler]") {
  GIVEN("an one-off event handler") {
    int call_count = 0;
    const auto handler_id = EventDispatcher::Instance().Register(
        kSampleEvent, [&call_count]() { ++call_count; });

    WHEN("the event is emitted") {
      EventDispatcher::Instance().Emit(kSampleEvent);

      WHEN("handler is canceled before the event gets batch processed") {
        EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);
        EventDispatcher::Instance().ProcessTriggeredEvents();
        THEN("the event handler is not executed") { REQUIRE(call_count == 0); }

        WHEN("canceling an already canceled handler") {
          THEN("expect no death") {
            EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);
          }
        }
      }
    }
  }
}

SCENARIO("One-off event handler cancelation after processing",
         "[EventDispatcher.OneOffHandler]") {
  GIVEN("an one-off event handler") {
    int call_count = 0;
    const auto handler_id = EventDispatcher::Instance().Register(
        kSampleEvent, [&call_count]() { ++call_count; });

    WHEN("the event is emitted and processed") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 1); }

      WHEN("handler is canceled after its execution") {
        THEN("expect no death") {
          EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);
        }
      }
    }
  }
}

SCENARIO("Unregister non-existing events and handlers",
         "[EventDispatcher.Unregister]") {
  GIVEN("an event registration") {
    int call_count = 0;
    const auto handler_id = EventDispatcher::Instance().Register(
        kSampleEvent, [&call_count]() { ++call_count; });

    WHEN("a non-existing handler in unregistered") {
      const auto non_existing_handler_id = 42;
      REQUIRE(non_existing_handler_id != handler_id);

      THEN("expect no death") {
        EventDispatcher::Instance().Unregister(kSampleEvent,
                                               non_existing_handler_id);
      }
    }

    WHEN("a non-registered event is unregistered") {
      REQUIRE(std::string(kSampleEvent) != std::string(kNoTriggerEvent));

      THEN("expect no death") {
        EventDispatcher::Instance().Unregister(kNoTriggerEvent, 1);
      }
    }
  }
}

SCENARIO("Permanent event handler", "[EventDispatcher.PermanentHandler]") {
  GIVEN("a permanent event handler") {
    int call_count = 0;
    const auto handler_id = EventDispatcher::Instance().RegisterPermanent(
        kSampleEvent, [&call_count]() { ++call_count; });

    WHEN("the event is emitted and processed") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 1); }
    }

    WHEN("the event is emitted and processed again") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 2); }
    }

    WHEN("the event is emitted and processed third time") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 3); }
    }

    WHEN(
        "the handler is canceled and the event is emitted and processed a "
        "fourth time") {
      EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("the event handler is not executed") { REQUIRE(call_count == 3); }
    }
  }
}

SCENARIO("Register multiple handlers for an event",
         "[EventDispatcher.MultipleHandler]") {
  GIVEN("some event handler for an event") {
    int count_a = 0;
    EventDispatcher::Instance().RegisterPermanent(kSampleEvent,
                                                  [&count_a]() { ++count_a; });
    int count_b = 0;
    EventDispatcher::Instance().Register(kSampleEvent,
                                         [&count_b]() { ++count_b; });
    int count_c = 0;
    const auto handler_id = EventDispatcher::Instance().RegisterPermanent(
        kSampleEvent, [&count_c]() { ++count_c; });
    int count_d = 0;
    EventDispatcher::Instance().RegisterPermanent(kNoTriggerEvent,
                                                  [&count_d]() { ++count_d; });

    WHEN("the event is emitted and processed") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("all attached handlers are executed") {
        REQUIRE(count_a == 1);
        REQUIRE(count_b == 1);
        REQUIRE(count_c == 1);
        REQUIRE(count_d == 0);
      }
    }

    WHEN("the event is emitted and processed second time") {
      EventDispatcher::Instance().Emit(kSampleEvent);
      EventDispatcher::Instance().ProcessTriggeredEvents();
      THEN("all attached permanent handlers are executed") {
        REQUIRE(count_a == 2);
        REQUIRE(count_b == 1);
        REQUIRE(count_c == 2);
        REQUIRE(count_d == 0);
      }
    }

    WHEN("one of the permanent handlers is canceled") {
      EventDispatcher::Instance().Unregister(kSampleEvent, handler_id);
      WHEN("the event is emitted and processed third time") {
        EventDispatcher::Instance().Emit(kSampleEvent);
        EventDispatcher::Instance().ProcessTriggeredEvents();
        THEN("only the non-terminated handler is executed") {
          REQUIRE(count_a == 3);
          REQUIRE(count_b == 1);
          REQUIRE(count_c == 2);
          REQUIRE(count_d == 0);
        }
      }
    }
  }
}

}  // namespace troll
