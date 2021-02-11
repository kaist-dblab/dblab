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
/*    Fine-Granule Locking Version                                            */
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
/******************************************************************************/
/*                                                                            */
/*    This module has been implemented based on "The Multilevel Grid File     */
/*    (MLGF) Version 4.0," which can be downloaded at                         */
/*    "http://dblab.kaist.ac.kr/Open-Software/MLGF/main.html".                */
/*                                                                            */
/******************************************************************************/

/*
 * Module: mlgf_SplitDirectoryPage.c
 *
 * Description:
 *  Split a given directory page.
 *
 * Exports:
 *  Four mlgf_SplitDirectoryPage(Four, PageID*, MLGF_KeyDesc*, mlgf_DirectoryEntry*,
 *                               mlgf_DirectoryEntry*, mlgf_DirectoryEntry*)
 *
 * Returns:
 *  Error code
 *    some errors caused by function calls
 */


#include <string.h>
#include "common.h"
#include "error.h"
#include "trace.h"
#include "Util.h"
#include "TM.h"
#include "RDsM.h"
#include "BfM.h"
#include "MLGF.h"
#include "LM.h"
#include "LOG.h"
#include "perProcessDS.h"
#include "perThreadDS.h"

Four mlgf_SplitDirectoryPage(
    Four 			handle,
    XactTableEntry_T 		*xactEntry, 		/* IN transaction table entry */
    MLGFIndexInfo 		*iinfo,        		/* IN MLGF index info */
    PageID 			*dirPid,		/* IN page to split */
    MLGF_KeyDesc 		*kdesc,			/* IN key descriptor of used index */
    mlgf_DirectoryEntry 	*insertedEntry, 	/* IN entry to insert */
    mlgf_DirectoryEntry 	*srcEntry,		/* INOUT entry for original directory page */
    mlgf_DirectoryEntry 	*dstEntry,		/* OUT entry for new directory page */
    LockParameter 		*lockup,      		/* IN request lock or not */
    LogParameter_T 		*logParam) 		/* IN log parameter */
{
    Four 			e;			/* error code */
    Four 			i, k;			/* index variable */
    Four 			domain;			/* domain used for split */
    Four 			entryLen;		/* length of a directory entry */
    PageID 			newPid;			/* PageID of the newly allocated page */
    MLGF_HashValue 		bitmask;		/* bitmask for region test */
    MLGF_HashValue 		*hashValue;		/* domain-th hash value of current entry */
    MLGF_HashValue 		insertedHashValue; 	/* domain-th hash value of insetedEntry */
    Buffer_ACC_CB 		*dirPage_BCB;		/* buffer control block for original page */
    Buffer_ACC_CB 		*newPage_BCB;		/* buffer control block for new page */
    mlgf_DirectoryPage 		*dirPage; 		/* a original directory page */
    mlgf_DirectoryPage 		*newPage; 		/* a newly allocated directory page */
    MLGF_HashValue 		*extremeHashValues;	/* points to array of extreme(= min or max) hash values within the leaf page */
    MLGF_HashValue 		*dirEntryHashValues;	/* points to array of hash values of a directory entry */
    LockReply 			lockReply; 
    LockMode 			oldMode;
    Lsn_T 			lsn;                  	/* lsn of the newly written log record */
    Four 			logRecLen;             	/* log record length */
    LOG_LogRecInfo_T 		logRecInfo; 		/* log record information */
    LockMode  			originalKeyRangeLockMode; 
    SegmentID_T 		pageSegmentID;    	/* page segment ID */
    LOG_Image_MLGF_SpecifyEntries_T entriesInfo;
    LOG_Image_MLGF_InitDirectoryPage_T initDirPageInfo; /* directory page information */

    /* pointer for COMMON Data Structure of perThreadTable */
    COMMON_PerThreadDS_T *common_perThreadDSptr = COMMON_PER_THREAD_DS_PTR(handle);

    TR_PRINT(handle, TR_MLGF, TR1,
	     ("mlgf_SplitDirectoryPage(handle, dirPid=%P, kdesc=%P, insertedEntry=%P, srcEntry=%P, dstEntry=%P, locup=%P)",
	      dirPid, kdesc, insertedEntry, srcEntry, dstEntry, lockup)); 

    /* get lock on  dirPid */
    if(lockup) {
	e = LM_getKeyRangeLock(handle, &xactEntry->xactId, dirPid, L_IX, L_MANUAL,
			       L_UNCONDITIONAL, &lockReply);
	if (e < eNOERROR) ERR(handle, e);

	if(lockReply == LR_DEADLOCK){
	    ERR(handle, eDEADLOCK);
	}

	originalKeyRangeLockMode = (LockMode)lockReply;

	e = LM_getFlatPageLock(handle, &xactEntry->xactId, dirPid, L_X, L_MANUAL,
			       L_UNCONDITIONAL, &lockReply, &oldMode);
	if (e < eNOERROR) ERR(handle, e);

	if(lockReply == LR_DEADLOCK){
	    ERR(handle, eDEADLOCK);
	}

    }

    /* read directory page from disk */
    e = BfM_getAndFixBuffer(handle, dirPid, M_FREE, &dirPage_BCB, PAGE_BUF);
    if (e < eNOERROR) ERR(handle, e);

    dirPage = (mlgf_DirectoryPage*)dirPage_BCB->bufPagePtr;

    /* select domain to split */
    mlgf_SplitDirectoryRegion(handle, dirPage, kdesc->nKeys, insertedEntry, srcEntry, dstEntry, &domain);

    e = mlgf_GetSegmentIDFromIndexInfo(handle, xactEntry, iinfo, &pageSegmentID, PAGESIZE2);
    if (e < eNOERROR) ERR(handle, e);

    e = RDsM_AllocTrains(handle, xactEntry, dirPid->volNo, &pageSegmentID, dirPid, 1, PAGESIZE2, FALSE, &newPid, logParam);
    if (e < eNOERROR) ERRB1(handle, e, dirPage_BCB, PAGE_BUF);

    /* get lock on newPid */
    if(lockup) {
	e = LM_getKeyRangeLock(handle, &xactEntry->xactId, &newPid, originalKeyRangeLockMode, L_MANUAL,
			       L_UNCONDITIONAL, &lockReply);
	if (e < eNOERROR) ERRB1(handle, e, dirPage_BCB, PAGE_BUF);

	if(lockReply == LR_DEADLOCK){
	    ERRB1(handle, eDEADLOCK, dirPage_BCB, PAGE_BUF);
	}

	e = LM_getFlatPageLock(handle, &xactEntry->xactId, &newPid, L_X, L_MANUAL,
			       L_UNCONDITIONAL, &lockReply, &oldMode);
	if (e < eNOERROR) ERRB1(handle, e, dirPage_BCB, PAGE_BUF);

	if(lockReply == LR_DEADLOCK){
	    ERRB1(handle, eDEADLOCK, dirPage_BCB, PAGE_BUF);
	}

    }

    /* Read the new page into the buffer. */
    e = BfM_fixNewBuffer(handle, &newPid, M_FREE, &newPage_BCB, PAGE_BUF);
    if (e < eNOERROR) ERRB1(handle, e, dirPage_BCB, PAGE_BUF);

    newPage = (mlgf_DirectoryPage*)newPage_BCB->bufPagePtr;

    /* Initialize the new directory page. */
    MLGF_INIT_DIRECTORY_PAGE(newPage, dirPage->hdr.iid, newPid, dirPage->hdr.height, FALSE, kdesc->nKeys);

    /* Calculate the length of a directory entry. */
    entryLen = MLGF_DIRENTRY_LENGTH(kdesc->nKeys);

    /* Get bit mask used for dividing the entries into two groups. */
    bitmask = MLGF_HASHVALUE_ITH_BIT_SET(srcEntry->nValidBits[domain]);

    /*
     * Divide the directory page into two direcory page.
     */

    /* hashValue points to the domain-th hash value. */
    hashValue =	MLGF_DIRENTRY_HASHVALUEPTR(MLGF_ITH_DIRENTRY(dirPage, 0, entryLen), kdesc->nKeys) + domain;

    /* Search the split boundary. */
    for (i = 0; i < dirPage->hdr.nEntries; i++) {
	if (*hashValue & bitmask) break;

	hashValue = (MLGF_HashValue*)((char*)hashValue + entryLen);
    }

    /* Move the entries to the new page. */
    memcpy((char*)MLGF_ITH_DIRENTRY(newPage, 0, entryLen),
	   (char*)MLGF_ITH_DIRENTRY(dirPage, i, entryLen),
	   (dirPage->hdr.nEntries-i)*entryLen);
    newPage->hdr.nEntries = dirPage->hdr.nEntries-i;

    dirPage->hdr.nEntries -= newPage->hdr.nEntries;

    /*
     * Write log record.
     */
    if (logParam->logFlag & LOG_FLAG_DATA_LOGGING) {

        /*
         * new page
         */
        initDirPageInfo.iid = newPage->hdr.iid;
        initDirPageInfo.height = newPage->hdr.height;
        initDirPageInfo.rootFlag = FALSE;
        initDirPageInfo.nKeys = newPage->hdr.nKeys;

        LOG_FILL_LOGRECINFO_1(logRecInfo, xactEntry->xactId, LOG_TYPE_UPDATE,
                              LOG_ACTION_MLGF_INIT_DIRECTORY_PAGE, LOG_REDO_ONLY,
                              newPage->hdr.pid, xactEntry->lastLsn, common_perThreadDSptr->nilLsn,
                              sizeof(LOG_Image_MLGF_InitDirectoryPage_T), &initDirPageInfo);

        e = LOG_WriteLogRecord(handle, xactEntry, &logRecInfo, &lsn, &logRecLen);
        if (e < eNOERROR) ERR(handle, e);

        entriesInfo.startEntryNo = 0;
        entriesInfo.nEntries = newPage->hdr.nEntries;

        LOG_FILL_LOGRECINFO_2(logRecInfo, xactEntry->xactId, LOG_TYPE_UPDATE,
                              LOG_ACTION_MLGF_INSERT_DIRECTORY_ENTRIES, LOG_REDO_ONLY,
                              newPage->hdr.pid, xactEntry->lastLsn, common_perThreadDSptr->nilLsn,
                              sizeof(LOG_Image_MLGF_SpecifyEntries_T), &entriesInfo,
                              entriesInfo.nEntries*entryLen, MLGF_ITH_DIRENTRY(newPage, 0, entryLen));

        e = LOG_WriteLogRecord(handle, xactEntry, &logRecInfo, &lsn, &logRecLen);
        if (e < eNOERROR) ERR(handle, e);

        /* mark the lsn in the page */
        newPage->hdr.lsn = lsn;
        newPage->hdr.logRecLen = logRecLen;

        /*
         * original page
         */
        entriesInfo.startEntryNo = dirPage->hdr.nEntries;
        entriesInfo.nEntries = newPage->hdr.nEntries;

        LOG_FILL_LOGRECINFO_2(logRecInfo, xactEntry->xactId, LOG_TYPE_UPDATE,
                              LOG_ACTION_MLGF_DELETE_DIRECTORY_ENTRIES, LOG_REDO_UNDO,
                              dirPage->hdr.pid, xactEntry->lastLsn, common_perThreadDSptr->nilLsn,
                              sizeof(LOG_Image_MLGF_SpecifyEntries_T), &entriesInfo,
                              entriesInfo.nEntries*entryLen, MLGF_ITH_DIRENTRY(newPage, 0, entryLen));

        e = LOG_WriteLogRecord(handle, xactEntry, &logRecInfo, &lsn, &logRecLen);
        if (e < eNOERROR) ERR(handle, e);

        /* mark the lsn in the page */
        dirPage->hdr.lsn = lsn;
        dirPage->hdr.logRecLen = logRecLen;
    }

    /* Insert the new entry, 'insertedEntry'. */

    /* insertedHashValue is the domain-th hash value of the insetedEntry. */
    insertedHashValue = MLGF_DIRENTRY_HASHVALUEPTR(insertedEntry, kdesc->nKeys)[domain];

    if (insertedHashValue & bitmask) /* insert into the new page */
	e = mlgf_InsertIntoDirectory(handle, xactEntry, newPage, kdesc, insertedEntry, logParam);
    else			/* insert into the original page */
	e = mlgf_InsertIntoDirectory(handle, xactEntry, dirPage, kdesc, insertedEntry, logParam);
    if (e < eNOERROR) ERRB2(handle, e, newPage_BCB, PAGE_BUF, dirPage_BCB, PAGE_BUF);

    /* Update the `srcEntry' and `dstEntry'. */
    srcEntry->theta = MLGF_DP_THETA(dirPage, entryLen);
    dstEntry->theta = MLGF_DP_THETA(newPage, entryLen);
    dstEntry->spid = newPid.pageNo;

    /*
    ** Set the MBR of the newPage.
    */
    extremeHashValues = MLGF_DIRENTRY_HASHVALUEPTR(dstEntry, kdesc->nKeys);
    for (k = 0; k < kdesc->nKeys; k++) {
        if (MLGF_KEYDESC_IS_MINTYPE(*kdesc, k))
            extremeHashValues[k] |= MLGF_HASHVALUE_SET_EXCEPT_UPPER_N_BITS(dstEntry->nValidBits[k]);
        else
            extremeHashValues[k] = MLGF_HASHVALUE_MASK_UPPER_N_BITS(extremeHashValues[k], dstEntry->nValidBits[k]);
    }

    dirEntryHashValues = MLGF_DIRENTRY_HASHVALUEPTR(MLGF_ITH_DIRENTRY(newPage, 0, entryLen), kdesc->nKeys);
    for (i = 0; i < newPage->hdr.nEntries; i++) {

	for (k = 0; k < kdesc->nKeys; k++) {

	    if ((MLGF_KEYDESC_IS_MINTYPE(*kdesc, k) && (extremeHashValues[k] > dirEntryHashValues[k])) ||
		(MLGF_KEYDESC_IS_MAXTYPE(*kdesc, k) && (extremeHashValues[k] < dirEntryHashValues[k]))) {
		extremeHashValues[k] = dirEntryHashValues[k];
	    }
	}

	dirEntryHashValues = (MLGF_HashValue*)((char*)dirEntryHashValues + entryLen);
    }

    /* Set the dirty flag of the buffer for new page. */
    newPage_BCB->dirtyFlag = 1;

    /* Unfix the new page. */
    e = BfM_unfixBuffer(handle, newPage_BCB, PAGE_BUF);
    if (e < eNOERROR) ERRB1(handle, e, dirPage_BCB, PAGE_BUF);


    /*
    ** Set the MBR of the dirPage.
    */
    extremeHashValues = MLGF_DIRENTRY_HASHVALUEPTR(srcEntry, kdesc->nKeys);
    for (k = 0; k < kdesc->nKeys; k++) {
	if (MLGF_KEYDESC_IS_MINTYPE(*kdesc, k))
            extremeHashValues[k] |= MLGF_HASHVALUE_SET_EXCEPT_UPPER_N_BITS(srcEntry->nValidBits[k]);
        else
            extremeHashValues[k] = MLGF_HASHVALUE_MASK_UPPER_N_BITS(extremeHashValues[k], srcEntry->nValidBits[k]);
    }

    dirEntryHashValues = MLGF_DIRENTRY_HASHVALUEPTR(MLGF_ITH_DIRENTRY(dirPage, 0, entryLen), kdesc->nKeys);
    for (i = 0; i < dirPage->hdr.nEntries; i++) {

	for (k = 0; k < kdesc->nKeys; k++) {

	    if ((MLGF_KEYDESC_IS_MINTYPE(*kdesc, k) && (extremeHashValues[k] > dirEntryHashValues[k])) ||
		(MLGF_KEYDESC_IS_MAXTYPE(*kdesc, k) && (extremeHashValues[k] < dirEntryHashValues[k]))) {
		extremeHashValues[k] = dirEntryHashValues[k];
	    }
	}

	dirEntryHashValues = (MLGF_HashValue*)((char*)dirEntryHashValues + entryLen);
    }

    /* Set the dirty flag of the buffer for original page. */
    dirPage_BCB->dirtyFlag = 1;

    /* Unfix the original page. */
    e = BfM_unfixBuffer(handle, dirPage_BCB, PAGE_BUF);
    if (e < eNOERROR) ERR(handle, e);

    /* release locks on dirPid and newPid */
    if(lockup){
	e = LM_releaseFlatPageLock(handle, &xactEntry->xactId, dirPid, L_MANUAL);
	if (e < eNOERROR) ERR(handle, e);

	e = LM_releaseFlatPageLock(handle, &xactEntry->xactId, &newPid, L_MANUAL);
	if (e < eNOERROR) ERR(handle, e);

    }

    return(eNOERROR);

} /* mlgf_SplitDirectoryPage() */
