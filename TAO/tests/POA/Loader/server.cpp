
//=============================================================================
/**
 *  @file     server.cpp
 *
 *   Server application to test the Servant Activator and Servant Locator.
 *
 *  @author  Kirthika Parameswaran <kirthika@cs.wustl.edu>
 */
//=============================================================================


#include "Server_Manager.h"

int
ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
  try
    {
      Server_i server;

      if (server.init (argc, argv) != 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                          "Failure during Initialisation: init ()\n"),
                          -1);

      // This POA is used to test the Servant Activator interface and
      // hence has an RETAIN policy.
      PortableServer::POA_var first_poa =
        server.create_poa ("firstPOA", 1);

      if (first_poa.in () == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                          "Failure during first POA creation: create_poa ()\n"),
                          -1);

      // This poa is used to test the Servant Activator interface and
      // hence has an NONRETAIN policy.
      PortableServer::POA_var second_poa =
        server.create_poa ("secondPOA", 0);

      if (second_poa.in () == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                          "Failure during second POA creation: create_poa ()\n"),
                          -1);

      // The Servant Activator interface is created and initialised.
      if (server.create_activator (first_poa) != 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                          "Failure during first Servant Activator creation: create_activator()\n"),
                          -1);

      // The Servant Locator interface is created and initialised.
      if (server.create_locator (second_poa) !=0)
        ACE_ERROR_RETURN ((LM_ERROR,
                          "Failure during first Servant Locator creation: create_activator()\n"),
                          -1);

      // The server begins processing requests.
      if (server.run () != 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                          "Failure during request processing: run ()\n"),
                          -1);
    }
  catch (const ::CORBA::Exception &ex)
    {
      ex._tao_print_exception("Caught unexpected CORBA exception :");
    }
  return 0;
}
