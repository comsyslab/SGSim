 

#include <sstream>
#include "MyTask_tr.hh"

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////
int rxTemperature=0;
unsigned int taskid;
 double txtime=0;
 double rxtime=0;
struct Payload_t
{
  unsigned int id;			//4
  double sender_times;        //8
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
  if(id==1)
   {
                SC_THREAD( _sender ); 
taskid=id; 
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
	Payload_t *temp=NULL; 
	bool c;
	if( p.get_command() == Scnsl::Tlm::PACKET_COMMAND )
	{
    	temp = reinterpret_cast<Payload_t *>( p.get_data_ptr() );
       txtime=(temp->sender_times);
      rxtime=sc_core::sc_time_stamp().to_double() ;
std::cout << "Task name: "<<name()<<" RECEIVED data: " << temp->Temperature << ", size: " << p.get_data_length()<<" delay "<< (rxtime-txtime)*1e-12<<std::endl  ;//sc_core::sc_time_stamp() <<".";
 rxTemperature=  temp->Temperature;

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
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 0 ;
  	Payload_t *p=static_cast<Payload_t *>(malloc(sizeof(Payload_t)*sizeof(p)));
 

    while ( true )
    {wait(_packetArrivedEvent);
        p->Temperature= (rxTemperature );
        p->id=taskid;
        std::cout<<"Task named "<<name()<< " ,Task id= "<<p->id<< " ,route Temperature="<< p->Temperature<< "\u2103 \t"<< std::endl;;
                       p->sender_times= txtime; 
     TlmTask_if_t::send( tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t)); 
   wait(10,sc_core::SC_MS);
    }
 
}
 
