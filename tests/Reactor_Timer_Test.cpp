// ============================================================================
// $Id$

//
// = LIBRARY
//    tests
// 
// = FILENAME
//    Reactor_Timer_Test.cpp
//
// = DESCRIPTION
//      This is a simple test that illustrates the timer mechanism of
//      the reactor. Scheduling timers, handling expired timers and
//      cancelling scheduled timers are all tested in this test. No
//      command line arguments are needed to run the test.
//
// = AUTHOR
//    Prashant Jain and Doug C. Schmidt
// 
// ============================================================================

#include "ace/Log_Msg.h"
#include "ace/Timer_Queue.h"
#include "ace/Reactor.h"
#include "test_config.h"

static int done = 0;
static int count = 0;
static int odd = 0;

class Reactor_Timer : public ACE_Event_Handler
{
public:
  virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg)
    {
      ACE_ASSERT ((int) arg == count);
      if (odd == 1)
	count += 2;
      else
	count += 1;
      ACE_DEBUG ((LM_DEBUG, "%d: Timer #%d timed out!\n", count, int (arg)));
      if ((int) arg == ACE_MAX_TIMERS - 1)
	done = 1;
      return 0;
    }
  
private:
};

int
main (int argc, char *argv[])
{
  ACE_START_TEST;

  ACE_Reactor reactor;
  Reactor_Timer rt[ACE_MAX_TIMERS];
  int t_id[ACE_MAX_TIMERS];
  int i;
  
  for (i = 0; i < ACE_MAX_TIMERS; i++)
    {
      t_id[i] = reactor.schedule_timer (&(rt[i]), (const void *) i, ACE_Time_Value (2 * i + 1));
    }
  while (!done)
    reactor.handle_events ();
  
  done = 0;
  count = 0;
  
  // Now try multiple timers for ONE event handler  (should produce the same result)
  for (i = 0; i < ACE_MAX_TIMERS; i++)
    {
      t_id[i] = reactor.schedule_timer (&(rt[0]), (const void *) i, ACE_Time_Value (2 * i + 1));
    }
  while (!done)
    reactor.handle_events ();
  
  done = 0;
  count = 1;
  odd = 1;
  // Cancel even numbered timers
  for (i = 0; i < ACE_MAX_TIMERS; i++)
    {
      t_id[i] = reactor.schedule_timer (&(rt[0]), (const void *) i, ACE_Time_Value (2 * i + 1));
      if (((i+2) % 2) == 0)
	reactor.cancel_timer (t_id[i]);
    }
  while (!done)
    reactor.handle_events ();

  ACE_END_TEST;

  return 0;
}

