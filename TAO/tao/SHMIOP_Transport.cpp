// This may look like C, but it's really -*- C++ -*-
// $Id$

#include "tao/SHMIOP_Transport.h"

#if defined (TAO_HAS_SHMIOP) && (TAO_HAS_SHMIOP != 0)

#include "tao/SHMIOP_Connect.h"
#include "tao/SHMIOP_Profile.h"
#include "tao/Timeprobe.h"
#include "tao/CDR.h"
#include "tao/Transport_Mux_Strategy.h"
#include "tao/Wait_Strategy.h"
#include "tao/Sync_Strategies.h"
#include "tao/Stub.h"
#include "tao/ORB_Core.h"
#include "tao/debug.h"

#if defined (ACE_ENABLE_TIMEPROBES)

static const char *TAO_Transport_Timeprobe_Description[] =
  {
    "SHMIOP_Transport::send - start",
    "SHMIOP_Transport::send - end",

    "SHMIOP_Transport::receive - start",
    "SHMIOP_Transport::receive - end",

    "SHMIOP_Client_Transport::start_request - start",
    "SHMIOP_Client_Transport::start_request - end"
  };

enum
  {
    TAO_SHMIOP_TRANSPORT_SEND_START = 1200,
    TAO_SHMIOP_TRANSPORT_SEND_END,

    TAO_SHMIOP_TRANSPORT_RECEIVE_START,
    TAO_SHMIOP_TRANSPORT_RECEIVE_END,

    TAO_SHMIOP_CLIENT_TRANSPORT_START_REQUEST_START,
    TAO_SHMIOP_CLIENT_TRANSPORT_START_REQUEST_END
  };


// Setup Timeprobes
ACE_TIMEPROBE_EVENT_DESCRIPTIONS (TAO_Transport_Timeprobe_Description,
                                  TAO_SHMIOP_TRANSPORT_SEND_START);

#endif /* ACE_ENABLE_TIMEPROBES */

TAO_SHMIOP_Transport::TAO_SHMIOP_Transport (TAO_SHMIOP_Handler_Base *handler,
                                        TAO_ORB_Core *orb_core)
  : TAO_Transport (TAO_TAG_SHMEM_PROFILE,
                   orb_core),
    handler_ (handler)
{
}

TAO_SHMIOP_Transport::~TAO_SHMIOP_Transport (void)
{
  // Cannot deal with errors, and therefore they are ignored.
  this->send_buffered_messages ();

  // Note that it also doesn't matter how much of the data was
  // actually sent.
  this->dequeue_all ();
}

TAO_SHMIOP_Handler_Base *&
TAO_SHMIOP_Transport::handler (void)
{
  return this->handler_;
}

int
TAO_SHMIOP_Transport::idle (void)
{
  return this->handler_->idle();
}

void
TAO_SHMIOP_Transport::close_connection (void)
{
  this->handler_->handle_close ();
}

ACE_HANDLE
TAO_SHMIOP_Transport::handle (void)
{
  return this->handler_->get_handle ();
}

ACE_Event_Handler *
TAO_SHMIOP_Transport::event_handler (void)
{
  return this->handler_;
}

// ****************************************************************

TAO_SHMIOP_Server_Transport::
    TAO_SHMIOP_Server_Transport (TAO_SHMIOP_Server_Connection_Handler *handler,
                               TAO_ORB_Core* orb_core)
  : TAO_SHMIOP_Transport (handler, orb_core),
    server_handler_ (handler),
    message_state_ (orb_core)
{
}

TAO_SHMIOP_Server_Transport::~TAO_SHMIOP_Server_Transport (void)
{
}

void
TAO_SHMIOP_Server_Transport::messaging_init (TAO_Pluggable_Messaging_Interface *mesg)
{
  this->server_mesg_factory_ = mesg;
}

// ****************************************************************

TAO_SHMIOP_Client_Transport::
    TAO_SHMIOP_Client_Transport (TAO_SHMIOP_Client_Connection_Handler *handler,
                               TAO_ORB_Core *orb_core)
  :  TAO_SHMIOP_Transport (handler,
                         orb_core),
     client_handler_ (handler)
{
}

TAO_SHMIOP_Client_Transport::~TAO_SHMIOP_Client_Transport (void)
{
}

TAO_SHMIOP_Client_Connection_Handler *
TAO_SHMIOP_Client_Transport::client_handler (void)
{
  return this->client_handler_;
}

