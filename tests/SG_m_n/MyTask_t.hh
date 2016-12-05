 


#ifndef MYTASK_T_HH
#define MYTASK_T_HH

#include <systemc>
#include <scnsl.hh>
 
class MyTask_t:
    public Scnsl::Tlm::TlmTask_if_t
{
public:

    SC_HAS_PROCESS( MyTask_t );
    MyTask_t( const sc_core::sc_module_name modulename,
              const bool is_sender,
              const task_id_t id,
              Scnsl::Core::Node_t * n,
              const size_t proxies )
    throw ();

    /// @brief Virtual destructor.
    virtual
    ~MyTask_t()
    throw ();

    /// @name Inherited interface methods.
    //@{


    virtual
    void b_transport( tlm::tlm_generic_payload & p, sc_core::sc_time & t );

    //@}

    /// @brief Signals when a packet is arrived.
    sc_core::sc_event _packetArrivedEvent;
    unsigned int taskid;
private:

    /// @name Processes.
    //@{

    /// @brief Sender process.
    void _sender()
        throw ();
 


    /// @brief Disabled copy constructor.
    MyTask_t( const MyTask_t & );

    /// @brief Disabled assignment operator.
    MyTask_t & operator = ( MyTask_t & );
};



#endif

