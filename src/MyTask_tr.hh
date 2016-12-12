 

#ifndef MYTASK_TR_HH
#define MYTASK_TR_HH

#include <systemc>
#include <scnsl.hh>
 
class MyTask_tr:
    public Scnsl::Tlm::TlmTask_if_t
{
public:

    SC_HAS_PROCESS( MyTask_tr );
 
    MyTask_tr( const sc_core::sc_module_name modulename,
              const task_id_t id,
              Scnsl::Core::Node_t * n,
              const size_t proxies )
    throw ();

    /// @brief Virtual destructor.
    virtual
    ~MyTask_tr()
    throw ();

    /// @name Inherited interface methods.
    //@{


    virtual
    void b_transport( tlm::tlm_generic_payload & p, sc_core::sc_time & t );

    //@}

    /// @brief Signals when a packet is arrived.
    sc_core::sc_event _packetArrivedEvent;

private:

    /// @name Processes.
    //@{

    /// @brief Sender process.
    void _sender()
        throw ();
  
    /// @brief Disabled copy constructor.
    MyTask_tr( const MyTask_tr & );

    /// @brief Disabled assignment operator.
    MyTask_tr & operator = ( MyTask_tr & );
};



#endif