void
TAO_SHMIOP_Client_Transport::start_request (TAO_ORB_Core * /*orb_core*/,
                                            TAO_Target_Specification & /*spec */,
                                            TAO_OutputCDR &output,
                                            CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_FUNCTION_PP_TIMEPROBE (TAO_SHMIOP_CLIENT_TRANSPORT_START_REQUEST_START);
  /*const TAO_SHMIOP_Profile* profile =
    ACE_dynamic_cast(const TAO_SHMIOP_Profile*, pfile);

  // @@ This should be implemented in the transport object, which
  //    would query the profile to obtain the version...
  if (TAO_GIOP::start_message (profile->version (),
                               TAO_GIOP::Request,
                               output,
                               orb_core) == 0)
                               ACE_THROW (CORBA::MARSHAL ());*/
  if (this->client_mesg_factory_->write_protocol_header
      (TAO_PLUGGABLE_MESSAGE_REQUEST, 
       output) == 0)
    ACE_THROW (CORBA::MARSHAL ());
}

void
TAO_SHMIOP_Client_Transport::start_locate (TAO_ORB_Core * /*orb_core*/,
                                           TAO_Target_Specification &spec,
                                           TAO_Operation_Details &opdetails,
                                           TAO_OutputCDR &output,
                                           CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  // See this is GIOP way of doing this..But anyway SHMIOP will be tied
  // up with GIOP. 
  if (this->client_mesg_factory_->write_protocol_header
      (TAO_PLUGGABLE_MESSAGE_LOCATEREQUEST, 
       output) == 0)
    ACE_THROW (CORBA::MARSHAL ());
  
  if (this->client_mesg_factory_->write_message_header (opdetails,
                                                        TAO_PLUGGABLE_MESSAGE_LOCATE_REQUEST_HEADER,   
                                                        spec,
                                                        output) == 0)
    ACE_THROW (CORBA::MARSHAL ());
}

int
TAO_SHMIOP_Client_Transport::send_request (TAO_Stub *stub,
                                           TAO_ORB_Core *orb_core,
                                           TAO_OutputCDR &stream,
                                           int two_way,
                                           ACE_Time_Value *max_wait_time)
{
  if (this->ws_->sending_request (orb_core,
                                  two_way) == -1)
    return -1;

  if (this->client_mesg_factory_->send_message (this,
                                                stream,
                                                max_wait_time,
                                                stub,
                                                two_way) == -1)
    return -1;
  return this->idle_after_send ();
}

// Return 0, when the reply is not read fully, 1 if it is read fully.
// @@ This code should go in the TAO_Transport class is repeated for
//    each transport!!
int
TAO_SHMIOP_Client_Transport::handle_client_input (int /* block */,
                                                  ACE_Time_Value *max_wait_time)
{

  // Notice that the message_state is only modified in one thread at a
  // time because the reactor does not call handle_input() for the
  // same Event_Handler in two threads at the same time.

  // Get the message state from the Transport Mux Strategy.
  TAO_GIOP_Message_State* message_state =
    this->tms_->get_message_state ();

  if (message_state == 0)
    {
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    ASYS_TEXT ("TAO (%P|%t) SHMIOP_Transport::handle_client_input -")
                    ASYS_TEXT (" nil message state\n")));
      return -1;
    }

  int result = this->client_mesg_factory_->handle_input (this,
                                                         this->orb_core_,
                                                         *message_state,
                                                         max_wait_time);
  if (result == -1)
    {
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    ASYS_TEXT ("TAO (%P|%t) - %p\n"),
                    ASYS_TEXT ("SHMIOP_Transport::handle_client_input, handle_input")));
      return -1;
    }
  if (result == 0)
    return result;

  // OK, the complete message is here...

  TAO_Pluggable_Connector_Params params;
  CORBA::ULong reply_status;

  result = this->client_mesg_factory_->parse_reply (*message_state,
                                                    params,
                                                    reply_status);
  if (result == -1)
    {
      if (TAO_debug_level > 0)
        ACE_DEBUG ((LM_DEBUG,
                    ASYS_TEXT ("TAO (%P|%t) - %p\n"),
                    ASYS_TEXT ("SHMIOP_Transport::handle_client_input, parse reply")));
      message_state->reset ();
      return -1;
    }

  result =
    this->tms_->dispatch_reply (params.request_id,
                                reply_status,
                                message_state->giop_version,
                                params.svc_ctx,
                                message_state);

  if (result == -1)
    {
      if (TAO_debug_level > 0)
        ACE_ERROR ((LM_ERROR,
                    ASYS_TEXT ("TAO (%P|%t) : SHMIOP_Client_Transport::")
                    ASYS_TEXT ("handle_client_input - ")
                    ASYS_TEXT ("dispatch reply failed\n")));
      message_state->reset ();
      return -1;
    }

  if (result == 0)
    {
      message_state->reset ();
      return 0;
    }

  // This is a NOOP for the Exclusive request case, but it actually
  // destroys the stream in the muxed case.
  this->tms_->destroy_message_state (message_state);

  return result;
}

