#ifndef NotesLoaderUCS_H
#define NotesLoaderUCS_H

#include "GameConstantsAndTypes.h"
#include "BackgroundUtil.h"
#include "Attack.h"

class Song;
class Steps;
class TimingData;

/**
 * @brief The SSCLoader handles all of the parsing needed for .ssc files.
 */
struct UCSLoader
{

	bool LoadFromSimfile( const RString &sPath, Song &out, bool bFromCache = false );
	
	bool LoadFromDir( const RString &sPath, Song &out );

	bool LoadNoteDataFromSimfile( const RString &path, Steps &out );

	void GetApplicableFiles( const RString &sPath, vector<RString> &out );

	//RString GetFileExtension() const { return fileExt; }

private:
	/** @brief The file extension in use. */
	const RString fileExt;
	/** @brief The song title that is being processed. */
	RString songTitle;
};


#endif

/*
* (c) 2020 Rhythm Lunatic
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, and/or sell copies of the Software, and to permit persons to
* whom the Software is furnished to do so, provided that the above
* copyright notice(s) and this permission notice appear in all copies of
* the Software and that both the above copyright notice(s) and this
* permission notice appear in supporting documentation.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
* THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
* INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
* OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
* OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
* OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*/
