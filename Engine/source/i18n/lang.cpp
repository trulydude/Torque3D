//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/stream/stream.h"
#include "core/stream/fileStream.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "core/util/safeDelete.h"
#include "console/engineAPI.h"

#include "i18n/lang.h"

//-----------------------------------------------------------------------------
// LangFile Class
//-----------------------------------------------------------------------------

LangFile::LangFile(const UTF8 *langName /* = NULL */)
{
   VECTOR_SET_ASSOCIATION( mStringTable );

	if(langName)
	{
		dsize_t langNameLen = dStrlen(langName) + 1;
		mLangName = new UTF8 [langNameLen];
		dStrcpy(mLangName, langName, langNameLen);
	}
	else
		mLangName = NULL;

	mLangFile = NULL;
}

LangFile::~LangFile()
{
	// [tom, 3/1/2005] Note: If this is freed in FreeTable() then when the file
	// is loaded, the language name will be blitzed.
	// Programming after 36 hours without sleep != good.

   SAFE_DELETE_ARRAY(mLangName);
   SAFE_DELETE_ARRAY(mLangFile);
	freeTable();
}

void LangFile::freeTable()
{
	U32 i;
	for(i = 0;i < mStringTable.size();i++)
	{
		if(mStringTable[i])
			delete [] mStringTable[i];
	}
	mStringTable.clear();
}

bool LangFile::load(const UTF8 *filename)
{
	FileStream * stream;
   if((stream = FileStream::createAndOpen( filename, Torque::FS::File::Read )) == NULL)
      return false;

	bool ret = load(stream);
	delete stream;
   return ret;
}

bool LangFile::load(Stream *s)
{
   freeTable();

   while (s->getStatus() == Stream::Ok)
   {
      char buf[2048];
      s->readLongString(2048, buf);
      if (s->getStatus() == Stream::Ok)
         addString((const UTF8*)buf);
   }
   return true;
}

bool LangFile::save(const UTF8 *filename)
{
	FileStream *fs;

	if(!isLoaded())
		return false;

   if((fs = FileStream::createAndOpen( filename, Torque::FS::File::Write )) == NULL)
      return false;

	bool ret = save(fs);
	delete fs;

   return ret;
}

bool LangFile::save(Stream *s)
{
   if (!isLoaded())
      return false;

   U32 i;
   for (i = 0; i < mStringTable.size(); i++)
   {
      s->writeLongString(2048, (char*)mStringTable[i]); //irei1as_ lang
   }
   return true;
}

const UTF8 * LangFile::getString(U32 id)
{
	if(id == LANG_INVALID_ID || id >= mStringTable.size())
		return NULL;
	return mStringTable[id];
}

U32 LangFile::addString(const UTF8 *str)
{
	dsize_t newstrLen = dStrlen(str) + 1;
	UTF8 *newstr = new UTF8 [newstrLen];
	dStrcpy(newstr, str, newstrLen);
	mStringTable.push_back(newstr);
	return mStringTable.size() - 1;
}

void LangFile::setString(U32 id, const UTF8 *str)
{
	if(id >= mStringTable.size())
   {
      U32 oldsize = mStringTable.size();
		mStringTable.setSize(id+1);
      for( U32 i=oldsize; i<mStringTable.size(); ++i )
      {
         mStringTable[i] = NULL;
      }
   }

   SAFE_DELETE_ARRAY(mStringTable[id]);

	dsize_t newstrLen = dStrlen(str) + 1;
	UTF8 *newstr = new UTF8 [newstrLen];
	dStrcpy(newstr, str, newstrLen);
	mStringTable[id] = newstr;
}

void LangFile::setLangName(const UTF8 *newName)
{
	if(mLangName)
		delete [] mLangName;

	dsize_t langNameLen = dStrlen(newName) + 1;
	mLangName = new UTF8 [langNameLen];
	dStrcpy(mLangName, newName, langNameLen);
}

void LangFile::setLangFile(const UTF8 *langFile)
{
	if(mLangFile)
		delete [] mLangFile;

	dsize_t langFileLen = dStrlen(langFile) + 1;
	mLangFile = new UTF8 [langFileLen];
	dStrcpy(mLangFile, langFile, langFileLen);
}

