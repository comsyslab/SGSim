 

#include <sstream>
#include "MyTask_tr.hh"

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////
 int rxTemperature=0;
 unsigned int taskid;
 double txtime=0;
 double rxtime=0;
 double delay=0;
 double delayn=0;
 unsigned int kk= 0;
 const sc_core::sc_time h23(83800,sc_core::SC_SEC);;//time for printing the last hour of the simulation time (23:00 to 24:00) 

struct Payload_t
{
  unsigned int id;			 
  double sender_times;        
  int LoadRequest;			 
  const char*  TaskName;
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
//	const sc_core::sc_time h23=82800000000;
//	unsigned int k= 0;
	if( p.get_command() == Scnsl::Tlm::PACKET_COMMAND )
	{
    		temp = reinterpret_cast<Payload_t *>( p.get_data_ptr() );
        	txtime=(temp->sender_times);
        	rxtime=sc_core::sc_time_stamp().to_double() ;
        	delay=(rxtime-txtime);
        	delayn=delay*1e-12+delayn; 
if(sc_core::sc_time_stamp()>h23)
{		
//        	std::cout << "Receiver name: "<<name()<<" RECEIVED data: " << temp->LoadRequest << ", size: " << p.get_data_length()<<" delay "<< delay*1e-12 <<std::endl;//sc_core::sc_time_stamp() <<".";
		std::cout<<"\t Accumulated delay between ("<<name() <<") and  Sender Task ID "<< temp->id << " (named " << temp->TaskName<<") is equal: "<<delayn<<" sec"<<std::endl  ;
	         std::cout<<"\t Average delay between ("<<name() <<") and  Sender Task ID "<< temp->id << " (named " << temp->TaskName<<") is equal: "<<delayn/kk<<" sec"<<std::endl  ;
}
// std::cout<<"Current time is "<< sc_core::sc_time_stamp() << std::endl;

		_packetArrivedEvent.notify();
		kk++;
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
  
}
 
