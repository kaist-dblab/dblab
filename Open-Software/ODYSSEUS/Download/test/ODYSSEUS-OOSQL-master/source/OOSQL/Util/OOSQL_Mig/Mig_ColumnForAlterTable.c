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
/*    ODYSSEUS/OOSQL DB-IR-Spatial Tightly-Integrated DBMS                    */
/*    Version 5.0                                                             */
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

#include "Mig_Catalog.h"


Four mig_CreateColumnCatalogTableForAlterTable(
    LOM_Handle      *handle,
	Four			volId,
	XactID			*xactId,
	char			*tableName,
	Four			createKind)
{
	Four			e;
	ColInfo 		lomSysColumns[LOM_SYSCOLUMNS_NUM_COLS];	/* lomSysColumns columns information */
	LRDS_IndexDesc 	idesc;									/* key information for the clustering index */
	char*			attrNames[LOM_MAXNUMOFATTRIBUTE];
	Four			classId;

	/* lomSysColumns Catalog table information */

	/* class id */
	lomSysColumns[LOM_SYSCOLUMNS_CLASSID_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_CLASSID_COLNO].type = LOM_LONG_VAR;
	lomSysColumns[LOM_SYSCOLUMNS_CLASSID_COLNO].length = sizeof(Four);

	/* complex type */
	lomSysColumns[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].type = LOM_SHORT_VAR;
	lomSysColumns[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].length = LOM_SHORT_SIZE_VAR;

	/* column type	*/
	/* incluing lom level type such as db_Ref, db_Link
	*/
	lomSysColumns[LOM_SYSCOLUMNS_COLTYPE_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_COLTYPE_COLNO].type = LOM_SHORT_VAR;
	lomSysColumns[LOM_SYSCOLUMNS_COLTYPE_COLNO].length = LOM_SHORT_SIZE_VAR;

	/* column number */
	lomSysColumns[LOM_SYSCOLUMNS_COLUMNNO_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_COLUMNNO_COLNO].type = LOM_SHORT_VAR;
	lomSysColumns[LOM_SYSCOLUMNS_COLUMNNO_COLNO].length = LOM_SHORT_SIZE_VAR;

	/* column name */
	lomSysColumns[LOM_SYSCOLUMNS_COLNAME_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_COLNAME_COLNO].type = SM_VARSTRING;
	lomSysColumns[LOM_SYSCOLUMNS_COLNAME_COLNO].length = MAXRELNAME;

	/* column offset */
	lomSysColumns[LOM_SYSCOLUMNS_COLOFFSET_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_COLOFFSET_COLNO].type = LOM_LONG_TYPE;
	lomSysColumns[LOM_SYSCOLUMNS_COLOFFSET_COLNO].length = sizeof(Four);

	/* inherited from */
	lomSysColumns[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].type = LOM_LONG_TYPE;
	lomSysColumns[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].length = sizeof(Four);

	/* domain */
	lomSysColumns[LOM_SYSCOLUMNS_DOMAINID_COLNO].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[LOM_SYSCOLUMNS_DOMAINID_COLNO].type = LOM_LONG_TYPE;
	lomSysColumns[LOM_SYSCOLUMNS_DOMAINID_COLNO].length = sizeof(Four);

	lomSysColumns[8].complexType = SM_COMPLEXTYPE_BASIC;
	lomSysColumns[8].type = LOM_SHORT_VAR;
	lomSysColumns[8].length = LOM_SHORT_SIZE_VAR;

	if (createKind == CREATE_TMP)	
	{
		e = LRDS_CreateRelation(volId, tableName, NULL, 9, &lomSysColumns[0], SM_FALSE);
		if (e < 0) {
			LRDS_AbortTransaction(xactId);
			LRDS_Dismount(volId);
			LOM_ERROR(handle,e);
		}
	}
	else if (createKind == CREATE_NEW)
	{

		/* key setting */
		idesc.indexType = SM_INDEXTYPE_BTREE;
		idesc.kinfo.btree.flag = KEYFLAG_CLUSTERING;
		idesc.kinfo.btree.nColumns = 1;
		idesc.kinfo.btree.columns[0].colNo = LOM_SYSCOLUMNS_CLASSID_COLNO;		
		idesc.kinfo.btree.columns[0].flag = KEYINFO_COL_ASC;		


		e = LRDS_CreateRelation(volId, tableName, &idesc, 9, &lomSysColumns[0], SM_FALSE);
		if (e < 0) {
			LRDS_AbortTransaction(xactId);
			LRDS_Dismount(volId);
			LOM_ERROR(handle,e);
		};

		attrNames[LOM_SYSCOLUMNS_CLASSID_COLNO] = LOM_SYSCOLUMNS_CLASSID_COLNAME;
		attrNames[LOM_SYSCOLUMNS_COLUMNNO_COLNO] = LOM_SYSCOLUMNS_COLUMNNO_COLNAME;
		attrNames[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO] = LOM_SYSCOLUMNS_COMPLEXTYPE_COLNAME;
		attrNames[LOM_SYSCOLUMNS_COLTYPE_COLNO] = LOM_SYSCOLUMNS_COLTYPE_COLNAME;
		attrNames[LOM_SYSCOLUMNS_COLNAME_COLNO] = LOM_SYSCOLUMNS_COLNAME_COLNAME;
		attrNames[LOM_SYSCOLUMNS_COLOFFSET_COLNO] = LOM_SYSCOLUMNS_COLOFFSET_COLNAME;
		attrNames[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO] = LOM_SYSCOLUMNS_INHERITEDFROM_COLNAME;
		attrNames[LOM_SYSCOLUMNS_DOMAINID_COLNO] = LOM_SYSCOLUMNS_DOMAINID_COLNAME;
		attrNames[8] = "activeColumnNo";

		classId = LOM_CATALOGCLASSID_START + 5;
		e = mig_Catalog_CreateLOMCatalogBasedOnLRDSCatalog(handle, volId, 
			LOM_SYSCOLUMNS_CLASSNAME, 9, attrNames, classId);
		if(e < eNOERROR) LOM_ERROR(handle, e);
	}


	return eNOERROR;
}


