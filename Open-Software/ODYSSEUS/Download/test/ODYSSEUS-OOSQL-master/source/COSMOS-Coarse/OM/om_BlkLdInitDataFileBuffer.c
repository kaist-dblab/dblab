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
 * Module: om_BlkLdInitDataFileBuffer.c
 *
 * Description:
 *  Initialize the data file bulk load buffer.
 *
 * Exports:
 *  Four om_BlkLdInitDataFileBuffer(Four, PageID*, Four, PageID*, SlottedPage*, PageID) 
 */

#include <string.h>
#include "common.h"
#include "param.h"
#include "bl_param.h"
#include "RDsM_Internal.h"
#include "BfM_Internal.h" 	/* TTT */
#include "OM_Internal.h"
#include "BL_OM_Internal.h"
#include "perThreadDS.h"
#include "perProcessDS.h"

/*@========================================
 *  om_BlkLdInitDataFileBuffer()
 * =======================================*/
/*
 * Function : Four om_BlkLdInitDataFileBuffer(Four, PageID*, Four, PageID*, SlottedPage*, PageID)
 *
 * Description :
 *  Initialize the data file bulk load buffer.
 *
 * Return Values :
 *  error code.
 *
 * Side Effects :
 *  0)
 *
 */

Four om_BlkLdInitDataFileBuffer(
    Four handle,
    Four        blkLdId,             /* IN  bulkload ID */
    PageID      *pid,                /* IN  page id for allocate extent in which contains that page */
    Four        bufSize,             /* IN  size of data file bulk load buffer by page */ 
    PageID      *pageIdAry,          /* IN  page id array of allocated data file bulk load buffer */
    SlottedPage *dataFileBuffer,     /* IN  data file bulk load buffer */
    PageID      lastAllocatedPageId) /* IN  last allocated page array  */
{

    Four        e;                   /* error number */
    Four        i;                   /* index variable */
    Unique      unique;              /* space for the returned unique number */
    Four        num;                 /* number of unique numbers newly allocated */

    /* initialize each slotted page in data file bulk load buffer */
    for (i = 0; i < bufSize; i++) {

        /* Slotted page header setting */
        dataFileBuffer[i].header.pid    = pageIdAry[i];
        dataFileBuffer[i].header.fid    = OM_BLKLD_TABLE(handle)[blkLdId].fid; /* insert a handle into OM_BLKLD_TABLE */
        dataFileBuffer[i].header.nSlots = 0;
        dataFileBuffer[i].header.free   = 0;
        dataFileBuffer[i].header.unused = 0;

        /* set unique */
        e = RDsM_GetUnique(handle,  &(dataFileBuffer[i].header.pid), &unique, &num);
        if (e < 0) ERRB1(handle, e, &(pageIdAry[i]), PAGE_BUF);
        dataFileBuffer[i].header.unique = unique;
        dataFileBuffer[i].header.uniqueLimit = unique + num;

        /* set previous page no */
        if (i == 0) {
            dataFileBuffer[i].header.prevPage = lastAllocatedPageId.pageNo;
        } else {
            dataFileBuffer[i].header.prevPage = pageIdAry[i-1].pageNo;
        }

        /* set next page no */
        if (i == bufSize -1) {
            dataFileBuffer[i].header.nextPage = NIL;
        } else {
            dataFileBuffer[i].header.nextPage = pageIdAry[i+1].pageNo;
        }
 
        SET_PAGE_TYPE(&(dataFileBuffer[i]), SLOTTED_PAGE_TYPE);
        RESET_TEMP_PAGE_FLAG(&(dataFileBuffer[i])); 

        dataFileBuffer[i].header.spaceListPrev = NIL;
        dataFileBuffer[i].header.spaceListNext = NIL;

        /* Slotted page slot setting */
        dataFileBuffer[i].slot[0].offset = EMPTYSLOT;

    }

    return(eNOERROR);

} /* om_BlkLdInitDataFileBuffer() */