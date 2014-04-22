/*======================================================================

              GNSS hash table header file

 GENERAL DESCRIPTION
  This file contains interface declarations for GNSS hash table utility
  
 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

* Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2004, National ICT Australia
 *
 * All rights reserved. 
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by National ICT Australia, free of charge, to
 * any person obtaining a copy of this software and any associated
 * documentation files (the "Software") to deal with the Software without
 * restriction, including (without limitation) the rights to use, copy,
 * modify, adapt, merge, publish, distribute, communicate to the public,
 * sublicense, and/or sell, lend or rent out copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimers in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of National ICT Australia, nor the names of its
 *       contributors, may be used to endorse or promote products derived
 *       from this Software without specific prior written permission.
 *
 * EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
 * PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
 * NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
 * WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
 * REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
 * THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
 * ERRORS, WHETHER OR NOT DISCOVERABLE.
 *
 * TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
 * NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
 * THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
 * LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
 * OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
 * OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
 * OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
 * CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
 * CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
 * DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
 * CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
 * DAMAGES OR OTHER LIABILITY.
 *
 * If applicable legislation implies representations, warranties, or
 * conditions, or imposes obligations or liability on National ICT
 * Australia or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of National ICT Australia or the contributor is limited, to
 * the full extent permitted by the applicable legislation, at its
 * option, to:
 * a.  in the case of goods, any one or more of the following:
 * i.  the replacement of the goods or the supply of equivalent goods;
 * ii.  the repair of the goods;
 * iii. the payment of the cost of replacing the goods or of acquiring
 *  equivalent goods;
 * iv.  the payment of the cost of having the goods repaired; or
 * b.  in the case of services:
 * i.  the supplying of the services again; or
 * ii.  the payment of the cost of having the services supplied again.
 *
 * The construction, validity and performance of this licence is governed
 * by the laws in force in New South Wales, Australia.


 * Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2007, Open Kernel Labs, Inc.
 *
 * All rights reserved. 
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by National ICT Australia, free of charge, to
 * any person obtaining a copy of this software and any associated
 * documentation files (the "Software") to deal with the Software without
 * restriction, including (without limitation) the rights to use, copy,
 * modify, adapt, merge, publish, distribute, communicate to the public,
 * sublicense, and/or sell, lend or rent out copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimers in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of National ICT Australia, nor the names of its
 *       contributors, may be used to endorse or promote products derived
 *       from this Software without specific prior written permission.
 *
 * EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
 * PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
 * NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
 * WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
 * REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
 * THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
 * ERRORS, WHETHER OR NOT DISCOVERABLE.
 *
 * TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
 * NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
 * THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
 * LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
 * OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
 * OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
 * OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
 * CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
 * CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
 * DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
 * CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
 * DAMAGES OR OTHER LIABILITY.
 *
 * If applicable legislation implies representations, warranties, or
 * conditions, or imposes obligations or liability on National ICT
 * Australia or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of National ICT Australia or the contributor is limited, to
 * the full extent permitted by the applicable legislation, at its
 * option, to:
 * a.  in the case of goods, any one or more of the following:
 * i.  the replacement of the goods or the supply of equivalent goods;
 * ii.  the repair of the goods;
 * iii. the payment of the cost of replacing the goods or of acquiring
 *  equivalent goods;
 * iv.  the payment of the cost of having the goods repaired; or
 * b.  in the case of services:
 * i.  the supplying of the services again; or
 * ii.  the payment of the cost of having the services supplied again.
 *
 * The construction, validity and performance of this licence is governed
 * by the laws in force in New South Wales, Australia.
 *

 
lookup3.c, by Bob Jenkins, May 2006, Public Domain.

These are functions for producing 32-bit hashes for hash table lookup.
hashword(), hashlittle(), hashlittle2(), hashbig(), mix(), and FINAL() 
are externally useful functions. Routines to test the hash are included 
if SELF_TEST is defined. You can use this free for any purpose. It's in
the public domain. It has no warranty

 Copyright (c) 2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/inc/gnss_hash_table.h#1 $

 when      who   what, where, why
 ----      ----  --- --------------------------------------------------- 
10/04/16   ss    Initial creation of file.
======================================================================*/
#ifndef GNSS_HASH_TABLE_H
#define GNSS_HASH_TABLE_H

