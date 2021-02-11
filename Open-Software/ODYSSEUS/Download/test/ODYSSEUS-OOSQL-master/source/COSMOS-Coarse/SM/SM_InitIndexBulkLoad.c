/******************************************************************************/
/*                                                                            */
/*    Copyright (c) 1990-2016, KAIST                                          */
/*    All rights reserved.                                                    */
/*                                                                            */
/*    Redistribution and use in source and binary forms, with or without      */
/*    modification, are permitted provided that the following conditions      */
/*    are met:                                                                */
/*                                                                            */
/*    1. Redistributions of source code must retain the above copyright       */
/*       notice, this list of conditions and the following disclaimer.        */
/*                                                                            */
/*    2. Redistributions in binary form must reproduce the above copyright    */
/*       notice, this list of conditions and the following disclaimer in      */
/*       the documentation and/or other materials provided with the           */
/*       distribution.                                                        */
/*                                                                            */
/*    3. Neither the name of the copyright holder nor the names of its        */
/*       contributors may be used to endorse or promote products derived      */
/*       from this software without specific prior written permission.        */
/*                                                                            */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     */
/*    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       */
/*    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       */
/*    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          */
/*    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,    */
/*    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        */
/*    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER        */
/*    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT      */
/*    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN       */
/*    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         */
/*    POSSIBILITY OF SUCH DAMAGE.                                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*    ODYSSEUS/COSMOS General-Purpose Large-Scale Object Storage System --    */
/*    Coarse-Granule Locking (Volume Lock) Version                            */
/*    Version 3.0                                                             */
/*                                                                            */
/*    Developed by Professor Kyu-Young Whang et al.                           */
/*                                                                            */
/*    Advanced Information Technology Research Center (AITrc)                 */
/*    Korea Advanced Institute of Science and Technology (KAIST)              */
/*                                                                            */
/*    e-mail: odysseus.oosql@gmail.com                                        */
/*                                                                            */
/*    Bibliography:                                                           */
/*    [1] Whang, K., Lee, J., Lee, M., Han, W., Kim, M., and Kim, J., "DB-IR  */
/*        Integration Using Tight-Coupling in the Odysseus DBMS," World Wide  */
/*        Web, Vol. 18, No. 3, pp. 491-520, May 2015.                         */
/*    [2] Whang, K., Lee, M., Lee, J., Kim, M., and Han, W., "Odysseus: a     */
/*        High-Performance ORDBMS Tightly-Coupled with IR Features," In Proc. */
/*        IEEE 21st Int'l Conf. on Data Engineering (ICDE), pp. 1104-1105     */
/*        (demo), Tokyo, Japan, April 5-8, 2005. This paper received the Best */
/*        Demonstration Award.                                                */
/*    [3] Whang, K., Park, B., Han, W., and Lee, Y., "An Inverted Index       */
/*        Storage Structure Using Subindexes and Large Objects for Tight      */
/*        Coupling of Information Retrieval with Database Management          */
/*        Systems," U.S. Patent No.6,349,308 (2002) (Appl. No. 09/250,487     */
/*        (1999)).                                                            */
/*    [4] Whang, K., Lee, J., Kim, M., Lee, M., Lee, K., Han, W., and Kim,    */
/*        J., "Tightly-Coupled Spatial Database Features in the               */
/*        Odysseus/OpenGIS DBMS for High-Performance," GeoInformatica,        */
/*        Vol. 14, No. 4, pp. 425-446, Oct. 2010.                             */
/*    [5] Whang, K., Lee, J., Kim, M., Lee, M., and Lee, K., "Odysseus: a     */
/*        High-Performance ORDBMS Tightly-Coupled with Spatial Database       */
/*        Features," In Proc. 23rd IEEE Int'l Conf. on Data Engineering       */
/*        (ICDE), pp. 1493-1494 (demo), Istanbul, Turkey, Apr. 16-20, 2007.   */
/*                                                                            */
/******************************************************************************/
/*
 * Module: SM_InitIndexBulkLoad.c
 *
 * Description :
 *  Initialize B+ tree index bulkload.
 *
 * Exports:
 *  Four SM_InitIndexBulkLoad(VolID, KeyDesc *)
 */


