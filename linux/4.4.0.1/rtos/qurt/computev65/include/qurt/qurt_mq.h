#ifndef QURT_MQ_H
#define QURT_MQ_H
/**
  @file  qurt_mq.h

  @brief  Prototypes of secure message queues API functions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2019-2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================*/
#include <qurt_types.h>
#include <qurt_error.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QURT_MQ_NAME_MAXLEN            16  /**< Maximum name length. */

/* this enum needs to be generated in accordance to process class class numbers
   for now it is made to match generated version, do not change this unless 
   there is a corresponding change in the process_class.py, indecies start from 0
   basically: QURT_MQ_SECURITY_SCOPE_<x> = (1 << QURTK_process_class_index_<x>)
*/
typedef enum {
    QURT_MQ_SECURITY_SCOPE_KERNEL =   ( 1 << 0 ),
    QURT_MQ_SECURITY_SCOPE_SRM =      ( 1 << 1 ),
    QURT_MQ_SECURITY_SCOPE_SECURE =   ( 1 << 2 ),
    QURT_MQ_SECURITY_SCOPE_CPZ =      ( 1 << 3 ),
    QURT_MQ_SECURITY_SCOPE_ROOT =     ( 1 << 4 ),
    QURT_MQ_SECURITY_SCOPE_SIGNED =   ( 1 << 5 ),
    QURT_MQ_SECURITY_SCOPE_UNSIGNED = ( 1 << 6 )
} qurt_mq_security_scope_t;

typedef enum {
    QURT_MQ_CARDINALITY_PTP =   (1 << 0),
    QURT_MQ_CARDINALITY_MTO =   (1 << 1)
}qurt_mq_cardinality_t;

typedef unsigned int qurt_mqd_t;

typedef union{
    struct {
        unsigned int perms:2;
        unsigned int cardinality:1;
        unsigned int blocking:1;

        qurt_mq_security_scope_t creator_scope: 8;
        qurt_mq_security_scope_t allowed_scope: 8; //can be a bitmask in case of MTO
        unsigned int queue_closed: 1;
        unsigned int reserved: 11;
    }; //try to do anonymous struct
    unsigned int raw;
} qurt_mq_flags_t;


/* permissions are from qurt_types.h , block X though */
#if 0
/** Memory access permission. */
typedef enum {
        QURT_PERM_READ=0x1, /**< */
        QURT_PERM_WRITE=0x2,  /**< */
        QURT_PERM_EXECUTE=0x4,  /**< */
        QURT_PERM_FULL=QURT_PERM_READ|QURT_PERM_WRITE|QURT_PERM_EXECUTE,  /**< */
} qurt_perm_t;
#endif

struct qurt_mq_attr {
   /* @brief Configured flags. Only meaningful with get_attr() */
   unsigned flags;  //Only used for qurt_mq_flags_t.perms

   /* @brief Maximum number of messages. Used with create() and get_attr */
   unsigned mq_maxmsg;

   /* @brief Maximum size (bytes) of message in receiver facing queue.
             i.e from sender -> receiver */
   unsigned short mq_send_msgsize;

   /* @brief Maximum size (bytes) of message in sender facing queue.
             i.e from receiver -> sender */
   unsigned short mq_recv_msgsize;

   /* @brief Process ID of client that is allowed to open the message queue
             which got created using qurt_mq_create() */
   unsigned client_pid;

    /* @brief cardinality of message queue connection, see below */
    qurt_mq_cardinality_t    cardinality; 

    /* @brief security scope of the senders to the queue */ 
    qurt_mq_security_scope_t scope;
};