Four mig_CopyColumnCatalogTableForAlterTable(
    LOM_Handle      *handle,
	Four			volId,
	char			*sourceTableName,
	char			*targetTableName,
	Four			copyKind)
{
	Two  			keyLen;		/* should be Two to store key length */
	Four 			e;			/* error number */
	Four 			orn1;		/* open relation number */
	Four 			orn2;		/* open relation number */
	Four 			catScanId;	/* scan id on a catalog table */
	Four 			classId;	/* class Id */
	Four			nCols;
	TupleID 		tid;		/* tuple id for catalog entry in LOM level catalog */
	LockParameter 	lockup;		/* lockup for SM_Fetch Tuple */
	ColListStruct 	clist[9];	/* a column list structure */
	char			colName[LOM_MAXATTRNAME];



	orn1 = LRDS_OpenRelation(volId,sourceTableName);
	if(orn1 < 0) LOM_ERROR(handle, orn1);

	orn2 = LRDS_OpenRelation(volId,targetTableName);
	if(orn2 < 0) LOM_ERROR(handle, orn2);
		
	lockup.mode = L_IX;
	lockup.duration = L_COMMIT;

	catScanId = LRDS_OpenSeqScan(orn1, FORWARD, 0, NULL, &lockup);
	if (catScanId < 0) LOM_ERROR(handle, catScanId);

	e = LRDS_NextTuple(catScanId, &tid, NULL);
	if (e < 0) LOM_ERROR(handle, e);

	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].colNo = LOM_SYSCOLUMNS_CLASSID_COLNO;
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].dataLength = sizeof(Four);

	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].colNo = LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].length = LOM_SHORT_SIZE_VAR;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].dataLength = LOM_SHORT_SIZE_VAR;

	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].colNo = LOM_SYSCOLUMNS_COLTYPE_COLNO;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].length = LOM_SHORT_SIZE_VAR;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].dataLength = LOM_SHORT_SIZE_VAR;

	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].colNo = LOM_SYSCOLUMNS_COLUMNNO_COLNO;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].length = LOM_SHORT_SIZE_VAR;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].dataLength = LOM_SHORT_SIZE_VAR;

	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].colNo = LOM_SYSCOLUMNS_COLNAME_COLNO;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].length = LOM_MAXATTRNAME;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].dataLength = LOM_MAXATTRNAME;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].data.ptr = &colName[0];

	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].colNo = LOM_SYSCOLUMNS_COLOFFSET_COLNO;
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].dataLength = sizeof(Four);

	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].colNo = LOM_SYSCOLUMNS_INHERITEDFROM_COLNO;
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].dataLength = sizeof(Four);

	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].colNo = LOM_SYSCOLUMNS_DOMAINID_COLNO;
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].dataLength = sizeof(Four);

	clist[8].colNo = 8;
	clist[8].start = ALL_VALUE;
	clist[8].length = LOM_SHORT_SIZE_VAR;
	clist[8].dataLength = LOM_SHORT_SIZE_VAR;
	

	while (e != EOS) 
	{
		if (copyKind == OLD_TO_TMP)
		{
			e = LRDS_FetchTuple(catScanId, SM_TRUE, &tid, 8, clist);
			if(e < 0) LOM_ERROR(handle, e);
		}
		else if (copyKind == TMP_TO_NEW)
		{
			e = LRDS_FetchTuple(catScanId, SM_TRUE, &tid, 9, clist);
			if(e < 0) LOM_ERROR(handle, e);
		}

		if (GET_VALUE_FROM_COL_LIST(clist[LOM_SYSCOLUMNS_CLASSID_COLNO], sizeof(Four)) != 505)
		{
			if (copyKind == OLD_TO_TMP)
			{
    			e = LRDS_CreateTuple(orn2, SM_FALSE, 8, clist, (TupleID*)NULL);
    			if (e < 0) LOM_ERROR(handle, e);
			}
			else if (copyKind == TMP_TO_NEW)
			{
				ASSIGN_VALUE_TO_COL_LIST(clist[8], clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO], sizeof(Two));
				clist[8].nullFlag = SM_FALSE;

    			e = LRDS_CreateTuple(orn2, SM_FALSE, 9, clist, (TupleID*)NULL);
    			if (e < 0) LOM_ERROR(handle, e);
			}
		}

		/* move cursor next */
		e = LRDS_NextTuple(catScanId, &tid, NULL);
		if (e < 0) LOM_ERROR(handle, e);
	}

	/* close scan */
	e = LRDS_CloseScan(catScanId);
	if (e < 0) LOM_ERROR(handle, e);

	e = LRDS_CloseRelation(orn1);
	if( e < 0 ) LOM_ERROR(handle, e);

	e = LRDS_CloseRelation(orn2);
	if( e < 0 ) LOM_ERROR(handle, e);

    
	return eNOERROR;
}


Four mig_DestroyColumnCatalogTableForAlterTable(
    LOM_Handle      *handle,
	Four			volId,
	char			*tableName,
	Four			destroyKind)
{
	Two  			keyLen;		/* should be Two to store key length */
	Four 			e;			/* error number */
	Four 			orn;		/* open relation number */
	Four 			catScanId;	/* scan id on a catalog table */
	Four 			classId;	/* class Id */
	TupleID 		tid;		/* tuple id for catalog entry in LOM level catalog */
	BoundCond 		bound;		/* boundary condition of a range scan */
	LockParameter 	lockup;		/* lockup for SM_Fetch Tuple */
	ColListStruct 	clist[4];	/* a column list structure */
	lrds_RelTableEntry *relTableEntry;



	if (destroyKind == DESTROY_OLD)
	{
    	/*
    	** LOM_SYSCLASSES
    	*/
    
    	orn = LRDS_OpenRelation(volId,LOM_SYSCLASSES_CLASSNAME);
    	if( orn < 0 ) LOM_ERROR(handle, orn);
    
    	bound.op = SM_EQ;
    	keyLen = strlen(tableName);
    	bound.key.len = sizeof(Two) + keyLen;
    	bcopy(&keyLen,&(bound.key.val[0]),sizeof(Two));
    	bcopy(tableName,&(bound.key.val[sizeof(Two)]),keyLen);
    
    	lockup.mode = L_IX;
    	lockup.duration = L_COMMIT;
    
    	relTableEntry = LRDS_GET_RELTABLE_ENTRY(orn);
    
    	catScanId = LRDS_OpenIndexScan(orn,
    			 &(LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry))[0].iid,
    			 &bound,&bound,0,(BoolExp*)NULL,&lockup);
    	if (catScanId < 0) LOM_ERROR(handle, catScanId);
    
    	e = LRDS_NextTuple(catScanId, &tid, NULL);
    	if (e < 0) LOM_ERROR(handle, e);
    
    	clist[0].colNo = LOM_SYSCLASSES_CLASSID_COLNO;
    	clist[0].start = ALL_VALUE;
    
    	e =  LRDS_FetchTuple(catScanId, SM_TRUE, &tid,1,&clist[0]);
    	if (e < 0) LOM_ERROR(handle, e);
    
    	classId = GET_VALUE_FROM_COL_LIST(clist[0], sizeof(Four));
    
    	e = LRDS_DestroyTuple(catScanId, SM_TRUE, &tid);
    	if (e < 0) LOM_ERROR(handle, e);
    
    	e = LRDS_CloseScan(catScanId);
    	if (e < 0) LOM_ERROR(handle, e);
    
    	e = LRDS_CloseRelation(orn);
    	if( e < 0 ) LOM_ERROR(handle, e);
    
    
    
    	/*
    	** LOM_SYSINDEXES
    	*/
    
    	orn = LRDS_OpenRelation(volId,LOM_SYSINDEXES_CLASSNAME);
    	if( orn < 0 ) LOM_ERROR(handle, orn);
    
    	lockup.mode = L_IX;
    	lockup.duration = L_COMMIT;
    
    	relTableEntry = LRDS_GET_RELTABLE_ENTRY(orn);
    
    	bound.op = SM_EQ;
    	keyLen = strlen(tableName);
    	bound.key.len = sizeof(Two) + keyLen;
    	bcopy(&keyLen,&(bound.key.val[0]),sizeof(Two));
    	bcopy(tableName,&(bound.key.val[sizeof(Two)]),keyLen);
    
    	/* index scan of LOM_SYSINDEXES */
    	/* we here shold use in-memory catalog manager to get index id 
    	*/
    	if((LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry))[1].colNo[0] == LOM_SYSINDEXES_CLASSNAME_COLNO)
    		catScanId = LRDS_OpenIndexScan(orn,
    						&(LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry))[1].iid,
    						&bound, &bound, 0, NULL, &lockup);
    	else
    		catScanId = LRDS_OpenIndexScan(orn,
    						&(LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry))[1].iid,
    						&bound, &bound, 0, NULL, &lockup);
    
    	if (catScanId < 0) LOM_ERROR(handle, catScanId);
    
    	e = LRDS_NextTuple(catScanId, (TupleID*)NULL, NULL);
    	if (e < 0) LOM_ERROR(handle, e);
    
    	while (e != EOS) {
    		e = LRDS_DestroyTuple(catScanId, SM_TRUE, (TupleID*)NULL);
    		if (e < 0) LOM_ERROR(handle, e);
    
    		e = LRDS_NextTuple(catScanId, (TupleID*)NULL, NULL);
    		if (e < 0) LOM_ERROR(handle, e);
    	}
    
    	e = LRDS_CloseScan(catScanId);
    	if (e < 0) LOM_ERROR(handle, e);
    
    	e = LRDS_CloseRelation(orn);
    	if( e < 0 ) LOM_ERROR(handle, e);
	}


	/*
    ** Destroy given table
    */
	e = LRDS_DestroyRelation(volId, tableName);
	if(e < 0) LOM_ERROR(handle, e);


	return eNOERROR;
}

