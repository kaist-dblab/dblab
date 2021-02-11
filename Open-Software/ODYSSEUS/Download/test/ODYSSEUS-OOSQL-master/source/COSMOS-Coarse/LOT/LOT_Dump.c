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
 * Module: LOT_Dump.c
 *
 * Description:
 *  Dump the components of the large object tree.
 *
 * Exports:
 *  Four LOT_DumpTree(PageID *)
 *  Four LOT_DumpInternal(PageID *)
 *  Four LOT_DumpLeaf(PageID *, Four)
 */


#include "common.h"
#include "trace.h"
#include "BfM.h"
#include "LOT_Internal.h"
#include "perThreadDS.h"
#include "perProcessDS.h"



/*@================================
 * LOT_DumpTree()
 *================================*/
/*
 * Function: Four LOT_DumpTree(PageID*)
 *
 * Description:
 *  Dump the entire large object tree; preorder traversal
 *
 * Returns:
 *
 * Side effects:
 */
Four LOT_DumpTree(
    Four handle,
    PageID              *root)          /* IN root PageID of Large Object Tree */
{
    Four                e;              /* error number */
    Two                 i;              /* subscript variable */
    PageID              childPid;       /* PageID of child node */
    Four                childCount;     /* bytes in the subtree */
    L_O_T_INode         *node;          /* pointer to buffer holding current node */
    
    TR_PRINT(TR_LOT, TR1, ("LOT_DumpTree(handle, root=%P)", root));

    e = LOT_DumpInternal(handle, root);
    if (e < 0) ERR(handle, e);
    
    e = BfM_GetTrain(handle, root, (char **)&node, PAGE_BUF);
    if (e < 0) ERR(handle, e);

    for(i = 0; i < node->header.nEntries; i++) {

	/* get the child PageID */
	MAKE_PAGEID(childPid, root->volNo, node->entry[i].spid);
	
	if (node->header.height == 1) { /* node just above leaf level */
	    childCount = node->entry[i].count - ((i == 0) ? 0:node->entry[i-1].count);
	    LOT_DumpLeaf(handle, &childPid, childCount);
	} else
	    LOT_DumpTree(handle, &childPid);
    }

    e = BfM_FreeTrain(handle, root, PAGE_BUF);
    if (e < 0) ERR(handle, e);

    return(eNOERROR);
    
} /* LOT_DumpTree() */



/*@================================
 * LOT_DumpInternal()
 *================================*/
/*
 * Function: Four LOT_DumpInternal(PageID*)
 *
 * Description:
 *  Dump the internal node
 *
 * Returns:
 *
 */
Four LOT_DumpInternal(
    Four handle,
    PageID              *pid)           /* IN PageID of internal node to print */
{
    Four                e;              /* error number */
    L_O_T_INode         *node;          /* pointer to buffer holding node */
    Two                 i;              /* subscript variable */
    
    TR_PRINT(TR_LOT, TR1, ("LOT_DumpInternal(handle, pid=%P)", pid));

    e = BfM_GetTrain(handle, pid, (char **)&node, PAGE_BUF);
    if (e < 0) ERR(handle, e);

    printf("+-------------------+\n");
    printf("|     pageid = %lld   |\n", pid->pageNo);
    printf("|     height = %ld    |\n", node->header.height);
    printf("|   nEntreis = %3ld  |\n", node->header.nEntries);
    printf("+-------------------+\n");
    for (i = 0; i < node->header.nEntries; i++)
        printf("| spid = %-4lld | count = %-10lld |\n", node->entry[i].spid, node->entry[i].count);
    printf("+-------------------+\n\n");

    e = BfM_FreeTrain(handle, pid, PAGE_BUF);
    if (e < 0) ERR(handle, e);

    return(eNOERROR);

} /* LOT_DumpInternal() */



/*@================================
 * LOT_DumpLeaf()
 *================================*/
/*
 * Function: Four LOT_DumpLeaf(PageID*, Four)
 *
 * Description:
 *  Dump the leaf node(= data page)
 *
 * Returns:
 *
 */
Four LOT_DumpLeaf(
    Four handle,
    PageID *pid,		/* IN PageID of leaf node to print */
    Four   count)		/* # of bytes in this node */
{
    Four e;			/* error number */
    L_O_T_LNode *leaf;		/* pointer to data page buffer */
    Four i, j;
    char ch;
    
    TR_PRINT(TR_LOT, TR1, ("LOT_DumpLeaf(handle, pid=%P)", pid));

    e = BfM_GetTrain(handle, pid, (char **)&leaf, LOT_LEAF_BUF);
    if (e < 0) ERR(handle, e);

    printf("+----------------------------------------------------+\n");
    printf("|                     pageid = %4lld                 |\n", pid->pageNo);
    printf("|                      count = %4lld                  |\n",count);
    printf("+----------------------------------------------------+\n");

    e = BfM_FreeTrain(handle, pid, LOT_LEAF_BUF);
    if (e < 0) ERR(handle, e);
    
    return(eNOERROR);
    
} /* LOT_DumpLeaf() */
    