#include "gnss_list.h"

namespace gnss_StlNameSpace
{

/* Initial value for the hash function */
#define GNSS_HASH_INITIAL_VALUE 13
#define ROT(x,k) (((x)<<(k)) | ((x)>>(32-(k))))


/* A template class for comparator functions in the hash table. 
   The consumers of this class have to do a template specialzation for defining their 
   own comparator functions for their hash table */
template <typename V>
class gnss_HashTableCompFn
{
  public:
    /* Hash function for the hash table */
    bool compare(V *p_Value1, V *p_Value2);
};


/*
-------------------------------------------------------------------------------
FINAL -- FINAL mixing of 3 32-bit values (a,b,c) into c

Pairs of (a,b,c) values differing in only a few bits will usually
produce values of c that look totally different.  This was tested for
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

These constants passed:
 14 11 25 16 4 14 24
 12 14 25 16 4 14 24
and these came close:
  4  8 15 26 3 22 24
 10  8 15 26 3 22 24
 11  8 15 26 3 22 24
-------------------------------------------------------------------------------
*/
#define FINAL(a,b,c) \
{ \
  c ^= b; c -= ROT(b,14); \
  a ^= c; a -= ROT(c,11); \
  b ^= a; b -= ROT(a,25); \
  c ^= b; c -= ROT(b,16); \
  a ^= c; a -= ROT(c,4);  \
  b ^= a; b -= ROT(a,14); \
  c ^= b; c -= ROT(b,24); \
}

/*
-------------------------------------------------------------------------------
MIX-- MIX3 32-bit values reversibly.

This is reversible, so any information in (a,b,c) before mix() is
still in (a,b,c) after mix().

If four pairs of (a,b,c) inputs are run through mix(), or through
mix() in reverse, there are at least 32 bits of the output that
are sometimes the same for one pair and different for another pair.
This was tested for:
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

Some k values for my "a-=c; a^=ROT(c,k); c+=b;" arrangement that
satisfy this are
    4  6  8 16 19  4
    9 15  3 18 27 15
   14  9  3  7 17  3
Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
for "differ" defined as + with a one-bit base and a two-bit delta.  I
used http://burtleburtle.net/bob/hash/avalanche.html to choose 
the operations, constants, and arrangements of the variables.

This does not achieve avalanche.  There are input bits of (a,b,c)
that fail to affect some output bits of (a,b,c), especially of a.  The
most thoroughly mixed value is c, but it doesn't really even achieve
avalanche in c.

This allows some parallelism.  Read-after-writes are good at doubling
the number of bits affected, so the goal of mixing pulls in the opposite
direction as the goal of parallelism.  I did what I could.  rotates
seem to cost as much as shifts on every machine I could lay my hands
on, and rotates are much kinder to the top and bottom bits, so I used
rotates.
-------------------------------------------------------------------------------
*/
#define MIX(a,b,c) \
{ \
  a -= c;  a ^= ROT(c, 4);  c += b; \
  b -= a;  b ^= ROT(a, 6);  a += c; \
  c -= b;  c ^= ROT(b, 8);  b += a; \
  a -= c;  a ^= ROT(c,16);  c += b; \
  b -= a;  b ^= ROT(a,19);  a += c; \
  c -= b;  c ^= ROT(b, 4);  b += a; \
}

/* 
  Class which implements the hash table utility 
  typename K : Specifies the generic type for the key for the hash table
  typename V : Specifies the generic type for the value to be inserted into the hash table
*/
template <typename K, typename V>
class gnss_HashTable
{
  private:
    /* The pointer to the hash table. The hash table is an array of pointers . Each pointer points to a list 
       of values to be inserted into the hash table . */
    gnss_ListClass<V> **m_pTable;

    /* Length of the hash table */
    uint32 m_qTableLength;

    /* Function to generate an internal key for the hash function */
    uint32 generateInternalKey(const K *p_UserKey, /* the key, an array of values */
                               uint32 q_Length,    /* the length of the key, in uint32 */
                               uint32 q_InitVal);  /* the previous hash, or an arbitrary value */
    /* Funciton to generate a hash valus using an internal key */
    uint32 hashFn(uint32 q_InternalKey);
    
    /* The number of items in the hash table */
    uint32 m_qNumItems; 

