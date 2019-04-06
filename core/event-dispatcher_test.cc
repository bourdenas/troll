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
    EventDispatcher dispatcher;
    int call_count = 0;
    dispatcher.Register(kSampleEvent,
                        [&call_count](const Event&) { ++call_count; });

    WHEN("the event is emitted and processed") {
      Event event;
      event.set_event_id(kSampleEvent);
      dispatcher.Emit(event);
      dispatcher.ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 1); }

      WHEN("batch processing runs again") {
        dispatcher.ProcessTriggeredEvents();
        THEN("the event handler is not executed a second time") {
          REQUIRE(call_count == 1);
        }
        WHEN("the event is emitted a second time") {
          dispatcher.Emit(event);
          THEN("has no effect on the event handler") {
            REQUIRE(call_count == 1);
          }
          WHEN("the second event is batch processed") {
            dispatcher.ProcessTriggeredEvents();
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
    EventDispatcher dispatcher;
    int call_count = 0;
    const auto handler_id = dispatcher.Register(
        kSampleEvent, [&call_count](const Event&) { ++call_count; });

    WHEN("the event is emitted") {
      Event event;
      event.set_event_id(kSampleEvent);
      dispatcher.Emit(event);

      WHEN("handler is canceled before the event gets batch processed") {
        dispatcher.Unregister(kSampleEvent, handler_id);
        dispatcher.ProcessTriggeredEvents();
        THEN("the event handler is not executed") { REQUIRE(call_count == 0); }

        WHEN("canceling an already canceled handler") {
          THEN("expect no death") {
            dispatcher.Unregister(kSampleEvent, handler_id);
          }
        }
      }
    }
  }
}

SCENARIO("One-off event handler cancelation after processing",
         "[EventDispatcher.OneOffHandler]") {
  GIVEN("an one-off event handler") {
    EventDispatcher dispatcher;
    int call_count = 0;
    const auto handler_id = dispatcher.Register(
        kSampleEvent, [&call_count](const Event&) { ++call_count; });

    WHEN("the event is emitted and processed") {
      Event event;
      event.set_event_id(kSampleEvent);
      dispatcher.Emit(event);
      dispatcher.ProcessTriggeredEvents();
      THEN("the event handler is executed") { REQUIRE(call_count == 1); }

      WHEN("handler is canceled after its execution") {
        THEN("expect no death") {
          dispatcher.Unregister(kSampleEvent, handler_id);
        }
      }
    }
  }
}

SCENARIO("Unregister non-existing events and handlers",
         "[EventDispatcher.Unregister]") {
  GIVEN("an event registration") {
    EventDispatcher dispatcher;
    int call_count = 0;
    const auto handler_id = dispatcher.Register(
        kSampleEvent, [&call_count](const Event&) { ++call_count; });

    WHEN("a non-existing handler in unregistered") {
      const auto non_existing_handler_id = 42;
      REQUIRE(non_existing_handler_id != handler_id);

      THEN("expect no death") {
        dispatcher.Unregister(kSampleEvent, non_existing_handler_id);
      }
    }

    WHEN("a non-registered event is unregistered") {
      REQUIRE(std::string(kSampleEvent) != std::string(kNoTriggerEvent));

      THEN("expect no death") { dispatcher.Unregister(kNoTriggerEvent, 1); }
    }
  }
}

SCENARIO("Permanent event handler", "[EventDispatcher.PermanentHandler]") {
  GIVEN("a permanent event handler") {
    EventDispatcher dispatcher;
    int call_count = 0;
    const auto handler_id = dispatcher.RegisterPermanent(
        kSampleEvent, [&call_count](const Event&) { ++call_count; });

    WHEN("the event is emitted and processed") {
      Event event;
      event.set_event_id(kSampleEvent);
      dispatcher.Emit(event);
      dispatcher.ProcessTriggeredEvents();

      THEN("the event handler is executed") { REQUIRE(call_count == 1); }

      AND_WHEN("the event is emitted and processed again") {
        dispatcher.Emit(event);
        dispatcher.ProcessTriggeredEvents();

        THEN("the event handler is executed") { REQUIRE(call_count == 2); }

        AND_WHEN("the event is emitted and processed third time") {
          dispatcher.Emit(event);
          dispatcher.ProcessTriggeredEvents();

          THEN("the event handler is executed") { REQUIRE(call_count == 3); }

          AND_WHEN(
              "the handler is canceled and the event is emitted and processed "
              "a "
              "fourth time") {
            dispatcher.Unregister(kSampleEvent, handler_id);
            dispatcher.Emit(event);
            dispatcher.ProcessTriggeredEvents();

            THEN("the event handler is not executed") {
              REQUIRE(call_count == 3);
            }
          }
        }
      }
    }
  }
}

SCENARIO("Register multiple handlers for an event",
         "[EventDispatcher.MultipleHandler]") {
  GIVEN("some event handler for an event") {
    EventDispatcher dispatcher;
    int count_a = 0;
    dispatcher.RegisterPermanent(kSampleEvent,
                                 [&count_a](const Event&) { ++count_a; });
    int count_b = 0;
    dispatcher.Register(kSampleEvent, [&count_b](const Event&) { ++count_b; });
    int count_c = 0;
    const auto handler_id = dispatcher.RegisterPermanent(
        kSampleEvent, [&count_c](const Event&) { ++count_c; });
    int count_d = 0;
    dispatcher.RegisterPermanent(kNoTriggerEvent,
                                 [&count_d](const Event&) { ++count_d; });

    WHEN("the event is emitted and processed") {
      Event event;
      event.set_event_id(kSampleEvent);
      dispatcher.Emit(event);
      dispatcher.ProcessTriggeredEvents();
      THEN("all attached handlers are executed") {
        REQUIRE(count_a == 1);
        REQUIRE(count_b == 1);
        REQUIRE(count_c == 1);
        REQUIRE(count_d == 0);

        AND_WHEN("the event is emitted and processed second time") {
          dispatcher.Emit(event);
          dispatcher.ProcessTriggeredEvents();
          THEN("all attached permanent handlers are executed") {
            REQUIRE(count_a == 2);
            REQUIRE(count_b == 1);
            REQUIRE(count_c == 2);
            REQUIRE(count_d == 0);

            AND_WHEN("one of the permanent handlers is canceled") {
              dispatcher.Unregister(kSampleEvent, handler_id);
              WHEN("the event is emitted and processed third time") {
                dispatcher.Emit(event);
                dispatcher.ProcessTriggeredEvents();
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
      }
    }
  }
}

}  // namespace troll
