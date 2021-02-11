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

#ifndef SLIMDOWN_TEXTIR

#include "OOSQL_StorageSystemHeaders.h"
#include "OOSQL_APIs_Internal.hxx"
#include "OOSQL_Error.h"
#include "OOSQL_Tool.hxx"
#include "OOSQL_Tool_MergePosting.hxx"
#include "DBM.h"

/*
 * MergePostingFiles class body definition
 */

MergePostingFiles::MergePostingFiles(LOM_Handle* handle, Four nPostingFiles, char** postingFileNames, char* tempDirPath)
{
	m_handle 				= handle;	
	m_nPostingFiles 		= nPostingFiles;
	m_nActivePostingFiles	= 0;
	m_isAllPostingRead		= false;
	
	strcpy(m_tempDirPath, tempDirPath);
	
	m_postingFileNames.resize(m_nPostingFiles);
	m_filePointers.resize(m_nPostingFiles);
	m_postingBuffers.resize(m_nPostingFiles);
	
	for (int i = 0; i < m_nPostingFiles; i++)
	{
		m_postingFileNames[i] = postingFileNames[i];
		m_postingBuffers[i]   = "";
	}
}

MergePostingFiles::~MergePostingFiles(void)
{
}


Four oosql_Tool_MergePosting(
	OOSQL_SystemHandle*	systemHandle,
	Four				nPostingFiles,
	char** 				postingFileNames,
	char* 				newPostingFileName
)
{
	LOM_Handle*			handle;
	char*				tempDir;
	Eight				count;
    FILE*				fp;
    char 				filename[MAXPATHLEN];
	string 				stringBuffer;
	char*  				stringPtr;
   	MergePostingFiles*	mpf;
   	Four				e;

	handle = &OOSQL_GET_LOM_SYSTEMHANDLE(systemHandle);

    /* check parameters */
	if (nPostingFiles < 1) OOSQL_ERR(eBADPARAMETER_LOM);
	if (postingFileNames == NULL) OOSQL_ERR(eBADPARAMETER_LOM);
	if (newPostingFileName == NULL) OOSQL_ERR(eBADPARAMETER_LOM);

    tempDir = getenv("ODYS_TEMP_PATH");
	if (tempDir == NULL) OOSQL_ERR(eTEMPDIRNOTDEFINED_UTIL);

	/* open output file */
	sprintf(filename, "%s%s%s", tempDir, DIRECTORY_SEPARATOR, newPostingFileName);
	e = lom_Text_OpenTempFile(handle, filename, "w", &fp);
	OOSQL_CHECK_ERROR(e);

	count = 0;
    	
   	/* initialize MergePostingFiles instance */
   	mpf = new MergePostingFiles(handle, nPostingFiles, postingFileNames, tempDir);
    	
   	e = mpf->m_openPostingFiles();
   	OOSQL_CHECK_ERR(e);
    	
   	while (1)
   	{
   		e = mpf->m_findMinPosting(stringBuffer);
   		if (e == EOS) break;
    		
		// parsing stringBuffer
		stringPtr      = (char*)stringBuffer.c_str();
		fprintf(fp, "%s", stringPtr);

        count++;
        if ((count%20000) == 0)
        {
	        printf("merging %ldth postings...\n", count);
			Util_fflush(fp);		
		}
	}

	e = mpf->m_closePostingFiles();
	OOSQL_CHECK_ERR(e);

	delete mpf;

	e = lom_Text_CloseTempFile(handle, fp);
	OOSQL_CHECK_ERROR(e);

    return eNOERROR;
}

#else /* SLIMDOWN_TEXTIR */

#include "OOSQL_StorageSystemHeaders.h"
#include "OOSQL_APIs_Internal.hxx"
#include "OOSQL_Error.h"
#include "OOSQL_Tool.hxx"
#include "OOSQL_Tool_MergePosting.hxx"

Four oosql_Tool_MergePosting(
    OOSQL_SystemHandle* systemHandle,
    Four                nPostingFiles,
    char**              postingFileNames,
    char*               newPostingFileName
)
{
    return eTEXTIR_NOTENABLED_OOSQL;
}


#endif /* SLIMDOWN_TEXTIR */