    /* The callback function member */
    gnss_HashTableCompFn<V> *m_pCompFn;


  public:
    /* Default Constructor */
    gnss_HashTable(void);
    /* Function to initialze the table */
    void initTable(uint32 qTableLength, gnss_HashTableCompFn<V> *p_CompFn);
    /* Function to insert an item into the hash table */
    ListElement<V>* insertItem(const K *p_key, uint32 q_KeyLength, V *p_value, bool *pb_IsDupItem,bool b_pushToFront = false);
    /* Function to delete an item from hash table */
    bool deleteItem(const K *p_key, uint32 q_KeyLength, V *p_value, bool delItemData = false);
    /*Function to delete all items in hash table */
    void deleteAll(bool delItemData = false);
    /* Function to search an item in hash table */
    ListElement<V>* searchItem(const K *p_Key, uint32 q_KeyLength, V *p_Value, bool *pb_IsDupItem);
    /* Function to search a value in the induvidual hash list */
    ListElement<V>* searchChainList(gnss_ListClass<V> *p_List, V *p_Item );
    gnss_ListClass<V>* getlist(const K *p_Key, uint32 q_KeyLength, uint32& q_Index);

    /* Function to display the hash table */
    void displayTable(void);
    
    /* Get the number of items inserted in the hash table */
    uint32 getNumItems();

    /* Destructor */
    ~gnss_HashTable(void);

    uint32 bucket_count()
    {
      return (m_qTableLength);
    }

    /* Bucket Iterator */
    struct bucket_iterator
    {
      gnss_ListClass<V> **m_pData;

      bucket_iterator() : m_pData(NULL) {}
      bucket_iterator(gnss_ListClass<V> **pData) : m_pData(pData) {}
      ~bucket_iterator() {}
      gnss_ListClass<V>* operator*() { return *m_pData; }

      //pre-increment
      bucket_iterator& operator++() {
        ++m_pData;
        return *this;
      }

      //pre-decrement
      bucket_iterator& operator--() {
        --m_pData;
        return *this;
      }

      //post-increment
      bucket_iterator operator++(int) {
        bucket_iterator tmp(*this);
        ++*this;
        return tmp;
      }

      //post-decrement
      bucket_iterator operator--(int) {
        bucket_iterator tmp(*this);
        --*this;
        return tmp;
      }
      
      bucket_iterator operator=(const bucket_iterator &rhs)
      {
        m_pData = rhs.m_pData;
        return *this;
      }
      bool operator==(const bucket_iterator &rhs) { return m_pData == rhs.m_pData; }
      bool operator!=(const bucket_iterator &rhs) { return !(*this == rhs); }
    };

    bucket_iterator bucket_begin()
    {
      if (m_pTable)
      {
        return bucket_iterator(&m_pTable[0]);
      }
      else
      {
        return NULL;
      }
    }

