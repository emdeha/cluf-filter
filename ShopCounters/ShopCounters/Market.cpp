#include "Market.h"

#include <assert.h>


Market::Market(int newNumberOfAllCashDesks, int newMaxClientsPerQueue)
	: numberOfAllCashDesks(newNumberOfAllCashDesks), maxClientsPerQueue(newMaxClientsPerQueue), 
	  marketState(numberOfAllCashDesks)
{
}

MarketState Market::GetMarketState() const
{
	return marketState;
}

ClientState Market::GetClientState(int clientID) const
{
	ClientState resultClientState;
	for (QueueList::const_iterator queue = clientState.begin(); queue != clientState.end(); ++queue)
	{
		for (Queue::const_iterator queueItem = queue->begin(); queueItem != queue->end(); ++queueItem)
		{
			if (queueItem->client->id == clientID)
			{
				return *queueItem;
			}
		}
	}
	return resultClientState;
}

// On every invocation of this method a 'tick' passes.
// Clients get reordered if necessary and new people come in.
void Market::AddClients(Client *clients, int number)
{
	assert (number > 0 && clients);

	// Cleans up old clients
	for (QueueList::iterator queue = clientState.begin(); queue != clientState.end(); ++queue)
	{
		while ( ! queue->empty())
		{
			if (queue->front().client->hasCreditCard)
			{
				queue->pop_front();
			}
			else
			{
				AdvanceAllWithOneTick(&*queue);
				break;
			}
		}
	}

	// Manipulate clients *evil grin*
	for (size_t idx = 0; idx < number; ++idx)
	{

	}
}

void Market::AdvanceAllWithOneTick(Queue *clientQueue)
{ 
	assert (clientQueue);

	// Hack for emulating ticks
	for (Queue::iterator queueItem = clientQueue->begin(); queueItem != clientQueue->end(); ++queueItem)
	{
		queueItem->client->hasCreditCard = true;
	}
}

void Market::CloseCashDesk(int cashDeskIndex)
{
	assert (cashDeskIndex < marketState.numberOfCashDesks);
	size_t clientsCount = 0;
	ClientState *clientsAtCashDesk = GetClientsAtCashDesk(cashDeskIndex, clientsCount);
	assert (clientsAtCashDesk);

	size_t queueIdx = 0;
	for (QueueList::iterator queue = clientState.begin();
	     queue != clientState.end() && clientsCount > 0;
		 ++queue, ++queueIdx)
	{
		if (queueIdx != cashDeskIndex && 
			marketState.numberOfClientsAtCashDesks[queueIdx] < maxClientsPerQueue)
		{
			int currentClients = marketState.numberOfClientsAtCashDesks[queueIdx];
			int toAdd = clientsCount;
			if (clientsCount > maxClientsPerQueue - currentClients)
			{
				toAdd = maxClientsPerQueue - currentClients;
				marketState.numberOfClientsAtCashDesks[queueIdx] += toAdd;
				clientsCount -= toAdd;
			}
			else
			{
				marketState.numberOfClientsAtCashDesks[queueIdx] += clientsCount;
				clientsCount = 0;
			}

			for (size_t clIdx = 0; clIdx < toAdd; ++clIdx)
			{
				clientsAtCashDesk[clIdx].cashDeskPosition = queueIdx;
				clientsAtCashDesk[clIdx].queuePosition = queue->size();
				queue->push_back(clientsAtCashDesk[clIdx]);
			}
		}
		else
		{
			queue->clear();
		}
	}

	delete [] clientsAtCashDesk;

	marketState.numberOfClientsAtCashDesks[cashDeskIndex] = 0;
}

void Market::MoveClients(int cashDeskFrom, int cashDeskTo, int howMany)
{
}

ClientState* Market::GetClientsAtCashDesk(int cashDeskIndex, size_t &clientsCount) const
{
	size_t queueIdx = 0;
	for (QueueList::const_iterator queue = clientState.begin(); queue != clientState.end(); ++queue, ++queueIdx)
	{
		if (queueIdx == cashDeskIndex)
		{
			ClientState *queueClients = new ClientState[queue->size()];
			for (Queue::const_iterator queueItem = queue->begin(); queueItem != queue->end(); ++queueItem)
			{
				queueClients[queueIdx] = *queueItem;
			}
			return queueClients;
		}
	}

	return NULL;
}

ClientState* Market::RetrieveLastNClientsAt(int cashDeskIndex, int howMany)
{
	size_t queueIdx = 0;
	for (QueueList::iterator queue = clientState.begin(); queue != clientState.end(); ++queue, ++queueIdx)
	{
		if (queueIdx == cashDeskIndex)
		{
			assert (howMany < queue->size());

			ClientState *queueClients = new ClientState[queue->size()];

		}
	}

	return NULL;
}


////////////
// Client //
////////////
Client::Client()
	: id(-1), numberOfGoods(-1), hasCreditCard(false)
{
}

Client::Client(int newNumberOfGoods, bool newHasCreditCard)
	: id(-1), numberOfGoods(newNumberOfGoods), hasCreditCard(newHasCreditCard)
{
}


/////////////////
// ClientState //
/////////////////
ClientState::ClientState()
	: cashDeskPosition(-1), queuePosition(-1), client(NULL)
{
}

ClientState::ClientState(int newCashDeskPosition, int newQueuePosition, const Client &newClientData)
	: cashDeskPosition(newCashDeskPosition), queuePosition(newQueuePosition)
{
	client = new Client(newClientData.numberOfGoods, newClientData.hasCreditCard);
}

ClientState::ClientState(const ClientState &other)
{
	if (this != &other)
	{
		CopyFrom(other);
	}
}

ClientState& ClientState::operator=(const ClientState &other)
{
	if (this != &other)
	{
		Destroy();
		CopyFrom(other);
	}
	return *this;
}

ClientState::~ClientState()
{
	Destroy();
}

void ClientState::CopyFrom(const ClientState &other)
{
	cashDeskPosition = other.cashDeskPosition;
	queuePosition = other.queuePosition;
	
	if (client)
	{
		delete client;
	}
	client = new Client(*other.client);
}

void ClientState::Destroy()
{
	delete client;
}


/////////////////
// MarketState //
/////////////////
MarketState::MarketState(int allCashDesksCount)
	: numberOfCashDesks(1), numberOfClientsAtExpressCashDesk(0)
{
	numberOfClientsAtCashDesks = new int[allCashDesksCount];
	for (size_t i = 0; i < allCashDesksCount; ++i)
	{
		numberOfClientsAtCashDesks[i] = -1;
	}
}

MarketState::MarketState(const MarketState &other)
{
	if (this != &other)
	{
		CopyFrom(other);
	}
}

MarketState& MarketState::operator=(const MarketState &other)
{
	if (this != &other)
	{
		Destroy();
		CopyFrom(other);
	}
	return *this;
}

MarketState::~MarketState()
{
	Destroy();
}

void MarketState::CopyFrom(const MarketState &other)
{
	numberOfCashDesks = other.numberOfCashDesks;
	numberOfClientsAtExpressCashDesk = other.numberOfClientsAtExpressCashDesk;

	if (numberOfClientsAtCashDesks)
	{
		delete [] numberOfClientsAtCashDesks;
	}
	numberOfClientsAtCashDesks = new int[numberOfCashDesks];
	for (size_t i = 0; i < numberOfCashDesks; ++i)
	{
		numberOfClientsAtCashDesks[i] = other.numberOfClientsAtCashDesks[i];
	}
}

void MarketState::Destroy()
{
	delete [] numberOfClientsAtCashDesks;
}