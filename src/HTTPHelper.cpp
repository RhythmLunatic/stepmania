#include "global.h"
#include "HTTPHelper.h"

RString HTTPHelper::SubmitGetRequest(const RString &URL) {
    RString sBUFFER = "";
#if !defined(WITHOUT_NETWORKING)
    EzSockets wSocket;
    //w
    RString Proto;
    RString Server;
    int Port=80;
    RString sAddress;


    if( HTTPHelper::ParseHTTPAddress( URL, Proto, Server, Port, sAddress)  )
    {
        //sAddress = HTTPHelper::URLEncode( sAddress );

        if ( sAddress != "/" )
            sAddress = "/" + sAddress;

        wSocket.close();
        wSocket.create();
        wSocket.setTimeout(0,250000); //acceptable timeout
        wSocket.setBlocking(true);

        if( !wSocket.connect( Server, (short) Port ) )
        {
            LOG->Warn("HTTPHelper::SubmitPostRequest failed to connect to %s:%d ", Server.c_str(),Port);
            return sBUFFER;
        }
        //Produce HTTP POST broadcast

        RString Header="";

        Header = "GET "+ sAddress + " HTTP/1.1\r\n";
        Header+= "Host: " + Server + "\r\n";
        Header+= "User-Agent: OpenITG/1.0.0\r\n";
        Header+= "Accept: */*\r\n";
        //Header+= "Content-Length: " + SSTR( strlen(GetData.c_str() ) ) + "\r\n";
        //Header+= "Content-Type: application/x-www-form-urlencoded\r\n";
        //Header+= GetData;

        wSocket.SendData( Header.c_str(), Header.length() );

        wSocket.setBlocking(false);
        int BytesGot=0;

        while(1)
        {
            char res[HTTP_CHUNK_SIZE];

            //WHY IS THIS BLOCKING UNTIL TIMEOUT OCCURS?!
            int iSize = wSocket.ReadData( res, HTTP_CHUNK_SIZE );
            if( iSize <= 0 )
                break;


            sBUFFER.append( res, iSize );
            BytesGot += iSize;
            if( iSize < HTTP_CHUNK_SIZE )
                break;
        }
        wSocket.close();
    }
    else
    {
        LOG->Warn("HTTPHelper::SubmitGetRequest could not parse %s ", URL.c_str());
    }
#endif

    return sBUFFER;
}

RString HTTPHelper::SubmitPostRequest(const RString &URL, const RString &PostData)
{
	RString sBUFFER = "";
	#if !defined(WITHOUT_NETWORKING)
		EzSockets wSocket;
		//w
		RString Proto;
		RString Server;
		int Port=80;
		RString sAddress;
	

		if( HTTPHelper::ParseHTTPAddress( URL, Proto, Server, Port, sAddress)  )
		{
				//sAddress = HTTPHelper::URLEncode( sAddress );

				if ( sAddress != "/" )
					sAddress = "/" + sAddress;

				wSocket.close();
				wSocket.create();
				wSocket.setTimeout(0,250000); //acceptable timeout
				wSocket.setBlocking(true);
				
				if( !wSocket.connect( Server, (short) Port ) )
				{
					LOG->Warn("HTTPHelper::SubmitPostRequest failed to connect to %s:%d ", Server.c_str(),Port);
					return sBUFFER;
				}
				//Produce HTTP POST broadcast

				RString Header="";

				Header = "POST "+ sAddress + " HTTP/1.1\r\n";
				Header+= "Host: " + Server + "\r\n";
				Header+= "User-Agent: OpenITG/1.0.0\r\n";
				Header+= "Accept: */*\r\n";
				Header+= "Content-Length: " + SSTR( strlen(PostData.c_str() ) ) + "\r\n";
				Header+= "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
				Header+= PostData;

				wSocket.SendData( Header.c_str(), Header.length() );

				wSocket.setBlocking(false);
				int BytesGot=0;

				while(1)
				{
					char res[HTTP_CHUNK_SIZE];
					
					//WHY IS THIS BLOCKING UNTIL TIMEOUT OCCURS?!
					int iSize = wSocket.ReadData( res, HTTP_CHUNK_SIZE );
					if( iSize <= 0 )
						break;
					

					sBUFFER.append( res, iSize );
					BytesGot += iSize;
					if( iSize < HTTP_CHUNK_SIZE )
						break;
				}
				wSocket.close();
		}
		else
		{
			LOG->Warn("HTTPHelper::SubmitPostRequest could not parse %s ", URL.c_str());
		}
	#endif

	return sBUFFER;
}

HTTPHelper::HTTPHelper()
{
	_URL="";
	_PostData="";
	_retBuffer="";
	m_ResultLock = new RageMutex("ResultLock");
}

HTTPHelper::~HTTPHelper()
{
	delete m_ResultLock;
	//delete _URL;
	//delete _PostData;
	//delete _retBuffer;
}


//Wrapper of function to the submit post request so we can use a mutex on the result
void HTTPHelper::Threaded_SubmitPostRequest_Thread_Wrapper()
{
	m_ResultLock->Lock();
	_retBuffer=SubmitPostRequest(_URL,_PostData);
	m_ResultLock->Unlock();
}

//Starts a threaded version of SubmitPostRequest -- Game can continue immediately
void HTTPHelper::Threaded_SubmitPostRequest(const RString &myURL, const RString &myPostData)
{
	RageThread HTTPThread;
	_URL=myURL;
	_PostData=myPostData;
	HTTPThread.SetName("HTTPThread_SubmitPostRequest");
	HTTPThread.Create(Threaded_SubmitPostRequest_Start, this);
}

//Returns the result stored in an object after the mutex has been released
RString HTTPHelper::GetThreadedResult()
{
	m_ResultLock->Lock();
	RString ret (_retBuffer);
	m_ResultLock->Unlock();
	return ret;
	
}

bool HTTPHelper::ParseHTTPAddress( const RString &URL, RString &sProto, RString &sServer, int &iPort, RString &sAddress )
{
	// [PROTO://]SERVER[:PORT][/URL]

	Regex re(
		"^([A-Z]+)://" // [0]: HTTP://
		"([^/:]+)"     // [1]: a.b.com
		"(:([0-9]+))?" // [2], [3]: :1234 (optional, default 80)
		"(/(.*))?$");    // [4], [5]: /foo.html (optional)

	vector<RString> asMatches;
	if( !re.Compare( URL, asMatches ) )
		return false;
	ASSERT( asMatches.size() == 6 );

	sProto = asMatches[0];
	sServer = asMatches[1];
	if( asMatches[3] != "" )
	{
		iPort = atoi(asMatches[3]);
		if( iPort == 0 )
			return false;
	}
	else
		iPort = 80;

	sAddress = asMatches[5];

	return true;
}


RString HTTPHelper::StripOutContainers( const RString & In )
{
	if( In.size() == 0 )
		return "";

	unsigned i = 0;
	char t = In.at(i);
	while( t == ' ' && i < In.length() )
	{
		t = In.at(++i);
	}

	if( t == '\"' || t == '\'' )
	{
		unsigned j = i+1; 
		char u = In.at(j);
		while( u != t && j < In.length() )
		{
			u = In.at(++j);
		}
		if( j == i )
			return StripOutContainers( In.substr(i+1) );
		else
			return StripOutContainers( In.substr(i+1, j-i-1) );
	}
	return In.substr( i );
}