    bucket_iterator bucket_end()
    {
      if (m_pTable)
      {
        return bucket_iterator(&m_pTable[m_qTableLength]);
      }
      else
      {
        return NULL;
      }
    }
};


/********************************************************************
*
* gnss_HashTable<K,V>::gnss_HashTable()
*
* Function description:
* This constrcutor for the class gnss_HashTable
*
* Parameters: 
* Void
*
* Return:
* None
*  
*********************************************************************/
template <typename K,typename V>
gnss_HashTable<K,V>::gnss_HashTable(void)
{
  m_pTable = NULL;
  m_qTableLength = 0;
  m_qNumItems = 0;
  m_pCompFn = NULL;
}

/********************************************************************
*
* gnss_HashTable<K,V>::gnss_HashTable()
*
* Function description:
* This constrcutor for the class gnss_HashTable
*
* Parameters: 
* Void
*
* Return:
* None
*  
*********************************************************************/
template <typename K,typename V>
gnss_HashTable<K,V>::~gnss_HashTable(void)
{
  
}


/********************************************************************
*
* gnss_HashTable<K,V>::initTable()
*
* Function description:
* This function is used to initialze the hash table
*
* Parameters: 
* length : The length of the hash table 
* 
* Return:
* None
*  
*********************************************************************/
template <typename K,typename V>
void gnss_HashTable<K,V>::initTable(uint32 q_Length, gnss_HashTableCompFn<V> *p_CompFn)
{
  uint32 i = 0;
  void* p_memptr = NULL;

  m_qTableLength = q_Length;
  /* Create the Hash table of the specified length. The hash table contains an array of lists .
     Each list contains the hash values.  */
  m_pTable = (gnss_ListClass<V>**)gnss_HalClass::MemAlloc(CGPS_COMMON_MODULE_LIST, sizeof(gnss_ListClass<V>*)*q_Length);
  
  if(NULL == m_pTable)
  {
    return;
  }
  
  /* For each hash table index create a list object which can be used for all the list operations */
  for ( i = 0; i < q_Length; i++)
  {
    GNSS_NEW_NOARGS(CGPS_COMMON_MODULE_LIST, gnss_ListClass<V>, m_pTable[i]);
  }
  if(p_CompFn != NULL)
  {
    m_pCompFn = p_CompFn;
  }
}



/********************************************************************
*
* gnss_HashTable<K,V>::generateInternalKey()
*
* Function description:
* This function is used to generate an internal key for the hash table
*
* Parameters: 
* p_UserKey : The user key, an array of K values 
* q_Length  : the length of the key, in uint32
* q_InitVal : the previous hash, or an arbitrary value
* Return:
* The generated internal key for the hash table 
*  
*********************************************************************/
template <typename K,typename V>
uint32 gnss_HashTable<K,V>::generateInternalKey(
                                                    const  K *p_UserKey,      /* the user key, an array of K values */
                                                    uint32 q_Length,          /* the length of the key, in uint32 */
                                                    uint32 q_InitVal          /* the previous hash, or an arbitrary value */
                                                  ) 
{
  uint32 a,b,c;

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + (((uint32)q_Length)<<2) + q_InitVal;

  /*------------------------------------------------- handle most of the key */
  while (q_Length > 3)
  {
    a += p_UserKey[0];
    b += p_UserKey[1];
    c += p_UserKey[2];
    MIX(a,b,c);
    q_Length -= 3;
    p_UserKey += 3;
  }

  /*------------------------------------------- handle the last 3 uint32_t's */
  switch(q_Length)                     /* all the case statements fall through */
  { 
    case 3 : c+=p_UserKey[2];
    case 2 : b+=p_UserKey[1];
    case 1 : a+=p_UserKey[0];
      FINAL(a,b,c);
    case 0:     /* case 0: nothing left to add */
      break;
  }
  /*------------------------------------------------------ report the result */
  return c;
}



/********************************************************************
*
* gnss_HashTable<K,V>::hashFn()
*
* Function description:
* This function is used to generate hash value from a given key 
*
* Parameters: 
* q_Key : The key for the hash table .
* Return:
* The generated hash value.
*  
*********************************************************************/
template <typename K,typename V>
uint32 gnss_HashTable<K,V>::hashFn(uint32 q_Key)
{
#if (UINTPTR_MAX == UINT32_MAX) /*lint -e30 */
    q_Key += ~(q_Key << 15);
    q_Key ^= (q_Key >> 10);
    q_Key += (q_Key << 3);
    q_Key ^= (q_Key >> 6);
    q_Key += ~(q_Key << 11);
    q_Key ^= (q_Key >> 16);
#elif(UINTPTR_MAX == UINT64_MAX)  /*lint -e30 The linter can't
                                    * grok UINTPTR_MAX */
  uint64  tempKey = (uint64)q_Key;
  tempKey += ~(tempKey << 32);
  tempKey ^= (tempKey >> 22);
  tempKey += ~(tempKey << 13);
  tempKey ^= (tempKey >> 8);
  tempKey += (tempKey << 3);
  tempKey ^= (tempKey >> 15);
  tempKey += ~(tempKey << 27);
  tempKey ^= (tempKey >> 31);
  /*Lets truncate to 32 bit to return key*/
  q_Key = (uint32)tempKey;
 #else
    /* Unsupported word size */
#endif
    return q_Key;
}


/********************************************************************
*
* gnss_HashTable<K,V>::getNumItems()
*
* Function description:
* This function is used to return the number of items inserted in the hash table
*
* Parameters: 
* None
* 
* Return:
* Number of items inserted in the hash table
*  
*********************************************************************/
template <typename K,typename V>
uint32 gnss_HashTable<K,V>::getNumItems(void)
{
  return m_qNumItems;
}


/********************************************************************
*
* gnss_HashTable<K,V>::insertItem
*
* Function description:
* This function is used to insert an item into the hash table 
*
* Parameters: 
* p_Key : The user key which is used to index into the hash table 
* q_KeyLength : Length of the user key 
* p_Value : The value which is stored in the hash table
* pb_IsDupItem : true if the value already exists in the hash table
* b_pushToFront ( input) : true if node to be inserted to front . 
*                          false if node to be inserted to end
* Return:
* The inserted node in the hash table so that the caller maintains a reference to the node. 
*  
*********************************************************************/
template <typename K,typename V>
inline ListElement<V>* gnss_HashTable<K,V>::insertItem(const K *p_Key, uint32 q_KeyLength, V *p_Value, bool *pb_IsDupItem, bool b_pushToFront)
{
  uint32 q_Index = 0, q_HashValue = 0;
  ListElement<V> *p_TempNode = NULL;
  *pb_IsDupItem = false;
  uint32 q_InternalKey = 0;  
  V *p_Data = NULL;

  if((NULL == p_Key) || (NULL == p_Value))
  {
    MSG(MSG_SSID_GPSSM,MSG_LEGACY_MED,"NULL key or value passed into hash table insert");
    return NULL;
  }
  q_InternalKey = generateInternalKey(p_Key, q_KeyLength, GNSS_HASH_INITIAL_VALUE);

  /* Get a hash value using the provided key and the registered hash function */
  q_HashValue = hashFn(q_InternalKey);

  /* Get an index into the hash table using the hash value  */
  q_Index = q_HashValue%m_qTableLength;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Hash table insertion @%d current Tblcnt :%d curListCnt:%d" ,q_Index, m_qNumItems,(NULL == m_pTable[q_Index]->front() ? 0 : m_pTable[q_Index]->getNumOfElements()));

  
  /* Check whether there is a collision in the entry */
  if(NULL == m_pTable[q_Index]->front())
  {
    /* There is no entry in this location of the hash table . So push the value into the hash table at this entry */    
    p_TempNode = m_pTable[q_Index]->push_back_ext(*p_Value);
    if(NULL == p_TempNode)
    {
      return NULL;
    }
    p_Data = p_TempNode->GetData();
    /* Increment the reference count for the data in the hash node .*/  
    if(p_Data != NULL)
    {
      p_Data->incRefCnt();
    }
    m_qNumItems++;
    *pb_IsDupItem = false;
  }
  else
  {
    /* There is already an entry in the table for this. Search if the entry is the same as the value */
    p_TempNode = (ListElement<V>*)searchChainList(m_pTable[q_Index], p_Value);
    if(NULL == p_TempNode)
    {
      /* Add as a new entry in the chained list maintained at this index */
      if (true == b_pushToFront)
      {
        p_TempNode = m_pTable[q_Index]->push_front_ext(*p_Value);
      }
      else
      {
        p_TempNode = m_pTable[q_Index]->push_back_ext(*p_Value);
      }
      if(NULL == p_TempNode)
      {
        return NULL;
      }
      p_Data = p_TempNode->GetData();
      /* Increment the reference count for the data in the hash node .*/  
      if(p_Data != NULL)
      {
        p_Data->incRefCnt();
      }

      /* Increment the number of items inserted in the hash table */
      m_qNumItems++;
      *pb_IsDupItem = false;
    }
    else
    {
      /* The item is already in the hash chained list, no need to insert again  , Increment the reference count for this hash node.
         Return the existing item */
      p_Data = p_TempNode->GetData();
      /* Increment the reference count for the data in the hash node .*/  
      if(p_Data != NULL)
      {
        p_Data->incRefCnt();
      }
      *pb_IsDupItem = true;
    }
  }
  return p_TempNode;
}

/********************************************************************
*
* gnss_HashTable<K,V>::deleteItem
*
* Function description:
* This function is used to delete an item into the hash table 
* 
* Parameters: 
* p_Key : The User key which is used to index into the hash table 
* q_KeyLength : Length of the User key 
* p_Value : The value which is stored in the hash table
* delItemData : Hash table is array of lists and each list is linked list of nodes. 
*               Each node has Data pointer, pointers to next and previous. 
*               delItemData will specify if Data pointer in node needs to be freed or not
*               true : data pointer will be freed. 
*               false : data pointer will not be freed
* Return:
* true if the deletion was succesful
* false otherwise
*  
*********************************************************************/
template <typename K,typename V>
inline bool gnss_HashTable<K,V>::deleteItem(const K *p_Key, uint32 q_KeyLength, V *p_Value,bool delItemData)
{
  uint32 q_Index = 0, q_HashValue = 0;
  ListElement<V>* p_DelNode = NULL; 
  bool b_RetVal = false,b_isNodeDel = false;
  uint32 q_InternalKey = 0;
  V *p_Data;

  /* Generate an internal key based on the user key */
  q_InternalKey = generateInternalKey(p_Key, q_KeyLength, GNSS_HASH_INITIAL_VALUE);
  /* Get a hash value from the generated internal key */
  q_HashValue = hashFn(q_InternalKey);

  /* Get an index into the hash table using the hash value */
  q_Index = q_HashValue%m_qTableLength;
  
  /* Check whether there is a collision in the entry */
  if(NULL == m_pTable[q_Index]->front())
  {
    /* There is no entry in this location of the hash table . So nothing to delete */
    return b_RetVal;
  }

  /* There is already an entry in the table for this. Search if the entry is the same as the value */
  p_DelNode = (ListElement<V>*)searchChainList(m_pTable[q_Index], p_Value);
  if(NULL == p_DelNode)
  {
    /* The value does not exist in the hash chained list . So nothing to delete */
    return b_RetVal;
  }
  p_Data = p_DelNode->GetData();
  /* Decrement the reference count for the data in the hash node .*/  
  if (p_Data == NULL)
  {
    b_isNodeDel = true;
  }
  else
  {
    p_Data->decRefCnt();
    if (0 == p_Data->getRefCnt())
    {
      b_isNodeDel = true;
    }
  }

  if (true == b_isNodeDel)
  {
    b_isNodeDel = true;
  
    if (true == delItemData)
    {
      GNSS_DELETE(CGPS_COMMON_MODULE_LIST, V, p_Data);
    }
    /* The reference count for the data in the hash node is 0 , nobody is referencing it
    so delete the hash node from the hash table */
    if (true == m_pTable[q_Index]->delete_node(p_DelNode))
    {
      /* Decrement the number of items inserted in the hash table */
      if (m_qNumItems > 0)
      {
        m_qNumItems--;
      }
      b_RetVal = true;
    }
  }
  return b_RetVal;
}

/********************************************************************
*
* gnss_HashTable<K,V>::deleteAll
*
* Function description:
* This function is used to delete all items in the hash table
*
* Parameters:
* delItemData : Hash table is array of lists and each list is linked list of nodes.
*               Each node has Data pointer, pointers to next and previous.
*               delItemData will specify if Data pointer in node needs to be freed or not
*               true : data pointer will be freed.
*               false : data pointer will not be freed

*
* Return:
* None
*
*********************************************************************/
template <typename K, typename V>
inline void gnss_HashTable<K, V>::deleteAll(bool delItemData )
{
  for (uint32 i = 0; i < m_qTableLength; ++i)
  {
    m_pTable[i]->clearall(delItemData);
  }

  m_qNumItems = 0;
}

/********************************************************************
*
* gnss_HashTable<K,V>::searchChainList
*
* Function description:
* This function is used to search an item in the chained hash list
*
* Parameters: 
* p_List : The induvidual List containing the hash elements
* p_Item : The item to be searched
* 
* Return:
* The node of the hash list if the item is found, NULL otherwise
*  
*********************************************************************/
template <typename K,typename V>
inline ListElement<V>* gnss_HashTable<K,V>::searchChainList(gnss_ListClass<V> *p_List, V *p_Item )
{
  ListElement<V> *p_Element = NULL;
  ListElement<V> *p_End = NULL;  
  if((NULL == p_List)||
     (NULL == p_Item))
  {
    return NULL;
  }
  /* search for the data and delete the node */
  p_Element = p_List->getHead();
  p_End = p_List->getEnd();

  while(p_Element != p_End)
  {
    /* Compare the data stored in the list if equal */
    if(m_pCompFn != NULL)
    {
      if(true == m_pCompFn->compare(p_Item, p_Element->m_pData))
      {
        /* Element found in the list */
        return p_Element;
      }
    }
    p_Element = p_Element->m_pNext;
  }
  /* Item not found in list */
  return NULL;
}






/********************************************************************
*
* gnss_HashTable<K,V>::searchItem
*
* Function description:
* This function is used to search an item in the hash table 
*
* Parameters: 
* p_Key : The user key which is used to index into the hash table 
* q_KeyLength : Length of the user key 
* p_Value : The value which is stored in the hash table
* pb_IsDupItem : true if the value already exists in the hash table
* 
* Return:
* whether the item is present or not
*  
*********************************************************************/
template <typename K,typename V>
inline ListElement<V>* gnss_HashTable<K,V>::searchItem(const K *p_Key, uint32 q_KeyLength, V *p_Value, bool *pb_IsDupItem )
{
  uint32 q_Index = 0, q_HashValue = 0;
  ListElement<V> *p_TempNode = NULL;
  *pb_IsDupItem = false;
  uint32 q_InternalKey = 0;

  if((NULL == p_Key) || (NULL == p_Value))
  {
    MSG(MSG_SSID_GPSSM,MSG_LEGACY_MED,"NULL key or value passed into hash table search");
    return NULL;
  }
  q_InternalKey = generateInternalKey(p_Key, q_KeyLength, GNSS_HASH_INITIAL_VALUE);

  /* Get a hash value using the provided key and the registered hash function */
  q_HashValue = hashFn(q_InternalKey);

  /* Get an index into the hash table using the hash value  */
  q_Index = q_HashValue%m_qTableLength;

  /* Check whether there is a collision in the entry */
  if(NULL == m_pTable[q_Index]->front())
  {
    /* There is no entry in this location of the hash table . The item is not present */    
    *pb_IsDupItem = false;
  }
  else
  {
    /* There is already an entry in the table for this. Search if the entry is the same as the value */
    p_TempNode = (ListElement<V>*)searchChainList(m_pTable[q_Index], p_Value);
    if(NULL == p_TempNode)
    {
      /* There is no entry in this location of the hash table . The item is not present */    
      *pb_IsDupItem = false;
    }
    else
    {
      /* The item is already in the hash chained listReturn search success */
      *pb_IsDupItem = true;
    }
  }
  return p_TempNode;
}

template <typename K, typename V>
gnss_ListClass<V>* gnss_HashTable<K,V>::getlist(const K *p_Key, uint32 q_KeyLength, uint32& q_Index)
{
  uint32 q_HashValue = 0;    
  uint32 q_InternalKey = 0;

  q_Index = 0;

  if (NULL == p_Key)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "NULL key or value passed into hash table search");
    return NULL;
  }
  q_InternalKey = generateInternalKey(p_Key, q_KeyLength, GNSS_HASH_INITIAL_VALUE);

  /* Get a hash value using the provided key and the registered hash function */
  q_HashValue = hashFn(q_InternalKey);

  /* Get an index into the hash table using the hash value  */
  q_Index = q_HashValue%m_qTableLength;
  return (m_pTable[q_Index]);
}

template <typename K,typename V>
inline void gnss_HashTable<K,V>::displayTable(void)
{
  uint32 i = 0,j = 0;
  ListElement<V> *p_Element = NULL;
  ListElement<V> *p_End = NULL;  
  V *p_Data;

  /* Traverse through the table */
  for ( i = 0; i < m_qTableLength; i++)
  {
   
    j = 0;
    if(NULL != m_pTable[i]->front())
    {
      uint32 no_of_elem = m_pTable[i]->getNumOfElements();
      p_Element = m_pTable[i]->getHead();
      p_End = m_pTable[i]->getEnd();
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Index i = %d Count = %d", i, m_pTable[i]->getNumOfElements());
      /* There is an entry at this location of hash table . Traverse through the list in case there was a collision */
      while((j < no_of_elem) && (p_Element != p_End))
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "         Node @%d", j);
        p_Data = p_Element->GetData();
        if(NULL != p_Data)
        {
          p_Data->displayData();  
        }        
        p_Element = p_Element->m_pNext;
        j++;
      }

    }
  }// end for( i = 0; i < m_qTableLength; i++)
}

} // end namespace
#endif // GNSS_HASH_TABLE_H


