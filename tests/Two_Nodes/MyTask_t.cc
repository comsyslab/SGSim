

#include <sstream>
#include "MyTask_t.hh"

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////



struct Payload_t
{
   unsigned int id;			 
   double sender_times;         
   int Temperature;			 
};

MyTask_t::MyTask_t( const sc_core::sc_module_name modulename,
                    const bool is_sender,
                    const task_id_t id,
                    Scnsl::Core::Node_t * n,
                    const size_t proxies )
    throw ():
    // Parents:
    Scnsl::Tlm::TlmTask_if_t( modulename, id, n, proxies ),
    // Fields:
    _packetArrivedEvent(), taskid()
{  
        taskid=id;
         if ( is_sender )
         {
           SC_THREAD( _sender );
         }

}


MyTask_t::~MyTask_t()
    throw ()
{
    // Nothing to do.
}

// ////////////////////////////////////////////////////////////////
// Inherited interface methods.
// ////////////////////////////////////////////////////////////////


void MyTask_t::b_transport( tlm::tlm_generic_payload & p, sc_core::sc_time & t )
{
	char result;
	bool c;

	if( p.get_command() == Scnsl::Tlm::PACKET_COMMAND )
	{
    	result = * reinterpret_cast< char * >( p.get_data_ptr() );

#if (SCNSL_LOG >= 1)
        std::stringstream ss;
        ss << "RECEIVE data: " << result << ", size: " << p.get_data_length() << ".";
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


void MyTask_t::_sender(char *payload,)
    throw ()
{
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 0 ;
  	Payload_t *p=static_cast<Payload_t *>(malloc(sizeof(Payload_t)*sizeof(p)));
 

    while ( true )
    {
        p->Temperature= (rand()%25 + 25 );
        p->id=taskid;
        std::cout<<"Sender name "<<name()<< " ,Task id= "<<p->id<< " ,Temperature="<< p->Temperature<< "\u2103 \t"<< std::endl;;
        p->sender_times= sc_core::sc_time_stamp().to_double(); 
        TlmTask_if_t::send( tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t)); 
        wait(10,sc_core::SC_MS);
    }
 
}

  