Four mig_CheckColumnCatalogTableForAlterTable(
    LOM_Handle      *handle,
	Four			volId,
	char			*tableName)
{
	Two  			keyLen;		/* should be Two to store key length */
	Four 			e;			/* error number */
	Four 			orn1;		/* open relation number */
	Four 			catScanId;	/* scan id on a catalog table */
	Four 			classId;	/* class Id */
	Four			nCols;
	TupleID 		tid;		/* tuple id for catalog entry in LOM level catalog */
	LockParameter 	lockup;		/* lockup for SM_Fetch Tuple */
	ColListStruct 	clist[9];	/* a column list structure */
	char			colName[LOM_MAXATTRNAME];



	orn1 = LRDS_OpenRelation(volId,tableName);
	if(orn1 < 0) LOM_ERROR(handle, orn1);

		
	lockup.mode = L_IS;
	lockup.duration = L_COMMIT;

	catScanId = LRDS_OpenSeqScan(orn1, FORWARD, 0, NULL, &lockup);
	if (catScanId < 0) LOM_ERROR(handle, catScanId);

	e = LRDS_NextTuple(catScanId, &tid, NULL);
	if (e < 0) LOM_ERROR(handle, e);

	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].colNo = LOM_SYSCOLUMNS_CLASSID_COLNO;
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].dataLength = sizeof(Four);

	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].colNo = LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].length = LOM_SHORT_SIZE_VAR;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].dataLength = LOM_SHORT_SIZE_VAR;

	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].colNo = LOM_SYSCOLUMNS_COLTYPE_COLNO;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].length = LOM_SHORT_SIZE_VAR;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].dataLength = LOM_SHORT_SIZE_VAR;

	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].colNo = LOM_SYSCOLUMNS_COLUMNNO_COLNO;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].length = LOM_SHORT_SIZE_VAR;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].dataLength = LOM_SHORT_SIZE_VAR;

	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].colNo = LOM_SYSCOLUMNS_COLNAME_COLNO;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].length = LOM_MAXATTRNAME;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].dataLength = LOM_MAXATTRNAME;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].data.ptr = &colName[0];

	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].colNo = LOM_SYSCOLUMNS_COLOFFSET_COLNO;
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].dataLength = sizeof(Four);

	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].colNo = LOM_SYSCOLUMNS_INHERITEDFROM_COLNO;
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].dataLength = sizeof(Four);

	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].colNo = LOM_SYSCOLUMNS_DOMAINID_COLNO;
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].length = sizeof(Four);
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].dataLength = sizeof(Four);

	clist[8].colNo = 8;
	clist[8].start = ALL_VALUE;
	clist[8].length = LOM_SHORT_SIZE_VAR;
	clist[8].dataLength = LOM_SHORT_SIZE_VAR;

	while (e != EOS) 
	{
		e = LRDS_FetchTuple(catScanId, SM_TRUE, &tid, 9, clist);
		if(e < 0) LOM_ERROR(handle, e);

		/* move cursor next */
		e = LRDS_NextTuple(catScanId, &tid, NULL);
		if (e < 0) LOM_ERROR(handle, e);
	}

	/* close scan */
	e = LRDS_CloseScan(catScanId);
	if (e < 0) LOM_ERROR(handle, e);

	e = LRDS_CloseRelation(orn1);
	if( e < 0 ) LOM_ERROR(handle, e);

    
	return eNOERROR;
}

Four mig_Catalog_CreateLOMCatalogBasedOnLRDSCatalog(
	LOM_Handle*		handle,
	Four			volId, 
	char*			className,
	Four			nAttrs,
	char**			attrNames,
	Four			classId
)
{
	AttrInfo			attrInfos[LOM_MAXNUMOFATTRIBUTE];
	Four				orn;
	lrds_RelTableEntry	*relTableEntry;
	ColDesc				*cdesc;
	IndexInfo			*indexInfo;
	Four				i, j;
	LOM_IndexDesc		idesc;
	char				indexName[LOM_MAXINDEXNAME];
	Four				e;

	orn = LRDS_OpenRelation(volId, className);
	if(orn < eNOERROR) LOM_ERROR(handle, orn);
	
	/* Get the relation table entry. */
    relTableEntry = LRDS_GET_RELTABLE_ENTRY(orn);
	for(i = 0; i < nAttrs; i++)
	{
		cdesc = &((LRDS_GET_COLDESC_FROM_RELTABLE_ENTRY(relTableEntry))[i]);

		attrInfos[i].complexType	= cdesc->complexType;
		attrInfos[i].domain			= cdesc->type;
		attrInfos[i].inheritedFrom	= LOM_INHERITEDFROM_THIS_CLASS;
		attrInfos[i].length			= cdesc->length;
		strcpy(attrInfos[i].name, attrNames[i]);
		attrInfos[i].type			= cdesc->type;
		attrInfos[i].offset         = cdesc->offset;
	}
	
	e = mig_Catalog_CreateClassCatalog(handle, volId, className, 0, NULL, nAttrs, attrInfos,
		                           0, NULL, 0, NULL, SM_FALSE, classId);
	if(e < eNOERROR) LOM_ERROR(handle, e);

	for (i = 0; i < relTableEntry->ri.nIndexes; i++) 
	{
		indexInfo = &(LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry))[i];

		sprintf(indexName, "%s_Index_%d", className, i);

		idesc.indexType = indexInfo->indexType;
		switch(idesc.indexType)
		{
		case SM_INDEXTYPE_BTREE:
			idesc.kinfo.btree.flag     = indexInfo->kdesc.btree.flag;
			idesc.kinfo.btree.nColumns = indexInfo->kdesc.btree.nparts;
			for(j = 0; j < idesc.kinfo.btree.nColumns; j++)
			{
				idesc.kinfo.btree.columns[j].colNo = indexInfo->colNo[j];
				idesc.kinfo.btree.columns[j].flag  = KEYINFO_COL_ASC;
			}
			break;

		case SM_INDEXTYPE_MLGF:
			idesc.kinfo.mlgf.flag			= indexInfo->kdesc.mlgf.flag;
			idesc.kinfo.mlgf.extraDataLen	= indexInfo->kdesc.mlgf.extraDataLen;
			idesc.kinfo.mlgf.nColumns		= indexInfo->kdesc.mlgf.nKeys;
			for(j = 0; j < idesc.kinfo.mlgf.nColumns; j++)
				idesc.kinfo.mlgf.colNo[j] = indexInfo->colNo[j];
			break;

		default:
			LOM_ERROR(handle, eINTERNAL_LOM);
		}

		e = mig_Catalog_CreateIndexCatalog(handle, volId, className, indexName, &idesc, &indexInfo->iid);
		if(e < eNOERROR) LOM_ERROR(handle, e);
	}				

	e = LRDS_CloseRelation(orn);
	if(e < eNOERROR) LOM_ERROR(handle, e);

	return eNOERROR;
}

