///////////////////////////////////////////////////////////
//
// ISEL - LEIC - Programação Concorrente
// October 2007
//
// lists library:
//     Definitions of singly and doubly linked lists
//
// Author: Carlos Martins
//    (revised by João Trindade)
// 

#ifndef LISTS_H
#define LISTS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* dlist_t: a doubly-linked list */

typedef struct dlist_node {
	struct dlist_node * pNextNode;
	struct dlist_node * pPrevNode;
} dlist_node_t, dlist_t;

#define dlist_init(pHead) \
    ((pHead)->pNextNode = (pHead)->pPrevNode = (pHead))

#define dlist_isEmpty(pHead) \
	((pHead)->pNextNode == (pHead))
	
#define dlist_getFirst(pHead) \
	(dlist_isEmpty((pHead)) ? 0 : (pHead)->pNextNode)

#define dlist_getLast(pHead) \
	(dlist_isEmpty((pHead)) ? 0 : (pHead)->pPrevNode)

#define dlist_getNext(pHead, pNode) \
	((pNode)->pNextNode == (pHead) ? 0 : (pNode)->pNextNode)

#define dlist_getPrevious(pHead, pNode) \
	((pNode)->pPrevNode == (pHead) ? 0 : (pNode)->pPrevNode)

#define dlist_removeFirst(pHead) \
	dlist_remove((pHead), (pHead)->pNextNode)
	
#define dlist_removeLast(pHead) \
	dlist_remove((pHead), (pHead)->pPrevNode)

#define dlist_remove(pHead, pNode)					     \
	do {											     \
		assert((pHead) != (pNode));                         \
		(pNode)->pPrevNode->pNextNode = (pNode)->pNextNode; \
		(pNode)->pNextNode->pPrevNode = (pNode)->pPrevNode; \
		(pNode)->pNextNode = (pNode)->pPrevNode = 0;        \
	}  while (0)

#define dlist_addFirst(pHead, pNode)             	\
	do {                                         	\
		(pNode)->pPrevNode = (pHead);            	\
		(pNode)->pNextNode = (pHead)->pNextNode; 	\
		(pHead)->pNextNode->pPrevNode = (pNode); 	\
		(pHead)->pNextNode = (pNode);            	\
	} while (0)

#define dlist_addLast(pHead, pNode)              	\
	do {                                         	\
		(pNode)->pNextNode = (pHead);            	\
		(pNode)->pPrevNode = (pHead)->pPrevNode; 	\
		(pHead)->pPrevNode->pNextNode = (pNode); 	\
		(pHead)->pPrevNode = (pNode);            	\
	} while (0)

#define dlist_enqueue(pHead, pNode) 	\
	dlist_addLast((pHead), (pNode))

__inline dlist_node_t * dlist_dequeue(dlist_t * pHead) {
	dlist_node_t * pNode = dlist_getFirst(pHead);
	if (pNode)
		dlist_remove(pHead, pNode);
	return pNode;
}

///////////////////////////////////////////////////////////

/* slist_t: a singly-linked list */

typedef struct slist_node {
	struct slist_node * pNextNode;
} slist_node_t, slist_t;

#define slist_init(pHead) \
	((pHead)->pNextNode = 0)

#define	slist_isEmpty(pHead) \
	((pHead)->pNextNode == 0)

#define	slist_getFirst(pHead) \
	(slist_isEmpty((pHead)) ? 0 : (pHead)->pNextNode)

#define slist_removeFirst(pHead)                            	\
	do {                                                    	\
		(pHead)->pNextNode = (pHead)->pNextNode->pNextNode;	\
	} while (0)

#define slist_addFirst(pHead, pNode)             	\
	do {                                         	\
		(pNode)->pNextNode = (pHead)->pNextNode; 	\
		(pHead)->pNextNode = (pNode);            	\
	} while (0)

#define slist_push(pHead, pNode) 		\
	slist_addFirst((pHead), (pNode))

__inline slist_node_t * slist_pop(slist_t * pHead) {
	slist_node_t * pNode = slist_getFirst(pHead);
	if (pNode)
		slist_removeFirst(pHead);
	return pNode;
}

#define slist_remove(pHead, pNode)                         	\
	do {                                                   	\
		slist_node_t * pCurrNode;                          	\
		for (pCurrNode = (pHead) ; pCurrNode->pNextNode; pCurrNode = pCurrNode->pNextNode) { \
			if (pCurrNode->pNextNode == (pNode)) {         	\
				pCurrNode->pNextNode = (pNode)->pNextNode; \
				break;                                    	\
			}                                              	\
		}                                                  	\
		(pNode)->pNextNode = 0;                            	\
	} while (0)

#ifdef __cplusplus
}
#endif // __cplusplus
	
#endif /* LISTS_H */