bool LangFile::activateLanguage()
{
	if(isLoaded())
		return true;

	if(mLangFile)
	{
		return load(mLangFile);
	}
	return false;
}

void LangFile::deactivateLanguage()
{
	if(mLangFile && isLoaded())
		freeTable();
}

//-----------------------------------------------------------------------------
// LangTable Class
//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(LangTable);

ConsoleDocClass( LangTable,
   "@brief Provides the code necessary to handle the low level management "
   "of the string tables for localization\n\n"

   "One LangTable is created for each mod, as well as one for the C++ code. "
   "LangTable is responsible for obtaining the correct strings from each "
   "and relaying it to the appropriate controls.\n\n"

   "@see Localization for a full description\n\n"

   "@ingroup Localization\n"
);

LangTable::LangTable() : mDefaultLang(-1), mCurrentLang(-1)
{
   VECTOR_SET_ASSOCIATION( mLangTable );
}

LangTable::~LangTable()
{
	S32 i;

	for(i = 0;i < mLangTable.size();i++)
	{
		if(mLangTable[i])
			delete mLangTable[i];
	}
	mLangTable.clear();
}

S32 LangTable::addLanguage(LangFile *lang, const UTF8 *name /* = NULL */)
{
	if(name)
		lang->setLangName(name);

	mLangTable.push_back(lang);

	if(mDefaultLang == -1)
		setDefaultLanguage(mLangTable.size() - 1);
	if(mCurrentLang == -1)
		setCurrentLanguage(mLangTable.size() - 1);

	return mLangTable.size() - 1;
}

S32 LangTable::addLanguage(const UTF8 *filename, const UTF8 *name /* = NULL */)
{
	LangFile * lang = new LangFile(name);

   if(Torque::FS::IsFile(filename))
	{
		lang->setLangFile(filename);

      S32 ret = addLanguage(lang);
		if(ret >= 0)
			return ret;
	}
	delete lang;

	return -1;
}

const UTF8 *LangTable::getString(const U32 id) const
{
	const UTF8 *s = NULL;

	if(mCurrentLang >= 0)
		s = mLangTable[mCurrentLang]->getString(id);
	if(s == NULL && mDefaultLang >= 0 && mDefaultLang != mCurrentLang)
		s = mLangTable[mDefaultLang]->getString(id);

	return s;
}

const U32 LangTable::getStringLength(const U32 id) const
{
	const UTF8 *s = getString(id);
	if(s)
		return dStrlen(s);

	return 0;
}

void LangTable::setDefaultLanguage(S32 langid)
{
	if(langid >= 0 && langid < mLangTable.size())
	{
		if(mLangTable[langid]->activateLanguage())
		{
			if(mDefaultLang >= 0)
				mLangTable[mDefaultLang]->deactivateLanguage();

			mDefaultLang = langid;
		}
	}
}

void LangTable::setCurrentLanguage(S32 langid)
{
	if(langid >= 0 && langid < mLangTable.size())
	{
		if(mLangTable[langid]->activateLanguage())
		{
			Con::printf("Language %s [%s] activated.", mLangTable[langid]->getLangName(), mLangTable[langid]->getLangFile());

			if(mCurrentLang >= 0 && mCurrentLang != mDefaultLang)
			{
				mLangTable[mCurrentLang]->deactivateLanguage();
				Con::printf("Language %s [%s] deactivated.", mLangTable[mCurrentLang]->getLangName(), mLangTable[mCurrentLang]->getLangFile());
			}
			mCurrentLang = langid;
		}
	}
}

//-----------------------------------------------------------------------------
// LangTable Console Methods
//-----------------------------------------------------------------------------



DefineEngineMethod(LangTable, addLanguage, S32, (String filename, String languageName), ("", ""),
			  "(string filename, [string languageName])"
			  "@brief Adds a language to the table\n\n"
			  "@param filename Name and path to the language file\n"
			  "@param languageName Optional name to assign to the new language entry\n\n"
			  "@return True If file was successfully found and language created\n"
			  )
{
	UTF8 scriptFilenameBuffer[1024];

	Con::expandScriptFilename((char*)scriptFilenameBuffer, sizeof(scriptFilenameBuffer), filename);
	return object->addLanguage(scriptFilenameBuffer, (const UTF8*)languageName);
}