#include "common.h"
#include "trace.h"
#include "Util_Sort.h"
#include "OM_Internal.h"
#include "BtM.h"
#include "SM_Internal.h"
#include "BL_SM_Internal.h"
#include "perThreadDS.h"
#include "perProcessDS.h"


/*@===========================
 * SM_InitIndexBulkLoad()
 *===========================*/
/*
 * Function: Four SM_InitIndexBulkLoad(VolID, KeyDesc *)
 *
 * Description: 
 *  Initialize B+ tree index bulkload.
 *  
 * Returns:
 *  error code
 *    eBADPARAMETER_SM
 *    some errors caused by function calss
 *
 * Side Effects:
 *  parameter indexBlkLdId is filled with index bulkload ID 
 */
Four _SM_InitIndexBulkLoad (
    Four handle,
    VolID                  volId,                  /* IN volume ID in which temporary files are allocated */
    KeyDesc                *kdesc)                 /* IN key descriptor of the given B+ tree */
{
    Four                   e;                      /* error number */
    Two                    i;                      /* a loop index */
    Four                   v;                      /* array index on scan manager mount table */
    SortTupleDesc          sortKeyDesc;            /* sort key descriptor of Btree index */
    Four                   blkLdId;                /* OM bulkload ID */
    SM_IdxBlkLdTableEntry* blkLdEntry;             /* entry in which information about bulkload is saved */


    TR_PRINT(TR_SM, TR1, 
            ("SM_InitIndexBulkLoad(handle, volId=%ld, kdesc=%P)", volId, kdesc));


    /* 
    **  O. Check parameters 
    */

    /* find the given volume in the scan manager mount table */
    for (v = 0; v < MAXNUMOFVOLS; v++)
        if (SM_PER_THREAD_DS(handle).smMountTable[v].volId == volId) break; /* found */

    if (v == MAXNUMOFVOLS)      ERR(handle, eNOTMOUNTEDVOLUME_SM);

    if (kdesc == NULL)          ERR(handle, eBADPARAMETER_SM);


    /* 
    **  I. Find empty entry from SM index bulkload table
    */
    for (blkLdId = 0; blkLdId < SM_IDXBLKLD_TABLE_SIZE; blkLdId++ ) {
        if (SM_IDXBLKLD_TABLE(handle)[blkLdId].isUsed == FALSE) 
	    break;
    }
    if (blkLdId == SM_IDXBLKLD_TABLE_SIZE) ERR(handle, eBLKLDTABLEFULL);

    /* set entry for fast access */
    blkLdEntry = &SM_IDXBLKLD_TABLE(handle)[blkLdId];

    /* set isUsed flag */
    blkLdEntry->isUsed = TRUE;


    /* 
    **  II. Make sortKeyDesc 
    */

    sortKeyDesc.nparts  = kdesc->nparts;
    sortKeyDesc.hdrSize = 0;
    for (i = 0; i < kdesc->nparts; i++) {
        sortKeyDesc.parts[i].type   = kdesc->kpart[i].type;
        sortKeyDesc.parts[i].length = kdesc->kpart[i].length;
        sortKeyDesc.parts[i].flag   = SORTKEYDESC_ATTR_ASC;
    }

    sortKeyDesc.parts[i].type   = SM_OBJECT_ID; 
    sortKeyDesc.parts[i].length = SM_OBJECT_ID_SIZE;
    sortKeyDesc.parts[i].flag   = SORTKEYDESC_ATTR_ASC; 
    sortKeyDesc.nparts++; 


    /* 
    **  III. Open sort stream for bulkload
    */

    blkLdEntry->streamId = Util_OpenSortStream(handle, volId, &sortKeyDesc);
    if (blkLdEntry->streamId < 0) ERR(handle, blkLdEntry->streamId);


    return blkLdId;


}   /* SM_InitIndexBulkLoad() */
