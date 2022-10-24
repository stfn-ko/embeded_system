///////////////////////////////////////////////////////////////////////////////
/// mq.cpp
///
/// Task message queuing system
///
/// Mechanism to allow IPC between interrupt and task contexts
///
/// Dr J A Gow 2020
///
///////////////////////////////////////////////////////////////////////////////

#include "mq.h"
#include "EventReceiver.h"
#include "interrupts.h"
#include <stdlib.h>

namespace Kernel {

	typedef class MESSAGEHANDLER * PMESSAGEHANDLER;
	class MESSAGEHANDLER {
		public:
			PMESSAGEHANDLER pNextHandler;
			virtual void Call(int id, void * context)=0;
			virtual boolean checkHandler(void * handler)=0;
			MESSAGEHANDLER(PMESSAGEHANDLER NextHandler) : pNextHandler(NextHandler) {};
			virtual ~MESSAGEHANDLER() {};
	};

	typedef class MHFUNCTION * PMHFUNCTION;
	class MHFUNCTION : public MESSAGEHANDLER {
		public:
			PFNMSGHANDLER	msgHandler;
			virtual void Call(int id, void * context) { this->msgHandler(context); };
			virtual boolean checkHandler(void * handler) { return (PFNMSGHANDLER)handler==msgHandler; };
			MHFUNCTION(PFNMSGHANDLER pHandler, PMESSAGEHANDLER NextHandler) : msgHandler(pHandler), MESSAGEHANDLER(NextHandler) {};
			virtual ~MHFUNCTION() {};
	};

	typedef class MHCLASS * PMHCLASS;
	class MHCLASS : public MESSAGEHANDLER {
		public:
			EventReceiver *	pReceiver;
			virtual void Call(int id, void * context) { this->pReceiver->EventHandler(id,context); };
			virtual boolean checkHandler(void * handler) { return static_cast<EventReceiver *>(handler)==pReceiver; };
			MHCLASS(EventReceiver * pReceiver, PMESSAGEHANDLER NextHandler) : pReceiver(pReceiver), MESSAGEHANDLER(NextHandler) {};
			virtual ~MHCLASS() {};
	};

	typedef class MESSAGE * 	PMESSAGE;
	class MESSAGE {
		public:
			int 		msgid;
			void *		context;
			MQOWNER		CallerOwns;
			PMESSAGE 	pNextMsg;
			MESSAGE(int id, void * context, MQOWNER owner) : msgid(id),context(context),CallerOwns(owner),pNextMsg(NULL) {};
	};

	// message queue block

	class MQInternals {
		public:
			PMESSAGEHANDLER		QueueBlock[MSG_MAX_MSG_IDS];
			PMESSAGE			MsgQueueFirst;
			PMESSAGE			MsgQueueLast;
	};

	//////////////////////////////////////////////////////////////////////////////
	/// MQClass
	///
	/// CONSTRUCTOR
	///
	/// Initialize the message queue block
	///
	/// @context: TASK
	/// @scope:   EXPORTED
	/// @param:   none
	///
	//////////////////////////////////////////////////////////////////////////////

