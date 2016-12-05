// Copyright (C) 2008
// by all contributors.

// This file is part of SCNSL.

// SCNSL is free software:
// you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option)
// any later version.

// SCNSL is distributed in the hope
// that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with SCNSL,
// in a file named LICENSE.txt.
// If not, see <http://www.gnu.org/licenses/>.



/// @file
/// A simple TLM task.


#include <sstream>
#include "MyTask_tr.hh"

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////

MyTask_tr::MyTask_tr( const sc_core::sc_module_name modulename,
                   // const bool is_sender,
                    const unsigned int test,
                    const task_id_t id,
                    Scnsl::Core::Node_t * n,
                    const size_t proxies )
    throw ():
    // Parents:
    Scnsl::Tlm::TlmTask_if_t( modulename, id, n, proxies ),
    // Fields:
    _packetArrivedEvent()
{
  
        switch ( test )
        {
            case 1:
            case 2:
            {
                SC_THREAD( _sender );
              //  SC_THREAD( _interference );
                break;
            }
            case 3:
            {
                SC_THREAD( _senderSynch );
                SC_THREAD( _receiverSynch );
                break;
            }
            default:
                ;
  
        
         }
}


MyTask_tr::~MyTask_tr()
    throw ()
{
    // Nothing to do.
}

// ////////////////////////////////////////////////////////////////
// Inherited interface methods.
// ////////////////////////////////////////////////////////////////


void MyTask_tr::b_transport( tlm::tlm_generic_payload & p, sc_core::sc_time & t )
{
	char result;
	bool c;
        sc_core::sc_time tt;
   tt =t;
	if( p.get_command() == Scnsl::Tlm::PACKET_COMMAND )
	{
    	result = * reinterpret_cast< char * >( p.get_data_ptr() );

#if (SCNSL_LOG >= 1)
        std::stringstream ss;
        ss << "RECEIVE data: " << result << ", size: " << p.get_data_length() << " Time stamp"<< tt ;//sc_core::sc_time_stamp() <<".";
        SCNSL_TRACE_LOG( 1, ss.str().c_str() );
#endif

        _packetArrivedEvent.notify();
	}
	else if( p.get_command() == Scnsl::Tlm::CARRIER_COMMAND )
	{
		c = * reinterpret_cast< char * >( p.get_data_ptr() );
#if (SCNSL_LOG >= 1)
        std::stringstream ss;
        ss << "carrier: " << c << ".";
        SCNSL_TRACE_LOG( 1, ss.str().c_str() );
#endif

	}
    else
    {
        // ERROR.
        SCNSL_TRACE_ERROR( 1, "Invalid PACKET_COMMAND." );

        // Just to avoid compiler warnings:
        t = sc_core::sc_time_stamp();
    }
}


// ////////////////////////////////////////////////////////////////
// Processes.
// ////////////////////////////////////////////////////////////////


void MyTask_tr::_sender()
    throw ()
{
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 1;
  //  const Scnsl::Tlm::tlm_taskproxy_id_t tp2 = 2;
    const size_t s = 1;

    while ( true )
    {
        byte_t i = static_cast<Scnsl::Core::byte_t>( rand()%25 + 65 );
 

#if ( SCNSL_LOG >= 1 )
        std::stringstream ss;
        ss << "SEND data: " << i << " size: " << s << ".";
        
        SCNSL_TRACE_LOG( 1, ss.str().c_str() ); 
#endif

	//	  wait( 20, sc_core::SC_MS );TlmTask_if_t::send( tp2, &i, s);
                 wait( 20, sc_core::SC_MS ); TlmTask_if_t::send( tp, &i, s);

               
    }

}
 
void MyTask_tr::_senderSynch()
    throw ()
{
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 1;  const Scnsl::Tlm::tlm_taskproxy_id_t tp2 = 2;
    const size_t s = 1;

    while ( true )
    {
        byte_t i = static_cast<Scnsl::Core::byte_t>( rand()%25 + 65 );

#if ( SCNSL_LOG >= 1 )
        std::stringstream ss;
        ss << "SEND data: " << i << " size: " << s << ".";
        SCNSL_TRACE_LOG( 1, ss.str().c_str() );
#endif

		TlmTask_if_t::send( tp, &i, s);	TlmTask_if_t::send( tp2, &i, s);

        // Waiting synch:
        wait( _packetArrivedEvent );
    }
}

void MyTask_tr::_interference()
    throw ()
{
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 0;
    const size_t s = 1;

    // Good time:
    wait( 6, sc_core::SC_MS );

    // Bad time:
    for ( register unsigned int index = 0; index < 3; ++index )
    {
        byte_t i = static_cast<Scnsl::Core::byte_t>( rand()%25 + 65 );

#if ( SCNSL_LOG >= 1 )
        std::stringstream ss;
        ss << "SEND data: " << i << " size: " << s << ".";
        SCNSL_TRACE_LOG( 1, ss.str().c_str() );
#endif

		TlmTask_if_t::send( tp, &i, s);
    }

    // Good time again! :)
}



void MyTask_tr::_receiverSynch()
    throw ()
{
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 0;
    const size_t s = 1;

    while ( true )
    {
        // Waiting synch:
        wait( _packetArrivedEvent );

        byte_t i = static_cast<Scnsl::Core::byte_t>( rand()%25 + 65 );

#if ( SCNSL_LOG >= 1 )
        std::stringstream ss;
        ss << "SEND data: " << i << " size: " << s << ".";
        SCNSL_TRACE_LOG( 1, ss.str().c_str() );
#endif

		TlmTask_if_t::send( tp, &i, s);
    }
}
