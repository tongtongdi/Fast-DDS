/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima RTPS is licensed to you under the terms described in the
 * EPROSIMARTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file RTPSWriter.h
 */


#ifndef RTPSWRITER_H_
#define RTPSWRITER_H_

#include "eprosimartps/rtps/Endpoint.h"
//
//#include "eprosimartps/history/WriterHistory.h"
//
//
//#include "eprosimartps/writer/RTPSMessageGroup.h"
//
//#include "eprosimartps/qos/WriterQos.h"
//#include "eprosimartps/pubsub/Publisher.h"
//
//#include "eprosimartps/qos/ParameterList.h"
//#include "eprosimartps/pubsub/attributes/PublisherAttributes.h"



namespace eprosima {

namespace rtps {

class UnsentChangesNotEmptyEvent;
class WriterListener;
class CacheChangePool;

/**
 * Class RTPSWriter, manages the sending of data to the readers. Is always associated with a HistoryCache.
 * @ingroup WRITERMODULE
 */
class RTPSWriter: public Endpoint
{
	friend class LivelinessPeriodicAssertion;
public:
	RTPSWriter(ParticipantImpl*,GUID_t guid,EndpointAttributes att);
	virtual ~RTPSWriter();

	/**
	 * Create a new change based on the provided data and instance handle.
	 * It assigns the correct values to each field and copies the data from data to change. The SequenceNumber is NOT assigned here but actually during
	 * the call to add_change in the HistoryCache, to prevent incorrect increments.
	 * @param changeKind The type of change.
	 * @param data Pointer to the serialized data that must be included in the change.
	 * @param change_out Pointer to pointer to return the change.
	 * @return True if correct.
	 */
	bool new_change(ChangeKind_t changeKind,void* data,CacheChange_t** change_out);



private:

	//!Is the data sent directly or announced by HB and THEN send to the ones who ask for it?.
	bool m_pushMode;
	//!Group created to send messages more efficiently
	RTPSMessageGroup_t m_cdrmessages;
	//!Writer Listener
	WriterListener* mp_listener;
	//!INdicates if the liveliness has been asserted
	bool m_livelinessAsserted;
	//!Event that manages unsent changes
	UnsentChangesNotEmptyEvent* mp_unsetChangesNotEmpty;
	//!CacheChangePool
	CacheChangePool* mp_changePool;

public:

	/**
	 * Add a change to the unsent list.
	 * @param change Pointer to the change to add.
	 */
	virtual void unsent_change_add(CacheChange_t* change)=0;
	/**
	 * Indicate the writer that a change has been removed by the history due to some HistoryQos requirement.
	 * @param a_change Pointer to the change that is going to be removed.
	 * @return True if removed correctly.
	 */
	virtual bool change_removed_by_history(CacheChange_t* a_change)=0;
	/**
	 * Add a matched reader.
	 * @param rdata Pointer to the ReaderProxyData object added.
	 * @return True if added.
	 */
	virtual bool matched_reader_add(ReaderProxyData* rdata)=0;
	/**
	 * Remove a matched reader.
	 * @param rdata Pointer to the object to remove.
	 * @return True if removed.
	 */
	virtual bool matched_reader_remove(ReaderProxyData* rdata)=0;
	/**
	 * Tells us if a specific Reader is matched against this writer
	 * @param rdata Pointer to the ReaderProxyData object
	 * @return True if it was matched.
	 */
	virtual bool matched_reader_is_matched(ReaderProxyData* rdata)=0;
	/**
	 * Remove the change with the minimum SequenceNumber
	 * @return True if removed.
	 */
	virtual bool removeMinSeqCacheChange()=0;
	/**
	 * Remove all changes from history
	 * @param n_removed Pointer to return the number of elements removed.
	 * @return True if correct.
	 */
	virtual bool removeAllCacheChange(size_t* n_removed)=0;
	//!Get the number of matched subscribers.
	virtual size_t getMatchedSubscribers()=0;
	//!Add a new change to the history.
	bool add_new_change(ChangeKind_t kind,void*Data);

	virtual void unsent_changes_not_empty()=0;

	/**
	 * Get the minimum sequence number in the HistoryCache.
	 * @param[out] seqNum Pointer to store the sequence number
	 * @param[out] writerGuid Pointer to store the writerGuid.
	 * @return True if correct.
	 */
	bool get_seq_num_min(SequenceNumber_t* seqNum,GUID_t* writerGuid);
	/**
	 * Get the maximum sequence number in the HistoryCache.
	 * @param[out] seqNum Pointer to store the sequence number
	 * @param[out] writerGuid Pointer to store the writerGuid.
	 * @return True if correct.
	 */
	bool get_seq_num_max(SequenceNumber_t* seqNum,GUID_t* writerGuid);
	//!Add a change to the History.
	bool add_change(CacheChange_t*change);

	bool get_last_added_cache(CacheChange_t**change)
	{
		m_writer_cache.get_max_change(change);
		return true;
	}

	void setQos( WriterQos& qos,bool first)
	{
		return m_qos.setQos(qos,first);
	}
	const WriterQos& getQos(){return m_qos;}
	bool canQosBeUpdated(WriterQos& qos){return m_qos.canQosBeUpdated(qos);}

	PublisherListener* getListener(){return mp_listener;}
	void setListener(PublisherListener* plisten){mp_listener = plisten;}

	ParameterList_t* getInlineQos(){return &m_inlineQos;}

	bool getLivelinessAsserted()
	{
		return m_livelinessAsserted;
	}

	void setLivelinessAsserted(bool live)
	{
		m_livelinessAsserted = live;
	}

	//!Get the number of changes in the History.
	size_t getHistoryCacheSize()
	{
		return this->m_writer_cache.getHistorySize();
	}
protected:

	//!Changes associated with this writer.
	WriterHistory m_writer_cache;


	/**
	 * Initialize the header message that is used in all RTPS Messages.
	 */
	void init_header();


	WriterQos m_qos;
	//Publisher* m_Pub;
	PublisherListener* mp_listener;

	ParameterList_t m_inlineQos;

	bool m_livelinessAsserted;

	UnsentChangesNotEmptyEvent* mp_unsetChangesNotEmpty;

};

} /* namespace rtps */
} /* namespace eprosima */

#endif /* RTPSWRITER_H_ */