Four mig_Catalog_CreateClassCatalog(
	LOM_Handle	*handle,
	Four    volId,			/* IN volume in which the relation will be placed */
	char    *className,			/* IN relation name to create */
	char	*indexName,			/* IN index name to create */
	LOM_IndexDesc *idesc,				/* IN key info for the culustering index */
	Four    nAttrs,				/* IN number of atrributes */
	AttrInfo *attrinfo,			/* IN attribute information */
	Four	nSuperclasses,		/* IN number of superclasss */
	char	(*superclassList)[LOM_MAXCLASSNAME],	/* IN names of superclasss */
	Four	nMethods,			/* IN number of methods	*/
	MethodInfo	*methodinfo,	/* IN method information */
	Boolean		tmpClassFlag,		/* IN flag to indicate if this class is temporary or not */
	Four	classId) 			/* IN class id */		
{
	Two  keyLen;			/* should be Two to store key length */
	Four e;					/* error number */
	Four i,j;					/* index variable */
	Four v;					/* index on LRDS mount table */
	Four catScanId;			/* scan id for a catalog table access */
	Four catScanId2;		/* scan id for a catalog table access */
	Four maxTupleLen;		/* maximum tuple length */
	FileID fid;				/* SM level file identifier */
	FileID superclassfid;	/* SM level file identifier */
	LOM_IndexID iid;			/* index used for clustering */
	LOM_IndexID logical_id_index;	/* index built on logical-id column */
	Boolean found;			/* TRUE if the relation is already defined */
	BoundCond bound;		/* boundary condition */
	ColListStruct clist[LOM_SYSCOLUMNS_NUM_COLS];	/* column list */
	ColListStruct clist_for_classid[LOM_CLASSID_NUM_COLS]; /* column list */
	LockParameter lockup;      
	Four orn;		/* open relation number */
	Four orn2;		/* open relation number */
	Four offset;	/* offset of the corresponging attribute of an object */
	BoolExp boolexp[1];
	TupleID	tupleid;
	TupleID tid;
	Four alignmentoffset;
	ColInfo	colInfo[LOM_MAXNUMOFATTRIBUTE];	/* ColInfo list */
	Boolean isFirstTextAttr;
	lrds_RelTableEntry *relTableEntry; /* pointer to an entry of relation table */
	lrds_RelTableEntry *relTableEntry2; /* pointer to an entry of relation table */
	char invertedIndexTableName[LOM_MAXCLASSNAME];
	char logicalIdIndexName[LOM_MAXCLASSNAME];
	LockParameter lockup2;      
	Four superclassId;
	Four fromClassId;
	Two  fromAttrNum;
	Four toClassId;
	Two  toAttrNum;
	One  direction;
	One  cardinality;
	One  reverseCardinality;
	Four relationshipId;
	char relationshipName[LOM_MAXRELATIONSHIPNAME];
	ColListStruct clist_for_rel[LOM_SYSRELATIONSHIP_NUM_COLS];
	ClassID superClassIdList[LOM_MAXSUPERCLASSNUM];
	Four mv;

	/* check parameters */
	if (volId < 0) LOM_ERROR(handle, eBADPARAMETER_LOM);
    
	if (className == NULL) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if ((indexName == NULL && idesc != NULL) || (indexName != NULL && idesc == NULL)) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if (nAttrs < 0) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if ( tmpClassFlag ) {
		if(nSuperclasses!=0 || nMethods != 0) LOM_ERROR(handle, eBADPARAMETER_LOM);
	}

	/*
		open LRDS_SYSTABLES and get DFileID
	*/
	e = LRDS_GetFileIdOfRelation( volId, className, &fid);
	if ( e < 0) LOM_ERROR(handle, e);
	
	/*
		LOM Catalogs Update
	*/

	/* START : LOM_SYSCLASSES */ /* THINK */

	/* set the lockup parameter */
	lockup.mode = L_IX;
	lockup.duration = L_COMMIT;

	orn = LRDS_OpenRelation(volId, LOM_SYSCLASSES_CLASSNAME);
	if (orn < 0) LOM_ERROR(handle, orn);

	/* Construct 'clist'. */
	clist[0].colNo = LOM_SYSCLASSES_CLASSNAME_COLNO;
	clist[0].start = ALL_VALUE;
	clist[0].dataLength = strlen(className);
	clist[0].data.ptr = className;
	clist[0].nullFlag = SM_FALSE;
	
	clist[1].colNo = LOM_SYSCLASSES_CLASSID_COLNO;
	clist[1].start = ALL_VALUE;
	clist[1].dataLength = sizeof(Four);
	ASSIGN_VALUE_TO_COL_LIST(clist[1], classId, sizeof(Four));
	clist[1].nullFlag = SM_FALSE;
	
	clist[2].colNo = LOM_SYSCLASSES_DATAFILEID_COLNO;
	clist[2].start = ALL_VALUE;
	clist[2].dataLength = sizeof(FileID);
	clist[2].data.fid = fid; 
	clist[2].nullFlag = SM_FALSE;

	e = LRDS_CreateTuple(orn, SM_FALSE, LOM_SYSCLASSES_NUM_COLS, &(clist[0]), (TupleID*)NULL);
	if (e < 0) LOM_ERROR(handle, e);
	
	/* Close the data file */
	e = LRDS_CloseRelation(orn);
	if (e < 0) LOM_ERROR(handle, e);

	/* END : LOM_SYSCLASSES */ 

	/* open relation number for LOM_SYSCLASSES_CLASSNAME */
	orn2 = LRDS_OpenRelation(volId,LOM_SYSCLASSES_CLASSNAME);
	if (orn2 < 0) LOM_ERROR(handle, orn2);

	/* START: LOM_INHERITANCE */ /* THINK */

	orn = LRDS_OpenRelation(volId,LOM_INHERITANCE_CLASSNAME);
	if (orn < 0) LOM_ERROR(handle, orn);

	/* set the lockup parameter */
	lockup.mode = L_IX;
	lockup.duration = L_COMMIT;

	for(i=0;i < nSuperclasses ; i++) {
		/* Construct 'clist'. */
		clist[0].colNo = LOM_INHERITANCE_CLASSID_COLNO;
		clist[0].start = ALL_VALUE;
		clist[0].dataLength = sizeof(Four);
		ASSIGN_VALUE_TO_COL_LIST(clist[0], classId, sizeof(Four));
		clist[0].nullFlag = SM_FALSE;

		/* set lock up parameters */
		lockup.mode = L_IS;
		lockup.duration = L_COMMIT;

		/* get class id of superclasss. */
		boolexp[0].op = SM_EQ;
		boolexp[0].colNo = LOM_SYSCLASSES_CLASSNAME_COLNO;
		boolexp[0].length = strlen(superclassList[i]);
		bcopy(superclassList[i],boolexp[0].data.str,boolexp[0].length);

		/* Open for finding superclasss */
		catScanId2 = LRDS_OpenSeqScan(orn2, FORWARD, 1, &boolexp[0], &lockup);
		if (catScanId2 < 0) LOM_ERROR(handle, catScanId2);
    
		e = LRDS_NextTuple(catScanId2, &tupleid, NULL);
		if (e < 0) LOM_ERROR(handle, e);

		if (e == EOS) LOM_ERROR(handle, eRELATIONNOTFOUND_LOM);

		clist_for_classid[0].colNo = LOM_SYSCLASSES_CLASSID_COLNO;
		clist_for_classid[0].start = ALL_VALUE;
     
		e =  LRDS_FetchTuple(catScanId2, SM_TRUE, &tupleid, 1, &clist_for_classid[0]);
		if (e < 0) LOM_ERROR(handle, e);

		e = LRDS_CloseScan(catScanId2);
		if (e < 0) LOM_ERROR(handle, e); 

		superClassIdList[i] = GET_VALUE_FROM_COL_LIST(clist_for_classid[0], sizeof(superClassIdList[i]));

		clist[1].colNo = LOM_INHERITANCE_SUPERCLASSID_COLNO;
		clist[1].start = ALL_VALUE;
		clist[1].dataLength = sizeof(Four);
#ifndef SUPPORT_LARGE_DATABASE2
		clist[1].data.l = clist_for_classid[0].data.l;
#else
		clist[1].data.ll = clist_for_classid[0].data.ll;
#endif
		clist[1].nullFlag = SM_FALSE;
	
		clist[2].colNo = LOM_INHERITANCE_ORDER_COLNO;
		clist[2].start = ALL_VALUE;
		clist[2].dataLength = sizeof(Four);
		ASSIGN_VALUE_TO_COL_LIST(clist[2], i, sizeof(Four));
		clist[2].nullFlag = SM_FALSE;

		e = LRDS_CreateTuple(orn, SM_FALSE, LOM_INHERITANCE_NUM_COLS, &(clist[0]), (TupleID*)NULL);
		if (e < 0) LOM_ERROR(handle, e);
	}
    
	/* Close the data file */
	e = LRDS_CloseRelation(orn);
	if (e < 0) LOM_ERROR(handle, e);

	/* END: LOM_INHERITANCE */ 

	/* START : LOM_SYSCOLUMNS */ /* THINK */

	/* set the lockup parameter */
	lockup.mode = L_IX;
	lockup.duration = L_COMMIT;

	orn = LRDS_OpenRelation(volId,LOM_SYSCOLUMNS_CLASSNAME);
	if (orn < 0) LOM_ERROR(handle, orn);

	/* Construct 'clist'. */
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].colNo = LOM_SYSCOLUMNS_CLASSID_COLNO;
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].dataLength = sizeof(Four);
	ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_CLASSID_COLNO], classId, sizeof(Four));
	clist[LOM_SYSCOLUMNS_CLASSID_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].colNo = LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].dataLength = sizeof(Two);
	clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].colNo = LOM_SYSCOLUMNS_COLTYPE_COLNO;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].dataLength = sizeof(Two);
	clist[LOM_SYSCOLUMNS_COLTYPE_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].colNo = LOM_SYSCOLUMNS_COLUMNNO_COLNO;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].dataLength = sizeof(Two);
	clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].colNo = LOM_SYSCOLUMNS_COLNAME_COLNO;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLNAME_COLNO].nullFlag = SM_FALSE;
	/* length is set where the offet is calculated */

	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].colNo = LOM_SYSCOLUMNS_COLOFFSET_COLNO;
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].dataLength =  sizeof(Four);
	clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO].nullFlag =  SM_FALSE;

	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].colNo = LOM_SYSCOLUMNS_INHERITEDFROM_COLNO;
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].dataLength =  sizeof(Four);
	clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO].nullFlag =  SM_FALSE;

	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].colNo = LOM_SYSCOLUMNS_DOMAINID_COLNO;
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].nullFlag = SM_FALSE;

	clist[8].colNo = 8;
	clist[8].start = ALL_VALUE;
	clist[8].dataLength = sizeof(Two);
	clist[8].nullFlag = SM_FALSE;

	offset = 0;

	isFirstTextAttr = SM_TRUE;

	for (i = 0; i < nAttrs; i++) {
		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLUMNNO_COLNO], i, sizeof(Two));
		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COMPLEXTYPE_COLNO], attrinfo[i].complexType, sizeof(Two));
		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLTYPE_COLNO], attrinfo[i].type, sizeof(Two));
		clist[LOM_SYSCOLUMNS_COLNAME_COLNO].dataLength = strlen(attrinfo[i].name) +1;	
		clist[LOM_SYSCOLUMNS_COLNAME_COLNO].data.ptr = attrinfo[i].name;
		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
		clist[LOM_SYSCOLUMNS_DOMAINID_COLNO].dataLength = sizeof(Four);
		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_DOMAINID_COLNO], attrinfo[i].domain, sizeof(Four));
		ASSIGN_VALUE_TO_COL_LIST(clist[8], i, sizeof(Two));

		if ( attrinfo[i].inheritedFrom == LOM_INHERITEDFROM_THIS_CLASS) 
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO], classId, sizeof(Four));
		else
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_INHERITEDFROM_COLNO], attrinfo[i].inheritedFrom, sizeof(Four));

		/* calcultate the offset of next attribute */
		if (attrinfo[i].type == SM_STRING) {
			offset += attrinfo[i].length;
		}
		else if ( attrinfo[i].type == LOM_TEXT) { 
			offset += sizeof(LOM_IndexDesc);
		}
		else if(attrinfo[i].complexType == LOM_COMPLEXTYPE_ODMG_COLLECTIONSET ||
			attrinfo[i].complexType == LOM_COMPLEXTYPE_ODMG_COLLECTIONBAG ||
			attrinfo[i].complexType == LOM_COMPLEXTYPE_ODMG_COLLECTIONLIST ||
			attrinfo[i].complexType == LOM_COMPLEXTYPE_ODMG_COLLECTIONVARARRAY) 
		{
			offset += sizeof(ODMG_CollectionDesc);
		}
		else if (attrinfo[i].type == SM_SHORT) {
			if(offset % SM_SHORT_SIZE) offset = offset + SM_SHORT_SIZE - (offset%SM_SHORT_SIZE);
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
			offset += SM_SHORT_SIZE;
		}
		else if (attrinfo[i].type == SM_LONG) {
			if(offset % SM_LONG_SIZE) offset = offset + SM_LONG_SIZE - (offset%SM_LONG_SIZE);
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
			offset += SM_LONG_SIZE;
		}
		else if (attrinfo[i].type == SM_LONG_LONG) {
			if(offset % SM_LONG_LONG_SIZE) offset = offset + SM_LONG_LONG_SIZE - (offset%SM_LONG_LONG_SIZE);
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
			offset += SM_LONG_LONG_SIZE;
		}
		else if (attrinfo[i].type == SM_MBR) {
			if(offset % SM_MBR_SIZE) offset = offset + SM_MBR_SIZE - (offset%SM_MBR_SIZE);
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
			offset += SM_MBR_SIZE;
		}
		else if (attrinfo[i].type == SM_OID || attrinfo[i].type == ODYS_REF || attrinfo[i].type == ODYS_LINK) {
			if(offset % SM_LONG_SIZE) offset = offset + SM_LONG_SIZE - (offset%SM_LONG_SIZE);
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
			offset += LOM_OBJREF_SIZE;
		}
		else {      /* SM_VARSTRING */ /* i.e. pointer to data */
			if(offset % sizeof(void *)) offset = offset + sizeof(void *) - (offset%sizeof(void *));
			ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSCOLUMNS_COLOFFSET_COLNO], offset, sizeof(Four));
			offset += sizeof(void *);
		}

		e = LRDS_CreateTuple(orn, SM_FALSE, LOM_SYSCOLUMNS_NUM_COLS, &(clist[0]), (TupleID*)&tid);
		if (e < 0) LOM_ERROR(handle, e);

		if( attrinfo[i].type == LOM_TEXT) {
			if(isFirstTextAttr) {
				LOM_IndexDesc idesc;

				/* create content table for storing text type attribute */
				e = lom_Text_CreateContentTable(handle, volId, className);
				if(e < eNOERROR) LOM_ERROR(handle, e);

				/* create index on 'logical id' column */
				idesc.indexType = SM_INDEXTYPE_BTREE;
				idesc.kinfo.btree.flag = 0; 
				idesc.kinfo.btree.nColumns = 1;
				idesc.kinfo.btree.columns[0].colNo = -1; /* logical id column */
				idesc.kinfo.btree.columns[0].flag = KEYINFO_COL_ASC;
				/* In LOM_AddIndex we increase column number by 1 for
				   we regrad first column as 1.
				   Here we mean really the 0th column. so set -1.
				 */


				lom_Text_MakeLogicalIdIndexName(handle, volId, className, logicalIdIndexName);

				/* add index on logical-id column */
				e = LOM_AddIndex(handle, volId, className, logicalIdIndexName, &idesc, &logical_id_index);
				if( e < eNOERROR) LOM_ERROR(handle, e);

				/* create logical counter */
				e = lom_Text_CreateCounter(handle, volId, className, 0);
				if( e < eNOERROR) LOM_ERROR(handle, e);

				/* make isFirstTextAttr be false */
				isFirstTextAttr = SM_FALSE;
			}

			/* create inverted index table */
			e = lom_Text_CreateInvertedIndexTable(handle, volId, className, attrinfo[i].name, i);
			if(e < eNOERROR) LOM_ERROR(handle, e);

			/* create docId index table */
			e = lom_Text_CreateDocIdIndexTable(handle, volId, className, attrinfo[i].name, i); 
			if(e < eNOERROR) LOM_ERROR(handle, e);

			/* add inverted index info. to catalog */
			e = lom_Text_AddIndexInfoIntoCatalog(handle, volId, className, attrinfo[i].name, i);
			if(e < eNOERROR) LOM_ERROR(handle, e);
		}
	}

	/* Close the data file */
	e = LRDS_CloseRelation(orn);
	if (e < 0) LOM_ERROR(handle, e);


	/* END : LOM_SYSCOLUMNS */ 

	/* START: LOM_SYSMETHODS */ /* THINK */

	orn = LRDS_OpenRelation(volId,LOM_SYSMETHODS_CLASSNAME);
	if (orn < 0) LOM_ERROR(handle, orn);

	/* set the lockup parameter */
	lockup.mode = L_IX;
	lockup.duration = L_COMMIT;

	/* Construct 'clist'. */
	clist[LOM_SYSMETHODS_CLASSID_COLNO].colNo = LOM_SYSMETHODS_CLASSID_COLNO;
	clist[LOM_SYSMETHODS_CLASSID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_CLASSID_COLNO].dataLength = sizeof(Four);
	ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSMETHODS_CLASSID_COLNO], classId, sizeof(Four));
	clist[LOM_SYSMETHODS_CLASSID_COLNO].nullFlag = SM_FALSE;
	
	clist[LOM_SYSMETHODS_DIRPATH_COLNO].colNo = LOM_SYSMETHODS_DIRPATH_COLNO;
	clist[LOM_SYSMETHODS_DIRPATH_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_DIRPATH_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSMETHODS_METHODNAME_COLNO].colNo = LOM_SYSMETHODS_METHODNAME_COLNO;
	clist[LOM_SYSMETHODS_METHODNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_METHODNAME_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSMETHODS_FUNCTIONNAME_COLNO].colNo = LOM_SYSMETHODS_FUNCTIONNAME_COLNO;
	clist[LOM_SYSMETHODS_FUNCTIONNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_FUNCTIONNAME_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSMETHODS_NARGUMENTS_COLNO].colNo = LOM_SYSMETHODS_NARGUMENTS_COLNO;
	clist[LOM_SYSMETHODS_NARGUMENTS_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_NARGUMENTS_COLNO].dataLength = sizeof(Two);
	clist[LOM_SYSMETHODS_NARGUMENTS_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSMETHODS_ARGUMENTLIST_COLNO].colNo = LOM_SYSMETHODS_ARGUMENTLIST_COLNO;
	clist[LOM_SYSMETHODS_ARGUMENTLIST_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_ARGUMENTLIST_COLNO].nullFlag = SM_FALSE;

	clist[LOM_SYSMETHODS_RETURNTYPE_COLNO].colNo = LOM_SYSMETHODS_RETURNTYPE_COLNO;
	clist[LOM_SYSMETHODS_RETURNTYPE_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_RETURNTYPE_COLNO].dataLength = sizeof(Four);
	clist[LOM_SYSMETHODS_RETURNTYPE_COLNO].nullFlag = SM_FALSE;
	
	clist[LOM_SYSMETHODS_INHERITEDFROM_COLNO].colNo = LOM_SYSMETHODS_INHERITEDFROM_COLNO;
	clist[LOM_SYSMETHODS_INHERITEDFROM_COLNO].start = ALL_VALUE;
	clist[LOM_SYSMETHODS_INHERITEDFROM_COLNO].dataLength = sizeof(Four);
	clist[LOM_SYSMETHODS_INHERITEDFROM_COLNO].nullFlag = SM_FALSE;

	for(i=0;i < nMethods; i++) {
		clist[LOM_SYSMETHODS_DIRPATH_COLNO].dataLength = strlen(methodinfo[i].dirPath);
		clist[LOM_SYSMETHODS_DIRPATH_COLNO].data.ptr = methodinfo[i].dirPath;

		clist[LOM_SYSMETHODS_FUNCTIONNAME_COLNO].dataLength = strlen(methodinfo[i].functionName);
		clist[LOM_SYSMETHODS_FUNCTIONNAME_COLNO].data.ptr = methodinfo[i].functionName;
		
		clist[LOM_SYSMETHODS_METHODNAME_COLNO].dataLength = strlen(methodinfo[i].name);
		clist[LOM_SYSMETHODS_METHODNAME_COLNO].data.ptr = methodinfo[i].name;

		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSMETHODS_NARGUMENTS_COLNO], methodinfo[i].nArguments, sizeof(Two));

		clist[LOM_SYSMETHODS_ARGUMENTLIST_COLNO].dataLength = sizeof(Four)*methodinfo[i].nArguments;
		clist[LOM_SYSMETHODS_ARGUMENTLIST_COLNO].data.ptr = methodinfo[i].ArgumentsList;

		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSMETHODS_RETURNTYPE_COLNO], methodinfo[i].returnType, sizeof(Four));

		ASSIGN_VALUE_TO_COL_LIST(clist[LOM_SYSMETHODS_INHERITEDFROM_COLNO], methodinfo[i].inheritedFrom, sizeof(Four));

		e = LRDS_CreateTuple(orn, SM_FALSE, LOM_SYSMETHODS_NUM_COLS, &(clist[0]), (TupleID*)NULL);
		if (e < 0) LOM_ERROR(handle, e);
	}
		
	/* Close the data file */
	e = LRDS_CloseRelation(orn);
	if (e < 0) LOM_ERROR(handle, e);

	/* END: LOM_SYSMETHODS */ 

	/* Close LOM_SYSCLASSES */
	e = LRDS_CloseRelation(orn2);
	if ( e < 0) LOM_ERROR(handle, e);

	/* START: LOM_INDEX */
	if(indexName != NULL) {
		orn = LRDS_OpenRelation(volId, className);
		if (orn < 0) LOM_ERROR(handle, orn);

		relTableEntry = LRDS_GET_RELTABLE_ENTRY(orn);
		/* The number of the existing index on this class is only one */

		orn2 = LRDS_OpenRelation(volId,LOM_SYSINDEXES_CLASSNAME);
		if (orn2 < 0) LOM_ERROR(handle, orn2);

		iid.isLogical = SM_FALSE;
		iid.index.physical_iid = (LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry))[0].iid;

		/* Construct 'clist'. */
		clist[LOM_SYSINDEXES_INDEXNAME_COLNO].colNo = LOM_SYSINDEXES_INDEXNAME_COLNO;
		clist[LOM_SYSINDEXES_INDEXNAME_COLNO].start = ALL_VALUE;
		clist[LOM_SYSINDEXES_INDEXNAME_COLNO].dataLength = strlen(indexName);
		clist[LOM_SYSINDEXES_INDEXNAME_COLNO].data.ptr = indexName;
		clist[LOM_SYSINDEXES_INDEXNAME_COLNO].nullFlag = SM_FALSE;
		   
		clist[LOM_SYSINDEXES_CLASSNAME_COLNO].colNo = LOM_SYSINDEXES_CLASSNAME_COLNO;
		clist[LOM_SYSINDEXES_CLASSNAME_COLNO].start = ALL_VALUE;
		clist[LOM_SYSINDEXES_CLASSNAME_COLNO].dataLength = strlen(className);
		clist[LOM_SYSINDEXES_CLASSNAME_COLNO].data.ptr = className;
		clist[LOM_SYSINDEXES_CLASSNAME_COLNO].nullFlag = SM_FALSE;
		   
		clist[LOM_SYSINDEXES_INDEXID_COLNO].colNo = LOM_SYSINDEXES_INDEXID_COLNO;
		clist[LOM_SYSINDEXES_INDEXID_COLNO].start = ALL_VALUE;
		clist[LOM_SYSINDEXES_INDEXID_COLNO].dataLength = sizeof(LOM_IndexID);
		clist[LOM_SYSINDEXES_INDEXID_COLNO].data.ptr = &iid;
		clist[LOM_SYSINDEXES_INDEXID_COLNO].nullFlag = SM_FALSE;

		e = LRDS_CreateTuple(orn2, SM_FALSE, LOM_SYSINDEXES_NUM_COLS, &(clist[0]), (TupleID*)NULL);
		if (e < 0) LOM_ERROR(handle, e);

		/* Close the LOM_SYSINDEXES file */
		e = LRDS_CloseRelation(orn2);
		if (e < 0) LOM_ERROR(handle, e);

		e = LRDS_CloseRelation(orn);
		if ( e < 0) LOM_ERROR(handle, e);

	} /* END : LOM_SYSINDEXES */


	/* START: LOM_SYSRELATIONSHIP */
	/* open LOM_SYSRELATIONSHIP and open scan on it */
	orn = LRDS_OpenRelation(volId, LOM_SYSRELATIONSHIP_CLASSNAME);
	if(orn < 0) LOM_ERROR(handle, orn);

	lockup.mode     = L_IX;
	lockup.duration = L_COMMIT;

	catScanId = LRDS_OpenSeqScan(orn, FORWARD, 0, NULL, &lockup);
	if(catScanId < 0) LOM_ERROR(handle, catScanId);

	for(i = 0; i < nSuperclasses; i++)
	{
        	e = LOM_GetClassID(handle, volId, superclassList[i], &superclassId);
	        if(e < 0) LOM_ERROR(handle, e);

		lockup2.mode     = L_IS;
		lockup2.duration = L_COMMIT;

		bound.op         = SM_EQ;
		bound.key.len    = sizeof(superclassId);
		bcopy(&superclassId, &(bound.key.val[0]), sizeof(superclassId));

		relTableEntry2 = LRDS_GET_RELTABLE_ENTRY(orn);
		catScanId2 = LRDS_OpenIndexScan(orn, &((LRDS_GET_IDXINFO_FROM_RELTABLE_ENTRY(relTableEntry2))[0].iid),
                                        &bound, &bound, 0, NULL, &lockup2);
		if(catScanId2 < 0) LOM_ERROR(handle, catScanId2);
    
		/* read superclass relationship infos and
		   inherites all relationships of the superclass */
		clist_for_rel[0].colNo = LOM_SYSRELATIONSHIP_FROMCLASSID_COLNO;
		clist_for_rel[0].start = ALL_VALUE;
		clist_for_rel[0].dataLength = sizeof(Four);
		clist_for_rel[0].length = sizeof(Four);
		clist_for_rel[0].nullFlag = SM_FALSE;
        
		clist_for_rel[1].colNo = LOM_SYSRELATIONSHIP_FROMATTRNUM_COLNO;
		clist_for_rel[1].start = ALL_VALUE;
		clist_for_rel[1].dataLength = SM_INT_SIZE;
		clist_for_rel[1].length = SM_INT_SIZE;
		clist_for_rel[1].nullFlag = SM_FALSE;
        
		clist_for_rel[2].colNo = LOM_SYSRELATIONSHIP_TOCLASSID_COLNO;
		clist_for_rel[2].start = ALL_VALUE;
		clist_for_rel[2].dataLength = sizeof(Four);
		clist_for_rel[2].length = sizeof(Four);
		clist_for_rel[2].nullFlag = SM_FALSE;
        
		clist_for_rel[3].colNo = LOM_SYSRELATIONSHIP_TOATTRNUM_COLNO;
		clist_for_rel[3].start = ALL_VALUE;
		clist_for_rel[3].dataLength = SM_INT_SIZE;
		clist_for_rel[3].length = SM_INT_SIZE;
		clist_for_rel[3].nullFlag = SM_FALSE;
        
		clist_for_rel[4].colNo = LOM_SYSRELATIONSHIP_CARDINALITY_COLNO;
		clist_for_rel[4].start = ALL_VALUE;
		clist_for_rel[4].dataLength = sizeof(One);
		clist_for_rel[4].length = sizeof(One);
		clist_for_rel[4].data.ptr = &cardinality;
		clist_for_rel[4].nullFlag = SM_FALSE;
        
		clist_for_rel[5].colNo = LOM_SYSRELATIONSHIP_RELATIONSHIPNAME_COLNO;
		clist_for_rel[5].start = ALL_VALUE;
		clist_for_rel[5].dataLength = sizeof(relationshipName);
		clist_for_rel[5].length = sizeof(relationshipName);
		clist_for_rel[5].data.ptr = relationshipName;
		clist_for_rel[5].nullFlag = SM_FALSE;

		clist_for_rel[6].colNo = LOM_SYSRELATIONSHIP_RELATIONSHIPID_COLNO;
		clist_for_rel[6].start = ALL_VALUE;
		clist_for_rel[6].dataLength = sizeof(Four);
		clist_for_rel[6].length = sizeof(Four);
		clist_for_rel[6].nullFlag = SM_FALSE;
        
		clist_for_rel[7].colNo = LOM_SYSRELATIONSHIP_DIRECTION_COLNO;
		clist_for_rel[7].start = ALL_VALUE;
		clist_for_rel[7].dataLength = sizeof(One);
		clist_for_rel[7].length = sizeof(One);
		clist_for_rel[7].data.ptr = &direction;
		clist_for_rel[7].nullFlag = SM_FALSE;

	    while((e = LRDS_NextTuple(catScanId2, &tid, NULL)) != EOS)
		{
			if(e < 0) LOM_ERROR(handle, e);

			/* get relationship info from catalog */
			e = LRDS_FetchTuple(catScanId2, SM_TRUE, &tid, 8, clist_for_rel);
			if(e < 0) LOM_ERROR(handle, e);

			fromClassId      = classId;
			fromAttrNum      = GET_VALUE_FROM_COL_LIST(clist_for_rel[1], sizeof(Two));
			toClassId        = GET_VALUE_FROM_COL_LIST(clist_for_rel[2], sizeof(Four));
			toAttrNum        = GET_VALUE_FROM_COL_LIST(clist_for_rel[3], sizeof(Two));
			relationshipId   = GET_VALUE_FROM_COL_LIST(clist_for_rel[6], sizeof(Four));
			/* cardinality, direction, relationshipName are automatically
			   loaded by LRDS_FetchTuple */
            
			/* generate reverse cardinality */
			switch(cardinality)
			{
				case LOM_RELATIONSHIP_ONE_TO_ONE:
				case LOM_RELATIONSHIP_MANY_TO_MANY:
					reverseCardinality = cardinality;
					break;
				case LOM_RELATIONSHIP_MANY_TO_ONE:
					reverseCardinality = LOM_RELATIONSHIP_ONE_TO_MANY;
					break;
				case LOM_RELATIONSHIP_ONE_TO_MANY:
					reverseCardinality = LOM_RELATIONSHIP_MANY_TO_ONE;
					break;
			}
            
			/* adjust from-attr-num caused by multiple inheritance */
			if(nSuperclasses > 1)
			{
				e = lom_AdjustAttrNum(handle, volId, superclassId, fromAttrNum, classId, &fromAttrNum);
				if(e < 0) LOM_ERROR(handle, e);
			}

			e = lom_AddRelationship(handle, catScanId, SM_TRUE, 
				fromClassId, fromAttrNum,
				toClassId, toAttrNum,
				direction, cardinality,
				relationshipId, relationshipName);
			if(e < 0) LOM_ERROR(handle, e);
            
			if(direction == LOM_RELATIONSHIP_BIDIRECTIONAL)
			{
				e = lom_AddRelationship(handle, catScanId, SM_TRUE, toClassId, toAttrNum,
					fromClassId, fromAttrNum,
                                        direction, reverseCardinality,
                                        relationshipId, relationshipName);
				if(e < 0) LOM_ERROR(handle, e);
			}
        	}
		e = LRDS_CloseScan(catScanId2);
		if(e < 0) LOM_ERROR(handle, e);
	}

	e = LRDS_CloseScan(catScanId);
	if(e < 0) LOM_ERROR(handle, e);

	e = LRDS_CloseRelation(orn);
	if(e < 0) LOM_ERROR(handle, e);

	if(Catalog_GetMountTableInfo(handle, volId, &mv) == eNOERROR)
	{
		/* Update inheritance informations */
		e = Catalog_CreateInheritanceInfo(handle, volId, classId, nSuperclasses, superClassIdList);
		if(e < 0) LOM_ERROR(handle, e);

		/* update in-memory catalog */
		e = Catalog_Relationship_CreateClass(handle, volId, classId, 
											 nSuperclasses, superclassList);
		if(e < 0) LOM_ERROR(handle, e);
	}
    
	/* END : LOM_SYSRELATIONSHIP */

	for(i = 0; i< nAttrs; i++) {
		if(attrinfo[i].type == LOM_TEXT) {

			e = LOM_Text_SetFilter(handle, volId, classId, GET_USERLEVEL_COLNO(i), LOM_DEFAULT_FILTER_NO);
			if(e < eNOERROR) LOM_ERROR(handle, e);

			e = LOM_Text_SetKeywordExtractor(handle, volId, classId, GET_USERLEVEL_COLNO(i), LOM_DEFAULT_KEYWORDEXTRACTOR_NO);
			if(e < eNOERROR) LOM_ERROR(handle, e);

			e = LOM_Text_SetStemizer(handle, volId, classId, GET_USERLEVEL_COLNO(i), LOM_DEFAULT_STEMIZER_NO);
			if(e < eNOERROR) LOM_ERROR(handle, e);
		}
	}

	return(eNOERROR);

} /* LOM_CreateClass() */