int
TAO_SHMIOP_Client_Transport::register_handler (void)
{
  // @@ It seems like this method should go away, the right reactor is
  //    picked at object creation time.
  ACE_Reactor *r = this->orb_core ()->reactor ();
  if (r == this->client_handler ()->reactor ())
    return 0;

  return r->register_handler (this->client_handler (),
                              ACE_Event_Handler::READ_MASK);
}

void
TAO_SHMIOP_Client_Transport::messaging_init (TAO_Pluggable_Messaging_Interface *mesg)
{
  this->client_mesg_factory_ = mesg;
}

CORBA::Boolean
TAO_SHMIOP_Client_Transport::send_request_header (TAO_Operation_Details &opdetails,
                                                  TAO_Target_Specification &spec,
                                                  TAO_OutputCDR & msg)
{
  // We are going to pass on this request to the underlying messaging
  // layer. It should take care of this request
    CORBA::Boolean retval = 
      this->client_mesg_factory_->write_message_header (opdetails,
                                                        TAO_PLUGGABLE_MESSAGE_REQUEST_HEADER,  
                                                        spec,
                                                        msg);
  
  return retval;
}

// *********************************************************************

ssize_t
TAO_SHMIOP_Transport::send (TAO_Stub *stub,
                            int two_way,
                            const ACE_Message_Block *message_block,
                            const ACE_Time_Value *max_wait_time)
{
  if (stub == 0 || two_way)
    {
      return this->send (message_block,
                         max_wait_time);
    }
  else
    {
      TAO_Sync_Strategy &sync_strategy = stub->sync_strategy ();

      return sync_strategy.send (*this,
                                 *stub,
                                 message_block,
                                 max_wait_time);
    }
}

ssize_t
TAO_SHMIOP_Transport::send (const ACE_Message_Block *message_block,
                            const ACE_Time_Value *max_wait_time)
{
  TAO_FUNCTION_PP_TIMEPROBE (TAO_SHMIOP_TRANSPORT_SEND_START);
  return this->handler_->peer ().send (message_block,
                                       max_wait_time);
}

ssize_t
TAO_SHMIOP_Transport::send (const u_char *buf,
                            size_t len,
                            const ACE_Time_Value *max_wait_time)
{
  TAO_FUNCTION_PP_TIMEPROBE (TAO_SHMIOP_TRANSPORT_SEND_START);

  return this->handler_->peer ().send (buf,
                                       len,
                                       max_wait_time);
}

ssize_t
TAO_SHMIOP_Transport::recv (char *buf,
                            size_t len,
                            const ACE_Time_Value *max_wait_time)
{
  TAO_FUNCTION_PP_TIMEPROBE (TAO_SHMIOP_TRANSPORT_RECEIVE_START);

  return this->handler_->peer ().recv (buf,
                                       len,
                                       max_wait_time);
}

// Default action to be taken for send request.
int
TAO_SHMIOP_Transport::send_request (TAO_Stub *,
                                    TAO_ORB_Core *  /* orb_core */,
                                    TAO_OutputCDR & /* stream   */,
                                    int             /* twoway   */,
                                    ACE_Time_Value * /* max_wait_time */)
{
  return -1;
}

CORBA::Boolean
TAO_SHMIOP_Transport::send_request_header (TAO_Operation_Details & /**/,
                                           TAO_Target_Specification & /*spec */ ,
                                           TAO_OutputCDR & /*msg*/)
{
  // We should never be here. So return an error.
  return 0;
}


#endif /* TAO_HAS_SHMIOP && TAO_HAS_SHMIOP != 0 */