	MQClass::MQClass(void)
	{
		MQInternals * pInternals=new MQInternals;
		for(int idx=0;idx<MSG_MAX_MSG_IDS;idx++) {
			pInternals->QueueBlock[idx]=(PMESSAGEHANDLER)NULL;
		}
		pInternals->MsgQueueFirst=(PMESSAGE)NULL;
		pInternals->MsgQueueLast=(PMESSAGE)NULL;
		internals=(void *)pInternals;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// Get
	///
	/// Returns a reference to the singleton class.
	///
	/// @context: ANY
	/// @scope: PUBLIC, STATIC
	/// @param: none
	/// @return: MQClass&
	///
	//////////////////////////////////////////////////////////////////////////////

	static MQClass& MQClass::Get(void)
	{
		static MQClass mq;
		return mq;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// Subscribe
	///
	/// Subscribe a static function to a given message. Pass address of handler
	/// function to be called.
	///
	/// @context:	TASK
	/// @scope:     EXPORTED
	/// @param:     PFNMSGHANDLER handler - pointer to the handler function
	/// @param:     void * context - pointer to context data
	/// @param:     boolean CallerOwns - set TRUE if the message queue is not to
	///	            free the context data when done.
	/// @return:	zero if successfully queued, nonzero if error occurred
	///
	//////////////////////////////////////////////////////////////////////////////

	int MQClass::Subscribe(int msgid, PFNMSGHANDLER handler)
	{
		MQInternals * pInternals = (MQInternals *)internals;
		int rc=-1;
		if((msgid<MSG_MAX_MSG_IDS) && (handler!=NULL)) {
			// don't attach it twice
			PMESSAGEHANDLER pHead=pInternals->QueueBlock[msgid];
			while(pHead) {
				if(pHead->checkHandler(handler)) {
					break;
				}
				pHead=pHead->pNextHandler;
			}
			if(!pHead) {
				pHead=new MHFUNCTION(handler,pInternals->QueueBlock[msgid]);
				if(pHead) {
					pInternals->QueueBlock[msgid]=pHead;
					rc=0;
				}
			}
		}
		return rc;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// Subscribe
	///
	/// Subscribe to a given message. Pass address of handler function to be
	/// called.
	///
	/// @context:	TASK
	/// @scope:     EXPORTED
	/// @param:     PFNERHANDLER handler - pointer to a member function handler
	/// @param:     void * context - pointer to context data
	/// @param:     boolean CallerOwns - set TRUE if the message queue is not to
	///	            free the context data when done.
	/// @return:	zero if successfully queued, nonzero if error occurred
	///
	//////////////////////////////////////////////////////////////////////////////

	int MQClass::Subscribe(int msgid, EventReceiver * handler)
	{
		MQInternals * pInternals = (MQInternals *)internals;
		int rc=-1;
		if((msgid<MSG_MAX_MSG_IDS) && (handler!=NULL)) {
			// don't attach it twice
			PMESSAGEHANDLER pHead=pInternals->QueueBlock[msgid];
			while(pHead) {
				if(pHead->checkHandler(handler)) {
					break;
				}
				pHead=pHead->pNextHandler;
			}
			if(!pHead) {
				pHead=new MHCLASS(handler,pInternals->QueueBlock[msgid]);
				if(pHead) {
					pInternals->QueueBlock[msgid]=pHead;
					rc=0;
				}
			}
		}
		return rc;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// Unsubscribe
	///
	/// Unsubscribe a static function from a given message. Pass address of handler
	/// function to be unsusbscribed.
	///
	/// @context:	TASK
	/// @scope:     EXPORTED
	/// @param:     msgid - message id to unsubscribe
	/// @param:     PFNMSGHANDLER handler - pointer to the handler function
	/// @return:	zero if successfully queued, nonzero if error occurred
	///
	//////////////////////////////////////////////////////////////////////////////

	int MQClass::Unsubscribe(int msgid, PFNMSGHANDLER handler)
	{
		MQInternals * pInternals = (MQInternals *)internals;
		int rc=-1;
		if(msgid<MSG_MAX_MSG_IDS) {

		// don't attach it twice

			PMESSAGEHANDLER pHead=pInternals->QueueBlock[msgid];
			PMESSAGEHANDLER pPrev=NULL;

			while(pHead) {
				if(pHead->checkHandler(handler)) {
					if(!pPrev) {
						pInternals->QueueBlock[msgid]=pHead->pNextHandler;
					} else {
						pPrev->pNextHandler=pHead->pNextHandler;
					}
					delete pHead;
					rc=0;
					break;
				}
				pPrev=pHead;
				pHead=pHead->pNextHandler;
			}
		}
		return rc;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// Unsubscribe
	///
	/// Unsubscribe an EventHandler (or descendent) class from a given message.
	/// Pass address of class to be unsusbscribed.
	///
	/// @context:	TASK
	/// @scope:     EXPORTED
	/// @param:     msgid - message id to unsubscribe
	/// @param:     PFNMSGHANDLER handler - pointer to the handler function
	/// @return:	zero if successfully queued, nonzero if error occurred
	///
	//////////////////////////////////////////////////////////////////////////////

	int MQClass::Unsubscribe(int msgid, EventReceiver * handler)
	{
		MQInternals * pInternals = (MQInternals *)internals;
		int rc=-1;
		if(msgid<MSG_MAX_MSG_IDS) {

		// don't attach it twice

			PMESSAGEHANDLER pHead=pInternals->QueueBlock[msgid];
			PMESSAGEHANDLER pPrev=NULL;

			while(pHead) {
				if(pHead->checkHandler(handler)) {
					if(!pPrev) {
						pInternals->QueueBlock[msgid]=pHead->pNextHandler;
					} else {
						pPrev->pNextHandler=pHead->pNextHandler;
					}
					delete pHead;
					rc=0;
					break;
				}
				pPrev=pHead;
				pHead=pHead->pNextHandler;
			}
		}
		return rc;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// UnsubscribeAllIDs
	///
	/// Unsubscribe an EventHandler (or descendent) class from all messages
	/// Pass address of class to be unsusbscribed. This is called by the event
	/// handler class destructors so we don't send events off into wonderland
	/// when a handler is deleted
	///
	/// @context:	TASK
	/// @scope:     EXPORTED
	/// @param:     PFNMSGHANDLER handler - pointer to the handler function
	/// @return:	zero if successfully queued, nonzero if error occurred
	///
	//////////////////////////////////////////////////////////////////////////////

	int MQClass::UnsubscribeAllIDs(EventReceiver * handler)
	{
		for(int idx=0;idx<MSG_MAX_MSG_IDS;idx++) {
			this->Unsubscribe(idx,handler);
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	/// Post
	///
	/// Post a message. Pass id of message and context to be passed.
	///
	/// @context:	TASK, INTERRUPT
	/// @scope:     EXPORTED
	/// @param:     int msgid
	/// @param:     void * context - pointer to context data
	/// @param:     boolean CallerOwns - set TRUE if the message queue is not to
	///	            free the context data when done.
	/// @param:     boolean isIntCtx - set this TRUE if called from an interrupt
	///             context.
	///
	/// @return:	zero if successfully posted, nonzero if error occurred
	///
	//////////////////////////////////////////////////////////////////////////////

	int MQClass::Post(int msgid, void * context, MQOWNER CallerOwns, MQCONTEXT isIntCtx)
	{
		MQInternals * pInternals = (MQInternals *)internals;
		int rc=-1;
		if(isIntCtx!=MQ_CONTEXT_INTERRUPT) INTDisableMasterInterrupts();
		if(msgid<MSG_MAX_MSG_IDS && (msgid!=MSG_ID_NOMESSAGE)) {
			PMESSAGE newMessage=new MESSAGE(msgid,context,CallerOwns);
			if(newMessage) {
				if(pInternals->MsgQueueFirst==NULL) {

					// the first in the list

					pInternals->MsgQueueFirst=pInternals->MsgQueueLast=newMessage;
				} else {

					// attach to the bottom

					pInternals->MsgQueueLast->pNextMsg=newMessage;
					pInternals->MsgQueueLast=newMessage;
				}
				rc=0;
			}
		}
		if(isIntCtx!=MQ_CONTEXT_INTERRUPT) INTEnableMasterInterrupts();
		return rc;
	}

	//////////////////////////////////////////////////////////////////////////////
	/// MQLoop
	///
	/// Called by the task handler. Processes up to MaxMessages and returns
	///
	/// @context:	TASK
	/// @scope:     EXPORTED
	/// @param:     int MaxMessages -  maximum number of messages to process in
	///             this iteration
	/// @return:    none
	///
	//////////////////////////////////////////////////////////////////////////////

	void MQClass::Loop(int MaxMessages)
	{
		MQInternals * pInternals = (MQInternals *)internals;
		while(MaxMessages) {

			// pop the first off the queue

			INTDisableMasterInterrupts();
			PMESSAGE msg=pInternals->MsgQueueFirst;
			if(msg) {
				pInternals->MsgQueueFirst=msg->pNextMsg;
				if(pInternals->MsgQueueFirst==NULL) {
					pInternals->MsgQueueLast=NULL;
				}
			}
			INTEnableMasterInterrupts();

			// send it in
			if(msg) {
				PMESSAGEHANDLER curHandler=pInternals->QueueBlock[msg->msgid];
				while(curHandler) {
					curHandler->Call(msg->msgid,msg->context);
					curHandler=curHandler->pNextHandler;
				}

				// free the message

				if(msg->CallerOwns!=MQ_OWNER_CALLER) {
					if(msg->context != NULL) {
						delete msg->context;
					}
				}
				delete msg;
			}
			MaxMessages--;
		}
	}

}