Four mig_Catalog_CreateIndexCatalog(
	LOM_Handle		*handle,
	Four			volId,			/* IN volume where the relation is placed */
	char			*className,     /* IN class name */
	char			*indexName,		/* IN index name */
	LOM_IndexDesc	*idesc,			/* IN index information */
	IndexID			*physical_iid	/* IN physical index id */
)
{
	LockParameter lockup;   /* relName will be locked commit duration */
	Four orn;				/* open relation number */
	Four e;
	ColListStruct	clist[LOM_SYSINDEXES_NUM_COLS + 1];
	Four classId;
	Four indexInfoTblEntryIndex;
	Four newIndexInfoTblEntryIndex;
	catalog_SysClassesOverlay *tmpNode;
	catalog_SysIndexesOverlay *tmpIndex;
	Four i;
	Four v;
	Boolean tmpFlag;
	LOM_IndexID	iid;
	Four mv;

	/* check parameters */
	if (volId < 0) LOM_ERROR(handle, eBADPARAMETER_LOM);
    
	if (className == NULL) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if (indexName == NULL) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if (idesc == NULL) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if (physical_iid == NULL) LOM_ERROR(handle, eBADPARAMETER_LOM);

	if(idesc->indexType == SM_INDEXTYPE_BTREE)
		for(i = 0; i< idesc->kinfo.btree.nColumns; i++) {
			idesc->kinfo.btree.columns[i].colNo++;
		}
	else 
		for(i = 0; i< idesc->kinfo.btree.nColumns; i++) {
			idesc->kinfo.mlgf.colNo[i]++;
		}

	orn = LRDS_OpenRelation(volId, className);
	if(orn < eNOERROR) LOM_ERROR(handle, orn);

	if(LRDS_IS_TEMPORARY_RELATION(orn)) tmpFlag = SM_TRUE;
	else tmpFlag = SM_FALSE;

	e = LRDS_CloseRelation(orn);
	if ( e < 0) LOM_ERROR(handle, e);

	iid.isLogical = SM_FALSE;
	iid.index.physical_iid = *physical_iid;

	if(tmpFlag) {
		e = Catalog_GetTemporaryClassId(handle, volId, className, &classId);

		/* reflect in memory catalog */
		e = Catalog_AddIndex(handle, volId, classId, indexName, idesc, &iid);
		if( e < 0 ) LOM_ERROR(handle, e);

		return eNOERROR;
	}

	if(idesc->indexType == SM_INDEXTYPE_BTREE)
		for(i = 0; i< idesc->kinfo.btree.nColumns; i++) {
			idesc->kinfo.btree.columns[i].colNo--;
		}
	else 
		for(i = 0; i< idesc->kinfo.btree.nColumns; i++) {
			idesc->kinfo.mlgf.colNo[i]--;
		}

	/* START : LOM_SYSINDEXES */ /* THINK */

	/* set the lockup parameter */
	lockup.mode = L_IX;
	lockup.duration = L_COMMIT;

	orn = LRDS_OpenRelation(volId,LOM_SYSINDEXES_CLASSNAME);
	if (orn < 0) LOM_ERROR(handle, orn);

	/* Construct 'clist'. */
	/* index name */
	clist[LOM_SYSINDEXES_INDEXNAME_COLNO].colNo = LOM_SYSINDEXES_INDEXNAME_COLNO;
	clist[LOM_SYSINDEXES_INDEXNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSINDEXES_INDEXNAME_COLNO].dataLength = strlen(indexName);
	clist[LOM_SYSINDEXES_INDEXNAME_COLNO].data.ptr = indexName;
	clist[LOM_SYSINDEXES_INDEXNAME_COLNO].nullFlag = SM_FALSE;
	
	/* class name */
	clist[LOM_SYSINDEXES_CLASSNAME_COLNO].colNo = LOM_SYSINDEXES_CLASSNAME_COLNO;
	clist[LOM_SYSINDEXES_CLASSNAME_COLNO].start = ALL_VALUE;
	clist[LOM_SYSINDEXES_CLASSNAME_COLNO].dataLength = strlen(className);
	clist[LOM_SYSINDEXES_CLASSNAME_COLNO].data.ptr = className;
	clist[LOM_SYSINDEXES_CLASSNAME_COLNO].nullFlag = SM_FALSE;
	
	/* index id */
	clist[LOM_SYSINDEXES_INDEXID_COLNO].colNo = LOM_SYSINDEXES_INDEXID_COLNO;
	clist[LOM_SYSINDEXES_INDEXID_COLNO].start = ALL_VALUE;
	clist[LOM_SYSINDEXES_INDEXID_COLNO].dataLength = sizeof(LOM_IndexID);
	clist[LOM_SYSINDEXES_INDEXID_COLNO].data.ptr = &iid; 
	clist[LOM_SYSINDEXES_INDEXID_COLNO].nullFlag = SM_FALSE;

	e = LRDS_CreateTuple(orn, SM_FALSE, LOM_SYSINDEXES_NUM_COLS, &(clist[0]), (TupleID*)NULL);
	if (e < 0) LOM_ERROR(handle, e);
	
	/* Close the data file */
	e = LRDS_CloseRelation(orn);
	if (e < 0) LOM_ERROR(handle, e);

    /* END : LOM_SYSINDEXES */ 

	e = lom_GetClassId(handle, volId, className, &classId);
	if( e < 0 ) LOM_ERROR(handle, e);

	/* reflect in memory catalog */
	if(Catalog_GetMountTableInfo(handle, volId, &mv) == eNOERROR)
	{
		e = Catalog_AddIndex(handle, volId, classId, indexName, idesc, &iid);
		if(e < 0) LOM_ERROR(handle, e);
	}

	return(eNOERROR);
}