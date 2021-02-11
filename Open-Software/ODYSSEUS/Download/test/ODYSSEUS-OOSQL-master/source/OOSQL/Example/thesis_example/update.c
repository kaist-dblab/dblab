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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "OOSQL_APIs.h"

#define ERROR_CHECK(e) \
do { \
if (e < 0) { \
    printf("File:%s Line:%d\n", __FILE__, __LINE__); fflush(stdout); \
	return e; \
}\
} while(0);

#define ERROR_CHECK_XCT(systemHandle, e, xactId) \
do { \
if (e < 0) { \
	char errorMessage[4096]; \
	OOSQL_GetErrorName(systemHandle, e, errorMessage, sizeof(errorMessage)); \
	printf("OOSQL ERROR(%s) : ", errorMessage); \
    OOSQL_GetErrorMessage(systemHandle, e, errorMessage, sizeof(errorMessage)); \
    puts(errorMessage); \
    if((xactId) != NULL) (int) OOSQL_TransAbort(systemHandle, xactId); \
        (int) OOSQL_DestroySystemHandle(systemHandle, procIndex); \
    if (1) exit(1); \
}\
} while(0);

#define MAXVARSTRINGLEN		1024
#define MAXQUERYSTRLEN		2048

typedef struct paperStruct {
	int id;
	char title[MAXVARSTRINGLEN];
	char author[MAXVARSTRINGLEN];
	char affiliation[MAXVARSTRINGLEN];
	char keyword[MAXVARSTRINGLEN];
	char abstract[MAXVARSTRINGLEN];
	char language[MAXVARSTRINGLEN];
	char journal[MAXVARSTRINGLEN];
	int year;
	int volume;
	int number;
	char pages[MAXVARSTRINGLEN];
} paperStruct;

int TITLE_COLNO = 0;
int AUTHOR_COLNO = 0;
int AFFILIATION_COLNO = 0;
int ABSTRACT_COLNO = 0;
int KEYWORD_COLNO = 0;

Four procIndex;

int make_query(paperStruct* paper, char* queryString)
{
	char line[MAXVARSTRINGLEN];
	char queryStr1[MAXQUERYSTRLEN];
	char queryStr2[MAXQUERYSTRLEN];
	char buf[MAXVARSTRINGLEN];
	short and_required = 0;
	short comma_required = 0;
	short nQmark = 0;

	printf("\nInput keyword to search\n");

	strcpy(queryStr1, "where ");

	printf("title      : ");
	gets(line);
	if (strlen(line) > 0)  {
		sprintf(buf, "MATCH(title, %s) > 0", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("author     : ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "MATCH(author, %s) > 0", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("affiliation: ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "MATCH(affiliation, %s) > 0", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("abstract   : ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "MATCH(abstract, %s) > 0", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("keyword    : ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "MATCH(keyword, %s) > 0", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("journal    : ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "journal = %s", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("language   : ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "language = %s", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	printf("year       : ");
	gets(line);
	if (strlen(line) > 0)  {
		if (and_required) strcat(queryStr1, " and ");
		sprintf(buf, "year = %s", line);
		strcat(queryStr1, buf);
		and_required = 1;
	}

	if (!and_required) 
		queryStr1[strlen(queryStr1)-6] = '\0';

	strcpy(queryString, "update Paper set ");

	printf("\nInput keyword to update\n");
	printf("Press Enter if nothing to update\n");

	printf("id         : ");
	gets(line);
	paper->id = atoi(line);

	printf("title      : ");
	gets(paper->title);

	printf("author     : ");
	gets(paper->author);

	printf("affiliation: ");
	gets(paper->affiliation);

	printf("keyword    : ");
	gets(paper->keyword);

	printf("abstract   : ");
	gets(paper->abstract);

	printf("language   : ");
	gets(paper->language);

	printf("journal    : ");
	gets(paper->journal);

	printf("year       : ");
	gets(line);
	paper->year = atoi(line);

	printf("volume     : ");
	gets(line);
	paper->volume = atoi(line);

	printf("number     : ");
	gets(line);
	paper->number = atoi(line);

	printf("pages      : ");
	gets(paper->pages);

	if (strlen(paper->title) > 0)  {
		TITLE_COLNO = nQmark++;
		strcpy(queryStr2, "title = text ?");			/* title */
		comma_required = 1;
	}

	if (strlen(paper->author) > 0)  {
		AUTHOR_COLNO = nQmark++;
		if (comma_required) strcat(queryStr2, ", ");
		strcat(queryStr2, "author = text ?");			/* author */
		comma_required = 1;
	}

	if (strlen(paper->affiliation) > 0)  {
		AFFILIATION_COLNO = nQmark++;
		if (comma_required) strcat(queryStr2, ", ");
		strcat(queryStr2, "affiliation = text ?");		/* affiliation */
		comma_required = 1;
	}

	if (strlen(paper->abstract) > 0)  {
		ABSTRACT_COLNO = nQmark++;
		if (comma_required) strcat(queryStr2, ", ");
		strcat(queryStr2, "abstract = text ?");			/* abstract */
		comma_required = 1;
	}

	if (strlen(paper->keyword) > 0)  {
		KEYWORD_COLNO = nQmark++;
		if (comma_required) strcat(queryStr2, ", ");
		strcat(queryStr2, "keyword = text ?");			/* keyword */
		comma_required = 1;
	}

	if (strlen(paper->journal) > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "journal = \"%s\"", paper->journal);
		strcat(queryStr2, buf);							/* journal */
		comma_required = 1;
	}

	if (strlen(paper->language) > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "language = \"%s\"", paper->language);
		strcat(queryStr2, buf);							/* language */
		comma_required = 1;
	}

	if (strlen(paper->pages) > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "pages = \"%s\"", paper->pages);
		strcat(queryStr2, buf);							/* pages */
		comma_required = 1;
	}

	if (paper->id > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "id = %d", paper->id);
		strcat(queryStr2, buf);							/* id */
		comma_required = 1;
	}

	if (paper->year > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "year = %d", paper->year);
		strcat(queryStr2, buf);							/* year */
		comma_required = 1;
	}

	if (paper->volume > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "volume = %d", paper->volume);
		strcat(queryStr2, buf);							/* volume */
		comma_required = 1;
	}

	if (paper->number > 0)  {
		if (comma_required) strcat(queryStr2, ", ");
		sprintf(buf, "number = %d", paper->number);
		strcat(queryStr2, buf);							/* number */
		comma_required = 1;
	}

	strcat(queryString, queryStr2);
	strcat(queryString, "\n");
	strcat(queryString, queryStr1);

	return eNOERROR;
}