DefineEngineMethod(LangTable, getString, const char *, (U32 id), ,
			  "(string filename)"
			  "@brief Grabs a string from the specified table\n\n"
			  "If an invalid is passed, the function will attempt to "
			  "to grab from the default table\n\n"
			  "@param filename Name of the language table to access\n\n"
			  "@return Text from the specified language table, \"\" if ID was invalid and default table is not set")
{
	const char * str =	(const char*)object->getString(id);
	if(str != NULL)
	{
		dsize_t retLen = dStrlen(str) + 1;
		char * ret = Con::getReturnBuffer(retLen);
		dStrcpy(ret, str, retLen);
		return ret;
	}

	return "";
}

DefineEngineMethod(LangTable, setDefaultLanguage, void, (S32 langId), , "(int language)"
			  "@brief Sets the default language table\n\n"
			  "@param language ID of the table\n")
{
	object->setDefaultLanguage(langId);
}

DefineEngineMethod(LangTable, setCurrentLanguage, void, (S32 langId), ,
			  "(int language)"
			  "@brief Sets the current language table for grabbing text\n\n"
			  "@param language ID of the table\n")
{
	object->setCurrentLanguage(langId);
}

DefineEngineMethod(LangTable, getCurrentLanguage, S32, (), , "()"
			  "@brief Get the ID of the current language table\n\n"
			  "@return Numerical ID of the current language table")
{
	return object->getCurrentLanguage();
}

DefineEngineMethod(LangTable, getLangName, const char *, (S32 langId), , "(int language)"
			  "@brief Return the readable name of the language table\n\n"
			  "@param language Numerical ID of the language table to access\n\n"
			  "@return String containing the name of the table, NULL if ID was invalid or name was never specified")
{
	const char * str = (const char*)object->getLangName(langId);
	if(str != NULL)
	{
		dsize_t retLen = dStrlen(str) + 1;
		char * ret = Con::getReturnBuffer(retLen);
		dStrcpy(ret, str, retLen);
		return ret;
	}

	return "";
}

DefineEngineMethod(LangTable, getNumLang, S32, (), , "()"
			  "@brief Used to find out how many languages are in the table\n\n"
			  "@return Size of the vector containing the languages, numerical")
{
	return object->getNumLang();
}

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

UTF8 *sanitiseVarName(const UTF8 *varName, UTF8 *buffer, U32 bufsize)
{
	if(! varName || bufsize < 10)	// [tom, 3/3/2005] bufsize check gives room to be lazy below
	{
		*buffer = 0;
		return NULL;
	}

	dStrcpy(buffer, (const UTF8*)"I18N::", bufsize);

	UTF8 *dptr = buffer + 6;
	const UTF8 *sptr = varName;
	while(*sptr)
	{
		if(dIsalnum(*sptr))
			*dptr++ = *sptr++;
		else
		{
			if(*(dptr - 1) != '_')
				*dptr++ = '_';
			sptr++;
		}

		if((dptr - buffer) >= (bufsize - 1))
			break;
	}
	*dptr = 0;

	return buffer;
}

UTF8 *getCurrentModVarName(UTF8 *buffer, U32 bufsize)
{
	char varName[256];
	StringTableEntry cbName = Con::getCurrentScriptModuleName();

	const UTF8 *slash = (const UTF8*)dStrchr(cbName, '/');
	if (slash == NULL)
	{
		Con::errorf("Illegal CodeBlock path detected in sanitiseVarName() (no mod directory): %s", cbName);
		return NULL;
	}

	dStrncpy(varName, cbName, slash - (const UTF8*)cbName);
	varName[slash - (const UTF8*)cbName] = 0;

	return sanitiseVarName((UTF8*)varName, buffer, bufsize);
}