/**@ingroup func_qurt_mq_attr_init
  Initialize attributes to default values used for creating the queue.

  The initialize operation sets the following default attribute values: \n
  - flag - QURT_PERM_READ | QURT_PERM_WRITE \n
  - maxmsg - 1 \n
  - mq_send_msgsize - 8 \n
  - mq_recv_msgsize - 8 \n
  - sender_pid -  -1 \n    
  - cardinality -  QURT_MQ_CARDINALITY_PTP \n    
  - scope -  QURT_MQ_SECURITY_SCOPE_SIGNED \n    

  @datatypes
  #qurt_mq_attr 
  
  @param[in,out] attr Pointer to the initialized mq object.

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_init(struct qurt_mq_attr * attr);

/**@ingroup qurt_mq_attr_set_send_msgsize
  Sets the message size in bytes the sender can send.
  Maximum message length is configurable using xml configuration, however, limited to maximum value of 62 bytes
  
  @datatypes
  #qurt_mq_attr

  @param[in,out] attr Pointer to the mq object.
  @param[in] len     Length of message in bytes.

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_set_send_msgsize (struct qurt_mq_attr *attr, size_t len);

/**@ingroup qurt_mq_attr_set_recv_msgsize
  Sets the message size in bytes the receiver can read.
  Maximum message length is configurable using xml configuration, however, limited to maximum value of 62 bytes
  
  @datatypes
  #qurt_mq_attr

  @param[in,out] attr Pointer to the mq object.
  @param[in] len     Length of message in bytes.

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_set_recv_msgsize (struct qurt_mq_attr *attr, size_t len);

/**@ingroup qurt_mq_attr_set_maxmsg
  Sets the maximum message that can be queued in the message queue.
  Message depth is configurable using xml configuration 
  
  @datatypes
  #qurt_mq_attr

  @param[in,out] attr Pointer to the mq object.
  @param[in] depth     Maximum message that can be queued.

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_set_maxmsg (struct qurt_mq_attr *attr, unsigned int depth);

/**@ingroup qurt_mq_attr_set_scope
  Sets the scope of the message queue. A message queue created with a security 
  scope will allow only a process class of that scope to open a message queue
  
  @datatypes
  #qurt_mq_attr
  #qurt_mq_security_scope_t

  @param[in,out] attr Pointer to the mq object.
  @param[in] scope     Scope of the message queue.
  Following are the scope supported for a message queue

  QURT_MQ_SECURITY_SCOPE_KERNEL
  QURT_MQ_SECURITY_SCOPE_SRM
  QURT_MQ_SECURITY_SCOPE_SECURE
  QURT_MQ_SECURITY_SCOPE_CPZ
  QURT_MQ_SECURITY_SCOPE_ROOT
  QURT_MQ_SECURITY_SCOPE_SIGNED
  QURT_MQ_SECURITY_SCOPE_UNSIGNED

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_set_scope (struct qurt_mq_attr *attr, qurt_mq_security_scope_t scope);


/**@ingroup qurt_mq_attr_set_client_pid
  Sets the client_pid which can open this message queue.
  If client_pid is set allowed_scope to open MQ shall not be considered.
  
  @datatypes
  #qurt_mq_attr

  @param[in,out] attr Pointer to the mq object.
  @param[in] client_pid  Valid PID for client process

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_set_client_pid (struct qurt_mq_attr *attr, unsigned client_pid);

/**@ingroup qurt_mq_attr_set_flags
  Sets the properties of the message queues. 
  The current implementation only uses to set the permission for message queue using flag attribute.
  Default is QURT_PERM_READ | QURT_PERM_WRITE, explicit permission is not implemented.
  
  @datatypes
  #qurt_mq_attr

  @param[in,out] attr Pointer to the mq object.
  @param[in] flags     Permission for message queue  

  @return
  None.

  @dependencies
  None.
*/
void qurt_mq_attr_set_flags (struct qurt_mq_attr *attr, unsigned int flags);

/**@ingroup qurt_mq_create
   Create a message queue with the provided name and attributes. 
   The calling process becomes the owner of the queue.
   Name of the message queue is limited to 16 characters including NULL terminator. 
  
  @datatypes
  #qurt_mq_attr
  #qurt_mqd_t

  @param[out] mqd Returns a pointer to the message queue identifier if 
              the message queue  was successfully created.
  @param[in] name     String Identifier of the message queue
  @param[in] attr     Pointer to the initialized message queue attribute 
             structure that specifies the attributes of the created message queue

  @return
  QURT_EOK  Message Queue created.
  QURT_EINVALID   Invalid arguments
  QURT_ENOSPC    Maximum number of queues in the system is exceeded.

  @dependencies
  None.
*/
int qurt_mq_create(qurt_mqd_t *mqd, const char *name, struct qurt_mq_attr * attr);