int execute_query(OOSQL_SystemHandle* systemHandle, paperStruct* paper, char *queryStr, Four volId)
{
	OOSQL_Handle handle;
	Four nObjects = 0;
	Four e;
	Four i;

	printf("\nNow Executing requested query:\n");
	printf("%s\n", queryStr);

	e = OOSQL_AllocHandle(systemHandle, volId, &handle); 
	ERROR_CHECK(e);

	e = OOSQL_Prepare(systemHandle, handle, queryStr, NULL);
	if(e < eNOERROR)
	{
		e = OOSQL_FreeHandle(systemHandle, handle); 
		ERROR_CHECK(e);
	}

	e = OOSQL_Execute(systemHandle, handle); 
	if(e < eNOERROR)
	{
		e = OOSQL_FreeHandle(systemHandle, handle); 
		ERROR_CHECK(e);
	}

	while((e = OOSQL_Next(systemHandle, handle)) != ENDOFEVAL)
	{
		if (strlen(paper->title) > 0) {
			e = OOSQL_PutData(systemHandle, handle, TITLE_COLNO, 0, &paper->title, strlen(paper->title)+1);  
			ERROR_CHECK(e);
		}

		if (strlen(paper->author) > 0) {
			e = OOSQL_PutData(systemHandle, handle, AUTHOR_COLNO, 0, &paper->author, strlen(paper->author)+1);  
			ERROR_CHECK(e);
		}

		if (strlen(paper->affiliation) > 0) {
			e = OOSQL_PutData(systemHandle, handle, AFFILIATION_COLNO, 0, &paper->affiliation, strlen(paper->affiliation)+1);
			ERROR_CHECK(e);
		}

		if (strlen(paper->abstract) > 0) {
			e = OOSQL_PutData(systemHandle, handle, ABSTRACT_COLNO, 0, &paper->abstract, strlen(paper->abstract)+1);
			ERROR_CHECK(e);
		}

		if (strlen(paper->keyword) > 0) {
			e = OOSQL_PutData(systemHandle, handle, KEYWORD_COLNO, 0, &paper->keyword, strlen(paper->keyword)+1);
			ERROR_CHECK(e);
		}

		nObjects++;
	}

	e = OOSQL_FreeHandle(systemHandle, handle);
	ERROR_CHECK(e);

	printf("\n%d objects are updated.\n", nObjects);

	return (eNOERROR);
}

int main(int argc, char **argv)
{
    char DBPATH[256];
	char queryString[MAXQUERYSTRLEN];
	paperStruct paper;
	Four databaseID;
    Four volID;
    XactID xactID;
    Four e;
	OOSQL_SystemHandle systemHandle;

    if (argc != 2)
    {
       printf("USAGE : update <database name>\n");
       exit(1);
    }

    printf("OOSQL_CreateSystemHandle\n");
    e = OOSQL_CreateSystemHandle(&systemHandle, &procIndex);
    ERROR_CHECK_XCT(&systemHandle, e, NULL);

	strcpy(DBPATH, argv[1]);

    printf("OOSQL_MountDB\n");
    e = OOSQL_MountDB(&systemHandle, DBPATH, &databaseID);
    ERROR_CHECK_XCT(&systemHandle, e, NULL);

	e = OOSQL_GetUserDefaultVolumeID(&systemHandle, databaseID, &volID);
    ERROR_CHECK_XCT(&systemHandle, e, NULL);
    
    printf("OOSQL_TransBegin\n");
    e = OOSQL_TransBegin(&systemHandle, &xactID, X_RR_RR);
    ERROR_CHECK_XCT(&systemHandle, e, &xactID);

	e = make_query(&paper, queryString);
	ERROR_CHECK_XCT(&systemHandle, e, &xactID);

	e = execute_query(&systemHandle, &paper, queryString, volID);
	ERROR_CHECK_XCT(&systemHandle, e, &xactID);

    printf("OOSQL_TransCommit\n");
    e = OOSQL_TransCommit(&systemHandle, &xactID);
    ERROR_CHECK_XCT(&systemHandle, e, &xactID);

    printf("OOSQL_DismountDB\n");
    e = OOSQL_DismountDB(&systemHandle, databaseID);
    ERROR_CHECK_XCT(&systemHandle, e, NULL);
    
    printf("OOSQL_DestroySystemHandle\n");
    e = OOSQL_DestroySystemHandle(&systemHandle, procIndex);
    ERROR_CHECK_XCT(&systemHandle, e, NULL);
    
    return eNOERROR;
}