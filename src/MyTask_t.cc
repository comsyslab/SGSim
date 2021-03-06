

#include <sstream>
#include "MyTask_t.hh"

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////



struct Payload_t
{
   unsigned int id;			 
   double sender_times;         
   int LoadRequest;			 
   const char*  TaskName;
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
//	name = modulename;
{  taskid=id;
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


void MyTask_t::b_transport( tlm::tlm_generic_payload & , sc_core::sc_time & )
{
 
}


// ////////////////////////////////////////////////////////////////
// Processes.
// ////////////////////////////////////////////////////////////////


void MyTask_t::_sender()
    throw ()
{
    const Scnsl::Tlm::tlm_taskproxy_id_t tp = 0 ;
  	Payload_t *p=static_cast<Payload_t *>(malloc(sizeof(Payload_t)*sizeof(p)));
 

    while ( true )
    {
        p->LoadRequest= (rand()%1000);
        p->id=taskid;
	p->TaskName= name();
//        std::cout<<"Sender name "<<name()<< " ,Task id= "<<p->id<< " ,Load Request="<< p->LoadRequest<< " Watt"<< std::endl;;
        p->sender_times= sc_core::sc_time_stamp().to_double(); 
        TlmTask_if_t::send( tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t)); 
        wait(rand()%60,sc_core::SC_SEC);
    }
 
}

  