/**@ingroup qurt_mq_open
  Open a message queue connection between a process and a created message queue. 
  
  @datatypes
  #qurt_mq_attr
  #qurt_mqd_t

  @param[out] mqd Returns a pointer to the message queue 
              identifier if the message queue  was successfully created.
  @param[in] name    String Identifier of the message queue 
  @param[in] flags    flag contains properties that defines the behavior of message queue connection
             Permissions:
             - #QURT_PERM_READ \n
             - #QURT_PERM_WRITE \n
             - #QURT_PERM_READ | QURT_PERM_WRITE @tablebulletend  
             Default is QURT_PERM_READ | QURT_PERM_WRITE, explicit permission is not implemented
             
             cardinality 
             - #QURT_MQ_CARDINALITY_PTP \n      
             - #QURT_MQ_CARDINALITY_MTO @tablebulletend
             Default is QURT_MQ_CARDINALITY_PTP  , QURT_MQ_CARDINALITY_MTO is not implemented

             blocking  suspend thread until the message queue with the given name is created

             scope   security boundary to which the message queue and its users are constrained to. It is coupled with process privilege level/scope
             -	#QURT_MQ_SECURITY_SCOPE_KERNEL   \n
             -	#QURT_MQ_SECURITY_SCOPE_SRM      \n
             -	#QURT_MQ_SECURITY_SCOPE_SECURE   \n
             -	#QURT_MQ_SECURITY_SCOPE_CPZ      \n
             -	#QURT_MQ_SECURITY_SCOPE_ROOT     \n
             -	#QURT_MQ_SECURITY_SCOPE_SIGNED   \n
             -	#QURT_MQ_SECURITY_SCOPE_UNSIGNED @tablebulletend
  @return
  QURT_EOK  Message Queue connection successfully opened.
  QURT_EFAILED  Message queue connection failed , if non-blocking message queue
  QURT_ENOTALLOWED   Open failed due to security scope mismatch


  @dependencies
  None.
*/
int qurt_mq_open (qurt_mqd_t *mqd, const char *name, qurt_mq_flags_t flags);

/**@ingroup qurt_mq_send
  Send a message over message queue.
  - If the message queue is full, the calling thread shall be 
  suspended until space becomes available to enqueue the message. 
  - If there exist a thread suspended on an empty queue 
  to receive a message,  qurt_mq_send shall resume that thread. 
  @datatypes
  #qurt_mqd_t

  @param[in] mqd Pointer to the message queue identifier.
  @param[in] msg_ptr     Pointer to message buffer  
  @param[in] msg_len     Length of message buffer in bytes  

  @return
  QURT_EOK  Message Queue send was successful.
  QURT_EMSGSIZE  Message size in msg_len field is greater than max_message_len specified 		     during queue creation.
  QURT_ENOTALLOWED   Send failed due to security scope mismatch

  @dependencies
  None.
*/
int qurt_mq_send(qurt_mqd_t mqd, const char *msg_ptr, size_t msg_len); 

