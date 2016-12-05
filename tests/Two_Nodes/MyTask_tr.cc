 

#include <sstream>
#include "MyTask_tr.hh"

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////
struct Payload_t
{
  int id;			//4
  double sender_times;        //8
//  Scnsl::Core::byte_t sender[10];	//5
   int Temperature;			 
};
MyTask_tr::MyTask_tr( const sc_core::sc_module_name modulename,
                    const task_id_t id,
                    Scnsl::Core::Node_t * n,
                    const size_t proxies )
    throw ():
    // Parents:
    Scnsl::Tlm::TlmTask_if_t( modulename, id, n, proxies ),
    // Fields:
    _packetArrivedEvent()
{
  
 
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
	Payload_t *temp=NULL; 
	bool c;
	if( p.get_command() == Scnsl::Tlm::PACKET_COMMAND )
	{
    	temp = reinterpret_cast<Payload_t *>( p.get_data_ptr() );
       double txtime=(temp->sender_times);
      double  rxtime=sc_core::sc_time_stamp().to_double() ;
std::cout << "Collector name: "<<name()<<" RECEIVED data: " << temp->Temperature << ", size: " << p.get_data_length()<<" delay "<< (rxtime-txtime)*1e-12<<std::endl  ;//sc_core::sc_time_stamp() <<".";
 

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
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 0;
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
 
