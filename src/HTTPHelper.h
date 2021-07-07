#ifndef HTTPHelper_H
#define HTTPHelper_H

#include "global.h" // StepMania only includes
#include "ezsockets.h"
#include "RageUtil.h"
#include "RageLog.h"
#include "RageThreads.h"
#include <sstream>
#define HTTP_CHUNK_SIZE 1024 //matches EZSockets
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

class HTTPHelper
{
public:

	HTTPHelper();
	~HTTPHelper();
	//True if proper string, false if improper
	static bool ParseHTTPAddress( const RString & URL, RString & Proto, RString & Server, int & Port, RString & Addy );
	static RString StripOutContainers( const RString & In );	//Strip off "'s and ''s
	static RString SubmitGetRequest( const RString &URL );
	static RString SubmitPostRequest( const RString &URL, const RString &PostData ); // Sends a URL Post Request
	void Threaded_SubmitPostRequest( const RString &URL, const RString &PostData ); // Sends a URL Post Request
	RString GetThreadedResult();

	
private:
	static RString HTTPThread_SubmitPostRequest( const RString &URL, const RString &PostData ); // Sends a URL Post Request
	void Threaded_SubmitPostRequest_Thread_Wrapper(); //wrapper for mutex use
	static int Threaded_SubmitPostRequest_Start(void *p) { ((HTTPHelper *)p)->Threaded_SubmitPostRequest_Thread_Wrapper(); return 0; };

	RString _URL;
	RString _PostData;
	RString _retBuffer;
	RageMutex *m_ResultLock;
};

#endif