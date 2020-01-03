#include "global.h"
#include "NotesLoaderUCS.h"
#include "BackgroundUtil.h"
#include "GameManager.h"
#include "MsdFile.h"
#include "NoteTypes.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "Song.h"
#include "SongManager.h"
#include "Steps.h"
#include "Attack.h"
#include "PrefsManager.h"
#include "MessageManager.h"
#include "RageFile.h"
#include "NoteData.h"

#include "NotesWriterSSC.h"
#include <vector>

static int stepcount = 0;

bool UCSLoader::LoadFromDir( const RString &sPath, Song &out )
{
	vector<RString> aFileNames;
	
	//Get all the UCS files in the directory
	GetApplicableFiles( sPath, aFileNames );

	stepcount = 0;

	bool success = false;
	for(auto it = aFileNames.begin(); it != aFileNames.end(); it++)
	{
		LOG->Info("UCSLoader::LoadFromDir() Loading %s",(*it).c_str());

		if(LoadFromSimfile(sPath + *it, out))
			success = true;
		else
			LOG->Info("Loading failed!");
	}

	return success;
}

void UCSLoader::GetApplicableFiles( const RString &sPath, vector<RString> &out )
{
	GetDirListing( sPath + RString("*.ucs"), out );
}

bool UCSLoader::LoadFromSimfile( const RString &sPath, Song &out, bool bFromCache )
{
	
	out.m_SongTiming.m_sFile = sPath;
	out.m_sSongFileName = sPath;

	LOG->Info( "UCSLoader::LoadFromSimfile(%s)", sPath.c_str() );
	
	/*RageFile * in = new RageFile();
	in->Open(sPath, RageFile::READ);*/

	Steps * pNewSteps = out.CreateSteps();
	if(LoadNoteDataFromSimfile(sPath, *pNewSteps))
	{
		pNewSteps->SetFilename(sPath);
		out.AddSteps(pNewSteps);
	} else {
		delete pNewSteps;
	}

	//in->Close();

	return true;
}

bool UCSLoader::LoadNoteDataFromSimfile( const RString &path, Steps &out )
{

	LOG->Trace( "UCSLoader::LoadNoteDataFromSimfile(%s)", path.c_str() );

	RageFile * in = new RageFile();
	in->Open(path, RageFile::READ);

    RString line;
    int beatsPerMeasure = 0;
    float currentBeat = 0.0f;

    int numTracks = 0;
    TimingData timingData;
    NoteData noteData;

    out.SetSavedToDisk(true);
    out.SetDifficulty(Difficulty_Edit);
    out.SetMeter(99);
    out.SetChartName("UCS Import");
    out.SetDescription("UCS Import");

	while(true)
    {
        int res = in->GetLine(line);
        if (res == 0)
        {
            break;
        }
        else if (res == -1)
        {
            return false;
        }
        //if we successfully read the line..
        //Check if it's timing/chart information (not steps)
        if (line.at(0) == ':')
        {
            size_t pos = line.rfind( '=' );
            RString key = line.substr(1,pos-1); //Start at 1 to skip the ':' char, cut before '=' char
            RString value = line.substr( pos+1, line.size()-pos+1 );
            //LOG->Info("UCSLoader: key: %s",key.c_str());
            //LOG->Info("UCSLoader: val: %s",value.c_str());
            if (key == "Format")
            {
                if (value != "1")
                {
                    LOG->Warn("UCSLoader: Unknown UCS type %s! Only type 1 is supported.", value.c_str());
                    return false;
                }
            }
            else if (key == "Mode")
            {
                if (value == "Single" || value == "S-Performance")
                {
                    out.m_StepsType = StepsType_pump_single;
                    noteData.SetNumTracks(5);
                }
                else if (value == "Double" || value == "D-Performance")
                {
                    out.m_StepsType = StepsType_pump_double;
                    noteData.SetNumTracks(10);
                }
                else
                {
                    LOG->Warn("UCSLoader: Unknown type %s is not supported yet.",value.c_str());
                    return false;
                }

                numTracks = noteData.GetNumTracks();
            }
            else if (key == "BPM")
            {
                timingData.SetBPMAtBeat(currentBeat,stoi(value));
            }
            else if (key == "Delay")
            {
                timingData.SetDelayAtBeat(currentBeat, stoi(value));
            }
            else if (key == "Beat")
            {
                beatsPerMeasure = stoi(value)*2;
                timingData.SetTimeSignatureAtBeat(currentBeat,stoi(value)*2, 4);
                timingData.SetBPMAtBeat(currentBeat,stoi(value));
            }
            else if (key == "Split")
            {
                //Do nothing
            }
            else
            {
                LOG->Warn("UCSLoader: Ignoring unknown tag %s",key.c_str());
            }


        }
        //It's note data
        else
        {
            //ProcessRow(in, noteData, timingData, (useData), tracksPerRow, BeatToNoteRow(currentBeat));
            //ProcessNote(in, noteData, timingData, useData, 0, row, data);
            for (int i = 0; i < numTracks; i++)
            {
                switch(line[i])
                {
                    //Hold head
                    case 'M':
                        //noteData.AddHoldNote(i,)
                        break;
                    //Hold body. This is unnecessary in SM so just ignore it..
                    case 'H':
                        break;
                    //Hold tail
                    case 'W':
                        break;
                    //Tap note
                    case 'X':
                        noteData.SetTapNote(i,BeatToNoteRow(currentBeat), TAP_ORIGINAL_TAP);
                }
            }
            currentBeat++;

        }

    }

	in->Close();

	timingData.TidyUpData(false);
	out.SetNoteData(noteData);
	out.m_Timing = timingData;
	out.TidyUpData();
	return true;
}