/**@ingroup qurt_mq_send_timed
  Send a message over message queue.
  - If the message queue is full, the calling thread shall be 
  suspended until space becomes available to enqueue the message or until timeout is reached. 
  - If there exist a thread suspended on an empty queue 
  to receive a message, qurt_mq_send_timed shall return with possible return codes.
  - If timeout is reached, qurt_mq_send_timed shall return QURT_ETIMEOUT.
  @datatypes
  #qurt_mqd_t

  @param[in] mqd Pointer to the message queue identifier.
  @param[in] msg_ptr     Pointer to message buffer
  @param[in] duration    duration Interval (in microseconds) duration value must be
             between #QURT_TIMER_MIN_DURATION and #QURT_TIMER_MAX_DURATION     
  @param[in] msg_len     Length of message buffer in bytes  

  @return
  QURT_EOK  Message queue send was successful.
  QURT_EMSGSIZE  Message size in msg_len field is greater than max_message_len specified during queue creation.
  QURT_ENOTALLOWED   Send failed due to security scope mismatch
  QURT_ETIMEDOUT - timeout
  
  @dependencies
  None.
*/
int qurt_mq_send_timed(qurt_mqd_t mqd, const char *msg_ptr, unsigned long long int duration, size_t msg_len);

 /**@ingroup qurt_mq_recv
  Receive a message from message queue. 
  -If the message queue is empty, the calling thread shall be 
   suspended until a message is enqueued in the message queue. 
  -If there exist a thread suspended on a full queue to 
   send a message, qurt_mq_recv shall resume the thread.

  @datatypes
  #qurt_mqd_t

  @param[in] mqd Pointer to the message queue identifier.
  @param[in] msg_ptr     Pointer to message buffer  
  @param[in,out] msg_len     Pointer to length of message buffer.  

  @return
  QURT_EOK  Message Queue created.
  QURT_EINVALID  msg ptr or msg_len ptr are NULL
  QURT_EBADR    message que descriptior, mqd is invalid
  QURT_EBADF    Message queue has been closed by the sender.

  @dependencies
  None.
*/
int qurt_mq_recv(qurt_mqd_t mqd, unsigned char *msg_ptr, size_t *msg_len);

 /**@ingroup qurt_mq_recv_timed
  Receive a message from message queue. 
  -If the message queue is empty, the calling thread shall be 
   suspended until a message is enqueued in the message queue or until timeout is reached.. 
  -If there exist a thread suspended on an full queue to 
   send a message, qurt_mq_recv_timed shall return with possible return codes.
  - If timeout is reached, qurt_mq_recv_timed shall return QURT_ETIMEOUT.
  
  @datatypes
  #qurt_mqd_t

  @param[in] mqd Pointer to the message queue identifier.
  @param[in] msg_ptr     Pointer to message buffer  
  @param[in] duration    duration Interval (in microseconds) duration value must be
             between #QURT_TIMER_MIN_DURATION and #QURT_TIMER_MAX_DURATION   
  @param[in,out] msg_len     Pointer to length of message buffer.  

  @return
  QURT_EOK  Message Queue created.
  QURT_EINVALID  msg ptr or msg_len ptr are NULL
  QURT_EBADR    message que descriptior, mqd is invalid
  QURT_EBADF    Message queue has been closed by the sender.
  QURT_ETIMEDOUT - timeout
  
  @dependencies
  None.
*/
int qurt_mq_recv_timed(qurt_mqd_t mqd, unsigned char *msg_ptr, unsigned long long int duration, size_t *msg_len);

 /**@ingroup qurt_mq_close
  Close message queue and disassociate the calling process (client) from the message queue 
  under this descriptor. Marks the queue as closed for the receiver. 
  This function is expected to be called from client side, if called 
  from server side, the function reduces to no-op and returns success. 

  @datatypes
  #qurt_mqd_t

  @param[in] mqd Pointer to the message queue identifier. 

  @return
  QURT_EOK  Message Queue close was successfully.
  QURT_EBADR  Invalid descriptor.
  QURT_ENOTALLOWED    Message queue close is not called from client side.

  @dependencies
  None.
*/
int qurt_mq_close(qurt_mqd_t mqd);

 /**@ingroup qurt_mq_destroy
  Destroy the message queue. This function ought to be 
  called from the process which called qurt_mq_create(). 

  @datatypes
  #qurt_mqd_t

  @param[in] mqd Pointer to the message queue identifier. 

  @return
  QURT_EOK  Message Queue destroy was successfully.
  QURT_EBADR  Invalid descriptor.
  QURT_ENOTALLOWED    Message queue close is not called from client side.

  @dependencies
  None.
*/
int qurt_mq_destroy(qurt_mqd_t mqd);


#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif
#endif //QURT_MQ_H