const LangTable *getCurrentModLangTable()
{
	UTF8 saneVarName[256];

	if(getCurrentModVarName(saneVarName, sizeof(saneVarName)))
	{
		const LangTable *lt = dynamic_cast<LangTable *>(Sim::findObject(Con::getIntVariable((const char*)saneVarName)));
		return lt;
	}
	return NULL;
}

const LangTable *getModLangTable(const UTF8 *mod)
{
	UTF8 saneVarName[256];

	if(sanitiseVarName(mod, saneVarName, sizeof(saneVarName)))
	{
		const LangTable *lt = dynamic_cast<LangTable *>(Sim::findObject(Con::getIntVariable((const char*)saneVarName)));
		return lt;
	}
	return NULL;
}

//lang_ localization
bool compiledFileNeedsUpdate(UTF8* filename)
{
   Torque::Path filePath = Torque::Path(filename);
   Torque::FS::FileNodeRef sourceFile = Torque::FS::GetFileNode(filePath);
   Torque::Path compiledPath = Torque::Path(filePath);
   compiledPath.setExtension("lso");
   Torque::FS::FileNodeRef compiledFile = Torque::FS::GetFileNode(compiledPath);

   Torque::Time sourceModifiedTime, compiledModifiedTime;

   if (sourceFile != NULL)
      sourceModifiedTime = sourceFile->getModifiedTime();

   if (compiledFile != NULL)
      compiledModifiedTime = compiledFile->getModifiedTime();

   if (sourceModifiedTime > compiledModifiedTime)
      return true;
   return false;
}

DefineEngineFunction(CompileLanguage, void, (const char* inputFile, bool createMap), (false),
   "@brief Compiles a LSO language file."
   " if createIndex is true, will also create languageMap." TORQUE_SCRIPT_EXTENSION " with"
   " the global variables for each string index."
   " The input file must follow this example layout:"
   " TXT_HELLO_WORLD = Hello world in english!")
{
   UTF8 scriptFilenameBuffer[1024];
   Con::expandScriptFilename((char*)scriptFilenameBuffer, sizeof(scriptFilenameBuffer), inputFile);

   if (!Torque::FS::IsFile(scriptFilenameBuffer))
   {
      Con::errorf("CompileLanguage - file %s not found", scriptFilenameBuffer);
      return;
   }

   FileObject file;
   if (!file.readMemory(scriptFilenameBuffer))
   {
      Con::errorf("CompileLanguage - couldn't read file %s", scriptFilenameBuffer);
      return;
   }

   if (compiledFileNeedsUpdate(scriptFilenameBuffer))
   {
      FileStream *mapStream = NULL;
      if (createMap)
      {
         Torque::Path mapPath = scriptFilenameBuffer;
         mapPath.setFileName("languageMap");
         mapPath.setExtension(TORQUE_SCRIPT_EXTENSION);
         if ((mapStream = FileStream::createAndOpen(mapPath, Torque::FS::File::Write)) == NULL)
            Con::errorf("CompileLanguage - failed creating languageMap." TORQUE_SCRIPT_EXTENSION);
      }

      LangFile langFile;
      const U8* inLine = NULL;
      const char* separatorStr = " = ";
      S32 stringId = 0;
      while ((inLine = file.readLine())[0] != 0)
      {
         char* line;
         chompUTF8BOM((const char *)inLine, &line);
         char* div = dStrstr(line, separatorStr);
         if (div == NULL)
         {
            Con::errorf("Separator %s not found in line: %s", separatorStr, line);
            Con::errorf("Could not determine string name ID");
            continue;
         }
         *div = 0;
         char* text = div + dStrlen(separatorStr);

         langFile.addString((const UTF8*)text);

         if (mapStream)
         {
            String mapLine = String::ToString("$%s = %i;", line, stringId);
            mapStream->writeLine((const U8*)mapLine.c_str());
            String commentLine = String::ToString("// %s", text);
            mapStream->writeLine((const U8*)commentLine.c_str());
         }

         stringId++;
      }

      Torque::Path lsoPath = scriptFilenameBuffer;
      lsoPath.setExtension("lso");
      langFile.save(lsoPath.getFullPath());

      if (mapStream)
         delete mapStream;
   }
}
//end lang_ localization
